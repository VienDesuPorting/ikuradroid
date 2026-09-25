package su.viende.ikuradroid;

import java.io.File;
import java.util.ArrayList;

import android.Manifest;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.PopupWindow;
import android.widget.TextView;
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

                // The app-bar menu is a hand-rolled popup in the tile
                // context menu style; the stock overflow looked alien next
                // to it (and carried no icons)
                View moreButton = findViewById(R.id.menu_more);
                if (moreButton != null) {
                        moreButton.setOnClickListener(new View.OnClickListener() {
                                @Override
                                public void onClick(View v) {
                                        showToolbarMenu(v);
                                }
                        });
                }

                final RecyclerView recyclerView = (RecyclerView) findViewById(R.id.my_recycler_view);
                recyclerView.setLayoutManager(new GridLayoutManager(this, spanCountForLibrary()));
                ra = new RunAdapter(new RunAdapter.OnGameClickListener() {
                        @Override
                        public void onGameClick(RunItem item) {
                                onGameClicked(item);
                        }
                }, new RunAdapter.OnGameLongClickListener() {
                        @Override
                        public void onGameLongClick(RunItem item, View anchor) {
                                showGameContextMenu(item, anchor);
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

        /**
         * Library columns from the available width (about 160 dp per
         * tile): two on a phone, three or four on tablets and in
         * landscape - instead of a hardcoded count for every device.
         */
        private int spanCountForLibrary() {
                float density = getResources().getDisplayMetrics().density;
                float widthDp = getResources().getDisplayMetrics().widthPixels / density;
                return Math.max(2, Math.min(4, (int) (widthDp / 160f)));
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
                // Every pick adds another root; re-picking a folder also
                // puts back the titles the user removed from the list
                GameLibrary.addRootPath(this, path);
                GameLibrary.unhideUnder(this, path);
                rescanLibrary();
                offerAddAnother(path);
        }

        /** After a pick: the "add one more folder" invitation. */
        private void offerAddAnother(String path) {
                String name = new File(path).getName();
                new MaterialAlertDialogBuilder(this)
                                .setTitle(R.string.library_added_title)
                                .setMessage(getString(R.string.library_added_message, name))
                                .setPositiveButton(R.string.library_add_more,
                                                new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which) {
                                                pickFolder();
                                        }
                                })
                                .setNegativeButton(android.R.string.ok, null)
                                .show();
        }

        // ------------------------------------------------------------------
        // Long-press context menu (remove from list / delete from device)
        // ------------------------------------------------------------------

        private void showGameContextMenu(final RunItem item, View anchor) {
                if (item == null) {
                        return;
                }
                // Hand-rolled popup instead of the stock PopupMenu: the menu
                // opens BELOW the tile on a compact rounded panel
                // (game_context_popup.xml + bg_game_context.xml)
                final View content = LayoutInflater.from(this)
                                .inflate(R.layout.game_context_popup, null);
                final PopupWindow popup = new PopupWindow(content,
                                ViewGroup.LayoutParams.WRAP_CONTENT,
                                ViewGroup.LayoutParams.WRAP_CONTENT, true);
                // Transparent background keeps the rounded panel look while
                // focusable=true dismisses on an outside tap or on Back
                popup.setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));
                popup.setElevation(getResources().getDisplayMetrics().density * 8f);
                content.findViewById(R.id.ctx_hide_row).setOnClickListener(
                                new View.OnClickListener() {
                                        @Override
                                        public void onClick(View v) {
                                                popup.dismiss();
                                                hideGame(item);
                                        }
                                });
                content.findViewById(R.id.ctx_delete_row).setOnClickListener(
                                new View.OnClickListener() {
                                        @Override
                                        public void onClick(View v) {
                                                popup.dismiss();
                                                confirmDeleteGame(item);
                                        }
                                });
                // Clip the row ripples to the rounded outline
                content.setClipToOutline(true);

                // Anchor the panel to the tile, never to the screen edge.
                // showAsDropDown lets the window manager clamp the popup
                // into the (edge-to-edge) display frame, which parks it on
                // the navbar whenever the tile sits on the last grid row.
                // The placement is computed against the safe area instead:
                // right below the card when it fits there, otherwise
                // flipped above the card - attached to the tile either way.
                content.measure(View.MeasureSpec.UNSPECIFIED,
                                View.MeasureSpec.UNSPECIFIED);
                final int popupWidth = content.getMeasuredWidth();
                final int popupHeight = content.getMeasuredHeight();
                final int[] anchorPos = new int[2];
                anchor.getLocationOnScreen(anchorPos);
                final int anchorLeft = anchorPos[0];
                final int anchorTop = anchorPos[1];
                final int anchorBottom = anchorPos[1] + anchor.getHeight();
                // The root view is padded by the system-bar insets
                // (applyInsets), so its padded bounds are the safe area:
                // the panel must stay inside them, clear of the navbar.
                final View root = findViewById(R.id.main_root);
                int safeLeft = anchorLeft;
                int safeTop = anchorTop;
                int safeRight = anchorLeft + anchor.getWidth();
                int safeBottom = anchorBottom;
                if (root != null) {
                        final int[] rootPos = new int[2];
                        root.getLocationOnScreen(rootPos);
                        safeLeft = rootPos[0];
                        safeTop = rootPos[1];
                        safeRight = rootPos[0] + root.getWidth();
                        safeBottom = rootPos[1] + root.getHeight()
                                        - root.getPaddingBottom();
                }
                int x = anchorLeft;
                if (x + popupWidth > safeRight) {
                        // Last column: keep the panel inside the safe frame
                        x = safeRight - popupWidth;
                }
                if (x < safeLeft) {
                        x = safeLeft;
                }
                int y;
                if (anchorBottom + popupHeight <= safeBottom) {
                        // The designed spot: right below the card
                        y = anchorBottom;
                } else {
                        // No room under the tile (last row): flip above
                        // the card, still attached to it
                        y = anchorTop - popupHeight;
                        if (y < safeTop) {
                                y = safeTop;
                        }
                }
                popup.showAtLocation(anchor, Gravity.NO_GRAVITY, x, y);
        }

        // ------------------------------------------------------------------
        // About modal: the same rounded-panel look as the tile context
        // menu (about_popup.xml + bg_game_context.xml), centered on a dim
        // backdrop. Tap outside or Back dismisses; the panel itself
        // consumes its own taps so they do not fall through to the dim.
        // ------------------------------------------------------------------
        private void showAboutPopup() {
                final View content = LayoutInflater.from(this)
                                .inflate(R.layout.about_popup, null);
                final PopupWindow popup = new PopupWindow(content,
                                ViewGroup.LayoutParams.MATCH_PARENT,
                                ViewGroup.LayoutParams.MATCH_PARENT, true);
                // Transparent background keeps the rounded panel look while
                // focusable=true dismisses on an outside tap or on Back
                popup.setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

                TextView version = (TextView) content.findViewById(R.id.about_version);
                String verName;
                try {
                        verName = getPackageManager()
                                        .getPackageInfo(getPackageName(), 0).versionName;
                } catch (Exception e) {
                        verName = "?";
                }
                version.setText(getString(R.string.app_name) + " " + verName);

                // Keep the panel inside the window on short screens: the
                // scroll view absorbs the overflow once the measured panel
                // would cover more than 82% of the popup height.
                final View panel = content.findViewById(R.id.about_panel);
                final View scroll = content.findViewById(R.id.about_scroll);
                content.post(new Runnable() {
                        @Override
                        public void run() {
                                int maxH = (int) (content.getHeight() * 0.82f);
                                if (panel.getHeight() > maxH && scroll.getHeight() > 0) {
                                        scroll.getLayoutParams().height = Math.max(0,
                                                        maxH - (panel.getHeight()
                                                                        - scroll.getHeight()));
                                        scroll.requestLayout();
                                }
                        }
                });

                content.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                                popup.dismiss();
                        }
                });
                panel.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                                // Consume: panel taps must not reach the dim backdrop
                        }
                });
                popup.showAtLocation(findViewById(R.id.main_root),
                                Gravity.CENTER, 0, 0);
        }

        /** "Remove from list": files stay; re-adding the folder restores it. */
        private void hideGame(RunItem item) {
                GameLibrary.setHidden(this, item.getTitle(), true);
                rescanLibrary();
                Toast.makeText(this, R.string.ctx_hidden_toast, Toast.LENGTH_SHORT).show();
        }

        /** "Delete from device": destructive - ask before wiping the files. */
        private void confirmDeleteGame(final RunItem item) {
                new MaterialAlertDialogBuilder(this)
                                .setTitle(R.string.ctx_delete_title)
                                .setMessage(getString(R.string.ctx_delete_message, item.getTitle()))
                                .setPositiveButton(R.string.ctx_delete_confirm,
                                                new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which) {
                                                deleteGame(item);
                                        }
                                })
                                .setNegativeButton(android.R.string.cancel, null)
                                .show();
        }

        private void deleteGame(final RunItem item) {
                ensureStorageAccess(new Runnable() {
                        @Override
                        public void run() {
                                // Storage I/O stays off the UI thread; the scan
                                // afterwards rebuilds the visible list
                                new Thread(new Runnable() {
                                        @Override
                                        public void run() {
                                                final boolean deleted = deleteGameFiles(item);
                                                runOnUiThread(new Runnable() {
                                                        @Override
                                                        public void run() {
                                                                Toast.makeText(MainActivity.this,
                                                                                deleted ? R.string.ctx_deleted_toast
                                                                                                : R.string.ctx_delete_failed_toast,
                                                                                Toast.LENGTH_LONG).show();
                                                                rescanLibrary();
                                                        }
                                                });
                                        }
                                }).start();
                        }
                });
        }

        /** Deletes both copies of the game: the fs folder and the legacy copy. */
        private static boolean deleteGameFiles(RunItem item) {
                boolean ok = true;
                String source = item.getSourcePath();
                if (source != null) {
                        ok &= GameLibrary.deleteRecursively(new File(source));
                }
                String installed = item.getInstalledPath();
                if (installed != null) {
                        ok &= GameLibrary.deleteRecursively(new File(installed));
                }
                return ok;
        }

        // ------------------------------------------------------------------
        // Toolbar menu: the same compact rounded panel as the tile context
        // menu (menu_toolbar_popup.xml + bg_game_context.xml), anchored
        // below the app bar and right-aligned to the "more" button
        // ------------------------------------------------------------------

        private void showToolbarMenu(View anchor) {
                final View content = LayoutInflater.from(this)
                                .inflate(R.layout.menu_toolbar_popup, null);
                final PopupWindow popup = new PopupWindow(content,
                                ViewGroup.LayoutParams.WRAP_CONTENT,
                                ViewGroup.LayoutParams.WRAP_CONTENT, true);
                // Transparent background keeps the rounded panel look while
                // focusable=true dismisses on an outside tap or on Back
                popup.setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));
                popup.setElevation(getResources().getDisplayMetrics().density * 8f);

                content.findViewById(R.id.menu_engines_row).setOnClickListener(
                                new View.OnClickListener() {
                                        @Override
                                        public void onClick(View v) {
                                                popup.dismiss();
                                                startActivity(new Intent(MainActivity.this,
                                                                EnginesActivity.class));
                                        }
                                });
                content.findViewById(R.id.menu_folder_row).setOnClickListener(
                                new View.OnClickListener() {
                                        @Override
                                        public void onClick(View v) {
                                                popup.dismiss();
                                                pickFolder();
                                        }
                                });
                content.findViewById(R.id.menu_about_row).setOnClickListener(
                                new View.OnClickListener() {
                                        @Override
                                        public void onClick(View v) {
                                                popup.dismiss();
                                                showAboutPopup();
                                        }
                                });
                // Clip the row ripples to the rounded outline
                content.setClipToOutline(true);

                // Right-align the panel under the anchor, clamped into the
                // safe area (the root view is padded by the system-bar
                // insets, so its padded bounds are the safe frame)
                content.measure(View.MeasureSpec.UNSPECIFIED,
                                View.MeasureSpec.UNSPECIFIED);
                final int popupWidth = content.getMeasuredWidth();
                final int popupHeight = content.getMeasuredHeight();
                final int[] anchorPos = new int[2];
                anchor.getLocationOnScreen(anchorPos);
                final int anchorRight = anchorPos[0] + anchor.getWidth();
                final int anchorBottom = anchorPos[1] + anchor.getHeight();
                final View root = findViewById(R.id.main_root);
                int safeLeft = anchorRight - popupWidth;
                int safeRight = anchorRight;
                int safeBottom = anchorBottom + popupHeight;
                if (root != null) {
                        final int[] rootPos = new int[2];
                        root.getLocationOnScreen(rootPos);
                        safeLeft = rootPos[0];
                        safeRight = rootPos[0] + root.getWidth();
                        safeBottom = rootPos[1] + root.getHeight()
                                        - root.getPaddingBottom();
                }
                int x = anchorRight - popupWidth;
                if (x < safeLeft) {
                        x = safeLeft;
                }
                if (x + popupWidth > safeRight) {
                        x = safeRight - popupWidth;
                }
                int y = anchorBottom;
                if (y + popupHeight > safeBottom) {
                        // No room below the app bar: flip above it
                        y = anchorPos[1] - popupHeight;
                }
                popup.showAtLocation(anchor, Gravity.NO_GRAVITY, x, y);
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
