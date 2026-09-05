package su.viende.vile;

public class RunItem {
    private String title;
    private String currentPath;
    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }
    
    public void setPath(String Path) {
        this.currentPath = Path;
    }    
    public String getPath() {
        return currentPath;
    }
}
