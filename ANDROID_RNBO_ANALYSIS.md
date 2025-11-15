# Android AutoUI & RNBO C++ Communication System - Detailed Analysis

## Executive Summary

The Android replacer system is a sophisticated code generation framework that transforms RNBO C++ exports into fully functional Android applications. It creates a bidirectional communication layer between Java UI and C++ RNBO audio engine via JNI (Java Native Interface).

---

## 1. System Architecture Overview

### High-Level Flow

```
RNBO Export (Max/MSP patch)
         |
         v
rnbo_replacer_with_presets/
  - Analyzes export structure
  - Generates template files
  - Creates Android project
         |
         v
Android App Structure
  - MainActivity.java (UI)
  - native-lib.cpp (JNI bridge)
  - MSPLoader.cpp (RNBO engine wrapper)
  - RNBO.h + C++ core library
         |
         v
Communication Flow:
  Java <-> JNI <-> C++ RNBO Engine <-> Audio Output (Oboe)
```

---

## 2. Main Entry Points and File Structure

### 2.1 Replacer System Entry Point

**File**: `/home/user/rnboid2/rnbo_replacer_with_presets/main.py`

```python
def main():
    root = tk.Tk()
    app = RNBOReplacerGUI(root)  # Launches GUI
    root.mainloop()
```

The GUI initializes the replacement system. Users select:
- Template Android project path
- RNBO export folder path

### 2.2 Core Replacer Logic

**File**: `/home/user/rnboid2/rnbo_replacer_with_presets/core/replacer.py`

**RNBOReplacer Class** handles 12 sequential replacement steps:

```
Step 1:  Backup cpp directory
Step 2:  Replace C++ export files
Step 3:  Replace JSON files (description.json, dependencies.json, presets.json)
Step 4:  Copy description.json to assets/
Step 5:  Replace rnbo/ folder (RNBO library)
Step 5a: Copy media/ folder to assets/ (buffer support)
Step 5b: Generate BufferLoader files
Step 5c: Generate MSPLoader.h
Step 5d: Generate native-lib.cpp
Step 6:  Apply MSPLoader.cpp optimization
Step 7:  Generate MainActivity.java with parameters
Step 8:  Generate activity_main.xml layout
Step 9:  Generate build.gradle
Step 10: Generate AndroidManifest.xml
Step 11: Generate CMakeLists.txt
Step 12: Generate themes.xml and colors.xml
```

### 2.3 Analysis & Parameter Extraction

**File**: `/home/user/rnboid2/rnbo_replacer_with_presets/core/analyzer.py`

The RNBOAnalyzer class extracts:

```python
def read_parameters():
    """Reads parameters from description.json"""
    - Opens description.json
    - Parses JSON array of parameters
    - Extracts: name, min, max, initialValue
    - Returns list of parameter dictionaries

def find_package_name():
    """Extracts Android package name from existing MainActivity"""
    - Searches for 'package ...' declaration
    - Falls back to default: "com.example.rnbo_test"

def find_mainactivity_dir():
    """Locates or creates MainActivity directory"""
    - Finds existing MainActivity.java
    - Creates package directory structure if needed
```

---

## 3. AutoUI File Structure & Integration

### 3.1 Description.json - Parameter Metadata

The description.json file (copied to assets/) contains parameter definitions:

```json
{
  "parameters": [
    {
      "type": "ParameterTypeNumber",
      "index": 0,
      "name": "gain",
      "paramId": "gain",
      "minimum": 0,
      "maximum": 128,
      "initialValue": 124,
      "visible": true,
      ...
    },
    {
      "type": "ParameterTypeNumber",
      "name": "triggerSpeed",
      "minimum": 0,
      "maximum": 1000,
      "initialValue": 6,
      "visible": true,
      ...
    },
    ...
  ]
}
```

**Key fields used by Android UI:**
- `name`: Parameter identifier (used as key)
- `minimum`, `maximum`: Slider range
- `initialValue`: Default slider position
- `visible`: Show/hide parameter

### 3.2 Android File Structure

