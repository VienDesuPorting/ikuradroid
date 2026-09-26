package su.viende.ikuradroid;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.ImageView;

/**
 * ImageView that always measures itself square (height == width). The
 * library tiles show game icons 1:1 and every grid column has the same
 * width, so the icon simply follows the width spec; a plain ImageView
 * cannot express that in XML and ConstraintLayout is not a dependency
 * of this app.
 */
public class SquareImageView extends ImageView {

    public SquareImageView(Context context) {
        super(context);
    }

    public SquareImageView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public SquareImageView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        // Feed the width spec as the height spec: the view is square
        super.onMeasure(widthMeasureSpec, widthMeasureSpec);
    }
}
