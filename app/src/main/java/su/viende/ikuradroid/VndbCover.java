package su.viende.ikuradroid;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;

/**
 * VNDB cover fetching over the kana API (api.vndb.org, anonymous - no
 * account data is involved, and the API already hides the most explicit
 * main covers from unauthenticated clients). The tile's display title
 * doubles as the search term: VNDB matches romaji names, English release
 * titles and aliases, so a manually entered name works.
 *
 * Every method here is blocking and must run on a worker thread
 * (MainActivity drives it); nothing in this class touches the UI.
 */
public final class VndbCover {

    private static final String API_URL = "https://api.vndb.org/kana/vn";
    // VNDB asks API clients to identify themselves in the User-Agent
    private static final String USER_AGENT =
                    "ikuradroid/2.0 (Android; VienDesu! Porting Team)";
    /** Cached covers and dialog thumbnails are downscaled to this max side. */
    private static final int MAX_CACHE_DIM = 720;
    private static final int MAX_THUMB_DIM = 240;
    private static final int CONNECT_TIMEOUT_MS = 10000;
    private static final int READ_TIMEOUT_MS = 15000;

    /** One search result worth showing in the cover picker. */
    public static class Candidate {
        public final String vndbId;    // "v145"
        public final String title;     // main (usually romaji) title
        public final String altTitle;  // English release title, null when absent
        public final String released;  // "1998-07-24", null when unknown
        public final String imageUrl;  // main cover, always set for returned candidates

        Candidate(String vndbId, String title, String altTitle,
                        String released, String imageUrl) {
            this.vndbId = vndbId;
            this.title = title;
            this.altTitle = altTitle;
            this.released = released;
            this.imageUrl = imageUrl;
        }
    }

    private VndbCover() {
    }

    /**
     * Searches VNDB for the query string and returns candidates that
     * carry a visible main cover (entries whose cover is hidden from
     * anonymous access are useless here and get skipped). Returns an
     * empty list when VNDB genuinely found nothing and null when the
     * request itself failed - the automatic cover fetch only retries
     * on null, so a flaky network never marks a tile as searched.
     */
    public static ArrayList<Candidate> search(String query) {
        ArrayList<Candidate> out = new ArrayList<>();
        if (query == null || query.trim().length() == 0) {
            return out;
        }
        try {
            JSONObject body = new JSONObject();
            body.put("filters", new JSONArray()
                            .put("search").put("=").put(query.trim()));
            body.put("fields", "title, titles.lang, titles.title, released, image.url");
            body.put("sort", "searchrank");
            body.put("results", 12);
            JSONObject response = postJson(API_URL, body.toString());
            if (response == null) {
                return null;
            }
            JSONArray results = response.optJSONArray("results");
            for (int i = 0; results != null && i < results.length(); i++) {
                JSONObject vn = results.optJSONObject(i);
                if (vn == null) {
                    continue;
                }
                JSONObject image = vn.optJSONObject("image");
                String imageUrl = image == null ? null : image.optString("url", null);
                if (imageUrl == null || imageUrl.length() == 0) {
                    continue;
                }
                out.add(new Candidate(vn.optString("id", null),
                                vn.optString("title", null),
                                englishTitle(vn.optJSONArray("titles")),
                                vn.optString("released", null),
                                imageUrl));
            }
        } catch (Exception e) {
            // A failed request is null (a genuine no-hit is empty), so
            // the automatic fetch can tell the two apart
            return null;
        }
        return out;
    }

    /** English release title from the titles array; null when there is none. */
    private static String englishTitle(JSONArray titles) {
        for (int i = 0; titles != null && i < titles.length(); i++) {
            JSONObject t = titles.optJSONObject(i);
            if (t != null && "eng".equals(t.optString("lang", null))) {
                String title = t.optString("title", null);
                if (title != null && title.length() > 0) {
                    return title;
                }
            }
        }
        return null;
    }

    /**
     * Downloads a candidate cover, downscales it to the tile cache size
     * and stores it together with its metadata; returns the cached file
     * or null on any failure. Safe to call repeatedly - the file is
     * overwritten in place.
     */
    public static File download(Context context, String folderName,
                    Candidate candidate) {
        Bitmap bitmap = decodeScaled(candidate.imageUrl, MAX_CACHE_DIM);
        if (bitmap == null) {
            return null;
        }
        File file = GameLibrary.coverFile(context, folderName);
        if (file == null) {
            bitmap.recycle();
            return null;
        }
        FileOutputStream out = null;
        try {
            out = new FileOutputStream(file);
            bitmap.compress(Bitmap.CompressFormat.JPEG, 88, out);
        } catch (Exception e) {
            bitmap.recycle();
            return null;
        } finally {
            if (out != null) {
                try {
                    out.close();
                } catch (Exception e) {
                    // Closing failures do not matter here
                }
            }
        }
        bitmap.recycle();
        try {
            JSONObject meta = new JSONObject();
            meta.put("v", candidate.vndbId);
            meta.put("u", candidate.imageUrl);
            meta.put("f", file.getName());
            GameLibrary.setCoverMeta(context, folderName, meta.toString());
        } catch (Exception e) {
            // The file is saved; metadata problems must not fail the fetch
        }
        return file;
    }

