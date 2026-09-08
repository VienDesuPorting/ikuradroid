package su.viende.ikuradroid;

import android.content.Context;
import android.content.SharedPreferences;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

/**
 * File-system game library.
 *
 * The user picks the library root once with the in-app folder browser
 * (FolderPickerActivity) after granting storage access; the absolute path
 * is persisted. The engine reads files with plain stdio, so game folders
 * are passed to it as real paths - nothing is copied.
 *
 * Games are discovered in the root folder itself and one level below it.
 * Legacy copies from the SAF era (getExternalFilesDir(null)/games/<title>/)
 * keep working and are merged into the same list; saves always live in the
 * app-private area.
 */
public final class GameLibrary {

    public static final String PCK_MARKER = "vilevn.pck";

    private static final String PREFS_FILE = "library";
    private static final String KEY_ROOT_PATH = "library_root";
    private static final String INSTALL_DIR = "games";

    private GameLibrary() {
    }

    // ------------------------------------------------------------------
    // Persisted library root
    // ------------------------------------------------------------------

    public static String getRootPath(Context context) {
        SharedPreferences prefs = context.getSharedPreferences(PREFS_FILE, Context.MODE_PRIVATE);
        return prefs.getString(KEY_ROOT_PATH, null);
    }

    public static void setRootPath(Context context, String rootPath) {
        SharedPreferences prefs = context.getSharedPreferences(PREFS_FILE, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = prefs.edit();
        if (rootPath == null) {
            editor.remove(KEY_ROOT_PATH);
        } else {
            editor.putString(KEY_ROOT_PATH, rootPath);
        }
        editor.apply();
    }

    /** Root of the legacy install area (game copies from the SAF era). */
    public static File installRoot(Context context) {
        File base = context.getExternalFilesDir(null);
        if (base == null) {
            base = context.getFilesDir();
        }
        return new File(base, INSTALL_DIR);
    }

    // ------------------------------------------------------------------
    // Scanning
    // ------------------------------------------------------------------

    /**
     * Builds the library list: games under the persisted root folder plus
     * the legacy copies in the install area (merged by title, so the
     * library still works when the root is missing or unreadable). Never
     * returns null.
     */
    public static ArrayList<RunItem> scan(Context context) {
        ArrayList<RunItem> items = new ArrayList<>();

        String rootPath = getRootPath(context);
        if (rootPath != null) {
            File root = new File(rootPath);
            if (root.isDirectory()) {
                // The root folder itself may be a game folder
                if (hasPck(root)) {
                    addFsGame(items, root);
                }
                File[] children = root.listFiles();
                if (children != null) {
                    for (File dir : children) {
                        if (dir.isDirectory() && hasPck(dir)) {
                            addFsGame(items, dir);
                        }
                    }
                }
            } else {
                // The folder is gone (unmounted, deleted) - drop it so the
                // empty state asks the user to pick the folder again
                setRootPath(context, null);
            }
        }

        // Merge the legacy install area - these always work, no permission
        // is needed for the app-private files
        File[] installed = installRoot(context).listFiles();
        if (installed != null) {
            for (File dir : installed) {
                if (!dir.isDirectory() || !new File(dir, PCK_MARKER).isFile()) {
                    continue;
                }
                RunItem known = findByTitle(items, dir.getName());
                if (known != null) {
                    known.setInstalledPath(dir.getAbsolutePath());
                } else {
                    RunItem item = new RunItem();
                    item.setTitle(dir.getName());
                    item.setInstalledPath(dir.getAbsolutePath());
                    items.add(item);
                }
            }
        }

        Collections.sort(items, new Comparator<RunItem>() {
            @Override
            public int compare(RunItem a, RunItem b) {
                return a.getTitle().compareToIgnoreCase(b.getTitle());
            }
        });
        return items;
    }

    private static void addFsGame(ArrayList<RunItem> items, File dir) {
        String name = dir.getName();
        if (name == null || name.length() == 0) {
            return;
        }
        RunItem known = findByTitle(items, name);
        if (known != null) {
            return;
        }
        RunItem item = new RunItem();
        item.setTitle(name);
        item.setSourcePath(dir.getAbsolutePath());
        items.add(item);
    }

    private static RunItem findByTitle(ArrayList<RunItem> items, String title) {
        for (RunItem item : items) {
            if (item.getTitle().equals(title)) {
                return item;
            }
        }
        return null;
    }

    private static boolean hasPck(File dir) {
        return new File(dir, PCK_MARKER).isFile();
    }
}