```
app/src/main/
├── java/com/example/rnbo_test/
│   ├── MainActivity.java          # Main UI Activity
│   ├── PureDataSlider.java        # Custom slider widget
│   ├── XYPadView.java             # 2D controller
│   └── SliderListener.java
├── cpp/
│   ├── native-lib.cpp             # JNI bridge
│   ├── MSPLoader.h/.cpp           # RNBO engine wrapper
│   ├── BufferLoader.h/.cpp        # Audio buffer management
│   ├── rnbo/                      # RNBO library (RNBO.h + internals)
│   └── CMakeLists.txt             # Build configuration
├── res/
│   ├── layout/activity_main.xml   # Layout definition
│   └── values/themes.xml          # UI themes
├── assets/
│   ├── description.json           # Parameter metadata
│   ├── dependencies.json          # Buffer information
│   └── media/                     # Audio samples/buffers
└── AndroidManifest.xml
```

### 3.3 MainActivity Template Generation

**File**: `/home/user/rnboid2/rnbo_replacer_with_presets/templates/mainactivity.py`

The MainActivityTemplate reads from `mainactivity_extracted.txt` and injects package name:

```python
@staticmethod
def generate(package_name, parameters=None):
    with open(TEMPLATE_FILE, 'r') as f:
        template = f.read()
    content = template.replace('{package_name}', package_name)
    return content
```

---

## 4. Android-RNBO C++ Communication Layer

### 4.1 JNI Method Declarations (MainActivity.java)

```java
public class MainActivity extends AppCompatActivity {
    
    static {
        System.loadLibrary("rnbo_test");  // Load compiled C++ library
    }

    // Native methods declared (implemented in native-lib.cpp)
    public native void init(android.content.res.AssetManager assetManager);
    public native void updateParam(String name, float val);
    public native boolean loadBuffer(String bufferId, String filePath);
}
```

**Three main JNI methods:**

1. **init()** - Initializes RNBO engine
   - Called once in MainActivity.onCreate()
   - Passes AssetManager for loading buffers
   - Sets up parameter callback

2. **updateParam()** - Updates RNBO parameter value
   - Called whenever slider/UI element changes
   - Parameters: parameter name (String), value (float)
   - Called frequently during playback

3. **loadBuffer()** - Loads audio file into RNBO buffer
   - Called from Setup mode when user selects audio file
   - Parameters: buffer ID, file path
   - Returns success/failure status

### 4.2 Native Implementation (native-lib.cpp)

**File**: `/home/user/rnboid2/Max-MSP-RNBO-CPP-Native-Android-master/app/src/main/cpp/native-lib.cpp`

#### Global JNI Variables
```cpp
MSPLoader *mspLoader;           // RNBO engine wrapper
jclass mClass;                  // Reference to MainActivity class
jobject mObject;                // Reference to MainActivity instance
jmethodID jDeclareSliders;      // Method ID for declareSliders()
static JavaVM *g_jvm;           // Global JVM reference
```

#### Callback Function: cDelcareParams()
```cpp
int cDelcareParams(std::string params[], int count, JavaVM *jvm) {
    // Called from C++ to notify Java about available parameters
    // Uses JNI to call back into Java's declareSliders() method
    
    JNIEnv *env;
    jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    
    // Create Java String array
    jobjectArray paramArray = env->NewObjectArray(
        count, 
        env->FindClass("java/lang/String"),
        env->NewStringUTF("")
    );
    
    // Populate array with parameter names
    for (int i = 0; i < count; i++) {
        env->SetObjectArrayElement(
            paramArray, i, 
            env->NewStringUTF(params[i].c_str())
        );
    }
    
    // Call Java method: declareSliders(String[] paramNames)
    env->CallVoidMethod(mObject, jDeclareSliders, paramArray);
    return 0;
}
```

#### JNI Implementation of updateParam()
```cpp
extern "C" JNIEXPORT void JNICALL
Java_com_example_rnbo_1test_MainActivity_updateParam(
    JNIEnv *env,
    jobject thiz,
    jstring name,
    jfloat val) {
    
    // Convert Java String to C++ string
    jboolean isCopy;
    const char *convertedValue = env->GetStringUTFChars(name, &isCopy);
    
    // Call MSPLoader to set parameter
    mspLoader->updateParam(convertedValue, val);
    
    // Release string reference
    env->ReleaseStringUTFChars(name, convertedValue);
}
```

