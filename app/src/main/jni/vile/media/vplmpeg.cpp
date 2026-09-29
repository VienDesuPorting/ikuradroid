/*
 * MPEG-1 video playback on top of pl_mpeg (pl_mpeg.h, MIT).
 *
 * The FFMPEG video path in media/hh never shipped in this port, so
 * EngineVN::PlayVideo stayed a stub: the Ikura drivers registered
 * dologo.mpg and the script's AVIP opcode silently swallowed it. This
 * player decodes the same streams with a single portable C decoder,
 * no platform media layers involved.
 *
 * The playback recipe comes from the Divi-Dead Android port:
 *   - Audio is decoded to S16 by pl_mpeg and mixed into SDL_mixer's
 *     already-open output device via Mix_HookMusic. Opening a second
 *     output device with SDL_QueueAudio is not an option on Android:
 *     the AudioTrack backend only allows one open device per process
 *     ("Audio device already open"), and SDL_mixer is holding it.
 *     The mixer's channels and music are halted for the duration so
 *     the hook owns the mix; the decode loop paces itself on the
 *     backlog of the ring buffer feeding the hook.
 *   - All buffer parameters adapt to the video duration: short logo
 *     splashes start fast, long openings get a large jitter cushion.
 *   - Streams without audio (or with an unsupported mixer spec) fall
 *     back to wall-clock pacing.
 *
 * Frames go YUV -> RGBA -> EDLTexture and are presented through the
 * engine renderer, so the logical viewport letterbox applies to video
 * the same way it applies to every widget. plm_frame_to_rgba leaves
 * the alpha byte untouched, so the frame buffer is primed to opaque
 * once before playback - the texture is blended, and alpha 0 would
 * render the whole video invisible. A finger release, mouse button or
 * key press skips the playback.
 */

#include "vplmpeg.h"
#include <stdlib.h>
#include <string.h>
#include <SDL_mixer.h>
#include "../common/edl_gfx.h"
#include "../common/edl_texture.h"
#include "../common/log.h"

#define PL_MPEG_IMPLEMENTATION
#include "pl_mpeg.h"

/*! \brief Feeds pl_mpeg from a ViLE resource stream */
static void plm_rw_load(plm_buffer_t *Buffer,void *User){
        RWops *ops=(RWops*)User;
        if (Buffer->discard_read_bytes){
                plm_buffer_discard_read_bytes(Buffer);
        }
        size_t available=Buffer->capacity-Buffer->length;
        if (available==0){
                return;
        }
        int got=ops->Read(Buffer->bytes+Buffer->length,(int)available);
        if (got>0){
                Buffer->length+=(size_t)got;
        }
        else{
                Buffer->has_ended=TRUE;
        }
}

/*! \brief pl_mpeg seek callback over a resource stream */
static void plm_rw_seek(plm_buffer_t *Buffer,size_t Offset,void *User){
        (void)Buffer;
        ((RWops*)User)->Seek((int)Offset,SEEK_SET);
}

/*! \brief pl_mpeg tell callback over a resource stream */
static size_t plm_rw_tell(plm_buffer_t *Buffer,void *User){
        (void)Buffer;
        return (size_t)((RWops*)User)->Tell();
}

/*! \brief Lock-free-ish SPSC ring buffer between decoder and mixer hook */
typedef struct{
        Uint8 *data;                            //!< Byte storage (capacity is a power of two)
        size_t capacity;                        //!< Storage size in bytes
        size_t mask;                            //!< capacity-1, for wrap indexing
        size_t read;                            //!< Consumer position
        size_t write;                           //!< Producer position
        SDL_mutex *lock;                        //!< Guards read/write positions
} PLM_RING;

