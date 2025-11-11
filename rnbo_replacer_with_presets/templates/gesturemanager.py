"""
GestureManager.java Template - MediaPipe + CameraX Setup (STEP 3)
"""


class GestureManagerTemplate:
    @staticmethod
    def get_content(package_name):
        return f'''package {package_name};

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.camera.core.Camera;
import androidx.camera.core.CameraSelector;
import androidx.camera.core.ImageAnalysis;
import androidx.camera.core.ImageProxy;
import androidx.camera.core.Preview;
import androidx.camera.lifecycle.ProcessCameraProvider;
import androidx.camera.view.PreviewView;
import androidx.core.content.ContextCompat;
import androidx.lifecycle.LifecycleOwner;

import com.google.common.util.concurrent.ListenableFuture;
import com.google.mediapipe.framework.image.BitmapImageBuilder;
import com.google.mediapipe.framework.image.MPImage;
import com.google.mediapipe.tasks.core.BaseOptions;
import com.google.mediapipe.tasks.vision.core.RunningMode;
import com.google.mediapipe.tasks.vision.handlandmarker.HandLandmarker;
import com.google.mediapipe.tasks.vision.handlandmarker.HandLandmarkerResult;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class GestureManager {{
    
    private static final String TAG = "GestureManager";
    private static final String MODEL_FILENAME = "hand_landmarker.task";
    
    private Context context;
    private HandLandmarker handLandmarker;
    private GestureCallback callback;
    
    // CameraX components
    private Camera camera;
    private Preview preview;
    private ImageAnalysis imageAnalysis;
    private ExecutorService cameraExecutor;
    private ProcessCameraProvider cameraProvider;
    private boolean flashEnabled = false;
    
    // Callback interface
    public interface GestureCallback {{
        void onHandsDetected(HandLandmarkerResult result);
        void onError(String error);
    }}
    
    // Constructor
    public GestureManager(Context context) {{
        this.context = context;
        this.cameraExecutor = Executors.newSingleThreadExecutor();
        setupMediaPipe();
    }}
    
    // Setup MediaPipe HandLandmarker
    private void setupMediaPipe() {{
        try {{
            Log.d(TAG, "Setting up MediaPipe HandLandmarker...");
            
            // Get model file from assets
            File modelFile = getModelFile();
            
            // Configure BaseOptions
            BaseOptions baseOptions = BaseOptions.builder()
                .setModelAssetPath(MODEL_FILENAME)
                .build();
            
            // Configure HandLandmarker options (VERSION A: Better detection)
            HandLandmarker.HandLandmarkerOptions options = 
                HandLandmarker.HandLandmarkerOptions.builder()
                    .setBaseOptions(baseOptions)
                    .setRunningMode(RunningMode.IMAGE)
                    .setNumHands(2)  // Detect up to 2 hands (more robust)
                    .setMinHandDetectionConfidence(0.3f)  // Lower threshold for better detection
                    .setMinHandPresenceConfidence(0.3f)   // Lower threshold
                    .setMinTrackingConfidence(0.3f)       // Lower threshold
                    .build();
            
            // Create HandLandmarker
            handLandmarker = HandLandmarker.createFromOptions(context, options);
            
            Log.d(TAG, "MediaPipe HandLandmarker initialized successfully");
            
        }} catch (Exception e) {{
            Log.e(TAG, "Error setting up MediaPipe: " + e.getMessage());
            e.printStackTrace();
            if (callback != null) {{
                callback.onError("Failed to initialize MediaPipe: " + e.getMessage());
            }}
        }}
    }}
    
    // Get model file from assets
    private File getModelFile() throws Exception {{
        File cacheDir = context.getCacheDir();
        File modelFile = new File(cacheDir, MODEL_FILENAME);
        
        // Copy from assets if not exists
        if (!modelFile.exists()) {{
            Log.d(TAG, "Copying model file from assets...");
            InputStream is = context.getAssets().open(MODEL_FILENAME);
            FileOutputStream fos = new FileOutputStream(modelFile);
            
            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = is.read(buffer)) != -1) {{
                fos.write(buffer, 0, bytesRead);
            }}
            
            fos.close();
            is.close();
            Log.d(TAG, "Model file copied successfully");
        }}
        
        return modelFile;
    }}
    
    // Set callback
    public void setCallback(GestureCallback callback) {{
        this.callback = callback;
        Log.d(TAG, "Callback set");
    }}
    
    // Process image (will be used later with camera)
    public void processImage(MPImage image) {{
        if (handLandmarker == null) {{
            Log.e(TAG, "HandLandmarker not initialized");
            return;
        }}
        
        try {{
            // Detect hands
            HandLandmarkerResult result = handLandmarker.detect(image);
            
            // Callback with result
            if (callback != null) {{
                callback.onHandsDetected(result);
            }}
            
        }} catch (Exception e) {{
            Log.e(TAG, "Error processing image: " + e.getMessage());
            if (callback != null) {{
                callback.onError("Error processing image: " + e.getMessage());
            }}
        }}
    }}
    
    // ============================================================================
    // STEP 3: CAMERAX SETUP
    // ============================================================================
    
    /**
     * Start camera with preview
     * @param lifecycleOwner Activity or Fragment lifecycle owner
     * @param previewView PreviewView to display camera feed
     */
    public void startCamera(LifecycleOwner lifecycleOwner, PreviewView previewView) {{
        ListenableFuture<ProcessCameraProvider> cameraProviderFuture = 
            ProcessCameraProvider.getInstance(context);
        
        cameraProviderFuture.addListener(() -> {{
            try {{
                cameraProvider = cameraProviderFuture.get();
                bindCameraUseCases(lifecycleOwner, previewView);
                Log.d(TAG, "Camera started successfully");
            }} catch (ExecutionException | InterruptedException e) {{
                Log.e(TAG, "Error starting camera: " + e.getMessage());
                if (callback != null) {{
                    callback.onError("Failed to start camera: " + e.getMessage());
                }}
            }}
        }}, ContextCompat.getMainExecutor(context));
    }}
    
    /**
     * Bind camera use cases (preview + analysis)
     */
    private void bindCameraUseCases(LifecycleOwner lifecycleOwner, PreviewView previewView) {{
        // Preview use case
        preview = new Preview.Builder()
            .build();
        preview.setSurfaceProvider(previewView.getSurfaceProvider());
        
        // Image analysis use case
        imageAnalysis = new ImageAnalysis.Builder()
            .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
            .build();
        
        imageAnalysis.setAnalyzer(cameraExecutor, imageProxy -> {{
            processImageProxy(imageProxy);
        }});
        
        // VERSION A: Select BACK camera (better quality)
        CameraSelector cameraSelector = CameraSelector.DEFAULT_BACK_CAMERA;
        
        // Unbind all use cases before rebinding
        cameraProvider.unbindAll();
        
        // Bind use cases to camera
        try {{
            camera = cameraProvider.bindToLifecycle(
                lifecycleOwner,
                cameraSelector,
                preview,
                imageAnalysis
            );
            
            // Enable flash if available and if flashEnabled is true
            if (camera.getCameraInfo().hasFlashUnit()) {{
                camera.getCameraControl().enableTorch(flashEnabled);
                Log.d(TAG, "Flash available - state: " + flashEnabled);
            }} else {{
                Log.d(TAG, "Flash not available on this device");
            }}
            
            Log.d(TAG, "Camera use cases bound successfully (BACK camera)");
        }} catch (Exception e) {{
            Log.e(TAG, "Error binding camera use cases: " + e.getMessage());
            if (callback != null) {{
                callback.onError("Failed to bind camera: " + e.getMessage());
            }}
        }}
    }}
    
    /**
     * Process ImageProxy from camera
     */
    private void processImageProxy(ImageProxy imageProxy) {{
        try {{
            // Convert ImageProxy to Bitmap
            Bitmap bitmap = imageProxyToBitmap(imageProxy);
            
            if (bitmap != null) {{
                // Convert Bitmap to MPImage
                MPImage mpImage = new BitmapImageBuilder(bitmap).build();
                
                // Process with MediaPipe
                processImage(mpImage);
                
                // Clean up
                bitmap.recycle();
            }}
            
        }} catch (Exception e) {{
            Log.e(TAG, "Error processing frame: " + e.getMessage());
        }} finally {{
            imageProxy.close();
        }}
    }}
    
    /**
     * Convert ImageProxy to Bitmap
     */
    private Bitmap imageProxyToBitmap(ImageProxy imageProxy) {{
        try {{
            ImageProxy.PlaneProxy[] planes = imageProxy.getPlanes();
            ByteBuffer yBuffer = planes[0].getBuffer();
            ByteBuffer uBuffer = planes[1].getBuffer();
            ByteBuffer vBuffer = planes[2].getBuffer();
            
            int ySize = yBuffer.remaining();
            int uSize = uBuffer.remaining();
            int vSize = vBuffer.remaining();
            
            byte[] nv21 = new byte[ySize + uSize + vSize];
            
            // Copy Y plane
            yBuffer.get(nv21, 0, ySize);
            
            // Copy UV planes
            int uvIndex = ySize;
            for (int i = 0; i < vSize; i++) {{
                nv21[uvIndex++] = vBuffer.get(i);
                nv21[uvIndex++] = uBuffer.get(i);
            }}
            
            // Convert NV21 to Bitmap
            android.graphics.YuvImage yuvImage = new android.graphics.YuvImage(
                nv21,
                android.graphics.ImageFormat.NV21,
                imageProxy.getWidth(),
                imageProxy.getHeight(),
                null
            );
            
            java.io.ByteArrayOutputStream out = new java.io.ByteArrayOutputStream();
            yuvImage.compressToJpeg(
                new android.graphics.Rect(0, 0, imageProxy.getWidth(), imageProxy.getHeight()),
                100,
                out
            );
            
            byte[] imageBytes = out.toByteArray();
            Bitmap bitmap = android.graphics.BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
            
            // Rotate bitmap if needed (back camera - no mirror needed)
            Matrix matrix = new Matrix();
            matrix.postRotate(imageProxy.getImageInfo().getRotationDegrees());
            // No horizontal flip for back camera
            
            return Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
            
        }} catch (Exception e) {{
            Log.e(TAG, "Error converting ImageProxy to Bitmap: " + e.getMessage());
            return null;
        }}
    }}
    
    /**
     * Stop camera and release resources
     */
    public void stopCamera() {{
        if (cameraProvider != null) {{
            cameraProvider.unbindAll();
            Log.d(TAG, "Camera stopped");
        }}
    }}
    
    // ============================================================================
    // VERSION A: FLASH CONTROL
    // ============================================================================
    
    /**
     * Toggle flash on/off
     */
    public void toggleFlash() {{
        if (camera != null && camera.getCameraInfo().hasFlashUnit()) {{
            flashEnabled = !flashEnabled;
            camera.getCameraControl().enableTorch(flashEnabled);
            Log.d(TAG, "Flash toggled: " + flashEnabled);
        }} else {{
            Log.w(TAG, "Flash not available");
        }}
    }}
    
    /**
     * Set flash state
     */
    public void setFlash(boolean enabled) {{
        if (camera != null && camera.getCameraInfo().hasFlashUnit()) {{
            flashEnabled = enabled;
            camera.getCameraControl().enableTorch(flashEnabled);
            Log.d(TAG, "Flash set to: " + flashEnabled);
        }} else {{
            Log.w(TAG, "Flash not available");
        }}
    }}
    
    /**
     * Check if flash is available
     */
    public boolean hasFlash() {{
        return camera != null && camera.getCameraInfo().hasFlashUnit();
    }}
    
    /**
     * Check if flash is enabled
     */
    public boolean isFlashEnabled() {{
        return flashEnabled;
    }}
    
    /**
     * Release all resources (MediaPipe + Camera + Executor)
     */
    public void release() {{
        // Stop camera
        stopCamera();
        
        // Release MediaPipe
        if (handLandmarker != null) {{
            handLandmarker.close();
            handLandmarker = null;
            Log.d(TAG, "HandLandmarker released");
        }}
        
        // Shutdown executor
        if (cameraExecutor != null && !cameraExecutor.isShutdown()) {{
            cameraExecutor.shutdown();
            Log.d(TAG, "Camera executor shutdown");
        }}
    }}
    
    // Check if initialized
    public boolean isInitialized() {{
        return handLandmarker != null;
    }}
}}
'''
