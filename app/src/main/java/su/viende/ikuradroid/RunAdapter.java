package su.viende.ikuradroid;

import android.net.Uri;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;
import java.util.ArrayList;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

/**
 * Library grid adapter. Tiles bind a game title and its per-game
 * icon.png - read straight from the real game folder, with the legacy
 * install-area copy as a fallback; until then the placeholder is shown.
 * Clicks are delegated to the host (MainActivity launches the game),
 * long clicks open the per-game context menu (remove/delete).
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
        public final ImageView mImageView;
        public RunItem mItem;

        public ViewHolder(View v) {
            super(v);
            mTextView = (TextView) v.findViewById(R.id.tv_recycler_item);
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
        holder.mTextView.setText(item.getTitle());
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

    @Override
    public int getItemCount() {
        return mDataset.size();
    }
}
