package com.example.rnbo_test;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.View;

import com.google.mediapipe.tasks.vision.handlandmarker.HandLandmarkerResult;
import com.google.mediapipe.tasks.components.containers.NormalizedLandmark;

import java.util.List;

/**
 * Custom View che disegna i landmarks della mano sopra la camera preview
 */
public class HandOverlayView extends View {
    
    private static final String TAG = "HandOverlayView";
    
    // Paint per disegnare
    private Paint landmarkPaint;
    private Paint connectionPaint;
    private Paint wristPaint;
    
    // Dati landmarks
    private HandLandmarkerResult lastResult;
    
    // Hand connections (MediaPipe standard)
    private static final int[][] HAND_CONNECTIONS = {
        // Thumb
        {0, 1}, {1, 2}, {2, 3}, {3, 4},
        // Index finger
        {0, 5}, {5, 6}, {6, 7}, {7, 8},
        // Middle finger
        {0, 9}, {9, 10}, {10, 11}, {11, 12},
        // Ring finger
        {0, 13}, {13, 14}, {14, 15}, {15, 16},
        // Pinky
        {0, 17}, {17, 18}, {18, 19}, {19, 20},
        // Palm
        {5, 9}, {9, 13}, {13, 17}
    };
    
    public HandOverlayView(Context context) {
        super(context);
        init();
    }
    
    public HandOverlayView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }
    
    public HandOverlayView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }
    
    private void init() {
        // Paint per i landmark points
        landmarkPaint = new Paint();
        landmarkPaint.setColor(Color.RED);
        landmarkPaint.setStyle(Paint.Style.FILL);
        landmarkPaint.setStrokeWidth(8f);
        landmarkPaint.setAntiAlias(true);
        
        // Paint per il wrist (polso) - più grande
        wristPaint = new Paint();
        wristPaint.setColor(Color.GREEN);
        wristPaint.setStyle(Paint.Style.FILL);
        wristPaint.setStrokeWidth(12f);
        wristPaint.setAntiAlias(true);
        
        // Paint per le connessioni (linee)
        connectionPaint = new Paint();
        connectionPaint.setColor(Color.CYAN);
        connectionPaint.setStyle(Paint.Style.STROKE);
        connectionPaint.setStrokeWidth(4f);
        connectionPaint.setAntiAlias(true);
    }
    
    /**
     * Update landmarks e ridisegna
     */
    public void updateLandmarks(HandLandmarkerResult result) {
        this.lastResult = result;
        postInvalidate(); // Trigger redraw
    }
    
    /**
     * Clear landmarks
     */
    public void clear() {
        this.lastResult = null;
        postInvalidate();
    }
    
    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        
        if (lastResult == null || lastResult.landmarks().isEmpty()) {
            return; // Nothing to draw
        }
        
        // Get view dimensions
        int viewWidth = getWidth();
        int viewHeight = getHeight();
        
        if (viewWidth == 0 || viewHeight == 0) {
            return;
        }
        
        // Draw each detected hand
        for (List<NormalizedLandmark> landmarks : lastResult.landmarks()) {
            if (landmarks.size() != 21) {
                continue; // Skip invalid data
            }
            
            // Draw connections first (so they appear behind points)
            drawConnections(canvas, landmarks, viewWidth, viewHeight);
            
            // Draw landmarks
            drawLandmarks(canvas, landmarks, viewWidth, viewHeight);
        }
    }
    
    /**
     * Draw landmark points
     */
    private void drawLandmarks(Canvas canvas, List<NormalizedLandmark> landmarks, 
                               int viewWidth, int viewHeight) {
        for (int i = 0; i < landmarks.size(); i++) {
            NormalizedLandmark landmark = landmarks.get(i);
            
            // Convert normalized coordinates (0-1) to pixel coordinates
            // Direct mapping (camera already mirrored)
            float x = landmark.x() * viewWidth;
            float y = landmark.y() * viewHeight;
            
            // Draw wrist (landmark 0) larger and different color
            if (i == 0) {
                canvas.drawCircle(x, y, 12f, wristPaint);
            } else {
                canvas.drawCircle(x, y, 8f, landmarkPaint);
            }
        }
    }
    
    /**
     * Draw connections between landmarks
     */
    private void drawConnections(Canvas canvas, List<NormalizedLandmark> landmarks,
                                 int viewWidth, int viewHeight) {
        for (int[] connection : HAND_CONNECTIONS) {
            int startIdx = connection[0];
            int endIdx = connection[1];
            
            if (startIdx >= landmarks.size() || endIdx >= landmarks.size()) {
                continue;
            }
            
            NormalizedLandmark start = landmarks.get(startIdx);
            NormalizedLandmark end = landmarks.get(endIdx);
            
            // Convert to pixel coordinates (direct)
            float startX = start.x() * viewWidth;
            float startY = start.y() * viewHeight;
            float endX = end.x() * viewWidth;
            float endY = end.y() * viewHeight;
            
            canvas.drawLine(startX, startY, endX, endY, connectionPaint);
        }
    }
}
