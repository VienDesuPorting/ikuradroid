package su.viende.ikuradroid;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.LruCache;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Locale;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

/**
 * Library grid adapter. Tiles bind a game title, its image (the cached
 * VNDB cover full-bleed when there is one, the square icon.png from the
 * real game folder centered on a neutral surface otherwise - with the
 * legacy install-area copy as a fallback and a vector placeholder until
 * then), a "brand + folder size" sub line and the engine family badge.
 * Clicks are delegated to the host (MainActivity launches the game),
 * long clicks open the per-game context menu (rename / cover / remove /
 * delete).
 */
public class RunAdapter extends RecyclerView.Adapter<RunAdapter.ViewHolder> {

    /** Long side of a decoded tile image (covers cache at the same size). */
    private static final int MAX_DECODE_DIM = 720;

    /**
     * In-memory bitmap cache: scrolling never re-decodes files and a
     * recycled tile redraws without a placeholder flash. Shared by all
     * holders, capped at 1/8 of the heap.
     */
    private static final LruCache<String, Bitmap> BITMAP_CACHE;
    static {
        int maxKb = (int) (Runtime.getRuntime().maxMemory() / 1024 / 8);
        BITMAP_CACHE = new LruCache<String, Bitmap>(maxKb) {
            @Override
            protected int sizeOf(String key, Bitmap value) {
                return value.getByteCount() / 1024;
            }
        };
    }

    /** Click callback for a library tile. */
    public interface OnGameClickListener {
        void onGameClick(RunItem item);
    }

    /** Long-click callback; {@code anchor} positions the context menu. */
    public interface OnGameLongClickListener {
        void onGameLongClick(RunItem item, View anchor);
    }

    private final ArrayList<RunItem> mDataset = new ArrayList<>();
    private final OnGameClickListener mListener;
    private final OnGameLongClickListener mLongListener;

    public static class ViewHolder extends RecyclerView.ViewHolder {
        public final TextView mTextView;
        public final TextView mSubText;
        public final TextView mChipText;
        public final ImageView mImageView;
        public RunItem mItem;

        public ViewHolder(View v) {
            super(v);
            mTextView = (TextView) v.findViewById(R.id.tv_recycler_item);
            mSubText = (TextView) v.findViewById(R.id.tv_recycler_item_sub);
            mChipText = (TextView) v.findViewById(R.id.chip_recycler_item);
            mImageView = (ImageView) v.findViewById(R.id.iv_recycler_item);
        }
    }

    public RunAdapter(OnGameClickListener listener, OnGameLongClickListener longListener) {
        mListener = listener;
        mLongListener = longListener;
    }

    public void swapArray(ArrayList<RunItem> dataset) {
        mDataset.clear();
        if (dataset != null) {
            mDataset.addAll(dataset);
        }
    }

    public RunItem getItem(int position) {
        return mDataset.get(position);
    }

