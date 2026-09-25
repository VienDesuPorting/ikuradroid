package su.viende.ikuradroid;

/**
 * One tile in the library grid - a folder the engine recognizes as a game
 * (GameLibrary.isGameFolder).
 *
 * A game comes either from the library root on the real file system
 * (sourcePath), from the legacy app-private install area (installedPath),
 * or from both. The engine runs the game straight from sourcePath when it
 * is available; installedPath covers copies made by the SAF era and keeps
 * the library working when the root is unavailable.
 */
public class RunItem {
    private String title;
    private String sourcePath;     // absolute path of the game folder in shared storage, null when legacy-copy-only
    private String installedPath;  // absolute path of the legacy copy in the app-private area, null when there is none
    private String engine;         // engine family display name (GameLibrary.detectEngine), null when unknown
    private long sizeBytes = -1;   // game folder size in bytes, -1 when it could not be measured

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getSourcePath() {
        return sourcePath;
    }

    public void setSourcePath(String sourcePath) {
        this.sourcePath = sourcePath;
    }

    public String getInstalledPath() {
        return installedPath;
    }

    public void setInstalledPath(String installedPath) {
        this.installedPath = installedPath;
    }

    /** Engine family display name ("Ikura GDL", "Will", ...), null when unknown. */
    public String getEngine() {
        return engine;
    }

    public void setEngine(String engine) {
        this.engine = engine;
    }

    /** Game folder size in bytes; -1 when it could not be measured. */
    public long getSizeBytes() {
        return sizeBytes;
    }

    public void setSizeBytes(long sizeBytes) {
        this.sizeBytes = sizeBytes;
    }
}