#### JNI Implementation of init()
```cpp
extern "C" JNIEXPORT void JNICALL
Java_com_example_rnbo_1test_MainActivity_init(
    JNIEnv *env,
    jobject thiz,
    jobject assetManager) {
    
    // Store references to MainActivity
    jclass clazz = env->GetObjectClass(thiz);
    mClass = (jclass) env->NewGlobalRef(clazz);
    mObject = (jobject) env->NewGlobalRef(thiz);
    
    // Get method ID for declareSliders callback
    jDeclareSliders = env->GetMethodID(
        mClass, 
        "declareSliders",
        "([Ljava/lang/String;)V"  // Signature: void(String[])
    );
    
    // Store JVM reference globally
    jint rs = env->GetJavaVM(&g_jvm);
    
    // Create MSPLoader instance (RNBO wrapper)
    mspLoader = new MSPLoader();
    
    // Convert Java AssetManager to native C++ AssetManager
    AAssetManager* nativeAssetManager = 
        AAssetManager_fromJava(env, assetManager);
    
    // Initialize RNBO engine
    mspLoader->open();                    // Open audio stream
    mspLoader->init(
        cDelcareParams,                   // Callback for parameter declaration
        g_jvm,                            // JVM reference
        nativeAssetManager                // For loading buffers
    );
    mspLoader->start();                   // Start audio processing
}
```

### 4.3 RNBO Engine Wrapper (MSPLoader)

**File**: `/home/user/rnboid2/Max-MSP-RNBO-CPP-Native-Android-master/app/src/main/cpp/MSPLoader.h`

```cpp
class MSPLoader : public oboe::AudioStreamDataCallback {
public:
    oboe::Result open();    // Open audio stream
    oboe::Result start();   // Start playback
    oboe::Result stop();    // Stop playback
    oboe::Result close();   // Close stream
    
    // Initialize with RNBO parameter callback
    int init(
        std::function<int(std::string[], int, JavaVM*)> cDecPsRef,
        JavaVM* jvm,
        AAssetManager* assetManager
    );
    
    // Update parameter in RNBO engine
    void updateParam(const char *paramName, float val);
    
    // Load audio buffer from file
    bool loadBuffer(const char *bufferId, const char *filePath);

private:
    RNBO::CoreObject rnboObject;          // Main RNBO engine
    std::shared_ptr<oboe::AudioStream> mStream;
    std::vector<std::string> paramNames;
    std::map<std::string, RNBO::ParameterInfo> paramInfoMap;
    RNBO::SampleValue** outputs;
};
```

#### Parameter Update Flow (MSPLoader.cpp)

```cpp
void MSPLoader::updateParam(const char *paramName, float val) {
    // Get parameter index from name
    RNBO::ParameterIndex parameterIndex = 
        rnboObject.getParameterIndexForID(paramName);
    
    if (parameterIndex != -1) {
        // Set value in RNBO engine
        rnboObject.setParameterValue(parameterIndex, val);
        
        Log.d("MSPLoader", "Parameter %s updated to %f", paramName, val);
    }
}
```

#### Audio Processing Loop (MSPLoader.cpp)

```cpp
oboe::DataCallbackResult MSPLoader::onAudioReady(
    oboe::AudioStream *oboeStream, 
    void *audioData, 
    int32_t numFrames) {
    
    // This callback is called ~44 times per second (at 44.1kHz with 512 frame buffer)
    // Any parameter updates are applied here
    
    float *outputBuffer = (float *)audioData;
    
    // Process audio through RNBO
    rnboObject.process(
        nullptr,              // No input
        0,                    // Input channel count
        outputs,              // Output buffer (from updateParam)
        cachedNumOutputChannels,
        numFrames
    );
    
    // Copy RNBO output to Android audio buffer
    // ...
}
```

---

## 5. UI Parameter Handling in MainActivity

### 5.1 Parameter Loading Flow

**File**: `/home/user/rnboid2/Max-MSP-RNBO-CPP-Native-Android-master/app/src/main/java/com/example/rnbo_test/MainActivity.java`

#### Step 1: Initialize (onCreate)
```java
@Override
protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    binding = ActivityMainBinding.inflate(getLayoutInflater());
    setContentView(binding.getRoot());
    
    // Load parameter metadata from JSON
    loadParameterInfoFromJSON();
    
    // Load UI configurations (presets, sensor controls, etc.)
    loadLiveConfig();
    loadCustomRanges();
    loadSensorControls();
    loadPresetsFromStorage();
    
    // Create mode switching buttons
    createModeButtons();
    
    // CRITICAL: Initialize native RNBO engine
    init(getAssets());  // JNI call to native-lib.cpp
}
```

