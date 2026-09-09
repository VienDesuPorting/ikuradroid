package su.viende.ikuradroid;

import android.graphics.Typeface;
import android.os.Bundle;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.text.style.ForegroundColorSpan;
import android.text.style.StyleSpan;
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

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;

/**
 * Engines and supported titles, data-driven from assets/engines.json.
 *
 * The JSON mirrors the script engines actually compiled into libikuradroid.so
 * (jni/vile): ikura, will, crowd, cware, jast, windy, tlove. Titles are
 * the engine's own NativeName() strings wherever one exists, so the
 * screen never promises more than the engine implements.
 *
 * Statuses are honest: a title is marked tested only after it has been
 * checked end to end on a real device in this port. Everything else
 * stays "not tested" no matter what the desktop ViLE documentation
 * claimed - compiled-in code is a baseline, not a verification.
 */
public class EnginesActivity extends AppCompatActivity {

    private static final class Engine {
        final String name;
        final boolean tested;
        final String[] titles;
        final boolean[] titleTested;

        Engine(String name, boolean tested, String[] titles, boolean[] titleTested) {
            this.name = name;
            this.tested = tested;
            this.titles = titles;
            this.titleTested = titleTested;
        }
    }

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
        Engine[] engines = loadEngines();
        if (engines == null) {
            // A broken data file must never look like "no engines exist"
            TextView error = new TextView(this);
            error.setText(R.string.engines_data_error);
            // Two-arg form: the one-arg overload exists only since API 23
            error.setTextAppearance(this, android.R.style.TextAppearance_Small);
            error.setPadding((int) (16 * getResources().getDisplayMetrics().density),
                    0, 0, 0);
            container.addView(error);
            return;
        }
        LayoutInflater inflater = getLayoutInflater();
        for (Engine engine : engines) {
            container.addView(buildEngineCard(inflater, container, engine));
        }
    }

    private View buildEngineCard(LayoutInflater inflater, LinearLayout container,
                                 Engine engine) {
        View card = inflater.inflate(R.layout.engines_item, container, false);

        TextView name = (TextView) card.findViewById(R.id.engine_name);
        name.setText(engine.name);

        TextView status = (TextView) card.findViewById(R.id.engine_status);
        status.setText(engine.tested ? R.string.engine_status_tested
                : R.string.engine_status_not_tested);
        status.setTextColor(themeColor(status,
                com.google.android.material.R.attr.colorPrimary,
                engine.tested));

        TextView titles = (TextView) card.findViewById(R.id.engine_titles);
        titles.setText(titlesSpan(titles, engine));

        return card;
    }

    /**
     * One line per title: tested titles carry a check mark in the primary
     * color (and a semi-bold face), the rest stay muted bullets.
     */
    private CharSequence titlesSpan(TextView view, Engine engine) {
        SpannableStringBuilder builder = new SpannableStringBuilder();
        int primary = themeColor(view,
                com.google.android.material.R.attr.colorPrimary, true);
        for (int i = 0; i < engine.titles.length; i++) {
            if (builder.length() > 0) {
                builder.append('\n');
            }
            boolean tested = engine.titleTested[i];
            int start = builder.length();
            builder.append(tested ? "\u2713 " : "\u2022 ")
                    .append(engine.titles[i]);
            if (tested) {
                builder.setSpan(new ForegroundColorSpan(primary), start,
                        builder.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                builder.setSpan(new StyleSpan(Typeface.BOLD), start,
                        builder.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            }
        }
        return builder;
    }

    /**
     * Resolves a theme attribute color; errors resolve to the fallback.
     * For muted text the caller passes android.R.attr.textColorSecondary
     * and a neutral gray fallback.
     */
    private int themeColor(View view, int attr, boolean emphasize) {
        int resolved = emphasize ? attr : android.R.attr.textColorSecondary;
        try {
            return com.google.android.material.color.MaterialColors.getColor(
                    view, resolved);
        } catch (IllegalArgumentException e) {
            return emphasize ? 0xFF808080 : 0xFF808080;
        }
    }

    /**
     * Parses assets/engines.json; null means the file is missing or
     * malformed (the UI shows an explicit error instead of an empty list).
     */
    private Engine[] loadEngines() {
        try {
            InputStream in = getAssets().open("engines.json");
            StringBuilder text = new StringBuilder();
            BufferedReader reader = new BufferedReader(
                    new InputStreamReader(in, StandardCharsets.UTF_8));
            String line;
            while ((line = reader.readLine()) != null) {
                text.append(line).append('\n');
            }
            reader.close();

            JSONArray array = new JSONObject(text.toString())
                    .getJSONArray("engines");
            Engine[] engines = new Engine[array.length()];
            for (int i = 0; i < array.length(); i++) {
                JSONObject entry = array.getJSONObject(i);
                JSONArray titles = entry.getJSONArray("titles");
                String[] names = new String[titles.length()];
                boolean[] tested = new boolean[titles.length()];
                for (int j = 0; j < titles.length(); j++) {
                    JSONObject title = titles.getJSONObject(j);
                    names[j] = title.getString("name");
                    tested[j] = title.optBoolean("tested", false);
                }
                engines[i] = new Engine(entry.getString("name"),
                        entry.optBoolean("tested", false), names, tested);
            }
            return engines;
        } catch (Exception e) {
            return null;
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
