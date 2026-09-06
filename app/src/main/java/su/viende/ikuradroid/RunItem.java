package su.viende.ikuradroid;

/**
 * 1.2.0: one tile in the library grid - a folder containing vilevn.pck.
 *
 * A game comes either from the SAF tree picked by the user (safUri), from
 * the app-private install area (installedPath), or from both. The install
 * area copy is what the native engine actually runs from (plain stdio),
 * see GameLibrary.install().
 */
public class RunItem {
    private String title;
    private String safUri;         // SAF document URI of the game folder, null when installed-only
    private String installedPath;  // absolute path of the installed copy, null when not installed

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getSafUri() {
        return safUri;
    }

    public void setSafUri(String safUri) {
        this.safUri = safUri;
    }

    public String getInstalledPath() {
        return installedPath;
    }

    public void setInstalledPath(String installedPath) {
        this.installedPath = installedPath;
    }
}