    /** Small poster thumbnail for the pick dialog; null on failure. */
    public static Bitmap thumb(Candidate candidate) {
        return decodeScaled(candidate.imageUrl, MAX_THUMB_DIM);
    }

    /**
     * One-shot image download with a two-pass decode: sample read to
     * about maxDim on the long side (never upscaled), so the cached
     * files and thumbnails stay small in memory and on disk.
     */
    private static Bitmap decodeScaled(String url, int maxDim) {
        byte[] raw = httpGet(url);
        if (raw == null || raw.length == 0) {
            return null;
        }
        BitmapFactory.Options bounds = new BitmapFactory.Options();
        bounds.inJustDecodeBounds = true;
        BitmapFactory.decodeByteArray(raw, 0, raw.length, bounds);
        int width = bounds.outWidth;
        int height = bounds.outHeight;
        if (width <= 0 || height <= 0) {
            return null;
        }
        int sample = 1;
        while (Math.max(width, height) / (sample * 2) >= maxDim) {
            sample *= 2;
        }
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inSampleSize = sample;
        Bitmap decoded = BitmapFactory.decodeByteArray(raw, 0, raw.length, opts);
        if (decoded == null) {
            return null;
        }
        int longSide = Math.max(decoded.getWidth(), decoded.getHeight());
        if (longSide > maxDim) {
            float scale = maxDim / (float) longSide;
            Bitmap scaled = Bitmap.createScaledBitmap(decoded,
                            Math.round(decoded.getWidth() * scale),
                            Math.round(decoded.getHeight() * scale), true);
            if (scaled != decoded) {
                decoded.recycle();
            }
            return scaled;
        }
        return decoded;
    }

    /** POSTs a JSON request body and parses the JSON response; null on failure. */
    private static JSONObject postJson(String url, String body) {
        HttpURLConnection conn = null;
        try {
            conn = (HttpURLConnection) new URL(url).openConnection();
            conn.setConnectTimeout(CONNECT_TIMEOUT_MS);
            conn.setReadTimeout(READ_TIMEOUT_MS);
            conn.setRequestMethod("POST");
            conn.setDoOutput(true);
            conn.setRequestProperty("Content-Type", "application/json");
            conn.setRequestProperty("User-Agent", USER_AGENT);
            byte[] payload = body.getBytes("UTF-8");
            conn.setFixedLengthStreamingMode(payload.length);
            OutputStream out = conn.getOutputStream();
            try {
                out.write(payload);
            } finally {
                out.close();
            }
            int code = conn.getResponseCode();
            if (code < 200 || code >= 300) {
                return null;
            }
            return new JSONObject(readAll(conn.getInputStream(), 512 * 1024));
        } catch (Exception e) {
            return null;
        } finally {
            if (conn != null) {
                conn.disconnect();
            }
        }
    }

    /** GETs a URL as capped bytes; null on any failure. */
    private static byte[] httpGet(String url) {
        HttpURLConnection conn = null;
        try {
            conn = (HttpURLConnection) new URL(url).openConnection();
            conn.setConnectTimeout(CONNECT_TIMEOUT_MS);
            conn.setReadTimeout(READ_TIMEOUT_MS);
            conn.setRequestProperty("User-Agent", USER_AGENT);
            int code = conn.getResponseCode();
            if (code < 200 || code >= 300) {
                return null;
            }
            return readBytes(conn.getInputStream(), 12 * 1024 * 1024);
        } catch (Exception e) {
            return null;
        } finally {
            if (conn != null) {
                conn.disconnect();
            }
        }
    }

    private static String readAll(InputStream in, int capBytes) throws Exception {
        return new String(readBytes(in, capBytes), "UTF-8");
    }

    /** Reads a stream fully, bailing out once it grows past capBytes. */
    private static byte[] readBytes(InputStream in, int capBytes) throws Exception {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        try {
            byte[] buffer = new byte[8192];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
                if (out.size() > capBytes) {
                    return null;
                }
            }
            return out.toByteArray();
        } finally {
            in.close();
        }
    }
}
