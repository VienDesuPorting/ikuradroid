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
 * Games are discovered in the root folder itself and one level below it;
 * a folder counts as a game when its contents match one of the engine
 * signatures (GAME_SIGNATURES), so Ikura GDL titles and every other
 * supported engine appear side by side. Legacy copies from the SAF era
 * (getExternalFilesDir(null)/games/<title>/) keep working and are merged
 * into the same list; saves always live in the app-private area.
 */
public final class GameLibrary {

    public static final String PCK_MARKER = "vilevn.pck";

    // Engine signatures mirrored from the native probes (ViLE::Probe* in
    // jni/vile/vile.cpp): a folder counts as a game when it contains all
    // files of one row. Names are compared case-insensitively - game data
    // comes from Windows installs where the on-disk case varies. The Will
    // family shares one signature; the exact title is picked natively by
    // archive size.
    private static final String[][] GAME_SIGNATURES = {
            {PCK_MARKER},                           // Ikura GDL (all titles)
            {"rio.arc", "chip.arc"},                // Will: Critical Point, Princess Waltz, Starry Sky, Yume Miru Kusuri
            {"scene00.bdt", "indexw.dat"},          // Crowd: Tokimeki Check-in!
            {"scene00.bdt", "index.dat"},           // Crowd: XChange 1
            {"xc3.sce"},                            // Crowd: XChange 3
            {"data/images.pck", "data/data.pck"},   // JAST USA Memorial Collection
            {"sg.dl1", "wv.dl1"},                   // C-Ware: DiviDead
            {"eff", "mrs", "date"},                 // T-Love: True Love
            {"eff", "mrs", "datg"},                 // T-Love: True Love (alt. data set)
            {"mug0.dat", "mug0.lst"},               // Windy: Nocturnal Illusion
            {"may0.dat", "may0.lst"},               // Windy: Mayclub
    };

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
                if (isGameFolder(root)) {
                    addFsGame(items, root);
                }
                File[] children = root.listFiles();
                if (children != null) {
                    for (File dir : children) {
                        if (dir.isDirectory() && isGameFolder(dir)) {
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
                if (!dir.isDirectory() || !isGameFolder(dir)) {
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

    /**
     * True when the folder matches at least one engine signature. The
     * check is deliberately looser than the native probes: a false
     * positive only means a failed launch attempt, while a false negative
     * would hide a working game from the library entirely.
     */
    public static boolean isGameFolder(File dir) {
        if (dir == null || !dir.isDirectory()) {
            return false;
        }
        for (String[] signature : GAME_SIGNATURES) {
            boolean matched = true;
            for (String marker : signature) {
                if (!hasFile(dir, marker)) {
                    matched = false;
                    break;
                }
            }
            if (matched) {
                return true;
            }
        }
        return false;
    }

    /** Case-insensitive lookup of dir/relativePath ("sub/file" works). */
    private static boolean hasFile(File dir, String relativePath) {
        File current = dir;
        for (String segment : relativePath.split("/")) {
            File[] entries = current.listFiles();
            if (entries == null) {
                return false;
            }
            File match = null;
            for (File entry : entries) {
                if (entry.getName().equalsIgnoreCase(segment)) {
                    match = entry;
                    break;
                }
            }
            if (match == null) {
                return false;
            }
            current = match;
        }
        return current.isFile();
    }
}