#### Step 2: Parse description.json
```java
private void loadParameterInfoFromJSON() {
    InputStream is = getAssets().open("description.json");
    String json = new String(buffer, "UTF-8");
    JSONObject obj = new JSONObject(json);
    JSONArray params = obj.getJSONArray("parameters");
    
    for (int i = 0; i < params.length(); i++) {
        JSONObject param = params.getJSONObject(i);
        String name = param.getString("name");
        double min = param.getDouble("minimum");
        double max = param.getDouble("maximum");
        double initial = param.getDouble("initialValue");
        
        // Store in map for later use
        parameterInfoMap.put(name, new ParameterInfo(
            name, min, max, initial
        ));
    }
}
```

#### Step 3: Callback from C++ - declareSliders()
```java
// Called from native code (C++) via JNI
public void declareSliders(@NonNull String[] paramNames) {
    allParamNames = paramNames;  // Store available parameters
    Log.i("RNBO_MultiMode", "Params: " + paramNames.length);
    
    // Now create UI elements for each parameter
    refreshParameterDisplay();
}
```

#### Step 4: Dynamic UI Creation
```java
private void refreshParameterDisplay() {
    paramHolder.removeAllViews();  // Clear existing UI
    
    if (currentMode.equals("live")) {
        // Show only "live" parameters
        for (String paramName : liveParameters) {
            addSliderUI(paramName);
        }
    } else if (currentMode.equals("all")) {
        // Show all parameters
        for (String paramName : allParamNames) {
            addSliderUI(paramName);
        }
    } else if (currentMode.equals("setup")) {
        // Show parameters + buffer loading UI
        for (String paramName : allParamNames) {
            addSliderUI(paramName);
        }
        addBufferManagementUI();
    }
}
```

### 5.2 Slider Widget Implementation

**File**: `/home/user/rnboid2/Max-MSP-RNBO-CPP-Native-Android-master/app/src/main/java/com/example/rnbo_test/PureDataSlider.java`

Custom slider widget with Pure Data style:

```java
public class PureDataSlider extends View {
    private String paramName = "";
    private float currentValue = 0f;
    private float progress = 0f;  // 0.0 to 1.0
    
    public void setParameterName(String name) {
        this.paramName = name;
    }
    
    public void setProgress(int progress) {
        this.progress = progress / 1000f;  // Convert to 0-1 range
        invalidate();  // Trigger redraw
    }
    
    // On user touch, calculate value from min/max range
    private double calculateRealValue(ParameterInfo info) {
        double range = info.max - info.min;
        double realValue = info.min + (progress * range);
        return Math.max(info.min, Math.min(info.max, realValue));
    }
    
    // When slider moves, call JNI function
    listener.onProgressChanged((int)(progress * 1000));
    // Which triggers: updateParam(paramName, realValue)
}
```

### 5.3 Parameter Update Flow

```
User touches slider
    |
    v
SliderListener.onProgressChanged()
    |
    v
Calculate real value from min/max range
    |
    v
updateParam(paramName, floatValue)  // JNI call
    |
    v
native-lib.cpp: Java_...updateParam()
    |
    v
mspLoader->updateParam(paramName, val)
    |
    v
MSPLoader.cpp: rnboObject.setParameterValue(index, val)
    |
    v
Next audio callback applies new parameter
    |
    v
Audio output changes in real time
```

---

## 6. Buffer Management System

### 6.1 Buffer Metadata (dependencies.json)

Similar to description.json, stores buffer information:

```json
{
  "buffers": [
    {
      "id": "drums",
      "initialSize": 44100,
      "exponent": 17
    }
  ]
}
```

### 6.2 Buffer Loading Flow

**Java Side** (MainActivity.java):
```java
private void openBufferFilePicker(String bufferId) {
    Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
    intent.setType("audio/*");
    startActivityForResult(intent, REQUEST_CODE_PICK_AUDIO_FILE);
}

@Override
protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    Uri audioFileUri = data.getData();
    String filePath = getFilePathFromUri(audioFileUri);
    
    // JNI call to load buffer
    boolean success = loadBuffer(bufferId, filePath);
}
```

**C++ Side** (native-lib.cpp):
```cpp
extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_rnbo_1test_MainActivity_loadBuffer(
    JNIEnv *env,
    jobject thiz,
    jstring bufferId,
    jstring filePath) {
    
    const char *bufferIdStr = env->GetStringUTFChars(bufferId, nullptr);
    const char *filePathStr = env->GetStringUTFChars(filePath, nullptr);
    
    // Delegate to MSPLoader
    bool result = mspLoader->loadBuffer(bufferIdStr, filePathStr);
    
    env->ReleaseStringUTFChars(bufferId, bufferIdStr);
    env->ReleaseStringUTFChars(filePath, filePathStr);
    
    return (jboolean)result;
}
```

