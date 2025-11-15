# Android AutoUI RNBO - Architecture Summary

## 1. Three-Layer Communication Model

```
┌─────────────────────────────────────────────────────────────────┐
│                          JAVA LAYER (UI)                         │
│  MainActivity.java (2757 lines)                                   │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │ onCreate()                                                │   │
│  │  - Load assets/description.json                          │   │
│  │  - Create UI sliders dynamically                         │   │
│  │  - Call init(AssetManager)          [JNI ↓]             │   │
│  │                                                          │   │
│  │ updateParam(String name, float val) [JNI ↓]             │   │
│  │  - Called when user moves slider                         │   │
│  │  - ~44 calls/second during playback                      │   │
│  │                                                          │   │
│  │ declareSliders(String[] paramNames) [Called from C++ ↑]  │   │
│  │  - Receives parameter list from RNBO                     │   │
│  │  - Triggers UI generation                               │   │
│  │                                                          │   │
│  │ loadBuffer(String id, String path)  [JNI ↓]             │   │
│  │  - Loads audio file into RNBO buffer                     │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                              ↑ ↓
                            JNI BRIDGE
                        (Java ↔ C++ communication)
                              ↑ ↓
┌─────────────────────────────────────────────────────────────────┐
│                      C++ LAYER (JNI Bridge)                       │
│  native-lib.cpp (145 lines)                                       │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │ init(JNIEnv*, jobject, jobject assetManager)             │   │
│  │  - Create MSPLoader instance                             │   │
│  │  - Store Java references (mClass, mObject, g_jvm)       │   │
│  │  - Initialize RNBO engine → Start audio                │   │
│  │                                                          │   │
│  │ updateParam(JNIEnv*, jobject, jstring, jfloat)           │   │
│  │  - Convert Java string to C++ string                     │   │
│  │  - Call mspLoader->updateParam()                         │   │
│  │                                                          │   │
│  │ loadBuffer(JNIEnv*, jobject, jstring, jstring)           │   │
│  │  - Call mspLoader->loadBuffer()                          │   │
│  │                                                          │   │
│  │ cDelcareParams(string[], int, JavaVM*)                   │   │
│  │  - Callback FROM C++ TO Java                             │   │
│  │  - Calls declareSliders(String[])                        │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                              ↑ ↓
┌─────────────────────────────────────────────────────────────────┐
│                    C++ LAYER (RNBO Engine)                        │
│  MSPLoader.cpp + RNBO.h                                           │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │ MSPLoader: RNBO::CoreObject Wrapper                       │   │
│  │                                                          │   │
│  │ init(callback, jvm, assetManager)                        │   │
│  │  - RNBO::CoreObject.prepareToProcess()                  │   │
│  │  - BufferLoader::loadBuffers()                           │   │
│  │  - configureUI() → Extract parameter names               │   │
│  │  - Call cDelcareParams callback                          │   │
│  │                                                          │   │
│  │ updateParam(name, value)                                 │   │
│  │  - rnboObject.setParameterValue(index, value)           │   │
│  │                                                          │   │
│  │ onAudioReady() [Real-time Audio Callback]                │   │
│  │  - ~44 times/second (512 frames @ 44.1kHz)              │   │
│  │  - rnboObject.process() → generates audio                │   │
│  │  - Output to Oboe audio stream                           │   │
│  │                                                          │   │
│  │ loadBuffer(id, path)                                     │   │
│  │  - BufferLoader::loadBufferFromFile()                    │   │
│  │  - Insert audio into RNBO buffer                         │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                      AUDIO LAYER (Oboe)                          │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │ Oboe::AudioStream                                         │   │
│  │  - 44.1 kHz sample rate                                  │   │
│  │  - 512 frame buffer                                      │   │
│  │  - Low-latency exclusive audio access                    │   │
│  │  - Output to speaker/headphones                          │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Parameter Flow Diagram

### Initialization Sequence
```
START
  |
  v
