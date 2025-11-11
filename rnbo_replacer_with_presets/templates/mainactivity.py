"""
MainActivity Template Generator - STEP 4: GestureManager Integration
Reads the MainActivity Java template and injects package name + GestureManager
"""

from pathlib import Path


class MainActivityTemplate:
    # Path to the extracted template
    TEMPLATE_FILE = Path(__file__).parent.parent / "mainactivity_extracted.txt"
    
    @staticmethod
    def generate(package_name, parameters=None, enable_gesture=True):
        """
        Generate MainActivity.java with the given package name
        
        Args:
            package_name: Java package name (e.g., "com.example.rnbo_test")
            parameters: List of parameter dicts (optional, for future use)
            enable_gesture: Enable GestureManager integration (STEP 4)
            
        Returns:
            String containing the complete MainActivity.java content
        """
        # Read template
        try:
            with open(MainActivityTemplate.TEMPLATE_FILE, 'r', encoding='utf-8') as f:
                template = f.read()
        except FileNotFoundError:
            # Fallback: use minimal template
            return MainActivityTemplate._get_minimal_template(package_name)
        
        # Replace package name placeholders
        content = template.replace('{package_name}', package_name)
        
        # STEP 4: Inject GestureManager integration
        if enable_gesture:
            content = MainActivityTemplate._inject_gesture_manager(content, package_name)
        
        return content
    
    @staticmethod
    def _inject_gesture_manager(content, package_name):
        """
        STEP 4: Inject GestureManager integration into MainActivity
        """
        # 1. Add imports after existing imports
        import_injection = '''
// STEP 4: GestureManager imports (DISABLED - TODO: Re-enable later)
/*
import androidx.camera.view.PreviewView;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import android.Manifest;
import android.content.pm.PackageManager;
import android.widget.Toast;
import com.google.mediapipe.tasks.vision.handlandmarker.HandLandmarkerResult;
*/
'''
        
        # Find position after last import
        last_import = content.rfind('import ')
        semicolon_pos = content.find(';', last_import)
        content = content[:semicolon_pos+1] + import_injection + content[semicolon_pos+1:]
        
        # 2. Add GestureManager field in class
        field_injection = '''
    
    // STEP 4: GestureManager integration (DISABLED - TODO: Re-enable later)
    /*
    private GestureManager gestureManager;
    private PreviewView cameraPreview;
    private HandOverlayView handOverlay;
    private static final int CAMERA_PERMISSION_REQUEST = 100;
    private boolean cameraEnabled = false;
    */
'''
        
        # Find position after "private ActivityMainBinding binding;"
        binding_pos = content.find('private ActivityMainBinding binding;')
        semicolon_pos = content.find(';', binding_pos)
        content = content[:semicolon_pos+1] + field_injection + content[semicolon_pos+1:]
        
        # 3. Add initialization in onCreate (after setContentView)
        oncreate_injection = '''
        
        // STEP 4: Initialize GestureManager (DISABLED - TODO: Re-enable later)
        // initializeGestureManager();
'''
        
        # Find setContentView in onCreate
        setcontent_pos = content.find('setContentView(binding.getRoot());')
        semicolon_pos = content.find(';', setcontent_pos)
        content = content[:semicolon_pos+1] + oncreate_injection + content[semicolon_pos+1:]
        
        # 4. UPDATE existing onPause method (don't create new one)
        old_onpause = '''    @Override
    protected void onPause() {
        super.onPause();
        unregisterSensorListeners();
    }'''
        
        new_onpause = '''    @Override
    protected void onPause() {
        super.onPause();
        unregisterSensorListeners();
        
        // STEP 4: Stop camera when app goes to background (DISABLED)
        /*
        if (gestureManager != null) {
            gestureManager.stopCamera();
        }
        */
    }'''
        
        content = content.replace(old_onpause, new_onpause)
        
        # 5. UPDATE existing onResume method (don't create new one)
        old_onresume = '''    @Override
    protected void onResume() {
        super.onResume();
        if (!currentMode.equals("setup")) {
            registerSensorListeners();
        }
    }'''
        
        new_onresume = '''    @Override
    protected void onResume() {
        super.onResume();
        if (!currentMode.equals("setup")) {
            registerSensorListeners();
        }
        
        // STEP 4: Restart camera if it was enabled (DISABLED)
        /*
        if (cameraEnabled && gestureManager != null && cameraPreview != null) {
            gestureManager.startCamera(this, cameraPreview);
        }
        */
    }'''
        
        content = content.replace(old_onresume, new_onresume)
        
        # 6. Add GestureManager methods at the end of class (before last })
        # But DON'T add onResume, onPause, onDestroy duplicates
        methods_injection = '''
    
    // ============================================================================
    // STEP 4: GESTUREMANAGER INTEGRATION (DISABLED - TODO: Re-enable later)
    // ============================================================================
    
    /*
    **
     * Initialize GestureManager and setup camera
     *
    private void initializeGestureManager() {
        Log.d("MainActivity", "🎥 Initializing GestureManager...");
        
        // Create GestureManager
        gestureManager = new GestureManager(this);
        
        // Set callback for hand detection
        gestureManager.setCallback(new GestureManager.GestureCallback() {
            @Override
            public void onHandsDetected(HandLandmarkerResult result) {
                handleHandDetection(result);
            }
            
            @Override
            public void onError(String error) {
                Log.e("MainActivity", "GestureManager error: " + error);
                runOnUiThread(() -> {
                    Toast.makeText(MainActivity.this, 
                        "Hand tracking error: " + error, 
                        Toast.LENGTH_SHORT).show();
                });
            }
        });
        
        // Get PreviewView from layout (if exists)
        cameraPreview = findViewById(R.id.cameraPreview);
        handOverlay = findViewById(R.id.handOverlay);
        
        // Request camera permission if not granted
        if (checkCameraPermission()) {
            startHandTracking();
        } else {
            requestCameraPermission();
        }
    }
    
    **
     * Handle hand detection results
     *
    private void handleHandDetection(HandLandmarkerResult result) {
        int numHands = result.landmarks().size();
        
        if (numHands > 0) {
            // Log detection (for STEP 4 testing)
            int numLandmarks = result.landmarks().get(0).size();
            Log.d("HandTracking", "[HAND] Hand detected! " + numLandmarks + " landmarks");
            
            // Get wrist position (landmark 0)
            float wristX = result.landmarks().get(0).get(0).x();
            float wristY = result.landmarks().get(0).get(0).y();
            Log.d("HandTracking", "   Wrist: x=" + String.format("%.3f", wristX) + 
                  " y=" + String.format("%.3f", wristY));
            
            // TODO STEP 6: Add gesture recognition here
            
            // Update overlay view with landmarks
            if (handOverlay != null) {
                handOverlay.updateLandmarks(result);
            }
            // TODO STEP 7: Map gestures to RNBO parameters
            
        } else {
            // No hand detected
            Log.d("HandTracking", "⚪ No hand");
            
            // Clear overlay
            if (handOverlay != null) {
                handOverlay.clear();
            }
        }
    }
    
    **
     * Start hand tracking with camera
     *
    private void startHandTracking() {
        if (gestureManager != null && cameraPreview != null) {
            Log.d("MainActivity", "[CAMERA] Starting camera for hand tracking...");
            gestureManager.startCamera(this, cameraPreview);
            cameraEnabled = true;
            
            runOnUiThread(() -> {
                Toast.makeText(this, 
                    "Hand tracking started! Show your hand", 
                    Toast.LENGTH_LONG).show();
            });
        } else {
            Log.w("MainActivity", "[WARN] Cannot start camera: " + 
                  (gestureManager == null ? "GestureManager null" : "PreviewView null"));
        }
    }
    
    **
     * Stop hand tracking
     *
    private void stopHandTracking() {
        if (gestureManager != null) {
            gestureManager.stopCamera();
            cameraEnabled = false;
            Log.d("MainActivity", "[CAMERA] Camera stopped");
        }
    }
    
    **
     * Toggle hand tracking on/off
     *
    public void toggleHandTracking() {
        if (cameraEnabled) {
            stopHandTracking();
            Toast.makeText(this, "Hand tracking OFF", Toast.LENGTH_SHORT).show();
        } else {
            if (checkCameraPermission()) {
                startHandTracking();
            } else {
                requestCameraPermission();
            }
        }
    }
    
    **
     * STEP 5: onClick handler for camera toggle button
     *
    public void onToggleCameraClick(View view) {
        toggleHandTracking();
    }
    
    **
     * VERSION A: onClick handler for flash toggle button
     *
    public void onToggleFlashClick(View view) {
        if (gestureManager != null) {
            if (gestureManager.hasFlash()) {
                gestureManager.toggleFlash();
                
                // Update button text
                Button btn = (Button) view;
                if (gestureManager.isFlashEnabled()) {
                    btn.setText("💡 Flash ON");
                    Toast.makeText(this, "Flash ON", Toast.LENGTH_SHORT).show();
                } else {
                    btn.setText("💡 Flash OFF");
                    Toast.makeText(this, "Flash OFF", Toast.LENGTH_SHORT).show();
                }
            } else {
                Toast.makeText(this, "Flash not available", Toast.LENGTH_SHORT).show();
            }
        }
    }
    
    **
     * Check if camera permission is granted
     *
    private boolean checkCameraPermission() {
        return ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                == PackageManager.PERMISSION_GRANTED;
    }
    
    **
     * Request camera permission
     *
    private void requestCameraPermission() {
        Log.d("MainActivity", "📸 Requesting camera permission...");
        ActivityCompat.requestPermissions(this,
                new String[]{Manifest.permission.CAMERA},
                CAMERA_PERMISSION_REQUEST);
    }
    
    **
     * Handle permission result
     *
    @Override
    public void onRequestPermissionsResult(int requestCode, 
                                          @NonNull String[] permissions, 
                                          @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        
        if (requestCode == CAMERA_PERMISSION_REQUEST) {
            if (grantResults.length > 0 && 
                grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                Log.d("MainActivity", "[PERMISSION] Camera permission granted");
                startHandTracking();
            } else {
                Log.w("MainActivity", "[PERMISSION] Camera permission denied");
                Toast.makeText(this, 
                    "Camera permission required for hand tracking", 
                    Toast.LENGTH_LONG).show();
            }
        }
    }
    */
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Release GestureManager resources (DISABLED)
        /*
        if (gestureManager != null) {
            gestureManager.release();
            gestureManager = null;
        }
        */
    }
'''
        
        # Find last closing brace of class
        last_brace = content.rfind('}')
        content = content[:last_brace] + methods_injection + '\n' + content[last_brace:]
        
        return content
    
    @staticmethod
    def _get_minimal_template(package_name):
        """Fallback minimal template if extraction fails"""
        return f'''package {package_name};

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import {package_name}.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {{

    static {{
        System.loadLibrary("rnbo_test");
    }}

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {{
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        init(getAssets());
    }}

    public void declareSliders(@NonNull String[] paramNames) {{
        // Minimal implementation
    }}

    public native void init(android.content.res.AssetManager assetManager);
    public native void updateParam(String name, float val);
}}
'''
