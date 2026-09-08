package su.viende.ikuradroid;

import android.app.Application;

import com.google.android.material.color.DynamicColors;

/**
 * Enables Material You dynamic color - on Android 12+ the
 * launcher palette is derived from the user's wallpaper instead of the
 * static "ikura" seed. On older systems the brand colors from colors.xml
 * (and values-night) stay in effect.
 */
public class IkuraApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        DynamicColors.applyToActivitiesIfAvailable(this);
    }
}
