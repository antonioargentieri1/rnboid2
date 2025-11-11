package com.example.rnbo_test;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

/**
 * Pure Data style slider with text inside
 * White outline rectangle that fills with white, text masked to be visible
 */
public class PureDataSlider extends View {

    private Paint outlinePaint;
    private Paint fillPaint;
    private Paint textWhitePaint;
    private Paint textBlackPaint;

    private String paramName = "";
    private float currentValue = 0f;
    private float progress = 0f; // 0.0 to 1.0
    private int maxValue = 1000;

    private OnProgressChangeListener listener;

    public interface OnProgressChangeListener {
        void onProgressChanged(int progress);
    }

    public PureDataSlider(Context context) {
        super(context);
        init();
    }

    public PureDataSlider(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    private void init() {
        // White outline
        outlinePaint = new Paint();
        outlinePaint.setColor(Color.WHITE);
        outlinePaint.setStyle(Paint.Style.STROKE);
        outlinePaint.setStrokeWidth(3);
        outlinePaint.setAntiAlias(true);

        // White fill
        fillPaint = new Paint();
        fillPaint.setColor(Color.WHITE);
        fillPaint.setStyle(Paint.Style.FILL);
        fillPaint.setAntiAlias(true);

        // White text (for empty part)
        textWhitePaint = new Paint();
        textWhitePaint.setColor(Color.WHITE);
        textWhitePaint.setTextSize(40);
        textWhitePaint.setAntiAlias(true);
        textWhitePaint.setTextAlign(Paint.Align.LEFT);

        // Black text (for filled part)
        textBlackPaint = new Paint();
        textBlackPaint.setColor(Color.BLACK);
        textBlackPaint.setTextSize(40);
        textBlackPaint.setAntiAlias(true);
        textBlackPaint.setTextAlign(Paint.Align.LEFT);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        int width = getWidth();
        int height = getHeight();

        // Draw white outline
        RectF rect = new RectF(1, 1, width - 1, height - 1);
        canvas.drawRect(rect, outlinePaint);

        // Calculate fill width based on progress
        float fillWidth = progress * (width - 2);

        // Draw white fill
        if (fillWidth > 0) {
            RectF fillRect = new RectF(1, 1, 1 + fillWidth, height - 1);
            canvas.drawRect(fillRect, fillPaint);
        }

        // Prepare text
        String text = String.format("%s: %.2f", paramName, currentValue);
        float textX = 12;
        float textY = height / 2f + 14; // Center vertically

        // Draw white text for empty part (clipped)
        canvas.save();
        canvas.clipRect(1 + fillWidth, 0, width, height);
        canvas.drawText(text, textX, textY, textWhitePaint);
        canvas.restore();

        // Draw black text for filled part (clipped)
        if (fillWidth > 0) {
            canvas.save();
            canvas.clipRect(1, 0, 1 + fillWidth, height);
            canvas.drawText(text, textX, textY, textBlackPaint);
            canvas.restore();
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float x = event.getX();
        int width = getWidth();

        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_MOVE:
                // Calculate progress from touch position
                progress = Math.max(0f, Math.min(1f, x / width));
                int newProgress = (int)(progress * maxValue);

                if (listener != null) {
                    listener.onProgressChanged(newProgress);
                }

                invalidate();
                return true;

            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                return true;
        }

        return super.onTouchEvent(event);
    }

    public void setProgress(int progress) {
        this.progress = progress / (float)maxValue;
        invalidate();
    }

    public void setMax(int max) {
        this.maxValue = max;
    }

    public void setParameterName(String name) {
        this.paramName = name;
        invalidate();
    }

    public void setCurrentValue(float value) {
        this.currentValue = value;
        invalidate();
    }

    public void setOnProgressChangeListener(OnProgressChangeListener listener) {
        this.listener = listener;
    }
}
