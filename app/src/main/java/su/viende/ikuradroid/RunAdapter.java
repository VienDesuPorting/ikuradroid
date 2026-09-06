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
 * 1.2.0: library grid adapter. Tiles bind a game title and its per-game
 * icon.png - readable from the install area once the game has been copied;
 * until then the placeholder is shown. Clicks are delegated to the host
 * (MainActivity installs and launches the game).
 */
public class RunAdapter extends RecyclerView.Adapter<RunAdapter.ViewHolder> {

    /** Click callback for a library tile. */
    public interface OnGameClickListener {
        void onGameClick(RunItem item);
    }

    private final ArrayList<RunItem> mDataset = new ArrayList<>();
    private final OnGameClickListener mListener;

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

    public RunAdapter(OnGameClickListener listener) {
        mListener = listener;
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
        File icon = item.getInstalledPath() == null ? null
                : new File(item.getInstalledPath(), "icon.png");
        // 1.1.0: reset on recycle so icons never bleed through tiles
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
    }

    @Override
    public int getItemCount() {
        return mDataset.size();
    }
}
