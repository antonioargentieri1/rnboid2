package com.example.rnbo_test;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.AppCompatSeekBar;

import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.ColorStateList;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.drawable.GradientDrawable;
import android.graphics.drawable.LayerDrawable;
import android.graphics.drawable.ClipDrawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Toast;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.google.android.material.slider.RangeSlider;

import com.example.rnbo_test.databinding.ActivityMainBinding;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
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


public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("rnbo_test");
    }

    private ActivityMainBinding binding;
    
    // STEP 4: GestureManager integration (DISABLED - TODO: Re-enable later)
    /*
    private GestureManager gestureManager;
    private PreviewView cameraPreview;
    private HandOverlayView handOverlay;
    private static final int CAMERA_PERMISSION_REQUEST = 100;
    private boolean cameraEnabled = false;
    */

    private Map<String, PureDataSlider> sliders;
    private LinearLayout paramHolder;
    private SliderListener msliderListener;

    // Buffer Management
    private static final int REQUEST_CODE_PICK_AUDIO_FILE = 200;
    private String currentBufferIdToLoad = null;

    // Multi-Mode System
    private String currentMode = "live";
    private Set<String> liveParameters = new HashSet<>();
    private String[] allParamNames;
    
    // Slider position storage
    private Map<String, Integer> sliderPositions = new HashMap<>();
    private Map<String, RangeInfo> lastUsedRanges = new HashMap<>();
    private Map<String, PureDataSlider> activeSliders = new HashMap<>();
    private Map<String, TextView> sliderValueLabels = new HashMap<>();
    
    private static class RangeInfo {
        double min;
        double max;
        
        RangeInfo(double min, double max) {
            this.min = min;
            this.max = max;
        }
    }
    
    private LinearLayout modeButtonsContainer;
    private Button btnLiveMode;
    private Button btnAllMode;
    private Button btnPresetsMode;
    private Button btnSetupMode;
    private Button btnSensorToggle;
    private Button btnXyPad;
    private LinearLayout toggleButtonsRow;
    private boolean xyPadEnabled = false;
    private XYPadView xyPadView;
    private float currentXyPadX = 0.5f;
    private float currentXyPadY = 0.5f;
    private boolean currentXyPadTouching = false;
    private List<CheckBox> setupCheckboxes = new ArrayList<>();
    
    // Presets system
    private List<Preset> savedPresets = new ArrayList<>();
    private static final String PRESETS_PREF_KEY = "saved_presets";
    
    private static class Preset {
        String name;
        long timestamp;
        Map<String, SensorControl> sensorControls;
        Map<String, CustomRange> customRanges;
        Set<String> liveParameters;
        boolean sensorsGloballyEnabled;
        Map<String, Float> parameterValues;
        boolean isQuickPreset;
    
        Preset(String name) {
            this.name = name;
            this.timestamp = System.currentTimeMillis();
            this.sensorControls = new HashMap<>();
            this.customRanges = new HashMap<>();
            this.liveParameters = new HashSet<>();
            this.sensorsGloballyEnabled = true;
            this.parameterValues = new HashMap<>();
            this.isQuickPreset = false;
        }
    }
    
    // UI Containers for fixed header
    private LinearLayout fixedHeader;
    private LinearLayout liveFixedControls;
    
    // Interpolation system
    private boolean interpolationEnabled = false;
    private int interpolationTimeMs = 1000;
    private boolean isInterpolating = false;
    private Map<String, Float> interpolationStartValues = new HashMap<>();
    private Map<String, Float> interpolationTargetValues = new HashMap<>();
    private long interpolationStartTime = 0;
    private android.os.Handler interpolationHandler = new android.os.Handler();
    private Runnable interpolationRunnable;
    
    // Quick presets
    private List<Preset> quickPresets = new ArrayList<>();
    private static final int MAX_QUICK_PRESETS = 8;
    
    // Sensor system
    private SensorManager sensorManager;
    private Sensor accelerometer;
    private Sensor gyroscope;
    private boolean sensorsGloballyEnabled = true;
    private Map<String, SensorControl> sensorControls = new HashMap<>();
    private Map<String, Float> lastSensorValues = new HashMap<>();
    
    // Auto-detected available sensors
    private List<String> availableSensorTypes = new ArrayList<>();
    private Map<String, Sensor> sensorMap = new HashMap<>();
    private Map<String, float[]> sensorRanges = new HashMap<>();
    
    private static class SensorControl {
        String controlMode;      // "manual" or "sensor"
        String sensorType;       // "Accelerometer", "Light", "Gyroscope", etc
        String dataSource;       // "X", "Y", "Z", "Lux", "Distance", "Magnitude", etc
        float sensitivity;       // 0.0 - 1.0
        boolean invert;
        float smoothing;         // 0.0 - 1.0 (low-pass filter)
        
        SensorControl() {
            this.controlMode = "manual";
            this.sensorType = "manual";
            this.dataSource = "";
            this.sensitivity = 0.5f;
            this.invert = false;
            this.smoothing = 0.3f;
        }
    }
    
    // Parameter info storage
    private Map<String, ParameterInfo> parameterInfoMap = new HashMap<>();
    private Map<String, CustomRange> customRangeMap = new HashMap<>();
    
    private static class ParameterInfo {
        String name;
        double min;
        double max;
        double initialValue;
        
        ParameterInfo(String name, double min, double max, double initialValue) {
            this.name = name;
            this.min = min;
            this.max = max;
            this.initialValue = initialValue;
        }
    }
    
    private static class CustomRange {
        boolean enabled;
        double customMin;
        double customMax;
        
        CustomRange(boolean enabled, double customMin, double customMax) {
            this.enabled = enabled;
            this.customMin = customMin;
            this.customMax = customMax;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        
        // STEP 4: Initialize GestureManager (DISABLED - TODO: Re-enable later)
        // initializeGestureManager();

        
        // Get UI containers
        fixedHeader = findViewById(R.id.fixed_header);
        liveFixedControls = findViewById(R.id.live_fixed_controls);
        paramHolder = binding.ParamHolder;
        
        // Initialize sensors
        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        gyroscope = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
        
        // Auto-detect available sensors
        detectAvailableSensors();
        initializeSensorRanges();
        
        loadParameterInfoFromJSON();
        loadLiveConfig();
        loadCustomRanges();
        loadSensorControls();
        loadPresetsFromStorage();
        loadQuickPresets();
        loadInterpolationSettings();
        createModeButtons();
        init(getAssets());
    }
    
    private void loadParameterInfoFromJSON() {
        try {
            InputStream is = getAssets().open("description.json");
            int size = is.available();
            byte[] buffer = new byte[size];
            is.read(buffer);
            is.close();
            
            String json = new String(buffer, "UTF-8");
            JSONObject obj = new JSONObject(json);
            JSONArray params = obj.getJSONArray("parameters");
            
            for (int i = 0; i < params.length(); i++) {
                JSONObject param = params.getJSONObject(i);
                String name = param.getString("name");
                double min = param.getDouble("minimum");
                double max = param.getDouble("maximum");
                double initial = param.getDouble("initialValue");
                
                parameterInfoMap.put(name, new ParameterInfo(name, min, max, initial));
                Log.i("RNBO_MultiMode", String.format("Loaded param: %s [%.2f - %.2f] initial=%.2f", 
                    name, min, max, initial));
            }
        } catch (Exception e) {
            Log.e("RNBO_MultiMode", "Error loading description.json: " + e.getMessage());
        }
    }
    
    private void createModeButtons() {
        modeButtonsContainer = new LinearLayout(this);
        modeButtonsContainer.setOrientation(LinearLayout.HORIZONTAL);
        modeButtonsContainer.setGravity(Gravity.CENTER);
        LayoutParams containerParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
        containerParams.setMargins(0, 10, 0, 10);
        modeButtonsContainer.setLayoutParams(containerParams);
        
        btnLiveMode = new Button(this);
        btnLiveMode.setText("LIVE");
        btnLiveMode.setOnClickListener(v -> switchMode("live"));
        
        btnAllMode = new Button(this);
        btnAllMode.setText("ALL");
        btnAllMode.setOnClickListener(v -> switchMode("all"));
        
        btnPresetsMode = new Button(this);
        btnPresetsMode.setText("PRESETS");
        btnPresetsMode.setOnClickListener(v -> switchMode("presets"));
        
        btnSetupMode = new Button(this);
        btnSetupMode.setText("SETUP");
        btnSetupMode.setOnClickListener(v -> switchMode("setup"));
        
        LayoutParams buttonParams = new LayoutParams(0, LayoutParams.WRAP_CONTENT, 1.0f);
        buttonParams.setMargins(5, 0, 5, 0);
        btnLiveMode.setLayoutParams(buttonParams);
        btnAllMode.setLayoutParams(buttonParams);
        btnPresetsMode.setLayoutParams(buttonParams);
        btnSetupMode.setLayoutParams(buttonParams);
        
        modeButtonsContainer.addView(btnLiveMode);
        modeButtonsContainer.addView(btnAllMode);
        modeButtonsContainer.addView(btnPresetsMode);
        modeButtonsContainer.addView(btnSetupMode);

        // Add to FIXED HEADER at index 0 (before live_fixed_controls)
        fixedHeader.addView(modeButtonsContainer, 0);
        
        // Create Sensor Toggle button
        createSensorToggleButton();
    }
    
    private void createSensorToggleButton() {
        // Create horizontal container for SENSOR and XY PAD buttons
        toggleButtonsRow = new LinearLayout(this);
        toggleButtonsRow.setOrientation(LinearLayout.HORIZONTAL);
        toggleButtonsRow.setGravity(Gravity.CENTER);
        LayoutParams rowParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
        rowParams.setMargins(20, 10, 20, 10);
        toggleButtonsRow.setLayoutParams(rowParams);

        // Create SENSOR toggle button
        btnSensorToggle = new Button(this);
        updateSensorToggleButton();
        btnSensorToggle.setOnClickListener(v -> toggleSensorsGlobally());
        LayoutParams sensorParams = new LayoutParams(0, LayoutParams.WRAP_CONTENT, 1.0f);
        sensorParams.setMargins(0, 0, 5, 0);
        btnSensorToggle.setLayoutParams(sensorParams);
        toggleButtonsRow.addView(btnSensorToggle);

        // Create XY PAD toggle button
        btnXyPad = new Button(this);
        updateXyPadButton();
        btnXyPad.setOnClickListener(v -> toggleXyPad());
        LayoutParams xyPadParams = new LayoutParams(0, LayoutParams.WRAP_CONTENT, 1.0f);
        xyPadParams.setMargins(5, 0, 0, 0);
        btnXyPad.setLayoutParams(xyPadParams);
        toggleButtonsRow.addView(btnXyPad);

        // Add row to live fixed controls
        liveFixedControls.addView(toggleButtonsRow);
        toggleButtonsRow.setVisibility(View.GONE);

        updateModeButtonStates();
    }
    
    private void switchMode(String mode) {
        currentMode = mode;
        updateModeButtonStates();
        if (allParamNames != null) {
            refreshParameterDisplay();
        }
    }
    
    private void updateModeButtonStates() {
        // Inactive state: gray background, white text
        btnLiveMode.setBackgroundColor(Color.parseColor("#666666"));
        btnLiveMode.setTextColor(Color.WHITE);
        btnAllMode.setBackgroundColor(Color.parseColor("#666666"));
        btnAllMode.setTextColor(Color.WHITE);
        btnPresetsMode.setBackgroundColor(Color.parseColor("#666666"));
        btnPresetsMode.setTextColor(Color.WHITE);
        btnSetupMode.setBackgroundColor(Color.parseColor("#666666"));
        btnSetupMode.setTextColor(Color.WHITE);

        // Hide live fixed controls by default
        liveFixedControls.setVisibility(View.GONE);
        toggleButtonsRow.setVisibility(View.GONE);

        // If leaving live mode, hide XY pad and show sliders
        if (!currentMode.equals("live") && xyPadEnabled) {
            xyPadEnabled = false;
            updateXyPadButton();
            if (xyPadView != null) {
                xyPadView.setVisibility(View.GONE);
            }
            paramHolder.setVisibility(View.VISIBLE);
        }

        switch (currentMode) {
            case "live":
                // Active state: white background, black text
                btnLiveMode.setBackgroundColor(Color.WHITE);
                btnLiveMode.setTextColor(Color.BLACK);
                liveFixedControls.setVisibility(View.VISIBLE);
                toggleButtonsRow.setVisibility(View.VISIBLE);
                registerSensorListeners();
                break;
            case "all":
                btnAllMode.setBackgroundColor(Color.WHITE);
                btnAllMode.setTextColor(Color.BLACK);
                registerSensorListeners();
                break;
            case "presets":
                btnPresetsMode.setBackgroundColor(Color.WHITE);
                btnPresetsMode.setTextColor(Color.BLACK);
                unregisterSensorListeners();
                break;
            case "setup":
                btnSetupMode.setBackgroundColor(Color.WHITE);
                btnSetupMode.setTextColor(Color.BLACK);
                unregisterSensorListeners();
                break;
        }
    }
    
    private void toggleSensorsGlobally() {
        sensorsGloballyEnabled = !sensorsGloballyEnabled;
        updateSensorToggleButton();
        saveSensorGlobalState();
        
        if (sensorsGloballyEnabled) {
            registerSensorListeners();
        } else {
            unregisterSensorListeners();
        }
        
        Log.i("RNBO_MultiMode", "Sensors globally: " + (sensorsGloballyEnabled ? "ENABLED" : "DISABLED"));
    }
    
    private void updateSensorToggleButton() {
        android.graphics.drawable.GradientDrawable drawable = new android.graphics.drawable.GradientDrawable();
        drawable.setCornerRadius(8);
        drawable.setStroke(3, Color.WHITE);

        if (sensorsGloballyEnabled) {
            btnSensorToggle.setText("SENSOR ON");
            drawable.setColor(Color.WHITE);
            btnSensorToggle.setTextColor(Color.BLACK);
        } else {
            btnSensorToggle.setText("SENSOR OFF");
            drawable.setColor(Color.BLACK);
            btnSensorToggle.setTextColor(Color.WHITE);
        }
        btnSensorToggle.setBackground(drawable);
    }

    private void toggleXyPad() {
        xyPadEnabled = !xyPadEnabled;
        updateXyPadButton();

        if (xyPadEnabled) {
            // Disable sensors when XY PAD is active
            if (sensorsGloballyEnabled) {
                sensorsGloballyEnabled = false;
                updateSensorToggleButton();
                unregisterSensorListeners();
            }

            // Hide sliders
            paramHolder.setVisibility(View.GONE);

            // Create XY PAD view if it doesn't exist
            if (xyPadView == null) {
                xyPadView = new XYPadView(this);
                LayoutParams xyPadParams = new LayoutParams(
                    LayoutParams.MATCH_PARENT,
                    800
                );
                xyPadParams.setMargins(20, 20, 20, 20);
                xyPadView.setLayoutParams(xyPadParams);
                xyPadView.setBackgroundColor(Color.BLACK);

                // Set parameter names for display (show first X and Y axis params found)
                String xAxisLabel = null;
                String yAxisLabel = null;
                for (String paramName : allParamNames) {
                    SensorControl sc = sensorControls.get(paramName);
                    if (sc != null) {
                        if ("X Axis".equals(sc.sensorType) && xAxisLabel == null) {
                            xAxisLabel = paramName;
                        } else if ("Y Axis".equals(sc.sensorType) && yAxisLabel == null) {
                            yAxisLabel = paramName;
                        }
                    }
                }
                xyPadView.setParameterNames(
                    xAxisLabel != null ? xAxisLabel : "X Axis",
                    yAxisLabel != null ? yAxisLabel : "Y Axis"
                );

                // Set listener to update all parameters controlled by X/Y Axis
                xyPadView.setListener(new XYPadView.XYPadListener() {
                    @Override
                    public void onValueChanged(float x, float y) {
                        // Store current position
                        currentXyPadX = x;
                        currentXyPadY = y;

                        // Update all parameters controlled by X Axis or Y Axis
                        updateXyPadParameters();
                    }

                    @Override
                    public void onTouchStateChanged(boolean isTouching) {
                        // Store current touch state
                        currentXyPadTouching = isTouching;

                        // Update all parameters controlled by Pad trigger
                        updateXyPadParameters();
                    }
                });

                // Add XY PAD to AppBody (parent of paramHolder)
                ViewGroup appBody = (ViewGroup) paramHolder.getParent();
                appBody.addView(xyPadView);
            }
            xyPadView.setVisibility(View.VISIBLE);

        } else {
            // Show sliders and hide XY PAD
            paramHolder.setVisibility(View.VISIBLE);
            if (xyPadView != null) {
                xyPadView.setVisibility(View.GONE);
            }
        }

        Log.i("RNBO_MultiMode", "XY PAD: " + (xyPadEnabled ? "ENABLED" : "DISABLED"));
    }

    private void updateXyPadParameters() {
        // Iterate through all parameters and update those controlled by X/Y Axis
        for (String paramName : allParamNames) {
            SensorControl sensorControl = sensorControls.get(paramName);
            if (sensorControl == null) continue;

            ParameterInfo info = parameterInfoMap.get(paramName);
            if (info == null) continue;

            // Get custom range if enabled
            CustomRange customRange = customRangeMap.get(paramName);
            double minValue = info.min;
            double maxValue = info.max;
            if (customRange != null && customRange.enabled) {
                minValue = customRange.customMin;
                maxValue = customRange.customMax;
            }

            float normalizedValue = 0;
            boolean shouldUpdate = false;

            // Check if this parameter is controlled by Pad trigger
            if ("Pad trigger".equals(sensorControl.sensorType)) {
                normalizedValue = currentXyPadTouching ? 1.0f : 0.0f;
                shouldUpdate = true;
            }
            // Check if this parameter is controlled by X Axis
            else if ("X Axis".equals(sensorControl.sensorType)) {
                normalizedValue = currentXyPadX;
                shouldUpdate = true;
            }
            // Check if this parameter is controlled by Y Axis
            else if ("Y Axis".equals(sensorControl.sensorType)) {
                normalizedValue = currentXyPadY;
                shouldUpdate = true;
            }

            if (shouldUpdate) {
                // Apply inversion if needed
                if (sensorControl.invert) {
                    normalizedValue = 1.0f - normalizedValue;
                }

                // Map to parameter range
                float paramValue = (float)(minValue + normalizedValue * (maxValue - minValue));

                // Update parameter
                updateParam(paramName, paramValue);
            }
        }
    }

    private void updateXyPadButton() {
        android.graphics.drawable.GradientDrawable drawable = new android.graphics.drawable.GradientDrawable();
        drawable.setCornerRadius(8);
        drawable.setStroke(3, Color.WHITE);

        if (xyPadEnabled) {
            btnXyPad.setText("XY PAD ON");
            drawable.setColor(Color.WHITE);
            btnXyPad.setTextColor(Color.BLACK);
        } else {
            btnXyPad.setText("XY PAD OFF");
            drawable.setColor(Color.BLACK);
            btnXyPad.setTextColor(Color.WHITE);
        }
        btnXyPad.setBackground(drawable);
    }

    /**
     * Auto-detect available sensors on device
     */
    private void detectAvailableSensors() {
        availableSensorTypes.clear();
        sensorMap.clear();
        
        // Check Accelerometer
        Sensor acc = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        if (acc != null) {
            availableSensorTypes.add("Accelerometer");
            sensorMap.put("Accelerometer", acc);
            Log.i("SensorDetection", "✓ Accelerometer available");
        }
        
        // Check Gyroscope
        Sensor gyro = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
        if (gyro != null) {
            availableSensorTypes.add("Gyroscope");
            sensorMap.put("Gyroscope", gyro);
            Log.i("SensorDetection", "✓ Gyroscope available");
        }
        
        // Check Light
        Sensor light = sensorManager.getDefaultSensor(Sensor.TYPE_LIGHT);
        if (light != null) {
            availableSensorTypes.add("Light");
            sensorMap.put("Light", light);
            Log.i("SensorDetection", "✓ Light sensor available");
        }
        
        // Check Proximity
        Sensor prox = sensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY);
        if (prox != null) {
            availableSensorTypes.add("Proximity");
            sensorMap.put("Proximity", prox);
            Log.i("SensorDetection", "✓ Proximity sensor available");
        }
        
        // Check Pressure (Barometer)
        Sensor press = sensorManager.getDefaultSensor(Sensor.TYPE_PRESSURE);
        if (press != null) {
            availableSensorTypes.add("Pressure");
            sensorMap.put("Pressure", press);
            Log.i("SensorDetection", "✓ Pressure sensor available");
        }
        
        // Check Magnetometer
        Sensor mag = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        if (mag != null) {
            availableSensorTypes.add("Magnetometer");
            sensorMap.put("Magnetometer", mag);
            Log.i("SensorDetection", "✓ Magnetometer available");
        }
        
        // Check Gravity
        Sensor grav = sensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY);
        if (grav != null) {
            availableSensorTypes.add("Gravity");
            sensorMap.put("Gravity", grav);
            Log.i("SensorDetection", "✓ Gravity sensor available");
        }
        
        // Check Linear Acceleration
        Sensor linear = sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
        if (linear != null) {
            availableSensorTypes.add("Linear Acceleration");
            sensorMap.put("Linear Acceleration", linear);
            Log.i("SensorDetection", "✓ Linear Acceleration available");
        }
        
        Log.i("SensorDetection", "Total sensors detected: " + availableSensorTypes.size());
    }
    
    /**
     * Initialize physical ranges for each sensor type
     */
    private void initializeSensorRanges() {
        // Movement sensors (m/s² or rad/s)
        sensorRanges.put("Accelerometer", new float[]{-10f, 10f});
        sensorRanges.put("Gyroscope", new float[]{-10f, 10f});
        sensorRanges.put("Gravity", new float[]{-10f, 10f});
        sensorRanges.put("Linear Acceleration", new float[]{-10f, 10f});
        
        // Environmental sensors
        sensorRanges.put("Light", new float[]{0f, 10000f});      // lux
        sensorRanges.put("Pressure", new float[]{300f, 1100f});  // hPa
        sensorRanges.put("Temperature", new float[]{-20f, 50f}); // °C
        sensorRanges.put("Humidity", new float[]{0f, 100f});     // %
        
        // Position sensors
        sensorRanges.put("Magnetometer", new float[]{-100f, 100f}); // μT
        sensorRanges.put("Proximity", new float[]{0f, 5f});         // cm (device-dependent)
    }
    
    /**
     * Get physical range for a sensor type
     */
    private float[] getSensorRange(String sensorType) {
        float[] range = sensorRanges.get(sensorType);
        if (range != null) {
            return range;
        }
        // Default fallback
        return new float[]{-10f, 10f};
    }
    
    /**
     * Get available data sources for a sensor type
     */
    private String[] getDataSourcesForSensor(String sensorType) {
        switch(sensorType) {
            case "Accelerometer":
            case "Gyroscope":
            case "Gravity":
            case "Linear Acceleration":
            case "Magnetometer":
                return new String[]{"X", "Y", "Z", "Magnitude"};
            case "Light":
                return new String[]{"Lux"};
            case "Proximity":
                return new String[]{"Distance"};
            case "Pressure":
                return new String[]{"hPa"};
            case "Temperature":
                return new String[]{"Celsius"};
            case "Humidity":
                return new String[]{"Percent"};
            default:
                return new String[]{"Value"};
        }
    }
    
    /**
     * Get sensor type from Android sensor constant
     */
    private String getSensorTypeName(int sensorType) {
        switch(sensorType) {
            case Sensor.TYPE_ACCELEROMETER: return "Accelerometer";
            case Sensor.TYPE_GYROSCOPE: return "Gyroscope";
            case Sensor.TYPE_LIGHT: return "Light";
            case Sensor.TYPE_PROXIMITY: return "Proximity";
            case Sensor.TYPE_PRESSURE: return "Pressure";
            case Sensor.TYPE_MAGNETIC_FIELD: return "Magnetometer";
            case Sensor.TYPE_GRAVITY: return "Gravity";
            case Sensor.TYPE_LINEAR_ACCELERATION: return "Linear Acceleration";
            default: return "Unknown";
        }
    }
    
    private final SensorEventListener sensorListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent event) {
            
            // ⚠️ IGNORA SENSORI DURANTE INTERPOLAZIONE ⚠️
            if (isInterpolating) {
                return;  // ← ESCE SUBITO
            }
            
            if (!sensorsGloballyEnabled) return;
            
            String currentSensorType = getSensorTypeName(event.sensor.getType());
            if (currentSensorType.equals("Unknown")) return;
            
            // Process all parameters controlled by this sensor
            for (Map.Entry<String, SensorControl> entry : sensorControls.entrySet()) {
                String paramName = entry.getKey();
                SensorControl control = entry.getValue();
                
                if (!control.controlMode.equals("sensor")) continue;
                if (!control.sensorType.equals(currentSensorType)) continue;
                
                // Extract the appropriate value from the sensor event
                float sensorValue = extractSensorValue(event, control.dataSource);
                
                // Process and map to parameter
                float processedValue = processSensorValue(sensorValue, control, paramName);
                updateParam(paramName, processedValue);
                updateSliderPositionForSensor(paramName, processedValue);
            }
        }
        
        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {}
    };
    
    /**
     * Extract specific value from sensor event based on data source
     */
    private float extractSensorValue(SensorEvent event, String dataSource) {
        switch(dataSource) {
            case "X":
                return event.values[0];
            case "Y":
                return event.values.length > 1 ? event.values[1] : 0;
            case "Z":
                return event.values.length > 2 ? event.values[2] : 0;
            case "Magnitude":
                // Calculate magnitude for 3-axis sensors
                if (event.values.length >= 3) {
                    return (float) Math.sqrt(
                        event.values[0] * event.values[0] +
                        event.values[1] * event.values[1] +
                        event.values[2] * event.values[2]
                    );
                }
                return event.values[0];
            case "Lux":
            case "Distance":
            case "hPa":
            case "Celsius":
            case "Percent":
            case "Value":
            default:
                return event.values[0];
        }
    }
    
    private String getAxisName(int axis) {
        switch(axis) {
            case 0: return "x";
            case 1: return "y";
            case 2: return "z";
            default: return "x";
        }
    }
    
    private float processSensorValue(float rawValue, SensorControl control, String paramName) {
        // Get sensor physical range
        float[] sensorRange = getSensorRange(control.sensorType);
        float sensorMin = sensorRange[0];
        float sensorMax = sensorRange[1];
        
        // Apply dead zone for motion sensors (ignore small movements)
        if (control.sensorType.equals("Accelerometer") || 
            control.sensorType.equals("Gyroscope") ||
            control.sensorType.equals("Gravity") ||
            control.sensorType.equals("Linear Acceleration")) {
            if (Math.abs(rawValue) < 0.5f) {
                rawValue = 0;
            }
        }
        
        // Apply sensitivity (amplify the signal)
        float scaled = rawValue * control.sensitivity;
        
        // Apply invert
        if (control.invert) {
            scaled = sensorMax + sensorMin - scaled;
        }
        
        // Apply smoothing (low-pass filter)
        String key = paramName + "_smoothed";
        Float lastValue = lastSensorValues.get(key);
        if (lastValue != null) {
            float alpha = 1.0f - control.smoothing;
            scaled = alpha * scaled + (1 - alpha) * lastValue;
        }
        lastSensorValues.put(key, scaled);
        
        // Map to parameter range
        ParameterInfo info = parameterInfoMap.get(paramName);
        if (info == null) return 0;
        
        CustomRange customRange = customRangeMap.get(paramName);
        double effectiveMin = info.min;
        double effectiveMax = info.max;
        
        if (customRange != null && customRange.enabled) {
            effectiveMin = customRange.customMin;
            effectiveMax = customRange.customMax;
        }
        
        // Normalize sensor value to 0-1
        float normalized = (scaled - sensorMin) / (sensorMax - sensorMin);
        normalized = Math.max(0f, Math.min(1f, normalized));
        
        // Map to parameter range
        float paramValue = (float)(effectiveMin + normalized * (effectiveMax - effectiveMin));
        
        return paramValue;
    }
    
    private void updateSliderPositionForSensor(String paramName, float realValue) {
        ParameterInfo info = parameterInfoMap.get(paramName);
        if (info == null) return;
        
        CustomRange customRange = customRangeMap.get(paramName);
        double effectiveMin = info.min;
        double effectiveMax = info.max;
        
        if (customRange != null && customRange.enabled) {
            effectiveMin = customRange.customMin;
            effectiveMax = customRange.customMax;
        }
        
        // Calculate slider position
        float normalized = (float)((realValue - effectiveMin) / (effectiveMax - effectiveMin));
        int position = (int)(normalized * 1000);
        
        // Update stored position
        sliderPositions.put(paramName, position);

        // Update UI slider visually (on UI thread)
        PureDataSlider slider = activeSliders.get(paramName);
        if (slider != null) {
            runOnUiThread(() -> {
                slider.setProgress(position);
                slider.setCurrentValue(realValue);
            });
        }
    }
    
    private void registerSensorListeners() {
        if (!sensorsGloballyEnabled) return;
        if (currentMode.equals("setup")) return;
        
        // Check which sensors are needed
        Set<String> neededSensors = new HashSet<>();
        
        for (SensorControl control : sensorControls.values()) {
            if (control.controlMode.equals("sensor")) {
                neededSensors.add(control.sensorType);
            }
        }
        
        // Register each needed sensor
        for (String sensorType : neededSensors) {
            Sensor sensor = sensorMap.get(sensorType);
            if (sensor != null) {
                sensorManager.registerListener(sensorListener, sensor, SensorManager.SENSOR_DELAY_GAME);
                Log.i("RNBO_MultiMode", sensorType + " registered");
            }
        }
    }
    
    private void unregisterSensorListeners() {
        sensorManager.unregisterListener(sensorListener);
        Log.i("RNBO_MultiMode", "All sensors unregistered");
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        unregisterSensorListeners();
        
        // STEP 4: Stop camera when app goes to background (DISABLED)
        /*
        if (gestureManager != null) {
            gestureManager.stopCamera();
        }
        */
    }
    
    @Override
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
    }
    
    private void refreshParameterDisplay() {
        paramHolder.removeAllViews();
        setupCheckboxes.clear();
        activeSliders.clear();
        if (currentMode.equals("setup")) {
            displaySetupMode();
        } else if (currentMode.equals("presets")) {
            displayPresetsMode();
        } else {
            displayParameterSliders();
        }
    }
    
    private void displaySetupMode() {
        TextView title = new TextView(this);
        title.setText("SELECT PARAMETERS FOR LIVE MODE");
        title.setTextSize(18);
        title.setTextColor(Color.WHITE);
        title.setTypeface(null, android.graphics.Typeface.BOLD);
        title.setGravity(Gravity.CENTER);
        title.setPadding(0, 20, 0, 20);
        paramHolder.addView(title);

        TextView instruction = new TextView(this);
        instruction.setText("Configure range and visibility for each parameter");
        instruction.setTextSize(14);
        instruction.setTextColor(Color.parseColor("#AAAAAA"));
        instruction.setGravity(Gravity.CENTER);
        instruction.setPadding(0, 0, 0, 30);
        paramHolder.addView(instruction);

        // Add Buffer Management section
        addBufferManagementUI();

        // Separator
        View separator = new View(this);
        separator.setBackgroundColor(Color.parseColor("#444444"));
        LayoutParams sepParams = new LayoutParams(LayoutParams.MATCH_PARENT, 2);
        sepParams.setMargins(20, 20, 20, 30);
        separator.setLayoutParams(sepParams);
        paramHolder.addView(separator);

        for (String paramName : allParamNames) {
            ParameterInfo info = parameterInfoMap.get(paramName);
            if (info == null) {
                info = new ParameterInfo(paramName, 0, 1, 0.5);
            }
            
            CustomRange customRange = customRangeMap.get(paramName);
            if (customRange == null) {
                customRange = new CustomRange(false, info.min, info.max);
                customRangeMap.put(paramName, customRange);
            }
            
            LinearLayout paramContainer = new LinearLayout(this);
            paramContainer.setOrientation(LinearLayout.VERTICAL);
            paramContainer.setBackgroundColor(Color.parseColor("#F5F5F5"));
            paramContainer.setPadding(20, 15, 20, 15);
            LayoutParams containerParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
            containerParams.setMargins(10, 5, 10, 5);
            paramContainer.setLayoutParams(containerParams);
            
            // Parameter name
            TextView paramLabel = new TextView(this);
            paramLabel.setText(paramName);
            paramLabel.setTextSize(18);
            paramLabel.setTextColor(Color.BLACK);
            paramLabel.setTypeface(null, android.graphics.Typeface.BOLD);
            paramContainer.addView(paramLabel);
            
            // Original range
            TextView originalRange = new TextView(this);
            originalRange.setText(String.format("Original: [%.2f - %.2f]", info.min, info.max));
            originalRange.setTextSize(12);
            originalRange.setTextColor(Color.parseColor("#666666"));
            originalRange.setPadding(0, 5, 0, 15);
            paramContainer.addView(originalRange);
            
            // Show in Live checkbox
            CheckBox showCheckbox = new CheckBox(this);
            showCheckbox.setText("Show in Live Mode");
            showCheckbox.setTextColor(Color.BLACK);
            showCheckbox.setButtonTintList(android.content.res.ColorStateList.valueOf(Color.BLACK));
            showCheckbox.setTag(paramName);
            showCheckbox.setChecked(liveParameters.contains(paramName));
            showCheckbox.setOnCheckedChangeListener((buttonView, isChecked) -> {
                String param = (String) buttonView.getTag();
                if (isChecked) {
                    liveParameters.add(param);
                } else {
                    liveParameters.remove(param);
                }
                saveLiveConfig();
                Log.i("RNBO_MultiMode", "Auto-saved: " + param + " = " + isChecked);
            });
            paramContainer.addView(showCheckbox);
            setupCheckboxes.add(showCheckbox);
            
            // Use Custom Range checkbox
            CheckBox customRangeCheckbox = new CheckBox(this);
            customRangeCheckbox.setText("Use Custom Range");
            customRangeCheckbox.setTextColor(Color.BLACK);
            customRangeCheckbox.setButtonTintList(android.content.res.ColorStateList.valueOf(Color.BLACK));
            customRangeCheckbox.setTag(paramName);
            customRangeCheckbox.setChecked(customRange.enabled);
            paramContainer.addView(customRangeCheckbox);
            
            // Custom range label (dynamic)
            TextView rangeLabel = new TextView(this);
            rangeLabel.setTextSize(14);
            rangeLabel.setTextColor(Color.parseColor("#333333"));
            rangeLabel.setPadding(0, 10, 0, 5);
            if (customRange.enabled) {
                rangeLabel.setText(String.format("Custom Range: [%.2f - %.2f]", 
                    customRange.customMin, customRange.customMax));
            } else {
                rangeLabel.setText("Custom Range: (disabled)");
                rangeLabel.setTextColor(Color.parseColor("#999999"));
            }
            paramContainer.addView(rangeLabel);
            
            // RangeSlider for custom range
            RangeSlider rangeSlider = new RangeSlider(this);
            rangeSlider.setValueFrom((float)info.min);
            rangeSlider.setValueTo((float)info.max);
            rangeSlider.setValues((float)customRange.customMin, (float)customRange.customMax);
            rangeSlider.setEnabled(customRange.enabled);
            rangeSlider.setTrackActiveTintList(android.content.res.ColorStateList.valueOf(Color.parseColor("#2196F3")));
            rangeSlider.setTrackInactiveTintList(android.content.res.ColorStateList.valueOf(Color.parseColor("#CCCCCC")));
            
            LayoutParams rangeSliderParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
            rangeSliderParams.setMargins(0, 5, 0, 10);
            rangeSlider.setLayoutParams(rangeSliderParams);
            
            // RangeSlider listener
            final CustomRange finalCustomRange = customRange;
            final ParameterInfo finalInfo = info;
            rangeSlider.addOnChangeListener((slider, value, fromUser) -> {
                if (fromUser) {
                    List<Float> values = slider.getValues();
                    if (values.size() == 2) {
                        finalCustomRange.customMin = values.get(0);
                        finalCustomRange.customMax = values.get(1);
                        rangeLabel.setText(String.format("Custom Range: [%.2f - %.2f]", 
                            finalCustomRange.customMin, finalCustomRange.customMax));
                        saveCustomRanges();
                        Log.i("RNBO_MultiMode", String.format("%s custom range: [%.2f - %.2f]", 
                            paramName, finalCustomRange.customMin, finalCustomRange.customMax));
                    }
                }
            });
            
            paramContainer.addView(rangeSlider);
            
            // Custom Range checkbox listener (enable/disable RangeSlider)
            customRangeCheckbox.setOnCheckedChangeListener((buttonView, isChecked) -> {
                String param = (String) buttonView.getTag();
                finalCustomRange.enabled = isChecked;
                rangeSlider.setEnabled(isChecked);
                
                if (isChecked) {
                    rangeLabel.setText(String.format("Custom Range: [%.2f - %.2f]", 
                        finalCustomRange.customMin, finalCustomRange.customMax));
                    rangeLabel.setTextColor(Color.parseColor("#333333"));
                } else {
                    rangeLabel.setText("Custom Range: (disabled)");
                    rangeLabel.setTextColor(Color.parseColor("#999999"));
                }
                
                saveCustomRanges();
                Log.i("RNBO_MultiMode", "Custom range " + param + " = " + isChecked);
            });
            
            // SENSOR CONTROL SECTION
            SensorControl sensorControl = sensorControls.get(paramName);
            if (sensorControl == null) {
                sensorControl = new SensorControl();
                sensorControls.put(paramName, sensorControl);
            }
            
            // Separator
            View separator = new View(this);
            separator.setBackgroundColor(Color.parseColor("#CCCCCC"));
            LayoutParams sepParams = new LayoutParams(LayoutParams.MATCH_PARENT, 2);
            sepParams.setMargins(0, 15, 0, 15);
            separator.setLayoutParams(sepParams);
            paramContainer.addView(separator);
            
            // Control Source label
            TextView controlLabel = new TextView(this);
            controlLabel.setText("Control Source:");
            controlLabel.setTextSize(14);
            controlLabel.setTextColor(Color.BLACK);
            controlLabel.setTypeface(null, android.graphics.Typeface.BOLD);
            controlLabel.setPadding(0, 0, 0, 5);
            paramContainer.addView(controlLabel);
            
            // First Dropdown: Sensor Type
            TextView sensorTypeLabel = new TextView(this);
            sensorTypeLabel.setText("1. Sensor Type:");
            sensorTypeLabel.setTextSize(12);
            sensorTypeLabel.setTextColor(Color.parseColor("#666666"));
            sensorTypeLabel.setPadding(0, 5, 0, 3);
            paramContainer.addView(sensorTypeLabel);
            
            Spinner sensorTypeSpinner = new Spinner(this);
            
            // Build sensor type options: "Manual" + detected sensors
            List<String> sensorTypeOptions = new ArrayList<>();
            sensorTypeOptions.add("Manual");
            sensorTypeOptions.add("Pad trigger");
            sensorTypeOptions.add("X Axis");
            sensorTypeOptions.add("Y Axis");
            sensorTypeOptions.addAll(availableSensorTypes);
            
            ArrayAdapter<String> sensorTypeAdapter = new ArrayAdapter<>(this, 
                android.R.layout.simple_spinner_item, sensorTypeOptions);
            sensorTypeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            sensorTypeSpinner.setAdapter(sensorTypeAdapter);
            
            // Set current selection for sensor type
            if (sensorControl.controlMode.equals("manual")) {
                sensorTypeSpinner.setSelection(0);
            } else {
                int sensorTypeIndex = sensorTypeOptions.indexOf(sensorControl.sensorType);
                if (sensorTypeIndex > 0) {
                    sensorTypeSpinner.setSelection(sensorTypeIndex);
                }
            }
            
            LayoutParams spinnerParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
            spinnerParams.setMargins(0, 0, 0, 10);
            sensorTypeSpinner.setLayoutParams(spinnerParams);
            paramContainer.addView(sensorTypeSpinner);
            
            // Second Dropdown: Data Source (initially hidden)
            LinearLayout dataSourceContainer = new LinearLayout(this);
            dataSourceContainer.setOrientation(LinearLayout.VERTICAL);
            dataSourceContainer.setVisibility(
                sensorControl.controlMode.equals("sensor") ? View.VISIBLE : View.GONE
            );
            
            TextView dataSourceLabel = new TextView(this);
            dataSourceLabel.setText("2. Data Source:");
            dataSourceLabel.setTextSize(12);
            dataSourceLabel.setTextColor(Color.parseColor("#666666"));
            dataSourceLabel.setPadding(0, 0, 0, 3);
            dataSourceContainer.addView(dataSourceLabel);
            
            Spinner dataSourceSpinner = new Spinner(this);
            
            // Initialize with current data sources
            String[] initialDataSources = sensorControl.controlMode.equals("sensor") ? 
                getDataSourcesForSensor(sensorControl.sensorType) : new String[]{"Value"};
            ArrayAdapter<String> dataSourceAdapter = new ArrayAdapter<>(this, 
                android.R.layout.simple_spinner_item, initialDataSources);
            dataSourceAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            dataSourceSpinner.setAdapter(dataSourceAdapter);
            
            // Set current data source selection
            if (sensorControl.controlMode.equals("sensor") && !sensorControl.dataSource.isEmpty()) {
                for (int i = 0; i < initialDataSources.length; i++) {
                    if (initialDataSources[i].equals(sensorControl.dataSource)) {
                        dataSourceSpinner.setSelection(i);
                        break;
                    }
                }
            }
            
            LayoutParams dataSpinnerParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
            dataSpinnerParams.setMargins(0, 0, 0, 10);
            dataSourceSpinner.setLayoutParams(dataSpinnerParams);
            dataSourceContainer.addView(dataSourceSpinner);
            
            paramContainer.addView(dataSourceContainer);
            
            // Sensor Settings Container (initially hidden if Manual)
            LinearLayout sensorSettingsContainer = new LinearLayout(this);
            sensorSettingsContainer.setOrientation(LinearLayout.VERTICAL);
            sensorSettingsContainer.setVisibility(
                sensorControl.controlMode.equals("sensor") ? View.VISIBLE : View.GONE
            );
            
            // Sensitivity
            TextView sensLabel = new TextView(this);
            sensLabel.setText(String.format("Sensitivity: %.0f%%", sensorControl.sensitivity * 100));
            sensLabel.setTextSize(12);
            sensLabel.setTextColor(Color.parseColor("#666666"));
            sensLabel.setPadding(0, 10, 0, 5);
            sensorSettingsContainer.addView(sensLabel);
            
            AppCompatSeekBar sensSlider = new AppCompatSeekBar(this);
            sensSlider.setMax(100);
            sensSlider.setProgress((int)(sensorControl.sensitivity * 100));
            LayoutParams sensParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
            sensorSettingsContainer.addView(sensSlider, sensParams);
            
            // Invert checkbox
            CheckBox invertCheck = new CheckBox(this);
            invertCheck.setText("Invert");
            invertCheck.setTextColor(Color.BLACK);
            invertCheck.setButtonTintList(android.content.res.ColorStateList.valueOf(Color.BLACK));
            invertCheck.setChecked(sensorControl.invert);
            invertCheck.setPadding(0, 5, 0, 5);
            sensorSettingsContainer.addView(invertCheck);
            
            // Smoothing
            TextView smoothLabel = new TextView(this);
            smoothLabel.setText(String.format("Smoothing: %.0f%%", sensorControl.smoothing * 100));
            smoothLabel.setTextSize(12);
            smoothLabel.setTextColor(Color.parseColor("#666666"));
            smoothLabel.setPadding(0, 10, 0, 5);
            sensorSettingsContainer.addView(smoothLabel);
            
            AppCompatSeekBar smoothSlider = new AppCompatSeekBar(this);
            smoothSlider.setMax(100);
            smoothSlider.setProgress((int)(sensorControl.smoothing * 100));
            LayoutParams smoothParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
            sensorSettingsContainer.addView(smoothSlider, smoothParams);
            
            paramContainer.addView(sensorSettingsContainer);
            
            // Sensor Type Spinner listener
            final SensorControl finalSensorControl = sensorControl;
            sensorTypeSpinner.setOnItemSelectedListener(new android.widget.AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(android.widget.AdapterView<?> parent, View view, int position, long id) {
                    String selectedSensorType = parent.getItemAtPosition(position).toString();
                    
                    if (selectedSensorType.equals("Manual")) {
                        // Set to manual mode
                        finalSensorControl.controlMode = "manual";
                        finalSensorControl.sensorType = "manual";
                        finalSensorControl.dataSource = "";
                        dataSourceContainer.setVisibility(View.GONE);
                        sensorSettingsContainer.setVisibility(View.GONE);
                    } else {
                        // Set to sensor mode
                        finalSensorControl.controlMode = "sensor";
                        finalSensorControl.sensorType = selectedSensorType;
                        
                        // Update data source spinner with appropriate options
                        String[] dataSources = getDataSourcesForSensor(selectedSensorType);
                        ArrayAdapter<String> newAdapter = new ArrayAdapter<>(MainActivity.this, 
                            android.R.layout.simple_spinner_item, dataSources);
                        newAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
                        dataSourceSpinner.setAdapter(newAdapter);
                        
                        // Set default data source
                        if (dataSources.length > 0) {
                            finalSensorControl.dataSource = dataSources[0];
                            dataSourceSpinner.setSelection(0);
                        }
                        
                        dataSourceContainer.setVisibility(View.VISIBLE);
                        sensorSettingsContainer.setVisibility(View.VISIBLE);
                    }
                    
                    saveSensorControls();
                    Log.i("RNBO_MultiMode", paramName + " sensor type: " + finalSensorControl.sensorType);
                }
                
                @Override
                public void onNothingSelected(android.widget.AdapterView<?> parent) {}
            });
            
            // Data Source Spinner listener
            dataSourceSpinner.setOnItemSelectedListener(new android.widget.AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(android.widget.AdapterView<?> parent, View view, int position, long id) {
                    String selectedDataSource = parent.getItemAtPosition(position).toString();
                    finalSensorControl.dataSource = selectedDataSource;
                    saveSensorControls();
                    Log.i("RNBO_MultiMode", paramName + " data source: " + selectedDataSource);
                }
                
                @Override
                public void onNothingSelected(android.widget.AdapterView<?> parent) {}
            });
            
            // Sensitivity slider listener
            sensSlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    finalSensorControl.sensitivity = progress / 100.0f;
                    sensLabel.setText(String.format("Sensitivity: %.0f%%", (float)progress));
                    if (fromUser) {
                        saveSensorControls();
                    }
                }
                @Override public void onStartTrackingTouch(SeekBar seekBar) {}
                @Override public void onStopTrackingTouch(SeekBar seekBar) {}
            });
            
            // Invert checkbox listener
            invertCheck.setOnCheckedChangeListener((buttonView, isChecked) -> {
                finalSensorControl.invert = isChecked;
                saveSensorControls();
            });
            
            // Smoothing slider listener
            smoothSlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    finalSensorControl.smoothing = progress / 100.0f;
                    smoothLabel.setText(String.format("Smoothing: %.0f%%", (float)progress));
                    if (fromUser) {
                        saveSensorControls();
                    }
                }
                @Override public void onStartTrackingTouch(SeekBar seekBar) {}
                @Override public void onStopTrackingTouch(SeekBar seekBar) {}
            });
            
            paramHolder.addView(paramContainer);
        }
    }
    
    // ============================================================================
    // PRESETS SYSTEM
    // ============================================================================
    
    /**
     * Display Presets Mode UI
     */
    private void displayPresetsMode() {
        // Title
        TextView title = new TextView(this);
        title.setText("PRESETS MANAGER");
        title.setTextSize(20);
        title.setTextColor(Color.WHITE);
        title.setTypeface(null, android.graphics.Typeface.BOLD);
        title.setGravity(Gravity.CENTER);
        title.setPadding(0, 20, 0, 10);
        paramHolder.addView(title);
        
        TextView subtitle = new TextView(this);
        subtitle.setText("Save and load complete configurations");
        subtitle.setTextSize(14);
        subtitle.setTextColor(Color.parseColor("#AAAAAA"));
        subtitle.setGravity(Gravity.CENTER);
        subtitle.setPadding(0, 0, 0, 30);
        paramHolder.addView(subtitle);
        
        // SAVE CURRENT button
        Button saveButton = new Button(this);
        saveButton.setText("SAVE CURRENT CONFIGURATION");
        saveButton.setTextSize(16);
        saveButton.setBackgroundColor(Color.WHITE);
        saveButton.setTextColor(Color.BLACK);
        saveButton.setTypeface(null, android.graphics.Typeface.BOLD);
        LayoutParams saveParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
        saveParams.setMargins(20, 10, 20, 20);
        saveButton.setLayoutParams(saveParams);
        saveButton.setOnClickListener(v -> showSavePresetDialog());
        paramHolder.addView(saveButton);
        
        // Separator
        View separator = new View(this);
        separator.setBackgroundColor(Color.parseColor("#444444"));
        LayoutParams sepParams = new LayoutParams(LayoutParams.MATCH_PARENT, 2);
        sepParams.setMargins(20, 10, 20, 20);
        separator.setLayoutParams(sepParams);
        paramHolder.addView(separator);
        
        // Saved Presets label
        TextView presetsLabel = new TextView(this);
        presetsLabel.setText("Saved Presets (" + savedPresets.size() + ")");
        presetsLabel.setTextSize(16);
        presetsLabel.setTextColor(Color.WHITE);
        presetsLabel.setTypeface(null, android.graphics.Typeface.BOLD);
        presetsLabel.setPadding(20, 10, 20, 10);
        paramHolder.addView(presetsLabel);
        
        // List of presets
        if (savedPresets.isEmpty()) {
            TextView emptyMsg = new TextView(this);
            emptyMsg.setText("No presets saved yet.\nCreate your first preset by clicking the button above!");
            emptyMsg.setTextSize(14);
            emptyMsg.setTextColor(Color.parseColor("#888888"));
            emptyMsg.setGravity(Gravity.CENTER);
            emptyMsg.setPadding(20, 40, 20, 40);
            paramHolder.addView(emptyMsg);
        } else {
            for (Preset preset : savedPresets) {
                LinearLayout presetItem = createPresetItem(preset);
                paramHolder.addView(presetItem);
            }
        }
    }
    
    /**
     * Create UI item for a preset
     */
    private LinearLayout createPresetItem(Preset preset) {
        LinearLayout container = new LinearLayout(this);
        container.setOrientation(LinearLayout.VERTICAL);
        container.setBackgroundColor(Color.parseColor("#2C2C2C"));
        LayoutParams containerParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
        containerParams.setMargins(20, 10, 20, 10);
        container.setLayoutParams(containerParams);
        container.setPadding(20, 20, 20, 20);
        
        // Preset name
        TextView nameLabel = new TextView(this);
        nameLabel.setText(preset.name);
        nameLabel.setTextSize(18);
        nameLabel.setTextColor(Color.WHITE);
        nameLabel.setTypeface(null, android.graphics.Typeface.BOLD);
        container.addView(nameLabel);
        
        // Timestamp
        TextView timeLabel = new TextView(this);
        java.text.SimpleDateFormat sdf = new java.text.SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        String dateStr = sdf.format(new java.util.Date(preset.timestamp));
        timeLabel.setText("Created: " + dateStr);
        timeLabel.setTextSize(12);
        timeLabel.setTextColor(Color.parseColor("#AAAAAA"));
        timeLabel.setPadding(0, 5, 0, 15);
        container.addView(timeLabel);
        
        // Buttons row
        LinearLayout buttonsRow = new LinearLayout(this);
        buttonsRow.setOrientation(LinearLayout.HORIZONTAL);
        buttonsRow.setGravity(Gravity.CENTER);
        
        // LOAD button
        Button loadBtn = new Button(this);
        loadBtn.setText("LOAD");
        loadBtn.setBackgroundColor(Color.parseColor("#666666"));
        loadBtn.setTextColor(Color.WHITE);
        LayoutParams loadParams = new LayoutParams(0, LayoutParams.WRAP_CONTENT, 1.0f);
        loadParams.setMargins(0, 0, 5, 0);
        loadBtn.setLayoutParams(loadParams);
        loadBtn.setOnClickListener(v -> loadPreset(preset));
        buttonsRow.addView(loadBtn);
        
        // RENAME button
        Button renameBtn = new Button(this);
        renameBtn.setText("RENAME");
        renameBtn.setBackgroundColor(Color.parseColor("#666666"));
        renameBtn.setTextColor(Color.WHITE);
        LayoutParams renameParams = new LayoutParams(0, LayoutParams.WRAP_CONTENT, 1.0f);
        renameParams.setMargins(5, 0, 5, 0);
        renameBtn.setLayoutParams(renameParams);
        renameBtn.setOnClickListener(v -> showRenameDialog(preset));
        buttonsRow.addView(renameBtn);
        
        // DELETE button
        Button deleteBtn = new Button(this);
        deleteBtn.setText("DELETE");
        deleteBtn.setBackgroundColor(Color.parseColor("#666666"));
        deleteBtn.setTextColor(Color.WHITE);
        LayoutParams deleteParams = new LayoutParams(0, LayoutParams.WRAP_CONTENT, 1.0f);
        deleteParams.setMargins(5, 0, 5, 0);
        deleteBtn.setLayoutParams(deleteParams);
        deleteBtn.setOnClickListener(v -> showDeleteConfirmation(preset));
        buttonsRow.addView(deleteBtn);
        
        // QUICK button
        Button quickBtn = new Button(this);
        quickBtn.setText("Q");
        quickBtn.setTextSize(16);
        quickBtn.setBackgroundColor(preset.isQuickPreset ?
            Color.WHITE : Color.parseColor("#666666"));
        quickBtn.setTextColor(preset.isQuickPreset ? Color.BLACK : Color.WHITE);
        LayoutParams quickParams = new LayoutParams(0, LayoutParams.WRAP_CONTENT, 0.5f);
        quickParams.setMargins(5, 0, 0, 0);
        quickBtn.setLayoutParams(quickParams);
        quickBtn.setOnClickListener(v -> {
            toggleQuickPreset(preset);
            refreshParameterDisplay();
        });
        buttonsRow.addView(quickBtn);
        
        container.addView(buttonsRow);
        
        return container;
    }
    
    /**
     * Show dialog to save current configuration as preset
     */
    private void showSavePresetDialog() {
        android.app.AlertDialog.Builder builder = new android.app.AlertDialog.Builder(this);
        builder.setTitle("Save Preset");
        builder.setMessage("Enter a name for this preset:");
        
        final android.widget.EditText input = new android.widget.EditText(this);
        input.setHint("Preset name...");
        input.setPadding(20, 20, 20, 20);
        builder.setView(input);
        
        builder.setPositiveButton("Save", (dialog, which) -> {
            String name = input.getText().toString().trim();
            if (!name.isEmpty()) {
                saveCurrentAsPreset(name);
            } else {
                android.widget.Toast.makeText(this, "Preset name cannot be empty", android.widget.Toast.LENGTH_SHORT).show();
            }
        });
        
        builder.setNegativeButton("Cancel", null);
        builder.show();
    }
    
        /**
    * Save current configuration as a new preset
    */
    private void saveCurrentAsPreset(String name) {
        Preset preset = new Preset(name);
        
        // Deep copy all configurations
        preset.sensorControls = deepCopySensorControls(sensorControls);
        preset.customRanges = deepCopyCustomRanges(customRangeMap);
        preset.liveParameters = new HashSet<>(liveParameters);
        preset.sensorsGloballyEnabled = sensorsGloballyEnabled;
        
        // Save current parameter values (real values, not slider positions)
        preset.parameterValues = new HashMap<>();
        for (String paramName : allParamNames) {
            float realValue = getCurrentParamValue(paramName);
            preset.parameterValues.put(paramName, realValue);
        }
        
        savedPresets.add(preset);
        savePresetsToStorage();
        
        // Refresh UI
        refreshParameterDisplay();
        
        android.widget.Toast.makeText(this, "Preset '" + name + "' saved!", android.widget.Toast.LENGTH_LONG).show();
        Log.i("RNBO_Presets", "Preset saved: " + name);
    }

    /**
    * Get current real value of a parameter
    */
    private float getCurrentParamValue(String paramName) {
        Integer position = sliderPositions.get(paramName);
        if (position == null) {
            // If no position stored, use initial value
            ParameterInfo info = parameterInfoMap.get(paramName);
            return info != null ? (float) info.initialValue : 0f;
        }
        
        ParameterInfo info = parameterInfoMap.get(paramName);
        if (info == null) return 0f;
        
        CustomRange customRange = customRangeMap.get(paramName);
        double effectiveMin = info.min;
        double effectiveMax = info.max;
        
        if (customRange != null && customRange.enabled) {
            effectiveMin = customRange.customMin;
            effectiveMax = customRange.customMax;
        }
        
        // Convert slider position to real value
        float normalized = position / 1000.0f;
        return (float)(effectiveMin + normalized * (effectiveMax - effectiveMin));
    }


        
    
    /**
     * Load a preset
     */
    /**
     * Load a preset (with or without interpolation)
     */
    private void loadPreset(Preset preset) {
        if (interpolationEnabled && !isInterpolating) {
            startInterpolation(preset);
        } else {
            loadPresetInstant(preset);
        }
    }
    
    /**
     * Load preset instantly (no interpolation)
     */
    private void loadPresetInstant(Preset preset) {
        sensorControls = deepCopySensorControls(preset.sensorControls);
        customRangeMap = deepCopyCustomRanges(preset.customRanges);
        liveParameters = new HashSet<>(preset.liveParameters);
        sensorsGloballyEnabled = preset.sensorsGloballyEnabled;
        
        if (preset.parameterValues != null) {
            sliderPositions.clear();
            for (Map.Entry<String, Float> entry : preset.parameterValues.entrySet()) {
                String paramName = entry.getKey();
                float realValue = entry.getValue();
                
                int position = calculateSliderPosition(paramName, realValue);
                sliderPositions.put(paramName, position);
                updateParam(paramName, realValue);
            }
        }
        
        saveSensorControls();
        saveCustomRanges();
        saveLiveConfig();
        saveSensorGlobalState();
        updateSensorToggleButton();
        
        if (!currentMode.equals("live")) {
            switchMode("live");
        }
        
        android.widget.Toast.makeText(this, "Preset '" + preset.name + "' loaded", android.widget.Toast.LENGTH_SHORT).show();
        Log.i("RNBO_Presets", "Preset loaded instantly: " + preset.name);
    }

    /**
    * Calculate slider position (0-1000) from real parameter value
    */
    private int calculateSliderPosition(String paramName, float realValue) {
        ParameterInfo info = parameterInfoMap.get(paramName);
        if (info == null) return 0;
        
        CustomRange customRange = customRangeMap.get(paramName);
        double effectiveMin = info.min;
        double effectiveMax = info.max;
        
        if (customRange != null && customRange.enabled) {
            effectiveMin = customRange.customMin;
            effectiveMax = customRange.customMax;
        }
        
        // Clamp value to range
        realValue = (float) Math.max(effectiveMin, Math.min(effectiveMax, realValue));
        
        // Convert to normalized (0-1)
        float normalized = (float)((realValue - effectiveMin) / (effectiveMax - effectiveMin));
        
        // Convert to slider position (0-1000)
        return (int)(normalized * 1000);
    }




        /**
    * Apply preset parameter values to RNBO
    */
    
    /**
     * Show rename dialog
     */
    private void showRenameDialog(Preset preset) {
        android.app.AlertDialog.Builder builder = new android.app.AlertDialog.Builder(this);
        builder.setTitle("Rename Preset");
        builder.setMessage("Enter new name:");
        
        final android.widget.EditText input = new android.widget.EditText(this);
        input.setText(preset.name);
        input.setSelection(preset.name.length());
        input.setPadding(20, 20, 20, 20);
        builder.setView(input);
        
        builder.setPositiveButton("Rename", (dialog, which) -> {
            String newName = input.getText().toString().trim();
            if (!newName.isEmpty()) {
                preset.name = newName;
                preset.timestamp = System.currentTimeMillis();
                savePresetsToStorage();
                refreshParameterDisplay();
                android.widget.Toast.makeText(this, "Preset renamed!", android.widget.Toast.LENGTH_SHORT).show();
            }
        });
        
        builder.setNegativeButton("Cancel", null);
        builder.show();
    }
    
    /**
     * Show delete confirmation dialog
     */
    private void showDeleteConfirmation(Preset preset) {
        android.app.AlertDialog.Builder builder = new android.app.AlertDialog.Builder(this);
        builder.setTitle("Delete Preset");
        builder.setMessage("Are you sure you want to delete '" + preset.name + "'?");
        
        builder.setPositiveButton("Delete", (dialog, which) -> {
            savedPresets.remove(preset);
            savePresetsToStorage();
            refreshParameterDisplay();
            android.widget.Toast.makeText(this, "Preset deleted", android.widget.Toast.LENGTH_SHORT).show();
            Log.i("RNBO_Presets", "Preset deleted: " + preset.name);
        });
        
        builder.setNegativeButton("Cancel", null);
        builder.show();
    }
    
    // ============================================================================
    // INTERPOLATION SYSTEM
    // ============================================================================
    
    /**
     * Start interpolation to preset
     */
    private void startInterpolation(final Preset preset) {
        isInterpolating = true;
        interpolationStartTime = System.currentTimeMillis();
        
        Log.i("RNBO_Interpolation", "Starting interpolation to '" + preset.name + "' over " + interpolationTimeMs + "ms");
        
        // Store current values as start
        interpolationStartValues.clear();
        for (String paramName : allParamNames) {
            float currentValue = getCurrentParamValue(paramName);
            interpolationStartValues.put(paramName, currentValue);
        }
        
        // Store target values from preset
        interpolationTargetValues.clear();
        if (preset.parameterValues != null) {
            interpolationTargetValues.putAll(preset.parameterValues);
        }
        
        // Load preset configuration (ranges, sensors, etc) immediately
        sensorControls = deepCopySensorControls(preset.sensorControls);
        customRangeMap = deepCopyCustomRanges(preset.customRanges);
        liveParameters = new HashSet<>(preset.liveParameters);
        
        // Temporarily disable sensors during interpolation
        final boolean wasSensorsEnabled = sensorsGloballyEnabled;
        if (sensorsGloballyEnabled) {
            unregisterSensorListeners();
        }
        
        // Start interpolation loop
        interpolationRunnable = new Runnable() {
            @Override
            public void run() {
                long elapsed = System.currentTimeMillis() - interpolationStartTime;
                float progress = Math.min(1.0f, (float) elapsed / interpolationTimeMs);
                
                if (progress < 1.0f) {
                    // Update all parameters with interpolated values
                    updateInterpolatedValues(progress);
                    
                    // Continue interpolation (~60fps)
                    interpolationHandler.postDelayed(this, 16);
                } else {
                    // Interpolation complete
                    finishInterpolation(preset, wasSensorsEnabled);
                }
            }
        };
        
        interpolationHandler.post(interpolationRunnable);
    }
    
    /**
     * Update parameters during interpolation
     */
    private void updateInterpolatedValues(float progress) {
        // Apply easing for smoother transitions
        float easedProgress = easeInOutQuad(progress);
        
        for (String paramName : allParamNames) {
            Float startVal = interpolationStartValues.get(paramName);
            Float targetVal = interpolationTargetValues.get(paramName);
            
            if (startVal != null && targetVal != null) {
                // Linear interpolation with easing
                float interpolatedValue = startVal + (targetVal - startVal) * easedProgress;
                
                // Update RNBO parameter
                updateParam(paramName, interpolatedValue);
                
                // Update slider position visually
                int sliderPos = calculateSliderPosition(paramName, interpolatedValue);
                sliderPositions.put(paramName, sliderPos);

                // Update slider UI if visible
                PureDataSlider slider = activeSliders.get(paramName);
                if (slider != null) {
                    slider.setProgress(sliderPos);
                    slider.setCurrentValue(interpolatedValue);
                }
            }
        }
    }
    
    /**
     * Finish interpolation
     */
    private void finishInterpolation(Preset preset, boolean restoreSensors) {
        isInterpolating = false;
        
        Log.i("RNBO_Interpolation", "Interpolation complete");
        
        // Apply final values
        if (preset.parameterValues != null) {
            for (Map.Entry<String, Float> entry : preset.parameterValues.entrySet()) {
                updateParam(entry.getKey(), entry.getValue());
                int pos = calculateSliderPosition(entry.getKey(), entry.getValue());
                sliderPositions.put(entry.getKey(), pos);
            }
        }
        
        // Save configuration
        sensorsGloballyEnabled = preset.sensorsGloballyEnabled;
        saveSensorControls();
        saveCustomRanges();
        saveLiveConfig();
        saveSensorGlobalState();
        updateSensorToggleButton();
        
        // Restore sensors if needed
        if (restoreSensors && preset.sensorsGloballyEnabled) {
            registerSensorListeners();
        }
        
        // Refresh UI
        if (!currentMode.equals("live")) {
            switchMode("live");
        }
        
        android.widget.Toast.makeText(this, "Preset '" + preset.name + "' loaded", android.widget.Toast.LENGTH_SHORT).show();
    }
    
    /**
     * Easing function for smooth interpolation
     */
    private float easeInOutQuad(float t) {
        return t < 0.5f ? 2 * t * t : 1 - (float)Math.pow(-2 * t + 2, 2) / 2;
    }
    
    /**
     * Deep copy sensor controls
     */
    private Map<String, SensorControl> deepCopySensorControls(Map<String, SensorControl> original) {
        Map<String, SensorControl> copy = new HashMap<>();
        for (Map.Entry<String, SensorControl> entry : original.entrySet()) {
            SensorControl orig = entry.getValue();
            SensorControl newControl = new SensorControl();
            newControl.controlMode = orig.controlMode;
            newControl.sensorType = orig.sensorType;
            newControl.dataSource = orig.dataSource;
            newControl.sensitivity = orig.sensitivity;
            newControl.smoothing = orig.smoothing;
            newControl.invert = orig.invert;
            copy.put(entry.getKey(), newControl);
        }
        return copy;
    }
    
    /**
     * Deep copy custom ranges
     */
    private Map<String, CustomRange> deepCopyCustomRanges(Map<String, CustomRange> original) {
        Map<String, CustomRange> copy = new HashMap<>();
        for (Map.Entry<String, CustomRange> entry : original.entrySet()) {
            CustomRange orig = entry.getValue();
            CustomRange newRange = new CustomRange(orig.enabled, orig.customMin, orig.customMax);
            copy.put(entry.getKey(), newRange);
        }
        return copy;
    }
    
    /**
     * Save presets to SharedPreferences
     */
    private void savePresetsToStorage() {
        try {
            SharedPreferences prefs = getSharedPreferences("RNBOPrefs", MODE_PRIVATE);
            SharedPreferences.Editor editor = prefs.edit();
            
            org.json.JSONArray presetsArray = new org.json.JSONArray();
            
            for (Preset preset : savedPresets) {
                org.json.JSONObject presetObj = new org.json.JSONObject();
                presetObj.put("name", preset.name);
                presetObj.put("timestamp", preset.timestamp);
                presetObj.put("sensorsGloballyEnabled", preset.sensorsGloballyEnabled);
                
                // Save sensor controls
                org.json.JSONObject sensorControlsObj = new org.json.JSONObject();
                for (Map.Entry<String, SensorControl> entry : preset.sensorControls.entrySet()) {
                    org.json.JSONObject controlObj = new org.json.JSONObject();
                    SensorControl sc = entry.getValue();
                    controlObj.put("controlMode", sc.controlMode);
                    controlObj.put("sensorType", sc.sensorType);
                    controlObj.put("dataSource", sc.dataSource);
                    controlObj.put("sensitivity", sc.sensitivity);
                    controlObj.put("smoothing", sc.smoothing);
                    controlObj.put("invert", sc.invert);
                    sensorControlsObj.put(entry.getKey(), controlObj);
                }
                presetObj.put("sensorControls", sensorControlsObj);
                
                // Save custom ranges
                org.json.JSONObject customRangesObj = new org.json.JSONObject();
                for (Map.Entry<String, CustomRange> entry : preset.customRanges.entrySet()) {
                    org.json.JSONObject rangeObj = new org.json.JSONObject();
                    CustomRange cr = entry.getValue();
                    rangeObj.put("enabled", cr.enabled);
                    rangeObj.put("customMin", cr.customMin);
                    rangeObj.put("customMax", cr.customMax);
                    customRangesObj.put(entry.getKey(), rangeObj);
                }
                presetObj.put("customRanges", customRangesObj);
                
                // Save live parameters
                org.json.JSONArray liveParamsArray = new org.json.JSONArray();
                for (String param : preset.liveParameters) {
                    liveParamsArray.put(param);
                }
                presetObj.put("liveParameters", liveParamsArray);
                
                // Save parameter values (real values)
                org.json.JSONObject parameterValuesObj = new org.json.JSONObject();
                if (preset.parameterValues != null) {
                    for (Map.Entry<String, Float> entry : preset.parameterValues.entrySet()) {
                        parameterValuesObj.put(entry.getKey(), entry.getValue());
                    }
                }
                presetObj.put("parameterValues", parameterValuesObj);
                
                // Save quick preset flag
                presetObj.put("isQuickPreset", preset.isQuickPreset);
                
                presetsArray.put(presetObj);
            }
            
            editor.putString(PRESETS_PREF_KEY, presetsArray.toString());
            editor.apply();
            
            Log.i("RNBO_Presets", "Presets saved to storage: " + savedPresets.size());
        } catch (Exception e) {
            Log.e("RNBO_Presets", "Error saving presets: " + e.getMessage());
            e.printStackTrace();
        }
    }
    
    /**
     * Load presets from SharedPreferences
     */
    private void loadPresetsFromStorage() {
        try {
            SharedPreferences prefs = getSharedPreferences("RNBOPrefs", MODE_PRIVATE);
            String presetsJson = prefs.getString(PRESETS_PREF_KEY, "[]");
            
            org.json.JSONArray presetsArray = new org.json.JSONArray(presetsJson);
            savedPresets.clear();
            
            for (int i = 0; i < presetsArray.length(); i++) {
                org.json.JSONObject presetObj = presetsArray.getJSONObject(i);
                
                String name = presetObj.getString("name");
                Preset preset = new Preset(name);
                preset.timestamp = presetObj.getLong("timestamp");
                preset.sensorsGloballyEnabled = presetObj.optBoolean("sensorsGloballyEnabled", true);
                
                // Load sensor controls
                org.json.JSONObject sensorControlsObj = presetObj.getJSONObject("sensorControls");
                java.util.Iterator<String> scKeys = sensorControlsObj.keys();
                while (scKeys.hasNext()) {
                    String key = scKeys.next();
                    org.json.JSONObject controlObj = sensorControlsObj.getJSONObject(key);
                    SensorControl sc = new SensorControl();
                    sc.controlMode = controlObj.getString("controlMode");
                    sc.sensorType = controlObj.getString("sensorType");
                    sc.dataSource = controlObj.optString("dataSource", "");
                    sc.sensitivity = (float) controlObj.getDouble("sensitivity");
                    sc.smoothing = (float) controlObj.getDouble("smoothing");
                    sc.invert = controlObj.getBoolean("invert");
                    preset.sensorControls.put(key, sc);
                }
                
                // Load custom ranges
                org.json.JSONObject customRangesObj = presetObj.getJSONObject("customRanges");
                java.util.Iterator<String> crKeys = customRangesObj.keys();
                while (crKeys.hasNext()) {
                    String key = crKeys.next();
                    org.json.JSONObject rangeObj = customRangesObj.getJSONObject(key);
                    CustomRange cr = new CustomRange(
                        rangeObj.getBoolean("enabled"),
                        rangeObj.getDouble("customMin"),
                        rangeObj.getDouble("customMax")
                    );
                    preset.customRanges.put(key, cr);
                }
                
                // Load live parameters
                org.json.JSONArray liveParamsArray = presetObj.getJSONArray("liveParameters");
                for (int j = 0; j < liveParamsArray.length(); j++) {
                    preset.liveParameters.add(liveParamsArray.getString(j));
                }
                
                // Load parameter values
                if (presetObj.has("parameterValues")) {
                    org.json.JSONObject parameterValuesObj = presetObj.getJSONObject("parameterValues");
                    java.util.Iterator<String> valKeys = parameterValuesObj.keys();
                    while (valKeys.hasNext()) {
                        String key = valKeys.next();
                        preset.parameterValues.put(key, (float) parameterValuesObj.getDouble(key));
                    }
                }
                
                // Load quick preset flag
                preset.isQuickPreset = presetObj.optBoolean("isQuickPreset", false);
                
                savedPresets.add(preset);
            }
            
            Log.i("RNBO_Presets", "Presets loaded from storage: " + savedPresets.size());
        } catch (Exception e) {
            Log.e("RNBO_Presets", "Error loading presets: " + e.getMessage());
            e.printStackTrace();
        }
    }
    
    /**
     * Toggle preset as quick preset
     */
    private void toggleQuickPreset(Preset preset) {
        if (quickPresets.contains(preset)) {
            quickPresets.remove(preset);
            preset.isQuickPreset = false;
            android.widget.Toast.makeText(this, "Removed from quick presets", android.widget.Toast.LENGTH_SHORT).show();
        } else {
            if (quickPresets.size() >= MAX_QUICK_PRESETS) {
                android.widget.Toast.makeText(this, "Max " + MAX_QUICK_PRESETS + " quick presets", android.widget.Toast.LENGTH_SHORT).show();
                return;
            }
            quickPresets.add(preset);
            preset.isQuickPreset = true;
            android.widget.Toast.makeText(this, "Added to quick presets", android.widget.Toast.LENGTH_SHORT).show();
        }
        savePresetsToStorage();
    }
    
    /**
     * Load quick presets from saved presets
     */
    private void loadQuickPresets() {
        quickPresets.clear();
        for (Preset preset : savedPresets) {
            if (preset.isQuickPreset && quickPresets.size() < MAX_QUICK_PRESETS) {
                quickPresets.add(preset);
            }
        }
        Log.i("RNBO_QuickPresets", "Loaded " + quickPresets.size() + " quick presets");
    }
    
    /**
     * Save interpolation settings
     */
    private void saveInterpolationSettings() {
        SharedPreferences prefs = getSharedPreferences("RNBOPrefs", MODE_PRIVATE);
        prefs.edit()
            .putBoolean("interpolation_enabled", interpolationEnabled)
            .putInt("interpolation_time_ms", interpolationTimeMs)
            .apply();
    }
    
    /**
     * Load interpolation settings
     */
    private void loadInterpolationSettings() {
        SharedPreferences prefs = getSharedPreferences("RNBOPrefs", MODE_PRIVATE);
        interpolationEnabled = prefs.getBoolean("interpolation_enabled", false);
        interpolationTimeMs = prefs.getInt("interpolation_time_ms", 1000);
        Log.i("RNBO_Interpolation", "Loaded settings: enabled=" + interpolationEnabled + ", time=" + interpolationTimeMs + "ms");
    }

    private void displayParameterSliders() {
        if (msliderListener == null) {
            msliderListener = new SliderListener();
        }
        
        // Setup live fixed controls if in LIVE mode
        if (currentMode.equals("live")) {
            setupLiveFixedControls();
        }
        
        for (String paramName : allParamNames) {
            if (shouldShowParameter(paramName)) {
                ParameterInfo info = parameterInfoMap.get(paramName);
                if (info == null) {
                    info = new ParameterInfo(paramName, 0, 1, 0.5);
                }
                
                // Get custom range if enabled
                CustomRange customRange = customRangeMap.get(paramName);
                double effectiveMin = info.min;
                double effectiveMax = info.max;
                
                if (customRange != null && customRange.enabled) {
                    effectiveMin = customRange.customMin;
                    effectiveMax = customRange.customMax;
                }
                
                // Create wrapper LinearLayout with dark background
                LinearLayout paramWrapper = new LinearLayout(this);
                paramWrapper.setOrientation(LinearLayout.VERTICAL);
                paramWrapper.setBackgroundColor(Color.parseColor("#2A2A2A"));
                LayoutParams wrapperParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
                wrapperParams.setMargins(6, 4, 6, 4);
                paramWrapper.setLayoutParams(wrapperParams);
                paramWrapper.setPadding(8, 8, 8, 8);

                // Calculate current value
                Integer savedPosition = sliderPositions.get(paramName);
                RangeInfo lastRange = lastUsedRanges.get(paramName);
                int newPosition;

                if (savedPosition != null && lastRange != null) {
                    // Calcola il valore audio reale dal range precedente
                    float normalizedOld = savedPosition / 1000.0f;
                    double realValue = lastRange.min + normalizedOld * (lastRange.max - lastRange.min);

                    // Se il valore è dentro il nuovo range, rimappa la posizione
                    if (realValue >= effectiveMin && realValue <= effectiveMax) {
                        // Rimappa sul nuovo range
                        float normalizedNew = (float)((realValue - effectiveMin) / (effectiveMax - effectiveMin));
                        newPosition = (int)(normalizedNew * 1000);

                        Log.i("RNBO_MultiMode", String.format("%s: remapped from %d to %d (value=%.2f in new range [%.2f-%.2f])",
                            paramName, savedPosition, newPosition, realValue, effectiveMin, effectiveMax));
                    } else {
                        // Valore fuori range, mantieni posizione (che ora produrrà un valore diverso)
                        newPosition = savedPosition;

                        Log.i("RNBO_MultiMode", String.format("%s: kept position %d (value=%.2f outside new range [%.2f-%.2f])",
                            paramName, savedPosition, realValue, effectiveMin, effectiveMax));
                    }
                } else {
                    // Prima volta o no range precedente, usa initialValue
                    float normalizedInitial = (float)((info.initialValue - effectiveMin) / (effectiveMax - effectiveMin));
                    newPosition = (int)(normalizedInitial * 1000);

                    Log.i("RNBO_MultiMode", String.format("%s: initial position %d (value=%.2f)",
                        paramName, newPosition, info.initialValue));
                }

                // Salva posizione e range corrente per il prossimo cambio
                sliderPositions.put(paramName, newPosition);
                lastUsedRanges.put(paramName, new RangeInfo(effectiveMin, effectiveMax));

                float currentValue = (float)(effectiveMin + (newPosition / 1000.0) * (effectiveMax - effectiveMin));

                // Create Pure Data style slider with text inside
                PureDataSlider slider = new PureDataSlider(this);
                slider.setTag(paramName);
                slider.setMax(1000);
                slider.setProgress(newPosition);
                slider.setParameterName(paramName);
                slider.setCurrentValue(currentValue);

                // Set listener for slider changes
                final String finalParamName = paramName;
                final double finalEffectiveMin = effectiveMin;
                final double finalEffectiveMax = effectiveMax;
                slider.setOnProgressChangeListener(new PureDataSlider.OnProgressChangeListener() {
                    @Override
                    public void onProgressChanged(int progress) {
                        // Save slider position
                        sliderPositions.put(finalParamName, progress);

                        // Check if controlled by sensor
                        SensorControl sensorControl = sensorControls.get(finalParamName);
                        if (sensorControl != null && sensorControl.controlMode.equals("sensor") && sensorsGloballyEnabled) {
                            return; // Ignore manual input
                        }

                        // Calculate value
                        float normalizedValue = progress / 1000.0f;
                        float realValue = (float)(finalEffectiveMin + normalizedValue * (finalEffectiveMax - finalEffectiveMin));

                        // Update slider display
                        slider.setCurrentValue(realValue);

                        // Update parameter
                        updateParam(finalParamName, realValue);
                    }
                });

                // Save reference to active slider
                activeSliders.put(paramName, slider);

                LayoutParams sliderParams = new LayoutParams(LayoutParams.MATCH_PARENT, 80);
                sliderParams.setMargins(0, 4, 0, 0);
                slider.setLayoutParams(sliderParams);
                paramWrapper.addView(slider);

                paramHolder.addView(paramWrapper);
            }
        }
    }
    
    /**
     * Setup fixed controls in LIVE mode
     */
    private void setupLiveFixedControls() {
        // Clear previous controls (except sensor toggle which is always first)
        int childCount = liveFixedControls.getChildCount();
        if (childCount > 1) {
            liveFixedControls.removeViews(1, childCount - 1);
        }
        
        // Add quick presets
        addQuickPresetsToFixed();
        
        // Add interpolation controls
        addInterpolationControlsToFixed();
        
        // Add separator
        View separator = new View(this);
        separator.setBackgroundColor(Color.parseColor("#444444"));
        LayoutParams sepParams = new LayoutParams(LayoutParams.MATCH_PARENT, 3);
        sepParams.setMargins(20, 10, 20, 10);
        separator.setLayoutParams(sepParams);
        liveFixedControls.addView(separator);
    }
    
    /**
     * Add quick preset buttons to live fixed controls
     */
    private void addQuickPresetsToFixed() {
        if (quickPresets.isEmpty()) return;
        
        TextView label = new TextView(this);
        label.setText("Quick Presets:");
        label.setTextSize(13);
        label.setTextColor(Color.WHITE);
        label.setTypeface(null, android.graphics.Typeface.BOLD);
        label.setPadding(20, 10, 20, 5);
        liveFixedControls.addView(label);
        
        // Create grid of buttons (4 per row)
        int buttonsPerRow = 4;
        LinearLayout currentRow = null;
        
        for (int i = 0; i < Math.min(quickPresets.size(), MAX_QUICK_PRESETS); i++) {
            if (i % buttonsPerRow == 0) {
                currentRow = new LinearLayout(this);
                currentRow.setOrientation(LinearLayout.HORIZONTAL);
                currentRow.setGravity(Gravity.CENTER);
                LayoutParams rowParams = new LayoutParams(
                    LayoutParams.MATCH_PARENT, 
                    LayoutParams.WRAP_CONTENT
                );
                rowParams.setMargins(20, 3, 20, 3);
                currentRow.setLayoutParams(rowParams);
                liveFixedControls.addView(currentRow);
            }
            
            final Preset preset = quickPresets.get(i);
            
            Button presetBtn = new Button(this);
            String displayName = (i + 1) + "." + preset.name;
            if (displayName.length() > 12) {
                displayName = displayName.substring(0, 10) + "..";
            }
            presetBtn.setText(displayName);
            presetBtn.setTextSize(11);
            presetBtn.setBackgroundColor(Color.parseColor("#333333"));
            presetBtn.setTextColor(Color.WHITE);
            presetBtn.setPadding(8, 12, 8, 12);
            
            LayoutParams btnParams = new LayoutParams(
                0, 
                LayoutParams.WRAP_CONTENT,
                1.0f
            );
            btnParams.setMargins(3, 0, 3, 0);
            presetBtn.setLayoutParams(btnParams);
            
            presetBtn.setOnClickListener(v -> {
                if (!isInterpolating) {
                    loadPreset(preset);
                } else {
                    android.widget.Toast.makeText(this, "Interpolation in progress...", android.widget.Toast.LENGTH_SHORT).show();
                }
            });
            
            currentRow.addView(presetBtn);
        }
    }
    
    /**
     * Add interpolation controls to live fixed controls
     */
    private void addInterpolationControlsToFixed() {
        // Compact layout for interpolation
        LinearLayout interpolationRow = new LinearLayout(this);
        interpolationRow.setOrientation(LinearLayout.HORIZONTAL);
        interpolationRow.setGravity(Gravity.CENTER_VERTICAL);
        LayoutParams rowParams = new LayoutParams(
            LayoutParams.MATCH_PARENT,
            LayoutParams.WRAP_CONTENT
        );
        rowParams.setMargins(20, 10, 20, 5);
        interpolationRow.setLayoutParams(rowParams);
        
        // Checkbox
        CheckBox interpolateCheck = new CheckBox(this);
        interpolateCheck.setText("Interpolate");
        interpolateCheck.setTextSize(12);
        interpolateCheck.setTextColor(Color.WHITE);
        interpolateCheck.setChecked(interpolationEnabled);
        LayoutParams checkParams = new LayoutParams(
            LayoutParams.WRAP_CONTENT,
            LayoutParams.WRAP_CONTENT
        );
        interpolateCheck.setLayoutParams(checkParams);
        interpolateCheck.setOnCheckedChangeListener((buttonView, isChecked) -> {
            interpolationEnabled = isChecked;
            saveInterpolationSettings();
        });
        interpolationRow.addView(interpolateCheck);
        
        // Time label + slider in vertical layout
        LinearLayout timeLayout = new LinearLayout(this);
        timeLayout.setOrientation(LinearLayout.VERTICAL);
        LayoutParams timeLayoutParams = new LayoutParams(
            0,
            LayoutParams.WRAP_CONTENT,
            1.0f
        );
        timeLayoutParams.setMargins(10, 0, 0, 0);
        timeLayout.setLayoutParams(timeLayoutParams);
        
        final TextView timeLabel = new TextView(this);
        timeLabel.setText(String.format("Time: %dms", interpolationTimeMs));
        timeLabel.setTextSize(11);
        timeLabel.setTextColor(Color.parseColor("#AAAAAA"));
        timeLayout.addView(timeLabel);
        
        AppCompatSeekBar timeSlider = new AppCompatSeekBar(this);
        timeSlider.setMax(1999);
        timeSlider.setProgress(interpolationTimeMs - 1);
        timeSlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                interpolationTimeMs = progress + 1;
                timeLabel.setText(String.format("Time: %dms", interpolationTimeMs));
                if (fromUser) saveInterpolationSettings();
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override public void onStopTrackingTouch(SeekBar seekBar) {}
        });
        timeLayout.addView(timeSlider);
        
        interpolationRow.addView(timeLayout);
        
        liveFixedControls.addView(interpolationRow);
    }
    
    private boolean shouldShowParameter(String paramName) {
        if (currentMode.equals("all")) {
            return true;
        } else if (currentMode.equals("live")) {
            return liveParameters.isEmpty() || liveParameters.contains(paramName);
        }
        return false;
    }
    
    private void saveLiveConfig() {
        SharedPreferences prefs = getSharedPreferences("rnbo_config", MODE_PRIVATE);
        SharedPreferences.Editor editor = prefs.edit();
        editor.putStringSet("live_parameters", liveParameters);
        editor.putInt("config_version", 1);
        editor.apply();
        Log.i("RNBO_MultiMode", "Config saved");
    }
    
    private void loadLiveConfig() {
        SharedPreferences prefs = getSharedPreferences("rnbo_config", MODE_PRIVATE);
        Set<String> savedParams = prefs.getStringSet("live_parameters", new HashSet<>());
        if (savedParams != null) {
            liveParameters = new HashSet<>(savedParams);
        }
        Log.i("RNBO_MultiMode", "Config loaded: " + liveParameters.size() + " live parameters");
    }

    private void saveCustomRanges() {
        SharedPreferences prefs = getSharedPreferences("rnbo_config", MODE_PRIVATE);
        SharedPreferences.Editor editor = prefs.edit();
        
        for (Map.Entry<String, CustomRange> entry : customRangeMap.entrySet()) {
            String paramName = entry.getKey();
            CustomRange range = entry.getValue();
            
            editor.putBoolean(paramName + "_custom_enabled", range.enabled);
            editor.putFloat(paramName + "_custom_min", (float)range.customMin);
            editor.putFloat(paramName + "_custom_max", (float)range.customMax);
        }
        
        editor.apply();
        Log.i("RNBO_MultiMode", "Custom ranges saved");
    }
    
    private void loadCustomRanges() {
        SharedPreferences prefs = getSharedPreferences("rnbo_config", MODE_PRIVATE);
        
        for (Map.Entry<String, ParameterInfo> entry : parameterInfoMap.entrySet()) {
            String paramName = entry.getKey();
            ParameterInfo info = entry.getValue();
            
            boolean enabled = prefs.getBoolean(paramName + "_custom_enabled", false);
            float customMin = prefs.getFloat(paramName + "_custom_min", (float)info.min);
            float customMax = prefs.getFloat(paramName + "_custom_max", (float)info.max);
            
            customRangeMap.put(paramName, new CustomRange(enabled, customMin, customMax));
        }
        
        Log.i("RNBO_MultiMode", "Custom ranges loaded");
    }
    
    private void saveSensorControls() {
        SharedPreferences prefs = getSharedPreferences("rnbo_config", MODE_PRIVATE);
        SharedPreferences.Editor editor = prefs.edit();
        
        for (Map.Entry<String, SensorControl> entry : sensorControls.entrySet()) {
            String paramName = entry.getKey();
            SensorControl control = entry.getValue();
            
            editor.putString(paramName + "_control_mode", control.controlMode);
            editor.putString(paramName + "_sensor_type", control.sensorType);
            editor.putFloat(paramName + "_sensitivity", control.sensitivity);
            editor.putBoolean(paramName + "_invert", control.invert);
            editor.putFloat(paramName + "_smoothing", control.smoothing);
        }
        
        editor.apply();
        Log.i("RNBO_MultiMode", "Sensor controls saved");
    }
    
    private void loadSensorControls() {
        SharedPreferences prefs = getSharedPreferences("rnbo_config", MODE_PRIVATE);
        
        for (Map.Entry<String, ParameterInfo> entry : parameterInfoMap.entrySet()) {
            String paramName = entry.getKey();
            
            SensorControl control = new SensorControl();
            control.controlMode = prefs.getString(paramName + "_control_mode", "manual");
            control.sensorType = prefs.getString(paramName + "_sensor_type", "manual");
            control.sensitivity = prefs.getFloat(paramName + "_sensitivity", 0.5f);
            control.invert = prefs.getBoolean(paramName + "_invert", false);
            control.smoothing = prefs.getFloat(paramName + "_smoothing", 0.3f);
            
            sensorControls.put(paramName, control);
        }
        
        // Load global sensor state
        sensorsGloballyEnabled = prefs.getBoolean("sensors_globally_enabled", true);
        
        Log.i("RNBO_MultiMode", "Sensor controls loaded");
    }
    
    private void saveSensorGlobalState() {
        SharedPreferences prefs = getSharedPreferences("rnbo_config", MODE_PRIVATE);
        SharedPreferences.Editor editor = prefs.edit();
        editor.putBoolean("sensors_globally_enabled", sensorsGloballyEnabled);
        editor.apply();
    }

    private class SliderListener implements SeekBar.OnSeekBarChangeListener {
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            String paramName = (String) seekBar.getTag();
            
            // Check if controlled by sensor - if yes, ignore manual touch
            SensorControl sensorControl = sensorControls.get(paramName);
            if (sensorControl != null && sensorControl.controlMode.equals("sensor") && sensorsGloballyEnabled) {
                // Sensor is active, ignore manual input
                return;
            }
            
            // Salva la posizione dello slider
            sliderPositions.put(paramName, progress);
            
            ParameterInfo info = parameterInfoMap.get(paramName);
            
            if (info != null) {
                // Use custom range if enabled
                CustomRange customRange = customRangeMap.get(paramName);
                double effectiveMin = info.min;
                double effectiveMax = info.max;
                
                if (customRange != null && customRange.enabled) {
                    effectiveMin = customRange.customMin;
                    effectiveMax = customRange.customMax;
                }
                
                float normalizedValue = progress / 1000.0f;
                float realValue = (float)(effectiveMin + normalizedValue * (effectiveMax - effectiveMin));

                // Update value label
                TextView valueLabel = sliderValueLabels.get(paramName);
                if (valueLabel != null) {
                    // Get sensor control indicator if present
                    SensorControl sc = sensorControls.get(paramName);
                    String controlIndicator = "";
                    if (sc != null && sc.controlMode.equals("sensor") && sensorsGloballyEnabled) {
                        String sensorDisplay = sc.sensorType;
                        if (!sc.dataSource.isEmpty()) {
                            sensorDisplay += " " + sc.dataSource;
                        }
                        controlIndicator = " [" + sensorDisplay.toUpperCase() + "]";
                    }
                    valueLabel.setText(String.format("%s: %.2f%s", paramName, realValue, controlIndicator));
                }

                Log.d("RNBO_MultiMode", String.format("%s: progress=%d -> value=%.2f [%.2f-%.2f]",
                    paramName, progress, realValue, effectiveMin, effectiveMax));

                updateParam(paramName, realValue);
            } else {
                Log.d("RNBO_MultiMode", "Progress: " + progress + " for " + paramName);
                updateParam(paramName, (float) progress);
            }
        }
        public void onStartTrackingTouch(SeekBar seekBar) {}
        public void onStopTrackingTouch(SeekBar seekBar) {}
    }


    public void declareSliders(@NonNull String[] paramNames) {
        allParamNames = paramNames;
        Log.i("RNBO_MultiMode", "Params: " + paramNames.length + ", Mode: " + currentMode);
        refreshParameterDisplay();
    }

    // ============================================================================
    // BUFFER MANAGEMENT
    // ============================================================================

    /**
     * Add buffer management UI in setup mode
     */
    private void addBufferManagementUI() {
        // Title
        TextView bufferTitle = new TextView(this);
        bufferTitle.setText("AUDIO BUFFERS");
        bufferTitle.setTextSize(16);
        bufferTitle.setTextColor(Color.WHITE);
        bufferTitle.setTypeface(null, android.graphics.Typeface.BOLD);
        bufferTitle.setGravity(Gravity.CENTER);
        bufferTitle.setPadding(0, 10, 0, 15);
        paramHolder.addView(bufferTitle);

        // Hardcoded buffers from dependencies.json
        // TODO: Parse dynamically from dependencies.json if needed
        String[] buffers = {"drums"}; // Add more buffer IDs here if you have them

        for (String bufferId : buffers) {
            // Create button layout
            LinearLayout bufferLayout = new LinearLayout(this);
            bufferLayout.setOrientation(LinearLayout.HORIZONTAL);
            bufferLayout.setGravity(Gravity.CENTER_VERTICAL);
            LayoutParams bufferParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
            bufferParams.setMargins(20, 5, 20, 5);
            bufferLayout.setLayoutParams(bufferParams);
            bufferLayout.setBackgroundColor(Color.parseColor("#2A2A2A"));
            bufferLayout.setPadding(15, 15, 15, 15);

            // Buffer name label
            TextView bufferLabel = new TextView(this);
            bufferLabel.setText("Buffer: " + bufferId);
            bufferLabel.setTextSize(15);
            bufferLabel.setTextColor(Color.WHITE);
            LinearLayout.LayoutParams labelParams = new LinearLayout.LayoutParams(
                0, LayoutParams.WRAP_CONTENT, 1f
            );
            bufferLabel.setLayoutParams(labelParams);
            bufferLayout.addView(bufferLabel);

            // Load button
            Button loadButton = new Button(this);
            loadButton.setText("LOAD FILE");
            loadButton.setTextSize(13);
            loadButton.setBackgroundColor(Color.parseColor("#4CAF50"));
            loadButton.setTextColor(Color.WHITE);
            LinearLayout.LayoutParams buttonParams = new LinearLayout.LayoutParams(
                LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT
            );
            buttonParams.setMargins(10, 0, 0, 0);
            loadButton.setLayoutParams(buttonParams);

            final String finalBufferId = bufferId;
            loadButton.setOnClickListener(v -> openBufferFilePicker(finalBufferId));

            bufferLayout.addView(loadButton);
            paramHolder.addView(bufferLayout);
        }
    }

    /**
     * Open file picker to select audio file for buffer
     */
    private void openBufferFilePicker(String bufferId) {
        currentBufferIdToLoad = bufferId;

        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("audio/*");
        intent.addCategory(Intent.CATEGORY_OPENABLE);

        try {
            startActivityForResult(
                Intent.createChooser(intent, "Select Audio File for " + bufferId),
                REQUEST_CODE_PICK_AUDIO_FILE
            );
        } catch (android.content.ActivityNotFoundException ex) {
            Toast.makeText(this, "Please install a File Manager", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_CODE_PICK_AUDIO_FILE && resultCode == RESULT_OK) {
            if (data != null && data.getData() != null) {
                Uri audioFileUri = data.getData();
                String filePath = getFilePathFromUri(audioFileUri);

                if (filePath != null && currentBufferIdToLoad != null) {
                    Log.i("BufferLoader", "Loading buffer '" + currentBufferIdToLoad + "' from: " + filePath);

                    boolean success = loadBuffer(currentBufferIdToLoad, filePath);

                    if (success) {
                        Toast.makeText(this,
                            "Buffer '" + currentBufferIdToLoad + "' loaded successfully!",
                            Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(this,
                            "Failed to load buffer. Check if file is .wav or .aif format",
                            Toast.LENGTH_LONG).show();
                    }

                    currentBufferIdToLoad = null;
                } else {
                    Toast.makeText(this, "Could not access file", Toast.LENGTH_SHORT).show();
                }
            }
        }
    }

    /**
     * Get real file path from content URI
     */
    private String getFilePathFromUri(Uri uri) {
        String filePath = null;

        if ("content".equalsIgnoreCase(uri.getScheme())) {
            // Try to get real path using ContentResolver
            String[] projection = { "_data" };
            android.database.Cursor cursor = null;

            try {
                cursor = getContentResolver().query(uri, projection, null, null, null);
                if (cursor != null && cursor.moveToFirst()) {
                    int columnIndex = cursor.getColumnIndexOrThrow("_data");
                    filePath = cursor.getString(columnIndex);
                }
            } catch (Exception e) {
                Log.w("BufferLoader", "Could not get file path from cursor, trying alternative method");
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }

            // Alternative: Copy file to cache directory
            if (filePath == null) {
                try {
                    java.io.InputStream inputStream = getContentResolver().openInputStream(uri);
                    if (inputStream != null) {
                        // Create temp file in cache
                        String fileName = "temp_audio_" + System.currentTimeMillis();
                        java.io.File tempFile = new java.io.File(getCacheDir(), fileName);

                        java.io.FileOutputStream outputStream = new java.io.FileOutputStream(tempFile);
                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = inputStream.read(buffer)) != -1) {
                            outputStream.write(buffer, 0, bytesRead);
                        }

                        inputStream.close();
                        outputStream.close();

                        filePath = tempFile.getAbsolutePath();
                        Log.i("BufferLoader", "Copied file to cache: " + filePath);
                    }
                } catch (Exception e) {
                    Log.e("BufferLoader", "Failed to copy file to cache", e);
                }
            }
        } else if ("file".equalsIgnoreCase(uri.getScheme())) {
            filePath = uri.getPath();
        }

        return filePath;
    }

    public native void init(android.content.res.AssetManager assetManager);
    public native void updateParam(String name, float val);
    public native boolean loadBuffer(String bufferId, String filePath);

    
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

}
