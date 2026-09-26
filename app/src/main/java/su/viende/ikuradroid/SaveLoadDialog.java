package su.viende.ikuradroid;

import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import org.libsdl.app.SDLActivity;

/**
 * The Java replacement for the native StdSave/StdLoad dialogs.
 *
 * A full-height dialog listing all 40 slots (the 5x8 grid the engine
 * addresses) with the saved screenshot, the scene caption and the save
 * date, read directly from disk by {@link SaveFileRepository}. No
 * pagination: the list scrolls, instead of splitting the screen into
 * five page buttons the way the 2010-era native dialog needed.
 *
 * Performing the save/load is the only thing that must happen on the
 * engine side: {@link SDLActivity#nativeSendSaveLoadEvent} pushes an
 * SDL_USEREVENT that the engine loop consumes between frames. Saves
 * made by the native dialogs are fully compatible - the file layout is
 * identical, it is only read here.
 *
 * The dialog is recreated per invocation, because the engine prefix
 * can change between game runs.
 */
public class SaveLoadDialog {

    // User event codes - must match VILE_JAVA_EVENT_* in javabridge.h.
    private static final int EVENT_LOAD = 1;
    private static final int EVENT_SAVE = 2;

    private final SDLActivity activity;
    private final boolean saveMode;
    private final String saveDir;
    private final String prefix;

    private final Handler ui = new Handler(Looper.getMainLooper());
    private Thread loader;
    private AlertDialog dialog;

    private RecyclerView list;
    private SlotAdapter adapter;
    private TextView hint;

    public SaveLoadDialog(SDLActivity activity, boolean saveMode,
                          String saveDir, String prefix) {
        this.activity = activity;
        this.saveMode = saveMode;
        this.saveDir = saveDir;
        this.prefix = prefix;
    }

    /** Builds the dialog, starts the disk scan, shows. */
    public void show() {
        LinearLayout body = new LinearLayout(activity);
        body.setOrientation(LinearLayout.VERTICAL);

        list = new RecyclerView(activity);
        list.setLayoutManager(new LinearLayoutManager(activity));
        RecyclerView.LayoutParams lp = new RecyclerView.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT, listHeight());
        list.setLayoutParams(lp);
        adapter = new SlotAdapter();
        list.setAdapter(adapter);
        body.addView(list);

        hint = new TextView(activity);
        hint.setTextSize(TypedValue.COMPLEX_UNIT_SP, 12);
        hint.setTextColor(attrColor(com.google.android.material.R.attr.colorOnSurfaceVariant));
        hint.setPadding(dp(16), dp(8), dp(16), dp(12));
        hint.setText(R.string.saveload_hint_save);
        hint.setVisibility(View.GONE);
        LinearLayout.LayoutParams hp = new LinearLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        body.addView(hint, hp);

