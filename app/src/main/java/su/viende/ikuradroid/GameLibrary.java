package su.viende.ikuradroid;

import android.content.Context;
import android.content.SharedPreferences;
import android.net.Uri;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import androidx.documentfile.provider.DocumentFile;

/**
 * SAF-based game library (no storage permissions).
 *
 * The user picks the library root once with the system folder picker
 * (ACTION_OPEN_DOCUMENT_TREE); the grant is persisted. The engine itself
 * uses plain stdio and cannot read content:// URIs, so on launch the
 * game folder is copied (idempotently - existing files with matching sizes
 * are skipped) into the app-private area:
 *
 *     getExternalFilesDir(null)/games/<title>/
 *
 * The native side chdir()s into the copied folder (sdl_main.c) and
 * writes saves to the same getExternalFilesDir(null) root.
 */
public final class GameLibrary {

    private static final String PREFS_FILE = "library";
    private static final String KEY_TREE_URI = "tree_uri";
    private static final String INSTALL_DIR = "games";
    private static final String PCK_MARKER = "vilevn.pck";

    /** Progress callbacks for the copy dialog. */
    public interface Progress {
        /** Called before each file that is about to be copied. */
        void onProgress(String fileName);

        /** Checked between every file and every buffer - abort when true. */
        boolean isCancelled();
    }

    private GameLibrary() {
    }

    // ------------------------------------------------------------------
    // Persisted library root
    // ------------------------------------------------------------------

    public static Uri getTreeUri(Context context) {
        SharedPreferences prefs = context.getSharedPreferences(PREFS_FILE, Context.MODE_PRIVATE);
        String uri = prefs.getString(KEY_TREE_URI, null);
        return uri == null ? null : Uri.parse(uri);
    }

    public static void setTreeUri(Context context, Uri treeUri) {
        SharedPreferences prefs = context.getSharedPreferences(PREFS_FILE, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = prefs.edit();
        if (treeUri == null) {
            editor.remove(KEY_TREE_URI);
        } else {
            editor.putString(KEY_TREE_URI, treeUri.toString());
        }
        editor.apply();
    }

    /** Root of the app-private install area. */
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
     * Builds the library list: games under the persisted SAF tree plus the
     * games already copied into the install area (merged by title, so the
     * library still works when the SAF grant is lost or the SD card is
     * unplugged). Never returns null.
     */
    public static ArrayList<RunItem> scan(Context context) {
        ArrayList<RunItem> items = new ArrayList<>();

        Uri treeUri = getTreeUri(context);
        if (treeUri != null) {
            try {
                DocumentFile root = DocumentFile.fromTreeUri(context, treeUri);
                if (root != null && root.canRead()) {
                    // The picked folder itself may be a game folder
                    if (hasPck(root)) {
                        addSafGame(items, root, root.getName());
                    }
                    for (DocumentFile dir : root.listFiles()) {
                        if (dir.isDirectory() && hasPck(dir)) {
                            addSafGame(items, dir, dir.getName());
                        }
                    }
                } else {
                    // Stale tree (grant revoked, provider gone) - drop it so
                    // the empty state asks the user to pick the folder again
                    setTreeUri(context, null);
                }
            } catch (SecurityException | IllegalStateException e) {
                setTreeUri(context, null);
            }
        }

        // Merge the install area - these always work, no permission needed
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

    private static void addSafGame(ArrayList<RunItem> items, DocumentFile dir, String name) {
        if (name == null || name.length() == 0) {
            return;
        }
        RunItem known = findByTitle(items, name);
        if (known != null) {
            return;
        }
        RunItem item = new RunItem();
        item.setTitle(name);
        item.setSafUri(dir.getUri().toString());
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

    private static boolean hasPck(DocumentFile dir) {
        try {
            return dir.findFile(PCK_MARKER) != null;
        } catch (SecurityException e) {
            return false;
        }
    }

    // ------------------------------------------------------------------
    // Install (idempotent copy into the app-private area)
    // ------------------------------------------------------------------

    /**
     * Copies the game from the SAF tree into the install area and returns
     * the absolute path of the installed folder. Files that already exist
     * with the same length are skipped, so repeated launches and updates
     * are cheap. The engine font is bootstrapped afterwards.
     */
    public static String install(Context context, RunItem game, Progress progress) throws Exception {
        if (game.getSafUri() == null) {
            // Installed-only game: nothing to copy
            if (game.getInstalledPath() == null) {
                throw new FileNotFoundException("Game has neither a SAF source nor an installed copy");
            }
            return game.getInstalledPath();
        }
        DocumentFile source = DocumentFile.fromTreeUri(context, Uri.parse(game.getSafUri()));
        if (source == null || !source.isDirectory()) {
            throw new FileNotFoundException("The game folder is no longer accessible, pick the library folder again");
        }
        File destination = new File(installRoot(context), safeName(game.getTitle()));
        copyTree(context, source, destination, progress);
        GameFontInstaller.ensureFont(context, destination.getAbsolutePath());
        return destination.getAbsolutePath();
    }

    private static void copyTree(Context context, DocumentFile source, File destination,
                                 Progress progress) throws Exception {
        if (!destination.exists() && !destination.mkdirs()) {
            throw new IOException("Cannot create " + destination);
        }
        DocumentFile[] children = source.listFiles();
        for (DocumentFile child : children) {
            if (progress != null && progress.isCancelled()) {
                throw new IOException("Cancelled");
            }
            String name = child.getName();
            if (name == null || name.length() == 0) {
                continue;
            }
            File out = new File(destination, safeName(name));
            if (child.isDirectory()) {
                copyTree(context, child, out, progress);
                continue;
            }
            // Idempotency: same length means same content for our purposes
            if (out.isFile() && out.length() == child.length()) {
                continue;
            }
            if (progress != null) {
                progress.onProgress(name);
            }
            copyFile(context, child, out, progress);
        }
    }

    private static void copyFile(Context context, DocumentFile source, File out,
                                 Progress progress) throws Exception {
        InputStream in = null;
        OutputStream os = null;
        try {
            in = context.getContentResolver().openInputStream(source.getUri());
            if (in == null) {
                throw new IOException("Cannot open " + source.getUri());
            }
            os = new FileOutputStream(out);
            byte[] buffer = new byte[65536];
            int n;
            while ((n = in.read(buffer)) > 0) {
                if (progress != null && progress.isCancelled()) {
                    throw new IOException("Cancelled");
                }
                os.write(buffer, 0, n);
            }
            os.flush();
        } catch (IOException e) {
            // Never keep a half-written file: the next run will retry it
            out.delete();
            throw e;
        } finally {
            if (os != null) {
                try {
                    os.close();
                } catch (IOException ignored) {
                }
            }
            if (in != null) {
                try {
                    in.close();
                } catch (IOException ignored) {
                }
            }
        }
    }

    /** Filesystem-safe name: keeps the title readable, strips separators. */
    private static String safeName(String name) {
        String cleaned = name.replaceAll("[/\\\\]", "_").trim();
        if (cleaned.length() == 0) {
            cleaned = "game";
        }
        return cleaned;
    }
}
