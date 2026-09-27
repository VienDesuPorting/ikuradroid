package su.viende.ikuradroid;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.zip.Inflater;

/**
 * Reads the engine's savegame files for the Java save/load UI.
 *
 * The container format lives in jni/vile/common/savegame.cpp and is a
 * tiny sequential archive: each entry is
 *
 *     [name: 32 bytes, zero padded] [start: u32 LE] [end: u32 LE] [data]
 *
 * where start..end are byte offsets of the payload inside the whole
 * file, and the next entry follows the previous entry's end offset.
 * Strings are stored raw as UTF-8 (uString keeps UTF-8 internally),
 * surfaces are "ZLIB" blobs (magic + u32 BE raw size + deflate stream
 * of [w: u16 BE][h: u16 BE][bpp: u8][pixels, A B G R each]) with a
 * legacy BMP fallback from pre-0.4.10 saves.
 *
 * Nothing here ever calls into the engine: slots are read from disk on
 * a worker thread, and the actual save/load is triggered separately
 * through SDLActivity.nativeSendSaveLoadEvent().
 */
public final class SaveFileRepository {

    private static final int NAME_LEN = 32;                 // SNLENGTH
    private static final int HDR_LEN = NAME_LEN + 8;        // SHLENGTH
    /** 5 pages x 8 slabs, the grid the native dialogs also address. */
    public static final int SLOT_COUNT = 40;

    private SaveFileRepository() {}

    /** Everything the UI shows for one slot. */
    public static class Slot {
        public int index;
        public boolean exists;
        public String message = "";   // "savemsg" - scene/chapter caption
        public String date = "";      // "savedate" - when it was written
        public Bitmap thumb;          // "screen-thumb", null when absent
        public long bytes;
    }

    /** Filename prefix for savegames of the current engine. */
    public static String saveFile(String prefix, int index) {
        return prefix + String.format(Locale.US, "%03d", index);
    }

    /** Reads the whole 0..SLOT_COUNT-1 grid for one game. */
    public static List<Slot> listSlots(String dir, String prefix) {
        List<Slot> out = new ArrayList<Slot>(SLOT_COUNT);
        for (int i = 0; i < SLOT_COUNT; i++) {
            out.add(readSlot(new File(dir, saveFile(prefix, i)), i));
        }
        return out;
    }

    /** Parses a single savegame file; missing files become empty slots. */
    public static Slot readSlot(File file, int index) {
        Slot slot = new Slot();
        slot.index = index;
        if (!file.isFile()) return slot;
        byte[] buf = readFile(file);
        if (buf == null) return slot;
        slot.bytes = buf.length;

        byte[] msg = findEntry(buf, "savemsg");
        if (msg != null) slot.message = asString(msg);
        byte[] date = findEntry(buf, "savedate");
        if (date != null) slot.date = asString(date);
        byte[] thumb = findEntry(buf, "screen-thumb");
        if (thumb != null) slot.thumb = decodeThumb(thumb);

        // A slot counts as occupied when it carries a screenshot: both
        // the ikura and the Will writers always store one, and the
        // native dialog gates on the very same entry.
        slot.exists = thumb != null;
        return slot;
    }

    /** Decodes a "ZLIB" surface blob (or legacy BMP) into a bitmap. */
    public static Bitmap decodeThumb(byte[] b) {
        if (b == null || b.length < 8) return null;
        if (b[0] == 'B' && b[1] == 'M') {
            return BitmapFactory.decodeByteArray(b, 0, b.length);
        }
        if (b[0] != 'Z' || b[1] != 'L' || b[2] != 'I' || b[3] != 'B') return null;

        Inflater inf = new Inflater();
        try {
            inf.setInput(b, 8, b.length - 8);
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte[] tmp = new byte[16384];
            while (!inf.finished()) {
                int n = inf.inflate(tmp);
                if (n == 0) {
                    if (inf.needsInput() || inf.needsDictionary()) break;
                    continue;
                }
                out.write(tmp, 0, n);
            }
            byte[] raw = out.toByteArray();
            if (raw.length < 5) return null;
            int w = ((raw[0] & 0xFF) << 8) | (raw[1] & 0xFF);
            int h = ((raw[2] & 0xFF) << 8) | (raw[3] & 0xFF);
            int bpp = raw[4] & 0xFF;
            if (w <= 0 || h <= 0 || w > 4096 || h > 4096 || bpp != 32) return null;
            if (raw.length < 5 + w * h * 4) return null;

            // SaveSurface() serialises the pixel value byte by byte:
            // pixel 0xAABBGGRR (EDL_CreateSurface masks on little-endian)
            // lands as A, B, G, R - rebuild ARGB accordingly.
            Bitmap bmp = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
            int[] px = new int[w * h];
            int o = 5;
            for (int p = 0; p < w * h; p++) {
                int a = raw[o] & 0xFF;
                int bl = raw[o + 1] & 0xFF;
                int g = raw[o + 2] & 0xFF;
                int r = raw[o + 3] & 0xFF;
                o += 4;
                px[p] = (a << 24) | (r << 16) | (g << 8) | bl;
            }
            bmp.setPixels(px, 0, w, 0, 0, w, h);
            return bmp;
        } catch (Throwable t) {
            return null;
        } finally {
            inf.end();
        }
    }

    /** Sequential-archive lookup, mirroring Savegame::search(). */
    private static byte[] findEntry(byte[] buf, String name) {
        int len = buf.length;
        int i = 0;
        while (i + HDR_LEN <= len) {
            int start = le32(buf, i + NAME_LEN);
            int end = le32(buf, i + NAME_LEN + 4);
            if (start < HDR_LEN || end < start || end > len) return null; // corrupt
            if (matchesAt(buf, i, name)) {
                byte[] data = new byte[end - start];
                System.arraycopy(buf, start, data, 0, data.length);
                return data;
            }
            if (end == i) return null; // no progress guard
            i = end;
        }
        return null;
    }

    private static boolean matchesAt(byte[] buf, int at, String name) {
        for (int k = 0; k < name.length(); k++) {
            if (at + k >= buf.length || buf[at + k] != (byte) name.charAt(k)) {
                return false;
            }
        }
        return true; // trailing zero padding of the name field is ignored
    }

    private static int le32(byte[] b, int off) {
        return (b[off] & 0xFF) | ((b[off + 1] & 0xFF) << 8)
                | ((b[off + 2] & 0xFF) << 16) | ((b[off + 3] & 0xFF) << 24);
    }

    private static String asString(byte[] data) {
        // Engine text is stored as raw CP1251 (the RU patch keeps game
        // strings in that codepage; EDL_RenderText converts to UTF-8
        // only at render time, and the savegame container holds the
        // unconverted bytes). Decoding here mirrors the renderer:
        // ASCII passes through, high bytes map to Cyrillic, so this
        // dialog shows exactly what the native dialogs show.
        // The writer stores no NUL terminator; strip padding anyway.
        int end = 0;
        while (end < data.length && data[end] != 0) end++;
        return new String(data, 0, end, java.nio.charset.Charset.forName("windows-1251"));
    }

    private static byte[] readFile(File f) {
        FileInputStream in = null;
        try {
            in = new FileInputStream(f);
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte[] tmp = new byte[16384];
            int n;
            while ((n = in.read(tmp)) > 0) out.write(tmp, 0, n);
            return out.toByteArray();
        } catch (IOException e) {
            return null;
        } finally {
            if (in != null) {
                try { in.close(); } catch (IOException ignored) {}
            }
        }
    }
}