/*! \brief State shared with the decode callbacks */
typedef struct{
        Uint8 *rgba;                            //!< Decoded frame in RGBA
        int vw;                                 //!< Video width
        int vh;                                 //!< Video height
        SDL_Surface *surface;                   //!< Wraps the rgba buffer
        EDLTexture *texture;                    //!< Upload path to the renderer
        SDL_Rect rect;                          //!< Destination within the game screen
        bool audiook;                           //!< Mixer hook active
        int streamrate;                         //!< Stream samplerate
        int mixrate;                            //!< Mixer samplerate
        double rpos;                            //!< Resampler phase (input frames)
        float rcarry[2];                        //!< Resampler carry (last input frame)
        PLM_RING *ring;                         //!< PCM backlog for the mixer hook
} PLM_PLAYBACK;

/*! \brief Allocates a power-of-two ring buffer */
static PLM_RING *plm_ring_create(size_t CapacityPow2){
        PLM_RING *ring=(PLM_RING*)malloc(sizeof(PLM_RING));
        if (!ring){
                return 0;
        }
        memset(ring,0,sizeof(*ring));
        ring->data=(Uint8*)malloc(CapacityPow2);
        ring->capacity=CapacityPow2;
        ring->mask=CapacityPow2-1;
        ring->lock=SDL_CreateMutex();
        if (!ring->data || !ring->lock){
                if (ring->lock){
                        SDL_DestroyMutex(ring->lock);
                }
                free(ring->data);
                free(ring);
                return 0;
        }
        return ring;
}

/*! \brief Frees a ring buffer */
static void plm_ring_destroy(PLM_RING *ring){
        if (!ring){
                return;
        }
        SDL_DestroyMutex(ring->lock);
        free(ring->data);
        free(ring);
}

/*! \brief Pushes bytes into the ring; drops the chunk when full */
static void plm_ring_push(PLM_RING *ring,const Uint8 *Bytes,size_t Count){
        if (!ring || !Count){
                return;
        }
        SDL_LockMutex(ring->lock);
        size_t used=ring->write-ring->read;
        if (Count<=ring->capacity-used){
                size_t start=ring->write & ring->mask;
                size_t first=Count;
                if (first>ring->capacity-start){
                        first=ring->capacity-start;
                }
                memcpy(ring->data+start,Bytes,first);
                memcpy(ring->data,Bytes+first,Count-first);
                ring->write+=Count;
        }
        SDL_UnlockMutex(ring->lock);
}

/*! \brief Mix hook: drains the ring into the mixer stream */
static void video_music_hook(void *Udata,Uint8 *Stream,int Len){
        PLM_RING *ring=(PLM_RING*)Udata;
        if (!ring){
                memset(Stream,0,Len);
                return;
        }
        SDL_LockMutex(ring->lock);
        size_t used=ring->write-ring->read;
        size_t take=(size_t)Len;
        if (take>used){
                take=used;
        }
        if (take){
                size_t start=ring->read & ring->mask;
                size_t first=take;
                if (first>ring->capacity-start){
                        first=ring->capacity-start;
                }
                memcpy(Stream,ring->data+start,first);
                memcpy(Stream+first,ring->data,take-first);
                ring->read+=take;
        }
        SDL_UnlockMutex(ring->lock);
        if (take<(size_t)Len){
                memset(Stream+take,0,Len-take);
        }
}

/*! \brief Current unconsumed backlog in bytes */
static Uint32 plm_ring_backlog(PLM_RING *ring){
        if (!ring){
                return 0;
        }
        SDL_LockMutex(ring->lock);
        size_t used=ring->write-ring->read;
        SDL_UnlockMutex(ring->lock);
        return (Uint32)used;
}

/*! \brief Converts a decoded frame and paints it to the screen */
static void video_decode_cb(plm_t *plm,plm_frame_t *frame,void *user){
        (void)plm;
        PLM_PLAYBACK *pb=(PLM_PLAYBACK*)user;
        if (!pb->texture){
                return;
        }
        plm_frame_to_rgba(frame,pb->rgba,pb->vw*4);
        pb->texture->loadFromSurface(pb->surface);
        SDL_SetRenderDrawColor(EDLRenderer,0,0,0,255);
        SDL_RenderClear(EDLRenderer);
        SDL_SetRenderTarget(EDLRenderer,NULL);
        pb->texture->render(NULL,&pb->rect);
        SDL_RenderPresent(EDLRenderer);
}

