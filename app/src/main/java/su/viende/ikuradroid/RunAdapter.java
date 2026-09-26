package su.viende.ikuradroid;

import android.content.Context;
import android.net.Uri;
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
 * Library grid adapter. Tiles bind a game title, its square icon (read
 * straight from the real game folder's icon.png, with the legacy
 * install-area copy as a fallback and a vector placeholder until then),
 * a "brand + folder size" sub line and the engine family badge. Clicks
 * are delegated to the host (MainActivity launches the game), long
 * clicks open the per-game context menu (remove/delete).
 */
public class RunAdapter extends RecyclerView.Adapter<RunAdapter.ViewHolder> {

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
        File icon = null;
        if (item.getSourcePath() != null) {
            icon = new File(item.getSourcePath(), "icon.png");
        }
        if ((icon == null || !icon.isFile()) && item.getInstalledPath() != null) {
            icon = new File(item.getInstalledPath(), "icon.png");
        }
        // Reset on recycle so icons never bleed through tiles
        if (icon != null && icon.isFile()) {
            holder.mImageView.setImageURI(Uri.fromFile(icon));
        } else {
            holder.mImageView.setImageResource(R.drawable.card_img);
        }
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

    /** Sub line: engine brand plus folder size; chip: engine family badge. */
    private void bindSubAndChip(ViewHolder holder, RunItem item) {
        Context context = holder.itemView.getContext();
        String size = formatSize(item.getSizeBytes(), context);
        if (size != null) {
            holder.mSubText.setText(context.getString(R.string.tile_sub_size, size));
        } else {
            holder.mSubText.setText(R.string.tile_sub_plain);
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
