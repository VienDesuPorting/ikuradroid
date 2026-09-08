package su.viende.ikuradroid;

import java.io.File;
import java.util.ArrayList;

import android.Manifest;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.appbar.MaterialToolbar;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;

public class MainActivity extends AppCompatActivity
{
        private static final int REQUEST_PICK_FOLDER = 42;
        private static final int REQUEST_STORAGE_ACCESS = 43;

        private RunAdapter ra;
        private View mEmptyState;
        // Action to rerun once storage access has been granted
        private Runnable mPendingAfterGrant;

        @Override
        protected void onCreate(Bundle savedInstanceState) {
                super.onCreate(savedInstanceState);

                // Edge-to-edge on every supported version.
                WindowCompat.setDecorFitsSystemWindows(getWindow(), false);
                setContentView(R.layout.main);
                applyInsets();

                MaterialToolbar toolbar = (MaterialToolbar) findViewById(R.id.toolbar);
                setSupportActionBar(toolbar);
                if (getSupportActionBar() != null) {
                        getSupportActionBar().setTitle(R.string.app_name);
                        // Branding of the 2026 revival; the original visual_android VK
                        // public is an independent project (see README / Provenance).
                        getSupportActionBar().setSubtitle("VienDesu! Porting Team");
                }

                final RecyclerView recyclerView = (RecyclerView) findViewById(R.id.my_recycler_view);
                recyclerView.setLayoutManager(new GridLayoutManager(this, 3));
                ra = new RunAdapter(new RunAdapter.OnGameClickListener() {
                        @Override
                        public void onGameClick(RunItem item) {
                                onGameClicked(item);
                        }
                });
                recyclerView.setAdapter(ra);

                mEmptyState = findViewById(R.id.empty_state);
                View pickButton = findViewById(R.id.button_pick_folder);
                if (pickButton != null) {
                        pickButton.setOnClickListener(new View.OnClickListener() {
                                @Override
                                public void onClick(View v) {
                                        pickFolder();
                                }
                        });
                }
        }

        // ------------------------------------------------------------------
        // Edge-to-edge insets (Material 3)
        // ------------------------------------------------------------------

