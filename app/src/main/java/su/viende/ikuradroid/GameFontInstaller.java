package su.viende.ikuradroid;

import android.content.Context;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Font bootstrap for game folders.
 *
 * The C++ engine resolves its UI/story font to an absolute path inside the
 * game folder on Android (vile.cpp, ANDROID branch: default_face =
 * "<cwd>/default.ttf") and then checks it with a plain fopen() before
 * calling TTF_OpenFont (widgets/printer.cpp SetFontFace, common/edl_gfx.cpp
 * EDL_CreateText). Plain fopen() cannot read files inside the APK, so the
 * default.ttf shipped in app assets was never seen by the engine: freshly
 * installed games rendered no text until the user copied a font by hand.
 *
 * The fix: copy assets/default.ttf into the game folder before the engine
 * starts. Games that ship their own .ttf are never touched.
 */
public final class GameFontInstaller {

    private static final String ASSET_FONT = "default.ttf";

    // Engine-bundled face for DRS story text (Kana): metric-compatible with
    // Times New Roman so line wraps match the PC original. Installed on
    // every run when missing, independent of the primary-font guard below -
    // no game ships a file under this name.
    private static final String ASSET_SERIF = "LiberationSerif-Regular.ttf";

    private GameFontInstaller() {
    }

    /**
     * Makes sure the engine will find a usable font in the given game folder.
     * Idempotent and silent: any failure leaves the folder untouched and the
     * engine falls back to its own error path (no text, as before).
     *
     * @param gameDirPath absolute path of the game folder (must contain the
     *                    game files)
     */
    public static void ensureFont(Context context, String gameDirPath) {
        if (context == null || gameDirPath == null || gameDirPath.length() == 0)
            return;
        File dir = new File(gameDirPath);
        if (!dir.isDirectory())
            return;
        try {
            // DRS story face: our own asset, installed whenever missing
            installAsset(context, dir, ASSET_SERIF);
            if (hasFont(dir))
                return; // game ships its own font - never touch it
            installAsset(context, dir, ASSET_FONT);
        } catch (Exception e) {
            // Non-fatal: without the font the engine behaves exactly as before
        }
    }

    /** Copies one asset into the game folder unless it is already there. */
    private static void installAsset(Context context, File dir, String name)
            throws Exception {
        File out = new File(dir, name);
        if (out.exists() && out.length() > 0)
            return; // already installed by a previous run
        InputStream in = context.getAssets().open(name);
        OutputStream os = new FileOutputStream(out);
        try {
            byte[] buffer = new byte[8192];
            int n;
            while ((n = in.read(buffer)) > 0)
                os.write(buffer, 0, n);
            os.flush();
        } finally {
            try {
                os.close();
            } finally {
                in.close();
            }
        }
    }

    /** True when the folder already contains a usable TrueType font. */
    private static boolean hasFont(File dir) {
        File[] files = dir.listFiles();
        if (files == null)
            return false;
        for (File f : files) {
            if (f.isFile() && f.getName().toLowerCase().endsWith(".ttf"))
                return true;
        }
        return false;
    }
}