/*! \brief Converts decoded float samples to S16 and queues them */
static void audio_decode_cb(plm_t *plm,plm_samples_t *samples,void *user){
        (void)plm;
        PLM_PLAYBACK *pb=(PLM_PLAYBACK*)user;
        if (!pb->audiook || !pb->ring){
                return;
        }

        // Upscaled staging: the mixer rate may exceed the stream rate, so
        // one decode callback can emit up to 2x+ the input frames.
        static Sint16 pcm[PLM_AUDIO_SAMPLES_PER_FRAME*2*2+4];
        const float *in=samples->interleaved;
        int frames=samples->count;
        int out=0;

        if (pb->mixrate==pb->streamrate){
                // Fast path: plain float -> S16 conversion
                for (int i=0;i<frames*2;i++){
                        float s=in[i];
                        if (s>1.0f) s=1.0f;
                        if (s<-1.0f) s=-1.0f;
                        pcm[i]=(Sint16)(s*32767.0f);
                }
                out=frames;
        }
        else{
                // Linear resampler. The previous callback's final frame is
                // carried over so consecutive batches interpolate seamlessly.
                double step=(double)pb->streamrate/(double)pb->mixrate;
                double pos=pb->rpos;
                while (out<PLM_AUDIO_SAMPLES_PER_FRAME*2){
                        int i0=(int)pos;
                        if (i0>=frames){
                                break;
                        }
                        double frac=pos-(double)i0;
                        for (int ch=0;ch<2;ch++){
                                float s0=pb->rcarry[ch];
                                float s1=in[i0*2+ch];
                                float s=s0+(s1-s0)*(float)frac;
                                if (s>1.0f) s=1.0f;
                                if (s<-1.0f) s=-1.0f;
                                pcm[out*2+ch]=(Sint16)(s*32767.0f);
                        }
                        out++;
                        pos+=step;
                }
                // Carry the last consumed input frame and the phase remainder
                pb->rcarry[0]=in[(frames-1)*2];
                pb->rcarry[1]=in[(frames-1)*2+1];
                pb->rpos=pos-(double)frames;
        }

        if (out>0){
                plm_ring_push(pb->ring,(Uint8*)pcm,(size_t)out*2*sizeof(Sint16));
        }
}