        AlertDialog.Builder builder = new MaterialAlertDialogBuilder(activity);
        builder.setTitle(saveMode ? R.string.saveload_title_save
                                  : R.string.saveload_title_load);
        builder.setView(body);
        builder.setOnDismissListener(d -> stopLoader());
        dialog = builder.create();
        dialog.show();
        reload();
    }

    /** Two thirds of the short screen side: comfortable in portrait,
     *  and in landscape the game stays visible above the dialog. */
    private int listHeight() {
        int shortSide = Math.min(
                activity.getResources().getDisplayMetrics().widthPixels,
                activity.getResources().getDisplayMetrics().heightPixels);
        return (int) (shortSide * 0.62f);
    }

    private void reload() {
        stopLoader();
        final String dir = saveDir;
        final String pfx = prefix;
        loader = new Thread(() -> {
            final List<SaveFileRepository.Slot> loaded =
                    SaveFileRepository.listSlots(dir, pfx);
            ui.post(() -> {
                if (adapter != null) adapter.setSlots(loaded);
            });
        }, "ikuradroid-saveload-scan");
        loader.start();
    }

    private void stopLoader() {
        if (loader != null) {
            loader.interrupt();
            loader = null;
        }
    }

    private void performSaveOrLoad(final int slotIndex) {
        int mode = saveMode ? EVENT_SAVE : EVENT_LOAD;
        SDLActivity.nativeSendSaveLoadEvent(mode, slotIndex);
        dismiss();
        activity.toast(activity.getString(saveMode
                ? R.string.saveload_saved : R.string.saveload_loaded,
                slotIndex + 1));
    }

    private void dismiss() {
        stopLoader();
        if (dialog != null) {
            dialog.dismiss();
            dialog = null;
        }
    }

    /** Confirmation before overwriting an existing save in save mode. */
    private void maybeConfirmOverwrite(final int slotIndex) {
        new MaterialAlertDialogBuilder(activity)
                .setTitle(R.string.saveload_overwrite_title)
                .setMessage(activity.getString(R.string.saveload_overwrite_message,
                        slotIndex + 1))
                .setPositiveButton(R.string.saveload_overwrite_ok,
                        (d, w) -> performSaveOrLoad(slotIndex))
                .setNegativeButton(android.R.string.cancel, null)
                .show();
    }

    /** Long-press: delete the savegame file with confirmation. */
    private void confirmDelete(final int slotIndex) {
        new MaterialAlertDialogBuilder(activity)
                .setTitle(R.string.saveload_delete_title)
                .setMessage(activity.getString(R.string.saveload_delete_message,
                        slotIndex + 1))
                .setPositiveButton(R.string.saveload_delete_ok, (d, w) -> {
                    final File f = new File(saveDir,
                            SaveFileRepository.saveFile(prefix, slotIndex));
                    new Thread(() -> f.delete(), "ikuradroid-saveload-del").start();
                    reload();
                })
                .setNegativeButton(android.R.string.cancel, null)
                .show();
    }

    private int attrColor(int attr) {
        TypedValue tv = new TypedValue();
        activity.getTheme().resolveAttribute(attr, tv, true);
        return tv.data;
    }

    private int dp(int v) {
        return (int) (v * activity.getResources().getDisplayMetrics().density + 0.5f);
    }

    /** Builds the row container once, outside the holder constructor:
     *  a supertype argument may not call instance methods. */
    private LinearLayout buildRow() {
        LinearLayout row = new LinearLayout(activity);
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setGravity(Gravity.CENTER_VERTICAL);
        int pad = dp(10);
        row.setPadding(pad, dp(6), pad, dp(6));
        // Theme ripple so taps and presses answer like M3 list rows.
        TypedValue tv = new TypedValue();
        activity.getTheme().resolveAttribute(
                android.R.attr.selectableItemBackground, tv, true);
        row.setBackgroundResource(tv.resourceId);
        return row;
    }

    // ----------------------------------------------------------------
    // List adapter: thumbnail, slot number, scene caption, date.
    // ----------------------------------------------------------------
    private class SlotAdapter extends RecyclerView.Adapter<SlotHolder> {
        private final List<SaveFileRepository.Slot> slots = new ArrayList<>();

        void setSlots(List<SaveFileRepository.Slot> loaded) {
            slots.clear();
            slots.addAll(loaded);
            notifyDataSetChanged();
            hint.setText(saveMode ? R.string.saveload_hint_save
                                  : R.string.saveload_hint_load);
            hint.setVisibility(saveMode ? View.VISIBLE : View.GONE);
        }

        @NonNull
        @Override
        public SlotHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            return new SlotHolder(buildRow());
        }

        @Override
        public void onBindViewHolder(@NonNull SlotHolder h, int position) {
            h.bind(slots.get(position));
        }

        @Override
        public int getItemCount() {
            return slots.size();
        }
    }

    private class SlotHolder extends RecyclerView.ViewHolder {
        final LinearLayout root;
        final ImageView thumb;
        final TextView name;
        final TextView message;
        final TextView date;

        SlotHolder(LinearLayout row) {
            super(row);
            root = row;
            thumb = new ImageView(activity);
            LinearLayout.LayoutParams tp = new LinearLayout.LayoutParams(dp(96), dp(72));
            tp.setMarginEnd(dp(12));
            thumb.setLayoutParams(tp);
            thumb.setScaleType(ImageView.ScaleType.FIT_CENTER);
            GradientDrawable ph = new GradientDrawable();
            ph.setColor(0x33000000);
            ph.setCornerRadius(dp(4));
            thumb.setBackground(ph);

            LinearLayout text = new LinearLayout(activity);
            text.setOrientation(LinearLayout.VERTICAL);
            LinearLayout.LayoutParams txp = new LinearLayout.LayoutParams(
                    0, ViewGroup.LayoutParams.WRAP_CONTENT, 1f);
            text.setLayoutParams(txp);
            name = new TextView(activity);
            name.setTextSize(TypedValue.COMPLEX_UNIT_SP, 15);
            name.setTextColor(attrColor(com.google.android.material.R.attr.colorOnSurface));
            name.setTypeface(name.getTypeface(), Typeface.BOLD);
            message = new TextView(activity);
            message.setTextSize(TypedValue.COMPLEX_UNIT_SP, 13);
            message.setTextColor(attrColor(com.google.android.material.R.attr.colorOnSurface));
            message.setMaxLines(2);
            message.setEllipsize(TextUtils.TruncateAt.END);
            date = new TextView(activity);
            date.setTextSize(TypedValue.COMPLEX_UNIT_SP, 12);
            date.setTextColor(attrColor(com.google.android.material.R.attr.colorOnSurfaceVariant));
            text.addView(name);
            text.addView(message);
            text.addView(date);

            root.addView(thumb);
            root.addView(text);
        }

        void bind(final SaveFileRepository.Slot slot) {
            int n = slot.index + 1;
            if (slot.exists) {
                name.setText(activity.getString(R.string.saveload_slot, n));
                name.setAlpha(1f);
                message.setVisibility(slot.message.isEmpty() ? View.GONE : View.VISIBLE);
                message.setText(slot.message);
                date.setVisibility(slot.date.isEmpty() ? View.GONE : View.VISIBLE);
                date.setText(slot.date);
                thumb.setImageBitmap(slot.thumb);
                root.setAlpha(1f);
                root.setOnClickListener(v -> {
                    if (saveMode) {
                        maybeConfirmOverwrite(slot.index);
                    } else {
                        performSaveOrLoad(slot.index);
                    }
                });
                root.setOnLongClickListener(v -> {
                    confirmDelete(slot.index);
                    return true;
                });
            } else {
                name.setText(activity.getString(R.string.saveload_slot_empty, n));
                name.setAlpha(0.6f);
                message.setVisibility(View.GONE);
                date.setVisibility(View.GONE);
                thumb.setImageBitmap(null);
                root.setAlpha(0.55f);
                root.setOnClickListener(null);
                root.setClickable(false);
                root.setOnLongClickListener(null);
            }
        }
    }
}