**RNBO Engine** (MSPLoader.cpp):
```cpp
bool MSPLoader::loadBuffer(const char *bufferId, const char *filePath) {
    // Use BufferLoader to read audio file
    return BufferLoader::loadBufferFromFile(rnboObject, bufferId, filePath);
}
```

---

## 7. Generation Pipeline Details

### 7.1 Template Generation Process

Templates are Python classes that generate code strings:

```python
# Example: MainActivityTemplate
class MainActivityTemplate:
    TEMPLATE_FILE = "mainactivity_extracted.txt"
    
    @staticmethod
    def generate(package_name, parameters=None):
        with open(TEMPLATE_FILE, 'r') as f:
            template = f.read()
        content = template.replace('{package_name}', package_name)
        return content

# Example: NativeLibTemplate  
class NativeLibTemplate:
    @staticmethod
    def generate(package_name):
        jni_package = package_name.replace('_', '_1').replace('.', '_')
        # Generate C++ code with properly formatted JNI method names:
        # Java_com_example_rnbo_1test_MainActivity_updateParam
        # Java_com_example_rnbo_1test_MainActivity_init
        return f"... JNI method definitions ..."
```

### 7.2 File Replacement Order (replacer.py)

1. **C++ Export Files**: Copy .cpp from export to app/src/main/cpp/
2. **JSON Metadata**: Copy description.json, dependencies.json, presets.json
3. **RNBO Library**: Replace entire rnbo/ folder with latest version
4. **Buffer Support**: Copy media/ and generate BufferLoader
5. **Java Code**: Generate MainActivity.java with proper package name
6. **Layout**: Generate activity_main.xml
7. **Build Config**: Generate build.gradle, CMakeLists.txt
8. **Manifest**: Generate AndroidManifest.xml
9. **Resources**: Generate themes.xml, colors.xml

### 7.3 Backup Strategy

Before any replacement:
- cpp_backup/ - Backup of C++ directory
- java_backup/ - Backup of Java files
- gradle_backup/ - Backup of build.gradle
- manifest_backup/ - Backup of AndroidManifest
- res_backup/ - Backup of resources

---

## 8. Communication Sequence Diagram

```
Startup Sequence:
================

App Launch
    |
    v
MainActivity.onCreate()
    |
    +----> loadParameterInfoFromJSON()       [Reads description.json from assets]
    |
    +----> init(getAssets())                 [JNI call to native code]
            |
            +----> native-lib.cpp: init()
                    |
                    +----> MSPLoader()        [Create wrapper object]
                    |
                    +----> AAssetManager_fromJava()  [Convert to C++ AssetManager]
                    |
                    +----> mspLoader->open()  [Open Oboe audio stream]
                    |
                    +----> mspLoader->init(callback, jvm, assetManager)
                            |
                            +----> RNBO::CoreObject.prepareToProcess()
                            |
                            +----> BufferLoader::loadBuffers()  [Load media/ files]
                            |
                            +----> MSPLoader::configureUI(cDelcareParams)
                                    |
                                    +----> Extract visible parameters from RNBO
                                    |
                                    +----> Create C++ string array of param names
                                    |
                                    +----> JNI callback: declareSliders(String[])
                                            |
                                            v
                                            MainActivity.declareSliders()
                                            |
                                            v
                                            refreshParameterDisplay()
                                            |
                                            v
                                            Create UI sliders for each param
                    |
                    +----> mspLoader->start()  [Start audio processing thread]
    |
    v
Audio Loop (Real-time, ~44x per second):
    |
    +----> Oboe AudioStream callback
    |       |
    |       +----> MSPLoader::onAudioReady()
    |       |       |
    |       |       +----> rnboObject.process()  [Apply any pending parameter changes]
    |       |       |
    |       |       +----> Copy output to audio buffer
    |       |
    |       +----> Output to speaker/headphones

User Interaction:
=================

User moves slider
    |
    v
PureDataSlider.onTouchEvent()
    |
    v
SliderListener.onProgressChanged()
    |
    v
MainActivity.updateParam(paramName, floatValue)  [JNI call]
    |
    v
native-lib.cpp: Java_...updateParam()
    |
    v
mspLoader->updateParam(paramName, val)
    |
    v
MSPLoader::updateParam()
    |
    v
rnboObject.setParameterValue(index, val)
    |
    v
Next audio callback uses new value
    |
    v
Audio changes in real-time

Buffer Loading:
===============

User clicks "LOAD FILE" button
    |
    v
MainActivity.openBufferFilePicker()
    |
    v
System file picker
    |
    v
User selects audio file -> Returns URI
    |
    v
MainActivity.onActivityResult()
    |
    v
loadBuffer(bufferId, filePath)  [JNI call]
    |
    v
native-lib.cpp: Java_...loadBuffer()
    |
    v
mspLoader->loadBuffer(bufferId, filePath)
    |
    v
BufferLoader::loadBufferFromFile()
    |
    v
Read audio file, decode, load into RNBO buffer
    |
    v
Return success/failure
    |
    v
UI updates with status
```

