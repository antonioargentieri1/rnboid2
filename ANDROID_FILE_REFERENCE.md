# Android AutoUI & RNBO System - File Reference Guide

## Quick File Locations & Purposes

### Python Replacer System
```
/home/user/rnboid2/rnbo_replacer_with_presets/
├── main.py                          # Entry point - launches GUI
├── core/
│   ├── replacer.py                  # Main replacement logic (12 steps)
│   └── analyzer.py                  # Extracts parameters from export
├── templates/
│   ├── mainactivity.py              # Generates MainActivity.java
│   ├── nativelib.py                 # Generates native-lib.cpp (JNI)
│   ├── msploader.py                 # Generates MSPLoader.cpp wrapper
│   ├── msploader_h.py               # Generates MSPLoader.h header
│   ├── build_gradle.py              # Generates build.gradle
│   ├── androidmanifest.py           # Generates AndroidManifest.xml
│   ├── cmakelists.py                # Generates CMakeLists.txt
│   ├── layout.py                    # Generates activity_main.xml
│   ├── buffer_loader_h.py           # Generates BufferLoader.h
│   ├── buffer_loader_cpp.py         # Generates BufferLoader.cpp
│   └── mainactivity_extracted.txt   # Template with {package_name} placeholder
└── utils/
    └── logger.py                    # Logging utility
```

**Key Files for Analysis:**
- `/home/user/rnboid2/rnbo_replacer_with_presets/core/replacer.py` (514 lines)
  - 12-step replacement pipeline
  - File verification and backup logic
  
- `/home/user/rnboid2/rnbo_replacer_with_presets/core/analyzer.py` (77 lines)
  - Reads description.json
  - Extracts parameters, package name

### Android Project Structure
```
/home/user/rnboid2/Max-MSP-RNBO-CPP-Native-Android-master/

app/src/main/
├── java/com/example/rnbo_test/
│   ├── MainActivity.java             # MAIN: JNI declarations, UI generation
│   │   - static { System.loadLibrary("rnbo_test"); }
│   │   - public native void init(AssetManager)
│   │   - public native void updateParam(String, float)
│   │   - public native boolean loadBuffer(String, String)
│   │   - declareSliders(String[]) - Called from C++
│   │   - loadParameterInfoFromJSON() - Reads assets/description.json
│   │   - refreshParameterDisplay() - Creates UI dynamically
│   │
│   ├── PureDataSlider.java           # Custom slider widget (162 lines)
│   │   - Displays parameter with name and value
│   │   - Calls updateParam() on touch
│   │
│   ├── XYPadView.java                # 2D XY controller
│   ├── SliderListener.java           # Event listener interface
│   └── BufferLoader.java             # Helper for buffer operations
│
├── cpp/
│   ├── native-lib.cpp                # MAIN: JNI implementation (145 lines)
│   │   - init(env, thiz, assetManager)
│   │   - updateParam(env, thiz, name, val)
│   │   - loadBuffer(env, thiz, bufferId, filePath)
│   │   - cDelcareParams() - Callback to Java
│   │   - Global: mspLoader, mClass, mObject, g_jvm
│   │
│   ├── MSPLoader.h                   # MAIN: RNBO wrapper header (59 lines)
│   │   - class MSPLoader : oboe::AudioStreamDataCallback
│   │   - open(), start(), stop(), close()
│   │   - init(callback, jvm, assetManager)
│   │   - updateParam(name, val)
│   │   - onAudioReady() - Audio processing callback
│   │
│   ├── MSPLoader.cpp                 # MAIN: RNBO wrapper implementation
│   │   - Initializes RNBO::CoreObject
│   │   - Manages Oboe audio stream
│   │   - Real-time audio processing loop
│   │   - configureUI() - Extracts parameter names
│   │
│   ├── BufferLoader.h/.cpp           # Audio file loading
│   ├── CMakeLists.txt                # C++ build configuration
│   ├── rnbo/                         # RNBO library (auto-included)
│   │   ├── RNBO.h                    # Main RNBO header
│   │   └── src/                      # RNBO source implementation
│   └── cpp_backup/                   # Backup of original C++
│
├── res/
│   ├── layout/activity_main.xml      # Activity layout
│   └── values/themes.xml             # UI themes
│
├── assets/                           # Runtime-accessible resources
│   ├── description.json              # CRITICAL: Parameter metadata
│   │   - "parameters": [
│   │     - name, minimum, maximum, initialValue, visible
│   │   ]
│   ├── dependencies.json             # Buffer metadata
│   │   - "buffers": [ id, initialSize, exponent ]
│   ├── presets.json                  # Saved presets
│   └── media/                        # Audio samples
│       └── *.wav, *.aiff, etc.
│
└── AndroidManifest.xml               # App manifest
```

