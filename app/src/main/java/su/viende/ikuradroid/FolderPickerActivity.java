package su.viende.ikuradroid;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.appbar.MaterialToolbar;
import com.google.android.material.button.MaterialButton;

/**
 * In-app folder browser that replaces the Storage Access Framework picker.
 *
 * Lists real directories under the primary shared storage; folders
 * matching an engine signature (GameLibrary.isGameFolder) are badged.
 * The selected path is returned to MainActivity, which stores it as the
 * library root - the engine then reads game files from these real paths
 * directly.
 *
 * The toolbar home button navigates one level up; upward navigation stops
 * at the storage root. Selecting the currently browsed folder is always
 * allowed: a game folder directly, its parent library folder, anything in
 * between works (GameLibrary scans the root and one level below it).
 */
public class FolderPickerActivity extends AppCompatActivity {

    public static final String EXTRA_PATH = "picked_path";

    private static final String STATE_CURRENT_DIR = "current_dir";

    private File currentDir;
    private FolderAdapter adapter;
    private View emptyView;
    private RecyclerView list;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        WindowCompat.setDecorFitsSystemWindows(getWindow(), false);
        setContentView(R.layout.activity_folder_picker);
        applyInsets();

        MaterialToolbar toolbar = (MaterialToolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.picker_title);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        }

        list = (RecyclerView) findViewById(R.id.folder_list);
        list.setLayoutManager(new LinearLayoutManager(this));
        adapter = new FolderAdapter();
        list.setAdapter(adapter);

        emptyView = findViewById(R.id.picker_empty);
        MaterialButton selectButton = (MaterialButton) findViewById(R.id.button_select);
        selectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                selectCurrent();
            }
        });

        File restored = savedInstanceState == null ? null
                : (File) savedInstanceState.getSerializable(STATE_CURRENT_DIR);
        if (restored != null && restored.isDirectory()) {
            currentDir = restored;
        } else {
            currentDir = startDirectory();
        }
        show();
    }

    private static File startDirectory() {
        File storage = Environment.getExternalStorageDirectory();
        if (storage == null || !storage.isDirectory()) {
            storage = new File("/storage/emulated/0");
        }
        return storage;
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putSerializable(STATE_CURRENT_DIR, currentDir);
    }

    // ------------------------------------------------------------------
    // Edge-to-edge insets (Material 3)
    // ------------------------------------------------------------------

    private void applyInsets() {
        View root = findViewById(R.id.picker_root);
        View appbar = findViewById(R.id.appbar);
        if (root == null) {
            return;
        }
        ViewCompat.setOnApplyWindowInsetsListener(root, new androidx.core.view.OnApplyWindowInsetsListener() {
            @Override
            public WindowInsetsCompat onApplyWindowInsets(View v, WindowInsetsCompat windowInsets) {
                Insets bars = windowInsets.getInsets(WindowInsetsCompat.Type.systemBars());
                if (appbar != null) {
                    appbar.setPadding(0, bars.top, 0, 0);
                }
                v.setPadding(0, 0, 0, bars.bottom);
                return WindowInsetsCompat.CONSUMED;
            }
        });
    }

    // ------------------------------------------------------------------
    // Navigation
    // ------------------------------------------------------------------

    private void show() {
        if (getSupportActionBar() != null) {
            getSupportActionBar().setSubtitle(currentDir.getAbsolutePath());
        }
        adapter.rescan();
    }

    private void navigate(File dir) {
        currentDir = dir;
        show();
    }

    private void up() {
        if (currentDir.equals(startDirectory())) {
            return;
        }
        File parent = currentDir.getParentFile();
        if (parent != null && parent.canRead()) {
            navigate(parent);
        }
    }

    private void selectCurrent() {
        Intent data = new Intent();
        data.putExtra(EXTRA_PATH, currentDir.getAbsolutePath());
        setResult(RESULT_OK, data);
        finish();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            up();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    // ------------------------------------------------------------------
    // Adapter
    // ------------------------------------------------------------------

    private class FolderAdapter extends RecyclerView.Adapter<FolderAdapter.Holder> {

        private final List<File> dirs = new ArrayList<>();

        void rescan() {
            dirs.clear();
            File[] children = currentDir.listFiles();
            if (children != null) {
                for (File f : children) {
                    if (f.isDirectory() && !f.getName().startsWith(".")) {
                        dirs.add(f);
                    }
                }
                Collections.sort(dirs, new Comparator<File>() {
                    @Override
                    public int compare(File a, File b) {
                        return a.getName().compareToIgnoreCase(b.getName());
                    }
                });
            }
            boolean empty = dirs.isEmpty();
            emptyView.setVisibility(empty ? View.VISIBLE : View.GONE);
            list.setVisibility(empty ? View.GONE : View.VISIBLE);
            notifyDataSetChanged();
        }

        @NonNull
        @Override
        public Holder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            View v = LayoutInflater.from(parent.getContext())
                    .inflate(R.layout.folder_item, parent, false);
            return new Holder(v);
        }

        @Override
        public void onBindViewHolder(@NonNull Holder holder, int position) {
            final File dir = dirs.get(position);
            holder.name.setText(dir.getName());
            boolean game = GameLibrary.isGameFolder(dir);
            holder.badge.setVisibility(game ? View.VISIBLE : View.GONE);
            holder.itemView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    navigate(dir);
                }
            });
        }

        @Override
        public int getItemCount() {
            return dirs.size();
        }

        class Holder extends RecyclerView.ViewHolder {
            final TextView name;
            final TextView badge;

            Holder(View v) {
                super(v);
                name = (TextView) v.findViewById(R.id.folder_name);
                badge = (TextView) v.findViewById(R.id.folder_badge);
            }
        }
    }
}