MainActivity.onCreate()
  |
  +─→ loadParameterInfoFromJSON()
  |   └─→ Assets: description.json
  |       ├─ Parameter names
  |       ├─ min/max values
  |       └─ initial values
  |
  +─→ init(AssetManager)  ← JNI Call
  |   |
  |   v
  |   native-lib.cpp: init()
  |   |
  |   +─→ MSPLoader()
  |   |   └─→ RNBO::CoreObject()
  |   |
  |   +─→ AAssetManager_fromJava()
  |   |
  |   +─→ mspLoader->open()
  |   |   └─→ Oboe::AudioStream.openStream()
  |   |
  |   +─→ mspLoader->init()
  |   |   |
  |   |   +─→ RNBO::CoreObject.prepareToProcess(44100, 512)
  |   |   |
  |   |   +─→ BufferLoader::loadBuffers()
  |   |   |   └─→ Assets: media/*.wav
  |   |   |
  |   |   +─→ configureUI(cDelcareParams)
  |   |       └─→ Extract visible parameters from RNBO
  |   |           └─→ Create C++ string array
  |   |
  |   +─→ cDelcareParams()  ← JNI Callback TO Java
  |   |   |
  |   |   v
  |   |   MainActivity.declareSliders(String[])
  |   |   |
  |   |   v
  |   |   refreshParameterDisplay()
  |   |   |
  |   |   v
  |   |   For each parameter:
  |   |   ├─ Create PureDataSlider
  |   |   ├─ Set min/max from parameterInfoMap
  |   |   └─ Set initial value
  |   |
  |   +─→ mspLoader->start()
  |       └─→ Begin audio callback loop
  |
  v
READY (UI shows, audio running)
```

### Real-Time Parameter Update Sequence
```
User touches slider (16ms - 1000ms duration)
  |
  v
PureDataSlider.onTouchEvent()
  |
  v
Calculate progress 0.0-1.0
  |
  v
SliderListener.onProgressChanged(progress)
  |
  v
MainActivity.updateParam(paramName, value)  ← JNI Call
  |
  v
native-lib.cpp: updateParam()
  |
  v
mspLoader->updateParam(paramName, value)
  |
  v
MSPLoader::updateParam()
  |
  v
rnboObject.getParameterIndexForID(paramName)
  |
  v
rnboObject.setParameterValue(index, value)
  |
  v
Next audio callback (in ~11ms):
  |
  +─→ MSPLoader::onAudioReady()
  |   |
  |   v
  |   rnboObject.process(
  |     nullptr,              // input buffer (unused)
  |     0,                    // input channels
  |     outputs,              // output buffer
  |     cachedNumOutputChannels,
  |     numFrames (512)
  |   )
  |   |
  |   v
  |   Audio with new parameter value
  |
  v
Output to speaker → User hears change
```

---

## 3. File Interaction Diagram

### Build Time (Python Replacer)

```
Python Replacer System:

1. Input Files
   └─ RNBO Export/
      ├─ description.json     ← Extract parameters
      ├─ dependencies.json    ← Extract buffers
      ├─ presets.json
      ├─ *.cpp               ← RNBO compiled code
      ├─ rnbo/               ← RNBO library headers
      └─ media/              ← Audio samples

2. Analysis (analyzer.py)
   ├─ Parse JSON
   ├─ Extract package name
   └─ Validate files

3. Code Generation (templates/)
   ├─ mainactivity.py       → MainActivity.java
   ├─ nativelib.py          → native-lib.cpp
   ├─ msploader.py          → MSPLoader.cpp
   ├─ msploader_h.py        → MSPLoader.h
   ├─ buffer_loader_*.py    → BufferLoader.cpp/h
   ├─ build_gradle.py       → build.gradle
   ├─ androidmanifest.py    → AndroidManifest.xml
   └─ cmakelists.py         → CMakeLists.txt

4. Output Structure
   └─ Android Project/
      ├─ app/src/main/
      │  ├─ java/
      │  │  └─ MainActivity.java          [Generated]
      │  ├─ cpp/
      │  │  ├─ native-lib.cpp             [Generated]
      │  │  ├─ MSPLoader.cpp              [Generated]
      │  │  ├─ MSPLoader.h                [Generated]
      │  │  ├─ BufferLoader.cpp           [Generated]
      │  │  ├─ rnbo/                      [Copied from export]
      │  │  └─ CMakeLists.txt             [Generated]
      │  ├─ res/
      │  │  ├─ layout/activity_main.xml   [Generated]
      │  │  └─ values/themes.xml          [Generated]
      │  └─ assets/
      │     ├─ description.json           [Copied from export]
      │     ├─ dependencies.json          [Copied from export]
      │     └─ media/                     [Copied from export]
      └─ build.gradle                     [Generated]

5. Compilation (CMake + Gradle)
   ├─ C++ files → librnbo_test.so (JNI)
   ├─ Java files → classes.dex
   └─ APK → Android package
```

---

## 4. JSON Data Structures

### description.json (Parameter Metadata)
```json
{
  "parameters": [
    {
      "name": "gain",
      "paramId": "gain",
      "minimum": 0,
      "maximum": 128,
      "initialValue": 124,
      "visible": true,
      "type": "ParameterTypeNumber",
      "exponent": 1,
      "steps": 0,
      "isEnum": false,
      "enumValues": []
    },
    {
      "name": "triggerSpeed",
      "minimum": 0,
      "maximum": 1000,
      "initialValue": 6,
      "visible": true,
      ...
    }
  ]
}
```

**Usage:**
- Java reads at startup: `loadParameterInfoFromJSON()`
- Maps to: `parameterInfoMap[name] = ParameterInfo(name, min, max, initial)`
- Used for: Slider range calculation, initial value display
- Accessible to: MainActivity.java via `getAssets().open("description.json")`

### dependencies.json (Buffer Metadata)
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

**Usage:**
- C++ reads via AssetManager during init
- Used by: BufferLoader to initialize RNBO buffers
- Referenced by: Setup UI for buffer loading

---

## 5. Key Method Signatures

### Java ↔ C++ JNI Declarations

```java
// Java declarations in MainActivity.java
public native void init(android.content.res.AssetManager assetManager);
public native void updateParam(String name, float val);
public native boolean loadBuffer(String bufferId, String filePath);
public void declareSliders(String[] paramNames);  // Called FROM C++
```

### C++ JNI Implementations

```cpp
// native-lib.cpp

extern "C" JNIEXPORT void JNICALL
Java_com_example_rnbo_1test_MainActivity_init(
    JNIEnv *env, jobject thiz, jobject assetManager);

extern "C" JNIEXPORT void JNICALL
Java_com_example_rnbo_1test_MainActivity_updateParam(
    JNIEnv *env, jobject thiz, jstring name, jfloat val);

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_rnbo_1test_MainActivity_loadBuffer(
    JNIEnv *env, jobject thiz, jstring bufferId, jstring filePath);

// Callback from C++ to Java
int cDelcareParams(std::string params[], int count, JavaVM *jvm);
```

### RNBO C++ Interface

```cpp
// RNBO::CoreObject (from RNBO.h)
class CoreObject {
public:
    void prepareToProcess(number samplerate, size_t blocksize);
    ParameterIndex getParameterIndexForID(ParameterID id);
    void setParameterValue(ParameterIndex index, ParameterValue value);
    void process(
        const SampleValue** inputs, size_t numInputs,
        SampleValue** outputs, size_t numOutputs,
        size_t numFrames
    );
    // ... parameter enumeration, buffer management, etc.
};
```

---

## 6. Timing & Performance Characteristics

### Real-Time Audio Thread
- **Callback frequency**: ~86 times/second (44100 Hz / 512 frames)
- **Duration per callback**: ~11.6 milliseconds
- **Parameter update latency**: 0-11.6ms (applied in next callback)
- **Safe operations**: Parameter updates (queued), read buffer operations
- **Unsafe operations**: JNI calls, allocations, locks

### Parameter Slider Updates
- **Max update frequency**: Limited by touch screen refresh (~60-120 Hz)
- **Typical frequency**: 10-60 times/second while dragging
- **Processing**: Checked in audio callback, applied next frame

### Initial Load Time
- **Description.json parse**: <1ms (small JSON)
- **UI generation**: 10-100ms (depends on parameter count)
- **RNBO initialization**: 10-50ms
- **Buffer loading**: 100ms-1s (depends on file size)
- **Audio startup**: <10ms
- **Total startup**: 200ms-2s typical

---

## 7. Memory Model

### Java Heap
- MainActivity instance
- UI widgets (sliders)
- JSON data
- Preset storage

### Native Heap (C++)
- RNBO::CoreObject
- Audio buffers (large)
- Oboe::AudioStream
- MSPLoader instance

### Shared Data
- Parameter values (queued)
- Buffer references
- JNI references (Java objects referenced from C++)

---

## 8. Error Handling Strategy

```
Audio Processing Thread
├─ If setParameterValue() fails
│  └─ Log error, continue with old value
├─ If process() fails
│  └─ Log error, output silence, continue
└─ If buffer unavailable
   └─ Skip processing, continue

UI Thread
├─ If description.json missing
│  └─ Create empty parameter map
├─ If init() fails
│  └─ Show error, disable controls
└─ If loadBuffer() fails
   └─ Show toast message, retry

Native Code
├─ If JNI call fails
│  └─ Log, continue operation
├─ If Oboe open fails
│  └─ Retry or use alternative config
└─ If BufferLoader fails
   └─ Log warning, continue without buffer
```

---

## 9. Thread Safety Model

### Threads Involved
1. **Main/UI Thread**: Android UI operations
2. **Audio Thread**: Oboe real-time audio callback
3. **JNI Bridge**: Calls cross thread boundary

### Thread-Safe Operations
- Parameter value updates (queued)
- Read-only buffer access
- Atomic flag updates

### Thread-Unsafe Operations
- Parameter metadata changes during playback
- Buffer reallocation during playback
- Parameter map access from multiple threads

### Protection Strategy
- Parameter updates applied in audio callback only
- No locks in real-time path
- Metadata fixed after initialization
- Buffers pre-allocated

---

## 10. Extension Points

### Adding a New Parameter Control Type
1. Modify `PureDataSlider.java` or create new widget
2. Update `MainActivity.refreshParameterDisplay()`
3. Add listener that calls `updateParam(name, value)`

### Adding a New Parameter Type
1. Update `description.json` parsing
2. Handle new parameter type in `ParameterInfo`
3. Add UI widget in `refreshParameterDisplay()`

### Adding Buffer Support
1. Update `dependencies.json` in export
2. Modify `addBufferManagementUI()` in MainActivity
3. Add buffer ID to hardcoded list or parse dynamically

### Modifying UI Appearance
1. Update `themes.xml` (generated template)
2. Modify `PureDataSlider.java` rendering
3. Customize `activity_main.xml` layout

---

