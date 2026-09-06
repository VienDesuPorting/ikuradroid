package su.viende.vile;

import java.io.File;
import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicBoolean;

import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
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

        private RunAdapter ra;
        private View mEmptyState;

        @Override
        protected void onCreate(Bundle savedInstanceState) {
                super.onCreate(savedInstanceState);

                // 1.2.0: edge-to-edge on every supported version (the Android 15
                // opt-out values-v35 was removed together with this change)
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
        // Edge-to-edge insets (Material 3, 1.2.0)
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
                                // SAF queries are slow; the scan runs off the UI thread
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
        // Game installation and launch
        // ------------------------------------------------------------------

        private void onGameClicked(final RunItem item) {
                if (item == null) {
                        return;
                }
                if (item.getSafUri() == null) {
                        // Installed-only game (e.g. the SAF grant was revoked) -
                        // the private copy always runs without any permissions
                        launchInstalled(item.getInstalledPath());
                        return;
                }
                installAndLaunch(item);
        }

        private void installAndLaunch(final RunItem item) {
                final AtomicBoolean cancelled = new AtomicBoolean(false);
                final AlertDialogHolder dialog = showInstallDialog(cancelled);

                new Thread(new Runnable() {
                        @Override
                        public void run() {
                                try {
                                        final String path = GameLibrary.install(MainActivity.this, item,
                                                        new GameLibrary.Progress() {
                                                                @Override
                                                                public void onProgress(final String fileName) {
                                                                        runOnUiThread(new Runnable() {
                                                                                @Override
                                                                                public void run() {
                                                                                        dialog.fileNameView.setText(fileName);
                                                                                }
                                                                        });
                                                                }

                                                                @Override
                                                                public boolean isCancelled() {
                                                                        return cancelled.get();
                                                                }
                                                        });
                                        item.setInstalledPath(path);
                                        runOnUiThread(new Runnable() {
                                                @Override
                                                public void run() {
                                                        dialog.holder.dismiss();
                                                        launchInstalled(path);
                                                }
                                        });
                                } catch (final Exception e) {
                                        runOnUiThread(new Runnable() {
                                                @Override
                                                public void run() {
                                                        dialog.holder.dismiss();
                                                        if (!cancelled.get()) {
                                                                Toast.makeText(MainActivity.this,
                                                                                R.string.install_failed,
                                                                                Toast.LENGTH_LONG).show();
                                                        }
                                                }
                                        });
                                }
                        }
                }).start();
        }

        /** Small aggregate: the dialog and the file-name label inside it. */
        private static class AlertDialogHolder {
                android.app.Dialog holder;
                TextView fileNameView;
        }

        private AlertDialogHolder showInstallDialog(final AtomicBoolean cancelled) {
                AlertDialogHolder result = new AlertDialogHolder();
                View content = LayoutInflater.from(this).inflate(R.layout.dialog_install, null);
                result.fileNameView = (TextView) content.findViewById(R.id.install_file);
                result.holder = new MaterialAlertDialogBuilder(this)
                                .setTitle(R.string.install_progress_title)
                                .setView(content)
                                .setNegativeButton(R.string.install_cancel, new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which) {
                                                cancelled.set(true);
                                        }
                                })
                                .create();
                result.holder.setCanceledOnTouchOutside(false);
                // Fires for the Cancel button as well as for BACK
                result.holder.setOnCancelListener(new DialogInterface.OnCancelListener() {
                        @Override
                        public void onCancel(DialogInterface dialog) {
                                cancelled.set(true);
                        }
                });
                result.holder.show();
                return result;
        }

        private void launchInstalled(String gamePath) {
                if (gamePath == null) {
                        return;
                }
                File game = new File(gamePath);
                // The engine contract (sdl_main.c): chdir(fpath + "/" + fname),
                // saves keep going to the app-private files root
                Intent intent = new Intent(this, org.libsdl.app.SDLActivity.class);
                intent.putExtra("fname", game.getName());
                intent.putExtra("fpath", game.getParent());
                startActivity(intent);
        }

        // ------------------------------------------------------------------
        // SAF folder picking
        // ------------------------------------------------------------------

        /** Launches the system folder picker (replaces the 2016 browser). */
        private void pickFolder() {
                try {
                        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
                        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION
                                        | Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION);
                        startActivityForResult(intent, REQUEST_PICK_FOLDER);
                } catch (Exception e) {
                        Toast.makeText(this, R.string.folder_picker_unavailable,
                                        Toast.LENGTH_SHORT).show();
                }
        }

        @Override
        protected void onActivityResult(int requestCode, int resultCode, Intent data) {
                super.onActivityResult(requestCode, resultCode, data);
                if (requestCode != REQUEST_PICK_FOLDER || data == null || data.getData() == null) {
                        return;
                }
                Uri treeUri = data.getData();
                // Persist the grant across reboots. Only the READ/WRITE bits
                // are persistable: since Android 11 the result intent may also
                // carry FLAG_GRANT_PREFIX_URI_PERMISSION, and passing that bit
                // to takePersistableUriPermission throws IllegalArgument-
                // Exception ("Requested flags 0x81, but only 0x3 are allowed",
                // a fatal crash right after the user taps "Allow") - so the
                // flags are masked down to the two accepted bits (1.4.0).
                int takeFlags = data.getFlags()
                                & (Intent.FLAG_GRANT_READ_URI_PERMISSION
                                                | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
                if (takeFlags != 0) {
                        try {
                                getContentResolver().takePersistableUriPermission(treeUri,
                                                takeFlags);
                        } catch (SecurityException e) {
                                // Provider refused to persist - the session grant
                                // is still valid for this process lifetime
                        }
                }
                GameLibrary.setTreeUri(this, treeUri);
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

        // 1.2.0: returning from a finished SDLActivity must rescan the
        // library (a first launch may have just installed a game copy)
        @Override
        protected void onResume() {
                super.onResume();
                rescanLibrary();
        }
}