bool VideoPLMPEG_Play(RWops *Resource,const SDL_Rect *Dest,int ScreenW,int ScreenH){
        if (!Resource){
                return false;
        }

        // The stream may live inside an archive, hence the callback buffer.
        plm_buffer_t *buffer=plm_buffer_create_with_callbacks(
                        plm_rw_load,plm_rw_seek,plm_rw_tell,
                        (size_t)Resource->Size(),Resource);
        plm_t *plm=buffer?plm_create_with_buffer(buffer,TRUE):0;
        if (!plm){
                LogError("Video: decoder init failed");
                return false;
        }

        int vw=plm_get_width(plm);
        int vh=plm_get_height(plm);
        double framerate=plm_get_framerate(plm);
        double duration=plm_get_duration(plm);
        int samplerate=plm_get_samplerate(plm);
        bool hasaudio=plm_get_audio_enabled(plm)!=0;
        LogVerbose("Video: %dx%d @%.2ffps %.1fs audio=%d rate=%d",
                vw,vh,framerate,duration,hasaudio?1:0,samplerate);
        if (vw<=0 || vh<=0){
                plm_destroy(plm);
                return false;
        }

        // The script carries display dimensions only, so the picture is
        // centred on the game screen; without a rect it covers it all.
        PLM_PLAYBACK pb;
        memset(&pb,0,sizeof(pb));
        pb.vw=vw;
        pb.vh=vh;
        pb.streamrate=samplerate;
        if (Dest && Dest->w>0 && Dest->h>0){
                pb.rect=*Dest;
                if (pb.rect.w>ScreenW) pb.rect.w=ScreenW;
                if (pb.rect.h>ScreenH) pb.rect.h=ScreenH;
                if (pb.rect.x<0) pb.rect.x=0;
                if (pb.rect.y<0) pb.rect.y=0;
        }
        else{
                pb.rect.x=0;
                pb.rect.y=0;
                pb.rect.w=ScreenW;
                pb.rect.h=ScreenH;
        }

        pb.rgba=(Uint8*)malloc(vw*vh*4);
        // plm_frame_to_rgba only writes RGB; the alpha byte stays whatever
        // the frame buffer held. Prime it to opaque once - the texture is
        // created with blending, and zero alpha would hide the whole video.
        if (pb.rgba){
                for (int i=3;i<vw*vh*4;i+=4){
                        pb.rgba[i]=0xFF;
                }
        }
        pb.surface=pb.rgba?SDL_CreateRGBSurfaceWithFormatFrom(
                pb.rgba,vw,vh,32,vw*4,SDL_PIXELFORMAT_RGBA32):0;
        if (!pb.surface){
                LogError("Video: can't create the frame surface");
                free(pb.rgba);
                plm_destroy(plm);
                return false;
        }
        pb.texture=EDL_CreateTexture(vw,vh);

        plm_set_video_decode_callback(plm,video_decode_cb,&pb);
        plm_set_audio_decode_callback(plm,audio_decode_cb,&pb);

        // Duration-adaptive buffering (thresholds from the Divi-Dead port):
        //                    short (<=10s)    long (>=30s)
        //   lead_time        200 ms           800 ms
        //   high water start 200 ms           500 ms
        //   high water max   1.0 s            2.5 s
        // Values interpolate linearly in between.
        double leadtime,hwstart,hwmax;
        if (duration<=10.0){
                leadtime=0.200; hwstart=0.200; hwmax=1.0;
        }
        else if (duration>=30.0){
                leadtime=0.800; hwstart=0.500; hwmax=2.5;
        }
        else{
                double t=(duration-10.0)/20.0;
                leadtime=0.200+t*0.600;
                hwstart=0.200+t*0.300;
                hwmax=1.0+t*1.5;
        }

        // Park SDL_mixer's regular output while the video owns the mix.
        bool mixerplaying=(Mix_PlayingMusic()||Mix_Playing(-1))!=0;
        Mix_HaltChannel(-1);
        Mix_HaltMusic();
        Mix_Pause(-1);
        SDL_Delay(20);

        // Route video PCM through the mixer's already-open device. The
        // ring buffer is sized to hold more than the largest high water
        // mark plus a full decode burst; 2^20 bytes cover ~5.9s at 44.1kHz
        // stereo S16, comfortably above the 2.5s cap.
        int mixfreq=0;
        Uint16 mixformat=0;
        int mixchannels=0;
        if (hasaudio && Mix_QuerySpec(&mixfreq,&mixformat,&mixchannels) &&
                        mixformat==AUDIO_S16SYS && mixchannels==2){
                pb.ring=plm_ring_create(1<<20);
                if (pb.ring){
                        pb.mixrate=mixfreq;
                        if (pb.streamrate<=0){
                                pb.streamrate=mixfreq;
                        }
                        pb.audiook=true;
                        Mix_HookMusic(video_music_hook,pb.ring);
                        plm_set_audio_lead_time(plm,leadtime);
                }
        }
        if (hasaudio && !pb.audiook){
                Uint16 fmt=mixformat;
                LogError("Video: mixer spec unsupported (%dHz fmt=%04x ch=%d)",
                        mixfreq,fmt,mixchannels);
        }

        int rate=pb.audiook?pb.mixrate:((samplerate>0)?samplerate:44100);
        Uint32 hwstartb=(Uint32)(hwstart*rate*2*2);
        Uint32 hwmaxb=(Uint32)(hwmax*rate*2*2);
        Uint32 growth=(hwmaxb>hwstartb)?(hwmaxb-hwstartb)/20:0;

        // Prebuffer to the initial high water mark before starting, so the
        // first seconds absorb startup jitter without delaying the start.
        if (pb.audiook){
                Uint32 mark=SDL_GetTicks();
                while (!plm_has_ended(plm) && plm_ring_backlog(pb.ring)<hwstartb){
                        Uint32 now=SDL_GetTicks();
                        double dt=(now-mark)/1000.0;
                        mark=now;
                        if (dt>1.0/30.0) dt=1.0/30.0;
                        plm_decode(plm,dt);
                        SDL_Delay(2);
                }
        }

        // Self-pacing decode loop. Naive wall-clock plm_decode bursts on
        // Android (SDL_Delay granularity), so the ring backlog drives the
        // cadence; the high water cap ramps toward hwmax over 20s.
        Uint32 lastticks=SDL_GetTicks();
        Uint32 playstart=lastticks;
        Uint32 silentstart=lastticks;
        double decoded=0;
        bool skipped=false;
        bool aborted=false;
        SDL_Event event;

        while (!plm_has_ended(plm)){
                if (pb.audiook){
                        Uint32 elapsed=SDL_GetTicks()-playstart;
                        Uint32 high=hwstartb+growth*(elapsed/1000);
                        if (high>hwmaxb) high=hwmaxb;
                        Uint32 backlog=plm_ring_backlog(pb.ring);
                        Uint32 lowwater=hwstartb/2;
                        if (backlog<lowwater){
                                do{
                                        if (plm_has_ended(plm)) break;
                                        Uint32 now=SDL_GetTicks();
                                        double dt=(now-lastticks)/1000.0;
                                        lastticks=now;
                                        if (dt>1.0/30.0) dt=1.0/30.0;
                                        plm_decode(plm,dt);
                                        backlog=plm_ring_backlog(pb.ring);
                                }while (backlog<lowwater);
                        }
                        else if (backlog<high){
                                Uint32 now=SDL_GetTicks();
                                double dt=(now-lastticks)/1000.0;
                                lastticks=now;
                                if (dt>1.0/30.0) dt=1.0/30.0;
                                plm_decode(plm,dt);
                                SDL_Delay(4);
                        }
                        else{
                                lastticks=SDL_GetTicks();
                                SDL_Delay(8);
                        }
                }
                else{
                        // No audio stream (or mixer spec): pace against wall clock,
                        // capping each step so a slow poll never bursts frames.
                        Uint32 now=SDL_GetTicks();
                        double dt=(now-silentstart)/1000.0-decoded;
                        if (dt<=0){
                                SDL_Delay(1);
                        }
                        else{
                                if (dt>0.100) dt=0.100;
                                decoded+=dt;
                                plm_decode(plm,dt);
                        }
                }

                // The engine's event pump is parked while this loop blocks, so
                // everything polled here belongs to the video.
                while (SDL_PollEvent(&event)){
                        if (event.type==SDL_QUIT ||
                                event.type==SDL_APP_TERMINATING ||
                                event.type==SDL_APP_WILLENTERBACKGROUND){
                                aborted=true;
                        }
                        else if (event.type==SDL_FINGERUP){
                                // Zero pressure marks the synthetic cancel-UP in the
                                // engine's input model; it must not skip anything.
                                if (event.tfinger.pressure>0.0f){
                                        skipped=true;
                                }
                        }
                        else if (event.type==SDL_MOUSEBUTTONUP ||
                                event.type==SDL_KEYDOWN){
                                skipped=true;
                        }
                        if (skipped||aborted) break;
                }
                if (skipped||aborted) break;
        }

        // Release the mixer hook and wake SDL_mixer back up. The legacy
        // SDL_LockAudio guards against a mix pass running inside the hook
        // while the ring it reads from is torn down.
        if (pb.audiook){
                SDL_LockAudio();
                Mix_HookMusic(NULL,NULL);
                SDL_UnlockAudio();
        }
        plm_ring_destroy(pb.ring);
        Mix_Resume(-1);
        if (mixerplaying){
                Mix_ResumeMusic();
        }

        LogVerbose("Video: finished (skipped=%d)",skipped?1:0);

        delete pb.texture;
        SDL_FreeSurface(pb.surface);
        free(pb.rgba);
        plm_destroy(plm);
        return true;
}
