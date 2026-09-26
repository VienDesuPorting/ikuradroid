package su.viende.ikuradroid;

import android.content.Context;
import android.content.SharedPreferences;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.nio.charset.CodingErrorAction;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.Locale;
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

    // Engine signatures mirrored from the native probes (ViLE::Probe* in
    // jni/vile/vile.cpp): a folder counts as a game when it contains all
    // files of one row. Names are compared case-insensitively - game data
    // comes from Windows installs where the on-disk case varies. The Will
    // family shares one signature; the exact title is picked natively by
    // archive size.
    private static final String[][] GAME_SIGNATURES = {
            {"ggd", "isf"},                         // Ikura GDL (classic data set)
            {"drssnr", "drsgrp"},                   // Ikura GDL (DRS data set)
            {"rio.arc", "chip.arc"},                // Will: Critical Point, Princess Waltz, Starry Sky, Yume Miru Kusuri, Little My Maid
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

    // Engine family display names, index-aligned with GAME_SIGNATURES;
    // shown as the badge on library tiles (RunAdapter). These are brand
    // names, so they stay untranslated in every locale.
    private static final String[] ENGINE_FAMILIES = {
            "Ikura GDL",                            // Ikura GDL (classic data set)
            "Ikura GDL",                            // Ikura GDL (DRS data set)
            "Will",                                 // Will: Critical Point, Princess Waltz, Starry Sky, ...
            "Crowd",                                // Crowd: Tokimeki Check-in!
            "Crowd",                                // Crowd: XChange 1
            "Crowd",                                // Crowd: XChange 3
            "JAST USA",                             // JAST USA Memorial Collection
            "C-Ware",                               // C-Ware: DiviDead
            "T-Love",                               // T-Love: True Love
            "T-Love",                               // T-Love: True Love (alt. data set)
            "Windy",                                // Windy: Nocturnal Illusion
            "Windy",                                // Windy: Mayclub
    };

    private static final String PREFS_FILE = "library";
    private static final String KEY_ROOT_PATH = "library_root";      // pre-1.9.0 single root, migrated on read
    private static final String KEY_ROOTS = "library_roots";         // StringSet of absolute paths
    private static final String KEY_HIDDEN = "hidden_titles";        // StringSet of game folder names
    private static final String KEY_DISPLAY = "display_name:";       // manual tile renames, keyed by folder name
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
                    item.setEngine(detectEngine(dir));
                    item.setSizeBytes(folderSize(dir));
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

        // Tile names: a manual rename wins, then the SUF startup title
        // of Ikura GDL games, otherwise the plain folder name
        for (RunItem item : items) {
            item.setDisplayName(resolveDisplayName(prefs, item));
        }

        Collections.sort(items, new Comparator<RunItem>() {
            @Override
            public int compare(RunItem a, RunItem b) {
                return a.displayTitle().compareToIgnoreCase(b.displayTitle());
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
     * Manual rename of a title (JoiPlay-style naming): the value is
     * stored per game folder name and shown on the tile instead of it;
     * null or a blank value drops the rename and returns the tile to
     * its automatic name (the SUF title for Ikura GDL games, otherwise
     * the folder name). The folder itself is never touched - saves,
     * hidden state and re-adding it keep working as before.
     */
    public static void setDisplayName(Context context, String folderName, String value) {
        SharedPreferences prefs = context.getSharedPreferences(PREFS_FILE, Context.MODE_PRIVATE);
        String name = value == null ? "" : value.trim();
        if (name.length() == 0) {
            prefs.edit().remove(KEY_DISPLAY + folderName).apply();
        } else {
            prefs.edit().putString(KEY_DISPLAY + folderName, name).apply();
        }
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
        item.setEngine(detectEngine(dir));
        item.setSizeBytes(folderSize(dir));
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
        return detectEngine(dir) != null;
    }

    /**
     * Engine family display name matched by the signatures ("Ikura GDL",
     * "Will", ...) or null when the folder is not a game. The Java-side
     * check is deliberately looser than the native probes (see
     * isGameFolder).
     */
    public static String detectEngine(File dir) {
        if (dir == null || !dir.isDirectory()) {
            return null;
        }
        for (int i = 0; i < GAME_SIGNATURES.length; i++) {
            boolean matched = true;
            for (String marker : GAME_SIGNATURES[i]) {
                if (!hasFile(dir, marker)) {
                    matched = false;
                    break;
                }
            }
            if (matched) {
                return ENGINE_FAMILIES[i];
            }
        }
        return null;
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

    /**
     * Recursive byte size of a folder tree; -1 when the folder cannot be
     * read. A stat-only walk - the scan already runs off the UI thread
     * (MainActivity.rescanLibrary).
     */
    private static long folderSize(File dir) {
        File[] children = dir.listFiles();
        if (children == null) {
            return -1L;
        }
        long total = 0L;
        for (File child : children) {
            if (child.isDirectory()) {
                long sub = folderSize(child);
                if (sub > 0) {
                    total += sub;
                }
            } else {
                total += child.length();
            }
        }
        return total;
    }

    // ------------------------------------------------------------------
    // Tile names: manual renames and SUF titles
    // ------------------------------------------------------------------

    /**
     * The name a tile shows, or null for the plain folder name: the
     * manual rename (setDisplayName) wins, then the SUF startup title
     * for Ikura GDL games (probeSufTitle).
     */
    private static String resolveDisplayName(SharedPreferences prefs, RunItem item) {
        String renamed = prefs.getString(KEY_DISPLAY + item.getTitle(), null);
        if (renamed != null && renamed.trim().length() > 0) {
            return renamed.trim();
        }
        if ("Ikura GDL".equals(item.getEngine())) {
            String path = item.getSourcePath() != null
                            ? item.getSourcePath() : item.getInstalledPath();
            if (path != null) {
                return probeSufTitle(new File(path));
            }
        }
        return null;
    }

    /**
     * Game title from the SUF startup-info files of an Ikura GDL folder
     * (a small Windows INI carrying a TITLE= line, read natively through
     * INIFile::Get which matches the first such line anywhere in the
     * file). Mirrors the file order of ViLE::ProbeSUF - game.suf first;
     * when several SUF files carry a title, a latin-script one wins over
     * the Japanese original, because localized releases ship an extra
     * *.suf with a romanized title next to it. Returns null when nothing
     * readable is found - the tile keeps the folder name then.
     */
    private static String probeSufTitle(File dir) {
        File[] entries = dir.listFiles();
        if (entries == null) {
            return null;
        }
        ArrayList<File> sufs = new ArrayList<>();
        for (File entry : entries) {
            if (entry.isFile() && entry.getName().toLowerCase(Locale.US)
                            .endsWith(".suf")) {
                sufs.add(entry);
            }
        }
        if (sufs.isEmpty()) {
            return null;
        }
        Collections.sort(sufs, new Comparator<File>() {
            @Override
            public int compare(File a, File b) {
                boolean ag = a.getName().equalsIgnoreCase("game.suf");
                boolean bg = b.getName().equalsIgnoreCase("game.suf");
                if (ag != bg) {
                    return ag ? -1 : 1;
                }
                return a.getName().compareToIgnoreCase(b.getName());
            }
        });
        String latin = null;
        String any = null;
        for (File suf : sufs) {
            String title = readSufTitle(suf);
            if (title == null) {
                continue;
            }
            if (any == null) {
                any = title;
            }
            if (isLatin(title)) {
                latin = title;
                break;
            }
        }
        return latin != null ? latin : any;
    }

    /** First TITLE= line of a SUF (INI) file; null when there is none. */
    private static String readSufTitle(File suf) {
        byte[] raw = readFileCapped(suf, 256 * 1024);
        if (raw == null || raw.length == 0) {
            return null;
        }
        String text = decodeLegacyText(raw);
        if (text == null) {
            return null;
        }
        for (String line : text.split("\n")) {
            line = line.trim();
            int eq = line.indexOf('=');
            if (eq <= 0) {
                continue;
            }
            String key = line.substring(0, eq).trim().toLowerCase(Locale.US);
            if (!key.equals("title")) {
                continue;
            }
            String value = line.substring(eq + 1).trim();
            if (value.length() > 0 && value.length() <= 128) {
                return value;
            }
        }
        return null;
    }

    /**
     * SUF text decoding: strict UTF-8 first (cp932 byte streams almost
     * never survive it), Shift-JIS as the fallback for Japanese
     * releases. Null when the data fits neither.
     */
    private static String decodeLegacyText(byte[] raw) {
        try {
            String text = Charset.forName("UTF-8").newDecoder()
                            .onMalformedInput(CodingErrorAction.REPORT)
                            .onUnmappableCharacter(CodingErrorAction.REPORT)
                            .decode(ByteBuffer.wrap(raw)).toString();
            if (text.startsWith("\uFEFF")) {
                text = text.substring(1);
            }
            return text;
        } catch (Exception e) {
            try {
                return new String(raw, "SHIFT_JIS");
            } catch (Exception e2) {
                return null;
            }
        }
    }

    /** Reads a small file fully, capped at capBytes; null on failure. */
    private static byte[] readFileCapped(File file, int capBytes) {
        FileInputStream in = null;
        try {
            in = new FileInputStream(file);
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte[] buffer = new byte[8192];
            int read;
            while ((read = in.read(buffer)) != -1 && out.size() < capBytes) {
                out.write(buffer, 0, read);
            }
            return out.toByteArray();
        } catch (Exception e) {
            return null;
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (Exception e) {
                    // Closing failures do not matter here
                }
            }
        }
    }

    /** True when the string is plain ASCII (no Japanese characters). */
    private static boolean isLatin(String text) {
        for (int i = 0; i < text.length(); i++) {
            if (text.charAt(i) > 0x7f) {
                return false;
            }
        }
        return true;
    }
}