    /** Re-sorts the grid by display title after a rename (A-Z). */
    public void sortByDisplayTitle() {
        Collections.sort(mDataset, new Comparator<RunItem>() {
            @Override
            public int compare(RunItem a, RunItem b) {
                return a.displayTitle().compareToIgnoreCase(b.displayTitle());
            }
        });
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.run_item, parent, false);
        return new ViewHolder(v);
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        final RunItem item = mDataset.get(position);
        holder.mTextView.setText(item.displayTitle());
        bindImage(holder, item);
        bindSubAndChip(holder, item);
        holder.mItem = item;
        holder.itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mListener != null) {
                    mListener.onGameClick(item);
                }
            }
        });
        holder.itemView.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                if (mLongListener != null) {
                    mLongListener.onGameLongClick(item, v);
                    return true;
                }
                return false;
            }
        });
    }

    /**
     * Tile image, in priority order: the cached VNDB cover (a 2:3
     * portrait, drawn full-bleed), then the game's own square icon.png
     * (fitCenter on a neutral surface, so it never crops), then the
     * vector placeholder. Files decode off the UI thread through
     * BITMAP_CACHE; the wanted path rides on the view tag, so a decode
     * finishing after a rebind (recycled tile) is dropped instead of
     * misapplied. Scale type and background reset on every bind so
     * nothing bleeds through recycled tiles.
     */
    private void bindImage(final ViewHolder holder, RunItem item) {
        final ImageView view = holder.mImageView;
        File image = null;
        boolean fullBleed = false;
        if (item.getCoverPath() != null
                        && new File(item.getCoverPath()).isFile()) {
            image = new File(item.getCoverPath());
            fullBleed = true;
        } else {
            File icon = null;
            if (item.getSourcePath() != null) {
                icon = new File(item.getSourcePath(), "icon.png");
            }
            if ((icon == null || !icon.isFile())
                            && item.getInstalledPath() != null) {
                icon = new File(item.getInstalledPath(), "icon.png");
            }
            if (icon != null && icon.isFile()) {
                image = icon;
            }
        }
        if (fullBleed) {
            view.setScaleType(ImageView.ScaleType.CENTER_CROP);
            view.setBackground(null);
        } else {
            view.setScaleType(ImageView.ScaleType.FIT_CENTER);
            view.setBackgroundResource(R.drawable.bg_tile_underlay);
        }
        if (image == null) {
            // The vector placeholder is cheap on the UI thread
            view.setTag(null);
            view.setImageResource(R.drawable.card_img);
            return;
        }
        // The worker thread captures locals: they must be final copies
        final File tileImage = image;
        final String key = tileImage.getAbsolutePath();
        Bitmap cached = BITMAP_CACHE.get(key);
        if (cached != null && !cached.isRecycled()) {
            view.setTag(key);
            view.setImageBitmap(cached);
            return;
        }
        view.setTag(key);
        view.setImageResource(R.drawable.card_img);
        new Thread(new Runnable() {
            @Override
            public void run() {
                final Bitmap bitmap = decodeTile(tileImage);
                if (bitmap == null) {
                    return;
                }
                BITMAP_CACHE.put(key, bitmap);
                view.post(new Runnable() {
                    @Override
                    public void run() {
                        if (key.equals(view.getTag())) {
                            view.setImageBitmap(bitmap);
                        }
                    }
                });
            }
        }).start();
    }

    /** Decodes a tile image downscaled for the grid; null on failure. */
    private static Bitmap decodeTile(File file) {
        BitmapFactory.Options bounds = new BitmapFactory.Options();
        bounds.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(file.getAbsolutePath(), bounds);
        int width = bounds.outWidth;
        int height = bounds.outHeight;
        if (width <= 0 || height <= 0) {
            return null;
        }
        int sample = 1;
        while (Math.max(width, height) / (sample * 2) >= MAX_DECODE_DIM) {
            sample *= 2;
        }
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inSampleSize = sample;
        return BitmapFactory.decodeFile(file.getAbsolutePath(), opts);
    }

    /** Sub line: folder size; chip: engine family badge. */
    private void bindSubAndChip(ViewHolder holder, RunItem item) {
        Context context = holder.itemView.getContext();
        String size = formatSize(item.getSizeBytes(), context);
        if (size != null) {
            holder.mSubText.setText(context.getString(R.string.tile_sub_size, size));
            holder.mSubText.setVisibility(View.VISIBLE);
        } else {
            // No size: no sub line at all (reset on recycle so stale
            // text never bleeds through tiles)
            holder.mSubText.setText(null);
            holder.mSubText.setVisibility(View.GONE);
        }
        String engine = item.getEngine();
        if (engine != null && engine.length() > 0) {
            holder.mChipText.setText(engine);
            holder.mChipText.setVisibility(View.VISIBLE);
        } else {
            // Reset on recycle so badges never bleed through tiles
            holder.mChipText.setVisibility(View.GONE);
        }
    }

    /** "312 MB"-style folder size for the sub line; null when unknown. */
    private static String formatSize(long bytes, Context context) {
        if (bytes < 0) {
            return null;
        }
        float kb = bytes / 1024f;
        float mb = kb / 1024f;
        float gb = mb / 1024f;
        Locale locale = Locale.getDefault();
        if (gb >= 1f) {
            return context.getString(R.string.size_gb, String.format(locale, "%.1f", gb));
        }
        if (mb >= 1f) {
            return context.getString(R.string.size_mb, String.format(locale, "%.0f", mb));
        }
        return context.getString(R.string.size_kb, String.format(locale, "%.0f", kb));
    }

    @Override
    public int getItemCount() {
        return mDataset.size();
    }
}
