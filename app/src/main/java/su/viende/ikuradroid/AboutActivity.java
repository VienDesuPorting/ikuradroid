package su.viende.ikuradroid;

import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;

import com.google.android.material.appbar.MaterialToolbar;

/**
 * The "About" full screen. Attribution of the ViLE engine (ViLE Team,
 * GPLv3), the original anonymous
 * Android developer ("Ivan") and the revival team (VienDesu! Porting Team).
 * The version line is filled in from PackageManager at runtime.
 */
public class AboutActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        WindowCompat.setDecorFitsSystemWindows(getWindow(), false);
        setContentView(R.layout.activity_about);

        MaterialToolbar toolbar = (MaterialToolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.menu_about);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        }

        View root = findViewById(R.id.about_root);
        View appbar = findViewById(R.id.appbar);
        ViewCompat.setOnApplyWindowInsetsListener(root, new androidx.core.view.OnApplyWindowInsetsListener() {
            @Override
            public WindowInsetsCompat onApplyWindowInsets(View v, WindowInsetsCompat windowInsets) {
                Insets bars = windowInsets.getInsets(WindowInsetsCompat.Type.systemBars());
                if (appbar != null) {
                    appbar.setPadding(0, bars.top, 0, 0);
                }
                v.setPadding(0, 0, 0, bars.bottom);
                return WindowInsetsCompat.CONSUMED;
            }
        });

        TextView version = (TextView) findViewById(R.id.about_version);
        String verName;
        try {
            verName = getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
        } catch (Exception e) {
            verName = "?";
        }
        version.setText(getString(R.string.app_name) + " " + verName);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