        private void applyInsets() {
                View root = findViewById(R.id.main_root);
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
        // Library scanning
        // ------------------------------------------------------------------

        private void rescanLibrary() {
                new Thread(new Runnable() {
                        @Override
                        public void run() {
                                // Storage scans are slow; the scan runs off the UI thread
                                final ArrayList<RunItem> dataset = GameLibrary.scan(MainActivity.this);
                                runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                                if (ra == null) {
                                                        return;
                                                }
                                                ra.swapArray(dataset);
                                                ra.notifyDataSetChanged();
                                                updateEmptyState();
                                        }
                                });
                        }
                }).start();
        }

        private void updateEmptyState() {
                if (mEmptyState == null || ra == null) {
                        return;
                }
                boolean empty = ra.getItemCount() == 0;
                mEmptyState.setVisibility(empty ? View.VISIBLE : View.GONE);
                findViewById(R.id.my_recycler_view).setVisibility(empty ? View.GONE : View.VISIBLE);
        }

        // ------------------------------------------------------------------
        // Game launch
        // ------------------------------------------------------------------

        private void onGameClicked(final RunItem item) {
                if (item == null) {
                        return;
                }
                // Real folder first; the legacy app-private copy as a fallback
                String path = item.getSourcePath() != null ? item.getSourcePath() : item.getInstalledPath();
                if (path == null) {
                        return;
                }
                launchGame(path);
        }

        private void launchGame(String gamePath) {
                File game = new File(gamePath);
                if (!game.isDirectory()) {
                        Toast.makeText(this, R.string.game_gone, Toast.LENGTH_LONG).show();
                        return;
                }
                GameFontInstaller.ensureFont(this, game.getAbsolutePath());
                // The engine contract (sdl_main.c): chdir(fpath + "/" + fname),
                // saves keep going to the app-private files root
                Intent intent = new Intent(this, org.libsdl.app.SDLActivity.class);
                intent.putExtra("fname", game.getName());
                intent.putExtra("fpath", game.getParent());
                startActivity(intent);
        }

        // ------------------------------------------------------------------
        // Storage access (real paths instead of SAF)
        // ------------------------------------------------------------------

        private boolean hasStorageAccess() {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                        return Environment.isExternalStorageManager();
                }
                return ContextCompat.checkSelfPermission(this,
                                Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED
                                && ContextCompat.checkSelfPermission(this,
                                Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
        }

        /** Runs the action now, or after the user grants storage access. */
        private void ensureStorageAccess(Runnable action) {
                if (hasStorageAccess()) {
                        action.run();
                        return;
                }
                mPendingAfterGrant = action;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                        new MaterialAlertDialogBuilder(this)
                                        .setTitle(R.string.storage_access_title)
                                        .setMessage(R.string.storage_access_message)
                                        .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                                                @Override
                                                public void onClick(DialogInterface dialog, int which) {
                                                        requestAllFilesAccess();
                                                }
                                        })
                                        .setNegativeButton(android.R.string.cancel, null)
                                        .show();
                } else {
                        ActivityCompat.requestPermissions(this, new String[] {
                                        Manifest.permission.READ_EXTERNAL_STORAGE,
                                        Manifest.permission.WRITE_EXTERNAL_STORAGE },
                                        REQUEST_STORAGE_ACCESS);
                }
        }

        private void requestAllFilesAccess() {
                try {
                        Intent intent = new Intent(
                                        Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION,
                                        Uri.parse("package:" + getPackageName()));
                        startActivity(intent);
                        return;
                } catch (Exception e) {
                        // Some builds do not resolve the app-specific screen -
                        // fall back to the generic all-files-access list
                }
                try {
                        startActivity(new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION));
                } catch (Exception e) {
                        Toast.makeText(this, R.string.storage_settings_unavailable,
                                        Toast.LENGTH_LONG).show();
                }
        }

        @Override
        public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
                super.onRequestPermissionsResult(requestCode, permissions, grantResults);
                if (requestCode != REQUEST_STORAGE_ACCESS) {
                        return;
                }
                boolean granted = grantResults.length > 0;
                for (int result : grantResults) {
                        granted &= result == PackageManager.PERMISSION_GRANTED;
                }
                if (granted && mPendingAfterGrant != null) {
                        Runnable action = mPendingAfterGrant;
                        mPendingAfterGrant = null;
                        action.run();
                } else if (!granted) {
                        Toast.makeText(this, R.string.storage_access_denied, Toast.LENGTH_LONG).show();
                }
        }

        // ------------------------------------------------------------------
        // Folder picking
        // ------------------------------------------------------------------

        private void pickFolder() {
                ensureStorageAccess(new Runnable() {
                        @Override
                        public void run() {
                                startActivityForResult(
                                                new Intent(MainActivity.this, FolderPickerActivity.class),
                                                REQUEST_PICK_FOLDER);
                        }
                });
        }

        @Override
        protected void onActivityResult(int requestCode, int resultCode, Intent data) {
                super.onActivityResult(requestCode, resultCode, data);
                String path = data == null ? null : data.getStringExtra(FolderPickerActivity.EXTRA_PATH);
                if (requestCode != REQUEST_PICK_FOLDER || path == null) {
                        return;
                }
                GameLibrary.setRootPath(this, path);
                rescanLibrary();
        }

        // ------------------------------------------------------------------
        // Menu
        // ------------------------------------------------------------------

        @Override
        public boolean onCreateOptionsMenu(Menu menu) {
                getMenuInflater().inflate(R.menu.main, menu);
                return super.onCreateOptionsMenu(menu);
        }

        @Override
        public boolean onOptionsItemSelected(MenuItem item) {
                int id = item.getItemId();
                if (id == R.id.menu_engines) {
                        startActivity(new Intent(this, EnginesActivity.class));
                        return true;
                }
                if (id == R.id.menu_about) {
                        startActivity(new Intent(this, AboutActivity.class));
                        return true;
                }
                if (id == R.id.dir_change) {
                        pickFolder();
                        return true;
                }
                return super.onOptionsItemSelected(item);
        }

        // Returning from the storage settings screen or from a finished
        // SDLActivity must rerun the pending action and rescan the library
        @Override
        protected void onResume() {
                super.onResume();
                if (mPendingAfterGrant != null && hasStorageAccess()) {
                        Runnable action = mPendingAfterGrant;
                        mPendingAfterGrant = null;
                        action.run();
                }
                rescanLibrary();
        }
}