## Critical Data Flow Points

### 1. Parameter Metadata Path
```
RNBO Export
    ↓
description.json (in export root)
    ↓ (Replacer copies to assets/)
app/src/main/assets/description.json
    ↓ (App reads at startup)
MainActivity.loadParameterInfoFromJSON()
    ↓ (Stored in parameterInfoMap)
Slider range calculation when creating UI
    ↓ (User interaction)
updateParam(name, value) - JNI call
```

### 2. Parameter Update Path
```
User moves slider
    ↓
PureDataSlider.onTouchEvent()
    ↓
SliderListener.onProgressChanged()
    ↓
MainActivity.updateParam(name, value)  [JNI]
    ↓
native-lib.cpp: Java_...updateParam()
    ↓
mspLoader->updateParam(name, value)
    ↓
MSPLoader::updateParam()
    ↓
rnboObject.setParameterValue(index, value)
    ↓ (Next audio callback)
Oboe audio stream gets new output
    ↓
Speaker/headphones output changes
```

### 3. Initialization Path
```
MainActivity.onCreate()
    ↓
loadParameterInfoFromJSON()  [Read assets/description.json]
    ↓
init(getAssets())  [JNI call]
    ↓
native-lib.cpp: init()
    ↓
Create MSPLoader instance
    ↓
MSPLoader.open()  [Open Oboe audio stream]
    ↓
MSPLoader.init(callback, jvm, assetManager)
    ↓
RNBO::CoreObject.prepareToProcess()
    ↓
MSPLoader::configureUI(cDelcareParams)  [Extract visible parameters]
    ↓
JNI Callback: declareSliders(String[])
    ↓
MainActivity.declareSliders()
    ↓
refreshParameterDisplay()  [Create UI sliders]
    ↓
MSPLoader.start()  [Start audio processing]
```

## File Sizes & Complexity

| File | Lines | Purpose |
|------|-------|---------|
| MainActivity.java | 2757 | Main UI Activity, parameter handling |
| native-lib.cpp | 145 | JNI bridge implementation |
| MSPLoader.h | 59 | RNBO wrapper header |
| MSPLoader.cpp | 150+ | RNBO engine initialization & audio loop |
| replacer.py | 514 | Main replacement logic |
| analyzer.py | 77 | Parameter extraction |
| PureDataSlider.java | 162 | Custom slider widget |
| description.json | varies | Parameter metadata (from export) |

## Key Classes & Objects

### Java Classes
- **MainActivity** - Main app activity, JNI calls, parameter UI
- **PureDataSlider** - Custom slider widget with Pure Data style
- **XYPadView** - 2D controller for two parameters
- **ParameterInfo** - Inner class storing min/max/initial values
- **Preset** - Inner class for saving parameter states

### C++ Classes
- **MSPLoader** - Wraps RNBO::CoreObject with Oboe audio I/O
- **RNBO::CoreObject** - Main RNBO audio engine
- **oboe::AudioStream** - Low-latency audio stream
- **BufferLoader** - Loads audio files into RNBO buffers

### Python Classes
- **RNBOReplacer** - Main replacement orchestrator
- **RNBOAnalyzer** - Parses export for metadata
- **MainActivityTemplate** - Generates MainActivity.java
- **NativeLibTemplate** - Generates native-lib.cpp

## Key Encoding Rules

### JNI Method Name Encoding
Java method: `MainActivity.updateParam(String, float)`
↓
JNI signature: `Java_com_example_rnbo_1test_MainActivity_updateParam`

Rules:
- Package separator `.` → `_`
- Inner class separator `_` → `_1`
- Class name capitalized
- Method name as-is

### Description.json Parameter Fields
```json
{
  "name": "gain",              // Parameter ID (used as key)
  "minimum": 0,                // Slider min value
  "maximum": 128,              // Slider max value
  "initialValue": 124,         // Default position
  "visible": true,             // Show in UI (if false, hidden)
  "type": "ParameterTypeNumber",
  "isEnum": false
}
```

---

Generated: Nov 15, 2025
For detailed explanation, see: ANDROID_RNBO_ANALYSIS.md
