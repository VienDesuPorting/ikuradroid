package su.viende.ikuradroid;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.graphics.Typeface;
import android.net.Uri;
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
import android.widget.Toast;

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
import java.util.Locale;

/**
 * Engines and supported titles, data-driven from assets/engines.json.
 *
 * The JSON mirrors the script engines actually compiled into libikuradroid.so
 * (jni/vile): ikura, will, crowd, cware, jast, windy, tlove. Titles are
 * the engine's own NativeName() strings wherever one exists, so the
 * screen never promises more than the engine implements.
 *
 * Titles carry the same four-level status as the compatibility table:
 * playable (beaten on a real device), runnable (playable, but with
 * known open issues), launches (confirmed start, no full playthrough)
 * and untested. A missing "status" falls back to the legacy
 * "tested" boolean so an old data file still renders correctly.
 *
 * The table's Notes column (what exactly still needs work or checking)
 * lives only in the repository, so a button under the cards opens the
 * full table in the browser instead of duplicating it here.
 */
public class EnginesActivity extends AppCompatActivity {

    private static final String STATUS_PLAYABLE = "playable";
    private static final String STATUS_RUNNABLE = "runnable";
    private static final String STATUS_LAUNCHES = "launches";

    private static final String REPO_COMPATIBILITY_RU =
            "https://github.com/VienDesuPorting/ikuradroid/blob/main/COMPATIBILITY.md";
    private static final String REPO_COMPATIBILITY_EN =
            "https://github.com/VienDesuPorting/ikuradroid/blob/main/COMPATIBILITY.en.md";

    private static final class Engine {
        final String name;
        final boolean tested;
        final String[] titles;
        final String[] titleStatus;

        Engine(String name, boolean tested, String[] titles, String[] titleStatus) {
            this.name = name;
            this.tested = tested;
            this.titles = titles;
            this.titleStatus = titleStatus;
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

        // Wired before the data load: the link must stay clickable even
        // when engines.json is broken and the screen shows the error text
        findViewById(R.id.engines_compat_button).setOnClickListener(
                new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                openCompatibilityTable();
            }
        });

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
     * One line per title: a status mark in front of the name - a check
     * mark for playable (primary color, semi-bold), a tilde for
     * runnable - playable, but with known open issues (primary color),
     * a dash for launches (still the muted body color) and a plain
     * bullet for untested.
     */
    private CharSequence titlesSpan(TextView view, Engine engine) {
        SpannableStringBuilder builder = new SpannableStringBuilder();
        int primary = themeColor(view,
                com.google.android.material.R.attr.colorPrimary, true);
        for (int i = 0; i < engine.titles.length; i++) {
            if (builder.length() > 0) {
                builder.append('\n');
            }
            String status = engine.titleStatus[i];
            boolean playable = STATUS_PLAYABLE.equals(status);
            boolean runnable = STATUS_RUNNABLE.equals(status);
            int start = builder.length();
            if (playable) {
                builder.append("\u2713 ");        // check mark
            } else if (runnable) {
                builder.append("~ ");             // tilde
            } else if (STATUS_LAUNCHES.equals(status)) {
                builder.append("\u2013 ");        // en dash
            } else {
                builder.append("\u2022 ");        // bullet
            }
            builder.append(engine.titles[i]);
            if (playable || runnable) {
                builder.setSpan(new ForegroundColorSpan(primary), start,
                        builder.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            }
            if (playable) {
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
     * Opens the repository compatibility table in the browser. The Notes
     * column (what still needs work or checking) exists only there, so
     * the table is the source of truth the button leads to. A device
     * without any browser is rare but possible - a toast keeps the tap
     * from being swallowed silently.
     */
    private void openCompatibilityTable() {
        String language = Locale.getDefault().getLanguage();
        String url = "ru".equals(language)
                ? REPO_COMPATIBILITY_RU : REPO_COMPATIBILITY_EN;
        try {
            startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse(url)));
        } catch (ActivityNotFoundException e) {
            Toast.makeText(this, R.string.engines_link_error,
                    Toast.LENGTH_SHORT).show();
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
                String[] statuses = new String[titles.length()];
                for (int j = 0; j < titles.length(); j++) {
                    JSONObject title = titles.getJSONObject(j);
                    names[j] = title.getString("name");
                    // explicit status wins; the legacy tested boolean is
                    // the fallback for data files that predate it
                    String status = title.optString("status", null);
                    if (status == null || status.length() == 0) {
                        status = title.optBoolean("tested", false)
                                ? STATUS_PLAYABLE : "untested";
                    }
                    statuses[j] = status;
                }
                engines[i] = new Engine(entry.getString("name"),
                        entry.optBoolean("tested", false), names, statuses);
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
