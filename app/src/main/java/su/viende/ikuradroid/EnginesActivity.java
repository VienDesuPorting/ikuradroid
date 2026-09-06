package su.viende.ikuradroid;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;

import com.google.android.material.appbar.MaterialToolbar;

/**
 * 1.2.0: engines and supported titles.
 *
 * The list below mirrors the script engines actually compiled into
 * libvile.so (jni/vile): ikura, will, crowd, cware, jast, windy, tlove.
 * Game titles are the engine's own NativeName() strings wherever one
 * exists, so the screen never promises more than the engine implements.
 *
 * Only Ikura GDL is verified on this build (Crescendo runs end to end);
 * every other engine and title is explicitly marked "not tested".
 */
public class EnginesActivity extends AppCompatActivity {

    private static final class Engine {
        final String name;
        final boolean tested;
        final String[] titles;

        Engine(String name, boolean tested, String[] titles) {
            this.name = name;
            this.tested = tested;
            this.titles = titles;
        }
    }

    // Titles = NativeName() strings from jni/vile (see class javadoc)
    private static final Engine[] ENGINES = {
            new Engine("Ikura GDL", true, new String[]{
                    "Crescendo",
                    "Heart de Roommate",
                    "Cat Girl Alliance",
                    "Hitomi -My Stepsister-",
                    "Idols Galore!",
                    "Kana ... Okaeri!",
                    "Kana ~ Little Sister",
                    "The Sagara Family",
                    "Snow",
                    "Virgin",
            }),
            new Engine("Will", false, new String[]{
                    "Critical Point",
                    "Princess Waltz",
                    "Starry Sky",
                    "Yume Miru Kusuri",
            }),
            new Engine("Crowd", false, new String[]{
                    "Tokimeki Check-in!",
                    "XChange 1",
                    "XChange 3",
            }),
            new Engine("C-Ware", false, new String[]{
                    "DiviDead",
            }),
            new Engine("JAST", false, new String[]{
                    "Season of the Sakura",
                    "3 Sisters Story",
                    "Runaway City",
            }),
            new Engine("Windy", false, new String[]{
                    "Mayclub VR Dating SX",
                    "Nocturnal Illusion Renewal",
            }),
            new Engine("T-Love", false, new String[]{
                    "True Love",
            }),
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        WindowCompat.setDecorFitsSystemWindows(getWindow(), false);
        setContentView(R.layout.activity_engines);

        MaterialToolbar toolbar = (MaterialToolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.menu_engines);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        }

        View root = findViewById(R.id.engines_root);
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

        LinearLayout container = (LinearLayout) findViewById(R.id.engines_container);
        LayoutInflater inflater = getLayoutInflater();
        for (Engine engine : ENGINES) {
            View card = inflater.inflate(R.layout.engines_item, container, false);

            TextView name = (TextView) card.findViewById(R.id.engine_name);
            name.setText(engine.name);

            TextView status = (TextView) card.findViewById(R.id.engine_status);
            status.setText(engine.tested ? R.string.engine_status_tested
                    : R.string.engine_status_not_tested);
            status.setTextColor(statusColor(status, engine.tested));

            TextView titles = (TextView) card.findViewById(R.id.engine_titles);
            StringBuilder builder = new StringBuilder();
            for (String title : engine.titles) {
                if (builder.length() > 0) {
                    builder.append('\n');
                }
                builder.append("\u2022 ").append(title);
            }
            titles.setText(builder.toString());

            container.addView(card);
        }
    }

    /** Theme-aware emphasis: tested engines pop, others stay muted. */
    private int statusColor(View view, boolean tested) {
        int attr = tested ? com.google.android.material.R.attr.colorPrimary
                : android.R.attr.textColorSecondary;
        try {
            return com.google.android.material.color.MaterialColors.getColor(view, attr);
        } catch (IllegalArgumentException e) {
            return 0xFF808080;
        }
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
