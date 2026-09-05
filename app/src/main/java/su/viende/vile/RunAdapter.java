package su.viende.vile;

import android.content.Intent;
import android.net.Uri;
import android.os.Environment;
import androidx.recyclerview.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
 
import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;

import org.libsdl.app.SDLActivity;
 
 
public class RunAdapter extends RecyclerView.Adapter<RunAdapter.ViewHolder> {
 
    private ArrayList<RunItem> mDataset;
 
    public static class ViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener{
        public TextView mTextView;
        public ImageView mImageView;
        public RunItem mFeedItem;
        public ViewHolder(View v) {
            super(v);            
            v.setOnClickListener(this);
            mTextView = (TextView) v.findViewById(R.id.tv_recycler_item);
            mImageView = (ImageView) v.findViewById(R.id.iv_recycler_item);
        }
        @Override
        public void onClick(View v) {
            int position = getPosition();
                        // 0.54.4: the engine fopen()s its font inside the game
                        // folder (see GameFontInstaller) - ship it there first,
                        // otherwise a fresh game renders no text at all.
                        GameFontInstaller.ensureFont(v.getContext(),
                                        mFeedItem.getPath() + "/" + mFeedItem.getTitle());
                        Intent intent = new Intent(v.getContext(), SDLActivity.class);
                        intent.putExtra("fname",  mFeedItem.getTitle());
                        intent.putExtra("fpath",  mFeedItem.getPath());
                        v.getContext().startActivity(intent);
        }
    }
 
    public RunAdapter(ArrayList<RunItem> dataset) {
        mDataset = dataset;        
    }
    
    public void swapArray(ArrayList<RunItem> dataset)  {
        mDataset = dataset;   
    }
    
    @Override
    public RunAdapter.ViewHolder onCreateViewHolder(ViewGroup parent,
                                                   int viewType) {
        View v = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.run_item, parent, false);
 
        ViewHolder vh = new ViewHolder(v);
        return vh;
    }
 
    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        String fName = mDataset.get(position).getTitle();
        String fPath = mDataset.get(position).getPath();
        String iPath = fPath + "/"+ fName+ "/icon.png";
        File file = new File(iPath);
        // 1.1.0: reset to the placeholder when a recycled tile loses its
        // per-game icon.png (prevents icon bleed-through in the grid)
        if (file.exists())
                holder.mImageView.setImageURI( Uri.fromFile( new File( iPath ) ) );
        else
                holder.mImageView.setImageResource(R.drawable.card_img);
        holder.mFeedItem = mDataset.get(position);
        holder.mTextView.setText(fName);
    }
 
    @Override
    public int getItemCount() {
        return mDataset.size();
    }
    
}