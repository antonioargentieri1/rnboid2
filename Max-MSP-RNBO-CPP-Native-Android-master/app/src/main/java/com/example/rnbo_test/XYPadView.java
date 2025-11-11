package com.example.rnbo_test;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

/**
 * XYPadView - 2D Touch Control for RNBO Parameters
 * Maps touch X position to one parameter and Y position to another
 */
public class XYPadView extends View {

    private Paint backgroundPaint;
    private Paint gridPaint;
    private Paint cursorPaint;
    private Paint cursorOutlinePaint;
    private Paint textPaint;

    private PointF currentPosition = new PointF(0.5f, 0.5f); // Normalized [0-1]
    private PointF lastTouchPosition = new PointF();
    private boolean isTouching = false;

    private String paramXName = "";
    private String paramYName = "";

    private XYPadListener listener;

    public interface XYPadListener {
        void onValueChanged(float x, float y);
        void onTouchStateChanged(boolean isTouching);
    }

    public XYPadView(Context context) {
        super(context);
        init();
    }

    public XYPadView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    private void init() {
        // Background
        backgroundPaint = new Paint();
        backgroundPaint.setColor(Color.parseColor("#1a1a1a"));
        backgroundPaint.setStyle(Paint.Style.FILL);

        // Grid lines
        gridPaint = new Paint();
        gridPaint.setColor(Color.parseColor("#333333"));
        gridPaint.setStrokeWidth(1);
        gridPaint.setStyle(Paint.Style.STROKE);

        // Cursor (main circle) - white, will toggle between STROKE and FILL
        cursorPaint = new Paint();
        cursorPaint.setColor(Color.WHITE);
        cursorPaint.setStyle(Paint.Style.STROKE);
        cursorPaint.setStrokeWidth(3);
        cursorPaint.setAntiAlias(true);

        // Cursor outline (not used anymore, but kept for compatibility)
        cursorOutlinePaint = new Paint();
        cursorOutlinePaint.setColor(Color.WHITE);
        cursorOutlinePaint.setStyle(Paint.Style.STROKE);
        cursorOutlinePaint.setStrokeWidth(2);
        cursorOutlinePaint.setAntiAlias(true);

        // Text
        textPaint = new Paint();
        textPaint.setColor(Color.WHITE);
        textPaint.setTextSize(32);
        textPaint.setAntiAlias(true);
        textPaint.setTextAlign(Paint.Align.CENTER);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        int width = getWidth();
        int height = getHeight();

        // Draw background
        canvas.drawRect(0, 0, width, height, backgroundPaint);

        // Draw grid (4x4)
        for (int i = 1; i < 4; i++) {
            float x = width * i / 4f;
            float y = height * i / 4f;
            canvas.drawLine(x, 0, x, height, gridPaint);
            canvas.drawLine(0, y, width, y, gridPaint);
        }

        // Draw center crosshair
        gridPaint.setStrokeWidth(2);
        canvas.drawLine(width / 2f, 0, width / 2f, height, gridPaint);
        canvas.drawLine(0, height / 2f, width, height / 2f, gridPaint);
        gridPaint.setStrokeWidth(1);

        // Draw cursor position
        float cursorX = currentPosition.x * width;
        float cursorY = currentPosition.y * height;
        float radius = 30;

        // Toggle cursor style: empty when not touching, filled when touching
        if (isTouching) {
            cursorPaint.setStyle(Paint.Style.FILL);
        } else {
            cursorPaint.setStyle(Paint.Style.STROKE);
        }

        // Draw cursor circle (white empty or white filled based on touch state)
        canvas.drawCircle(cursorX, cursorY, radius, cursorPaint);

        // Draw parameter names if set
        textPaint.setTextSize(28);
        if (!paramXName.isEmpty()) {
            canvas.drawText(paramXName, width / 2f, height - 20, textPaint);
        }
        if (!paramYName.isEmpty()) {
            canvas.save();
            canvas.rotate(-90, 30, height / 2f);
            canvas.drawText(paramYName, 30, height / 2f + 10, textPaint);
            canvas.restore();
        }

        // Draw position values
        textPaint.setTextSize(24);
        textPaint.setColor(Color.parseColor("#AAAAAA"));
        String posText = String.format("X: %.2f  Y: %.2f", currentPosition.x, currentPosition.y);
        canvas.drawText(posText, width / 2f, 40, textPaint);
        textPaint.setColor(Color.WHITE);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float x = event.getX();
        float y = event.getY();

        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                isTouching = true;
                if (listener != null) {
                    listener.onTouchStateChanged(true);
                }
                updatePosition(x, y);
                return true;

            case MotionEvent.ACTION_MOVE:
                updatePosition(x, y);
                return true;

            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                isTouching = false;
                if (listener != null) {
                    listener.onTouchStateChanged(false);
                }
                invalidate();
                return true;
        }

        return super.onTouchEvent(event);
    }

    private void updatePosition(float touchX, float touchY) {
        int width = getWidth();
        int height = getHeight();

        // Normalize to [0-1]
        currentPosition.x = Math.max(0, Math.min(1, touchX / width));
        currentPosition.y = Math.max(0, Math.min(1, touchY / height));

        lastTouchPosition.set(touchX, touchY);

        // Notify listener
        if (listener != null) {
            listener.onValueChanged(currentPosition.x, currentPosition.y);
        }

        invalidate();
    }

    /**
     * Set current position programmatically (normalized [0-1])
     */
    public void setPosition(float x, float y) {
        currentPosition.x = Math.max(0, Math.min(1, x));
        currentPosition.y = Math.max(0, Math.min(1, y));
        invalidate();
    }

    /**
     * Get current X position (normalized [0-1])
     */
    public float getX() {
        return currentPosition.x;
    }

    /**
     * Get current Y position (normalized [0-1])
     */
    public float getY() {
        return currentPosition.y;
    }

    /**
     * Set parameter names for display
     */
    public void setParameterNames(String xParam, String yParam) {
        this.paramXName = xParam != null ? xParam : "";
        this.paramYName = yParam != null ? yParam : "";
        invalidate();
    }

    /**
     * Set listener for value changes
     */
    public void setListener(XYPadListener listener) {
        this.listener = listener;
    }

    /**
     * Clear parameter assignment
     */
    public void clearParameters() {
        this.paramXName = "";
        this.paramYName = "";
        invalidate();
    }
}
