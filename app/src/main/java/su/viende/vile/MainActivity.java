package su.viende.vile;

import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;

import android.Manifest;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentPagerAdapter;
import androidx.fragment.app.FragmentTransaction;
import androidx.viewpager.widget.ViewPager;

public class MainActivity extends AppCompatActivity
{
        private SharedPreferences mPrefs = null;
        public static String SETTINGS_FOLDER_DEFAULT_KEY = null;
        private static final String LAST_DIRECTORY = "last_directory_key";
        private static File DEFAULT_LOCATION;
        public String currentPath;
        private static RunAdapter ra;
        private TextView mHint;

        @Override
        protected void onCreate(Bundle savedInstanceState) {
                super.onCreate(savedInstanceState);

                requestAllFilesAccessIfNeeded();

                mPrefs = PreferenceManager.getDefaultSharedPreferences(this);
                SETTINGS_FOLDER_DEFAULT_KEY = getString(R.string.settings_folder_default_key);
                DEFAULT_LOCATION = Environment2.getExternalSDCardDirectory() != null ?
                                Environment2.getExternalSDCardDirectory() : Environment.getExternalStorageDirectory();
                File directory = getStartingDirectory();
                if (directory != null)
                        currentPath = directory.getAbsolutePath();

                setContentView(R.layout.main);
                mHint = (TextView) findViewById(R.id.library_hint);
                mSaveDirBrowse = new FolderBrowserDialogWrapper(this, true, true);
                createDirectoryBrowserDialog();
                final ActionBar actionBar = getSupportActionBar();
                // actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);
                actionBar.setTitle(R.string.app_name);
                // Branding of the 2026 revival; the original visual_android VK
                // public is an independent project (see README / Provenance).
                actionBar.setSubtitle("VienDesu! Porting Team");
                actionBar.setDisplayShowHomeEnabled(true);

                final RecyclerView recyclerView = findViewById(R.id.my_recycler_view);

                GridLayoutManager layoutManager = new GridLayoutManager(this, 3);
                recyclerView.setLayoutManager(layoutManager);

                new Thread(new Runnable() {
                        @Override
                        public void run() {
                                // final RunAdapter adapter = new RunAdapter(getDataSet());
                                ra = new RunAdapter(getDataSet());
                                runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                                recyclerView.setAdapter(ra);
                                                updateHint();
                                        }
                                });
                        }
                }).start();
        }

        // 0.54.2: returning from the All-Files-Access settings screen (or from a
        // finished SDLActivity) must rescan the library, otherwise the grid stays
        // empty until the process is killed.
        @Override
        protected void onResume() {
                super.onResume();
                if (ra != null) {
                        ra.swapArray(getDataSet());
                        runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                        ra.notifyDataSetChanged();
                                        updateHint();
                                }
                        });
                }
        }

        // 0.54.2: the grid shows one tile per game. A game is a folder with a
        // vilevn.pck inside. Two layouts are accepted:
        //   <selected>/Crescendo/vilevn.pck  (library root selected - classic flow)
        //   <selected>/vilevn.pck            (game folder selected directly)
        // The selected folder itself is scanned as a game too, which fixes the
        // "empty folder" report when the novel sits in the storage root.
        private ArrayList<RunItem> getDataSet() {
                ArrayList<RunItem> mDataSet = new ArrayList<>();
                File mCurrentDirectory = getStartingDirectory();
                if (mCurrentDirectory == null)
                        return mDataSet;
                currentPath = mCurrentDirectory.getAbsolutePath();

                // The selected folder itself may be a game folder
                if (new File(currentPath, "vilevn.pck").isFile()) {
                        RunItem m = new RunItem();
                        m.setTitle(mCurrentDirectory.getName());
                        m.setPath(mCurrentDirectory.getParent());
                        mDataSet.add(m);
                }

                File[] mDirectoryFiles = mCurrentDirectory.listFiles(new FileFilter() {
                        public boolean accept(File file) {
                                return (!file.isHidden() && file.isDirectory());
                        }
                });

                // listFiles() returns null when the All-Files-Access grant is
                // missing on Android 11+; never crash on it (0.54.1 bug)
                if (mDirectoryFiles == null)
                        return mDataSet;

                for (int i = 0; i < mDirectoryFiles.length; i++) {
                        File[] mDirectoryFiles2 = mDirectoryFiles[i].listFiles(new FileFilter() {
                                public boolean accept(File file) {
                                        return (file.isFile() && file.getName().equals("vilevn.pck"));
                                }
                        });

                        if (mDirectoryFiles2 != null && mDirectoryFiles2.length != 0){
                                RunItem m = new RunItem();
                                m.setTitle(mDirectoryFiles[i].getName());
                                m.setPath(mDirectoryFiles[i].getParent());
                                mDataSet.add(m);
                        }
                }

                return mDataSet;
        }

        private void updateHint() {
                if (mHint == null)
                        return;
                boolean empty = (ra == null || ra.getItemCount() == 0);
                boolean granted = true;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R)
                        granted = Environment.isExternalStorageManager();
                // Show the long troubleshooting hint only when storage is readable,
                // otherwise the permission dialog explains what to do
                mHint.setText(empty ? R.string.empty_library_hint : R.string.library_hint);
                mHint.setVisibility(empty && granted ? View.VISIBLE : View.GONE);
        }

        private File getStartingDirectory() {
                // Detect folder location if none is provided
                String path = mPrefs.getString(SETTINGS_FOLDER_DEFAULT_KEY, null);

                if (path == null) {
                        // Check to see if there is a <internal storage>/ons
                        File onsDefaultDir = new File(Environment.getExternalStorageDirectory() + "/ons");
                        if (onsDefaultDir.exists()) {
                                path = onsDefaultDir.getPath();
                                mPrefs.edit().putString(SETTINGS_FOLDER_DEFAULT_KEY, path).apply();
                        } else if (Environment2.hasExternalSDCard()) {
                                // Check to see if there is a <extSdCard storage>/ons
                                onsDefaultDir = new File(Environment2.getExternalSDCardDirectory() + "/ons");
                                if (onsDefaultDir.exists()) {
                                        path = onsDefaultDir.getPath();
                                        mPrefs.edit().putString(SETTINGS_FOLDER_DEFAULT_KEY, path).apply();
                                }
                        }
                }
                // Set path unless it still can't find it, then set default folder
                File directory = null;
                if (path != null && new File(path).exists()) {
                        directory = new File(path);
                } else {
                        directory = DEFAULT_LOCATION;
                        if (directory != null)
                                mPrefs.edit().putString(SETTINGS_FOLDER_DEFAULT_KEY, DEFAULT_LOCATION.getPath()).apply();
                }
                if (directory == null || !directory.exists()) {
                        // showError(getString(R.string.message_cannot_find_internal_storage));
                        return null;
                }
                return directory;
        }

        // Launcher contributed by katane-san
        @Override
        public boolean onCreateOptionsMenu(Menu menu) {
                // Inflate the menu; this adds items to the action bar if it is present.
                getMenuInflater().inflate(R.menu.main, menu);
                return super.onCreateOptionsMenu(menu);
        }

        @Override
        public boolean onOptionsItemSelected(MenuItem item) {
                int id = item.getItemId();
                if (id == R.id.menu_about) {
                        showAboutDialog();
                        return true;
                }

                if (id == R.id.dir_change) {
                        mSaveDirBrowse.show(currentPath);
                        return true;
                }

                return super.onOptionsItemSelected(item);
        }

        private void createDirectoryBrowserDialog() {
                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                builder.setView(mSaveDirBrowse.getDialogLayout());
                // 0.54.2: was a hardcoded "test"
                builder.setTitle(R.string.dialog_select_folder_title);
                builder.setPositiveButton(R.string.dialog_select_button_text, new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                                Editor editor = mPrefs.edit();
                                String path = mSaveDirBrowse.getResultDirectory().getPath();
                                editor.putString(SETTINGS_FOLDER_DEFAULT_KEY, path);
                                editor.apply();
                                setPath(path);
                                ra.swapArray(getDataSet());
                                ra.notifyDataSetChanged();
                                updateHint();
                        }
                });
                builder.setNegativeButton(android.R.string.cancel, null);
                mSaveDirBrowse.setDialog(builder.create());
        }

        // 1.1.0: attribution dialog - the ViLE engine (ViLE Team, GPLv3), the
        // original anonymous Android developer ("Ivan"), the revival team
        // (VienDesu! Porting Team) and third-party components. Replaces the
        // bare version toast of the 2016 launcher.
        private void showAboutDialog() {
                View v = getLayoutInflater().inflate(R.layout.about_dialog, null);
                TextView version = (TextView) v.findViewById(R.id.about_version);
                String verName;
                try {
                        verName = getPackageManager()
                                        .getPackageInfo(getPackageName(), 0).versionName;
                } catch (Exception e) {
                        verName = "?";
                }
                version.setText(getString(R.string.app_name) + " " + verName);
                new AlertDialog.Builder(this)
                                .setView(v)
                                .setPositiveButton(android.R.string.ok, null)
                                .show();
        }

        protected void setPath(String path) {
                currentPath = path;
        }

        private FolderBrowserDialogWrapper mSaveDirBrowse = null;

        // Android 11+ (API 30+): the 2016-era browser uses java.io.File on shared
        // storage, which requires the MANAGE_EXTERNAL_STORAGE (All-Files-Access) grant.
        // 0.54.2: explain why before jumping into settings; the previous build
        // silently launched the settings screen on every start, which looked
        // like the app "forgetting" the user.
        private void requestAllFilesAccessIfNeeded() {
                if (Build.VERSION.SDK_INT < Build.VERSION_CODES.R
                                || Environment.isExternalStorageManager())
                        return;
                try {
                        new AlertDialog.Builder(this)
                                .setTitle(R.string.storage_access_title)
                                .setMessage(R.string.storage_access_message)
                                .setPositiveButton(R.string.storage_access_grant, new OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which) {
                                                launchAllFilesAccessSettings();
                                        }
                                })
                                .setNegativeButton(android.R.string.cancel, null)
                                .show();
                } catch (Exception e) {
                        // Theme/window issues at onCreate time - fall back to direct launch
                        launchAllFilesAccessSettings();
                }
        }

        private void launchAllFilesAccessSettings() {
                try {
                        android.content.Intent intent =
                                        new android.content.Intent(
                                                        android.provider.Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION,
                                                        Uri.parse("package:" + getPackageName()));
                        startActivity(intent);
                } catch (Exception e) {
                        try {
                                startActivity(new android.content.Intent(
                                                android.provider.Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION));
                        } catch (Exception ignored) {
                        }
                }
        }
}