---

## 9. Key Integration Points

### 9.1 From Python Replacer to Built App

```
1. Input:
   - RNBO export directory (with description.json)
   - Android template project directory

2. Analysis:
   - RNBOAnalyzer reads description.json
   - Extracts parameters, buffer info, dependencies
   - Extracts package name from existing MainActivity

3. File Generation:
   - Templates render with proper package name
   - JNI method names encoded with package (com_example_rnbo_1test)
   - Native code references correct Java methods

4. File Placement:
   - C++ files to app/src/main/cpp/
   - JSON metadata to app/src/main/assets/
   - Java/XML generated to correct locations
   - CMakeLists.txt configured with correct native library name

5. Output:
   - Fully functional Android app
   - Compiles with gradlew build
   - No manual JNI configuration needed
```

### 9.2 RNBO Library Integration

The RNBO C++ library is included in the export:
- RNBO.h - Main header file
- RNBO source files - Compiled via CMakeLists.txt
- RNBO::CoreObject - Main audio engine class
- Parameter interface - ParameterIndex, setParameterValue()
- Data buffer support - DataRef for audio buffers

### 9.3 Oboe Audio Integration

- Audio output library for Android
- Low-latency audio streaming
- Configured in CMakeLists.txt
- MSPLoader extends oboe::AudioStreamDataCallback
- 44.1 kHz, 512 frame buffer, mono output

---

## 10. Summary: Complete Communication Path

```
RNBO Export
    |
    v
Python Replacer Analyzes:
    - description.json (parameters: name, min, max, initial)
    - dependencies.json (buffers, media files)
    - C++ export file
    - RNBO library folder
    |
    v
Generate Android Code:
    - MainActivity.java:
        * Loads JSON from assets
        * Creates UI dynamically
        * Calls init(AssetManager)
        * Calls updateParam(name, value) on slider movement
        * Calls loadBuffer(id, path) for audio files
    
    - native-lib.cpp (JNI bridge):
        * init() - Creates MSPLoader, initializes RNBO, starts audio
        * updateParam() - Sets parameter value in RNBO engine
        * loadBuffer() - Loads audio file into RNBO buffer
        * Provides C++ callback for declareSliders()
    
    - MSPLoader.cpp (RNBO wrapper):
        * open() - Opens Oboe audio stream
        * init() - Initializes RNBO engine, loads buffers
        * start() - Begins audio processing
        * updateParam() - Updates RNBO parameter value
        * onAudioReady() - Real-time audio callback
        * loadBuffer() - Loads audio file into buffer
    |
    v
Runtime Execution:
    - Slider move -> updateParam(JNI) -> RNBO parameter change
    - Audio callback processes with new parameter value
    - User hears result in real-time (44 updates per second)
    
    - Load audio -> loadBuffer(JNI) -> BufferLoader -> RNBO buffer
    - RNBO patch can use loaded audio in its algorithm
```

---

## 11. Key Design Decisions

1. **Bidirectional JNI Communication**
   - Java -> C++ for parameter updates and buffer loading
   - C++ -> Java for parameter declaration (declareSliders callback)

2. **Automatic UI Generation**
   - Parameters declared dynamically from RNBO engine
   - UI created in Java based on parameter metadata
   - No hardcoding of parameters in templates

3. **Asset-based Metadata**
   - description.json, dependencies.json in assets/
   - Accessible to Java via AssetManager
   - Accessible to C++ via AAssetManager

4. **Real-time Safety**
   - Parameter updates queued in audio thread
   - No locks in audio callback (Oboe requirement)
   - Buffer operations safe during playback

5. **Template-based Code Generation**
   - Single mainactivity_extracted.txt → Generated for any package
   - Single nativelib.py → Generates JNI code with correct method names
   - Package name automatically encoded in JNI method names

---

