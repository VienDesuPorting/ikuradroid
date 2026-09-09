package su.viende.ikuradroid;

import android.content.Context;
import android.content.SharedPreferences;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.Set;

/**
 * File-system game library.
 *
 * The user picks library roots with the in-app folder browser
 * (FolderPickerActivity) after granting storage access; the absolute
 * paths are persisted as a set - every pick adds another root, so games
 * scattered across the shared storage all live in one library. The
 * engine reads files with plain stdio, so game folders are passed to it
 * as real paths - nothing is copied.
 *
 * A title can also be hidden from the list ("remove from list" in the
 * long-press menu). Hidden titles are remembered by name, and picking
 * the folder that contains them again puts them back into the library.
 *
 * Games are discovered in each root folder itself and one level below it;
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
    private static final String KEY_ROOT_PATH = "library_root";      // pre-1.9.0 single root, migrated on read
    private static final String KEY_ROOTS = "library_roots";         // StringSet of absolute paths
    private static final String KEY_HIDDEN = "hidden_titles";        // StringSet of game folder names
    private static final String INSTALL_DIR = "games";

    private GameLibrary() {
    }

    // ------------------------------------------------------------------
    // Persisted library root
    // ------------------------------------------------------------------

    /**
     * All persisted library roots, in insertion order. The pre-1.9.0
     * single-root setting is migrated into the set on first read.
     */
    public static LinkedHashSet<String> getRoots(Context context) {
        SharedPreferences prefs = context.getSharedPreferences(PREFS_FILE, Context.MODE_PRIVATE);
        LinkedHashSet<String> roots = new LinkedHashSet<>();
        Set<String> stored = prefs.getStringSet(KEY_ROOTS, null);
        if (stored != null) {
            roots.addAll(stored);
        } else {
            String legacy = prefs.getString(KEY_ROOT_PATH, null);
            if (legacy != null) {
                roots.add(legacy);
                prefs.edit().putStringSet(KEY_ROOTS, roots).remove(KEY_ROOT_PATH).apply();
            }
        }
        return roots;
    }

    /** Adds another library root. Picking the same folder twice is a no-op. */
    public static void addRootPath(Context context, String rootPath) {
        if (rootPath == null) {
            return;
        }
        SharedPreferences prefs = context.getSharedPreferences(PREFS_FILE, Context.MODE_PRIVATE);
        LinkedHashSet<String> roots = getRoots(context);
        roots.add(rootPath);
        prefs.edit().putStringSet(KEY_ROOTS, roots).apply();
    }

    /**
     * Restores titles hidden from the list when their folder comes back:
     * un-hides the picked folder itself (when it is a game) plus every
     * game folder one level below it - exactly the titles "re-adding this
     * folder" would put back into the library.
     */
    public static void unhideUnder(Context context, String rootPath) {
        if (rootPath == null) {
            return;
        }
        SharedPreferences prefs = context.getSharedPreferences(PREFS_FILE, Context.MODE_PRIVATE);
        Set<String> hidden = prefs.getStringSet(KEY_HIDDEN, null);
        if (hidden == null || hidden.isEmpty()) {
            return;
        }
        LinkedHashSet<String> restored = new LinkedHashSet<>(hidden);
        File root = new File(rootPath);
        if (isGameFolder(root)) {
            restored.remove(root.getName());
        }
        File[] children = root.listFiles();
        if (children != null) {
            for (File dir : children) {
                if (dir.isDirectory() && isGameFolder(dir)) {
                    restored.remove(dir.getName());
                }
            }
        }
        if (restored.size() != hidden.size()) {
            prefs.edit().putStringSet(KEY_HIDDEN, restored).apply();
        }
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
     * Builds the library list: games under every persisted root folder
     * plus the legacy copies in the install area (merged by title, so the
     * library still works when roots are missing or unreadable). Titles
     * hidden by the user are left out. Never returns null.
     */
    public static ArrayList<RunItem> scan(Context context) {
        ArrayList<RunItem> items = new ArrayList<>();

        // A missing root (unmounted SD card, deleted folder) is kept in
        // the set: dropping it here would lose the whole root on one
        // transient failure, and an empty scan just shows the empty state.
        for (String rootPath : getRoots(context)) {
            File root = new File(rootPath);
            if (!root.isDirectory()) {
                continue;
            }
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

        // Long-press "remove from list": hidden titles never reach the UI
        SharedPreferences prefs = context.getSharedPreferences(PREFS_FILE, Context.MODE_PRIVATE);
        Set<String> hidden = prefs.getStringSet(KEY_HIDDEN, null);
        if (hidden != null && !hidden.isEmpty()) {
            Iterator<RunItem> it = items.iterator();
            while (it.hasNext()) {
                if (hidden.contains(it.next().getTitle())) {
                    it.remove();
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

    /**
     * "Remove from list": the title disappears from the library while its
     * files stay untouched on the storage. Picking the containing folder
     * again (unhideUnder) puts the title back.
     */
    public static void setHidden(Context context, String title, boolean isHidden) {
        SharedPreferences prefs = context.getSharedPreferences(PREFS_FILE, Context.MODE_PRIVATE);
        LinkedHashSet<String> hidden = new LinkedHashSet<>();
        Set<String> stored = prefs.getStringSet(KEY_HIDDEN, null);
        if (stored != null) {
            hidden.addAll(stored);
        }
        if (isHidden) {
            hidden.add(title);
        } else {
            hidden.remove(title);
        }
        prefs.edit().putStringSet(KEY_HIDDEN, hidden).apply();
    }

    /**
     * Deletes a directory tree. Used by the long-press "delete from
     * device" action; fails soft - the caller reports leftovers.
     */
    public static boolean deleteRecursively(File dir) {
        if (dir == null || !dir.exists()) {
            return true;
        }
        File[] children = dir.listFiles();
        if (children != null) {
            for (File child : children) {
                deleteRecursively(child);
            }
        }
        return dir.delete();
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
