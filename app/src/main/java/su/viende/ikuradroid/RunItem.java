package su.viende.ikuradroid;

/**
 * One tile in the library grid - a folder containing vilevn.pck.
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
}
