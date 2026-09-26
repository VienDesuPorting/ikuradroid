package su.viende.ikuradroid;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.ImageView;

/**
 * ImageView that always measures itself at a 2:3 width:height ratio -
 * the poster proportions of the VNDB covers the library shows since the
 * cover stage (the class was SquareImageView, 1:1, before it). Every
 * grid column has the same width, so the height simply follows the
 * width spec; a plain ImageView cannot express that in XML and
 * ConstraintLayout is not a dependency of this app.
 */
public class TileImageView extends ImageView {

    public TileImageView(Context context) {
        super(context);
    }

    public TileImageView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public TileImageView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        // Height spec derived from the width at exactly 3/2; rounding
        // keeps neighboring tiles seam-free on fractional densities
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int heightSpec = MeasureSpec.makeMeasureSpec(
                        Math.round(width * 3f / 2f), MeasureSpec.EXACTLY);
        super.onMeasure(widthMeasureSpec, heightSpec);
    }
}
