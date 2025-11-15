# Android AutoUI + RNBO C++ Communication System
## Complete Analysis & Documentation

This folder contains comprehensive analysis of how the Android replacer system transforms RNBO patches into fully functional Android applications with real-time parameter control.

---

## Documentation Files

### 1. **ANDROID_RNBO_ANALYSIS.md** (27 KB)
**Comprehensive 11-section technical analysis**

Start here for deep understanding of the system architecture and communication mechanisms.

**Sections:**
- Executive Summary
- System Architecture Overview
- Main Entry Points & File Structure
- AutoUI File Structure & Integration
- Android-RNBO C++ Communication Layer
- UI Parameter Handling in MainActivity
- Buffer Management System
- Generation Pipeline Details
- Communication Sequence Diagrams
- Key Integration Points
- Design Decisions & Summary

**Key Content:**
- JNI method declarations and implementations
- Parameter flow from description.json to RNBO engine
- Buffer loading mechanism
- Real-time audio callback details
- Template generation process

---

### 2. **ANDROID_FILE_REFERENCE.md** (8.4 KB)
**Quick reference guide for all source files**

Perfect for quickly locating files and understanding their purposes.

**Contents:**
- Python Replacer System structure
- Android Project file organization
- Key file locations with line counts
- Critical data flow points
- Key classes and objects
- JNI encoding rules
- Parameter metadata structure

**Use this when you need to:**
- Find a specific file quickly
- Understand file relationships
- Look up class names and methods
- Reference JSON structures

---

### 3. **ANDROID_ARCHITECTURE_SUMMARY.md** (19 KB)
**Visual diagrams and flow charts**

Best for understanding system behavior and timing.

**Contents:**
- Three-layer communication model (with ASCII diagram)
- Parameter initialization sequence (detailed flow)
- Real-time parameter update sequence
- File interaction diagram
- JSON data structures with examples
- Method signatures
- Timing & performance characteristics
- Memory model
- Error handling strategy
- Thread safety model
- Extension points for customization

**Use this when you need to:**
- Visualize the system architecture
- Understand initialization sequence
- Learn timing/performance details
- Understand thread safety
- Extend the system

---

## Quick Start: Understanding the System

### For System Overview (5 minutes)
Read the **Executive Summary** section of `ANDROID_RNBO_ANALYSIS.md`

### For Understanding Parameter Flow (15 minutes)
1. Read **Section 5: UI Parameter Handling** in `ANDROID_RNBO_ANALYSIS.md`
2. View **Parameter Flow Diagram** in `ANDROID_ARCHITECTURE_SUMMARY.md`
3. Reference **Parameter Update Path** in `ANDROID_FILE_REFERENCE.md`

### For Understanding Initialization (20 minutes)
1. Read **Section 2.2: Core Replacer Logic** in `ANDROID_RNBO_ANALYSIS.md`
2. Read **Section 4: Android-RNBO C++ Communication Layer** in `ANDROID_RNBO_ANALYSIS.md`
3. View **Initialization Sequence** in `ANDROID_ARCHITECTURE_SUMMARY.md`

### For Code Generation Understanding (15 minutes)
1. Read **Section 2.3: Analysis & Parameter Extraction** in `ANDROID_RNBO_ANALYSIS.md`
2. Read **Section 7: Generation Pipeline Details** in `ANDROID_RNBO_ANALYSIS.md`
3. View **File Interaction Diagram** in `ANDROID_ARCHITECTURE_SUMMARY.md`

---

## Key Files Mentioned in Documentation

### Python Replacer System
```
rnbo_replacer_with_presets/
├── main.py                         # Entry point (11 lines)
├── core/replacer.py                # Core logic (514 lines)
├── core/analyzer.py                # Parameter extraction (77 lines)
└── templates/
    ├── mainactivity.py             # Generates MainActivity.java
    ├── nativelib.py                # Generates native-lib.cpp
    └── [other template generators]
```

### Android Application
```
app/src/main/
├── java/com/example/rnbo_test/
│   └── MainActivity.java            # Main UI Activity (2757 lines)
├── cpp/
│   ├── native-lib.cpp              # JNI bridge (145 lines)
│   ├── MSPLoader.h                 # RNBO wrapper header (59 lines)
│   └── MSPLoader.cpp               # RNBO wrapper (150+ lines)
└── assets/
    ├── description.json             # Parameter metadata (from export)
    ├── dependencies.json            # Buffer metadata (from export)
    └── media/                       # Audio samples (from export)
```

---

## The Three-Layer Communication Model

```
┌────────────────────────┐
│   JAVA LAYER (UI)      │  MainActivity.java
│  - UI creation         │  - Parameter listening
│  - User interaction    │  - Buffer management
└────────────────────────┘
           ↕ JNI Bridge
┌────────────────────────┐
│  C++ LAYER (Bridge)    │  native-lib.cpp
│  - JNI methods         │  - Type conversion
│  - Callback handlers   │  - Delegations
└────────────────────────┘
           ↕
┌────────────────────────┐
│  C++ LAYER (Engine)    │  MSPLoader.cpp + RNBO.h
│  - Audio processing    │  - Real-time callback
│  - Parameter updates   │  - Buffer management
└────────────────────────┘
           ↓
┌────────────────────────┐
│   AUDIO LAYER (Oboe)   │  Android audio output
│  - 44.1 kHz, 512 frames
└────────────────────────┘
```

---

## Critical Data Flows

### 1. Initialization
```
App Start → MainActivity.onCreate()
         → Read assets/description.json
         → JNI init(AssetManager)
         → Create RNBO engine
         → JNI Callback declareSliders()
         → Create UI sliders
         → Start audio
```

### 2. Parameter Update (44x/second during playback)
```
User slider → updateParam(name, value) [JNI]
           → MSPLoader.updateParam()
           → RNBO::CoreObject.setParameterValue()
           → Next audio callback
           → Audio output changes
```

### 3. Buffer Loading
```
User selects file → loadBuffer(id, path) [JNI]
                 → BufferLoader::loadBufferFromFile()
                 → RNBO buffer loaded
                 → Next audio callback uses buffer
```

---

## Key Technical Concepts

### JNI (Java Native Interface)
- Bridges Java UI and C++ audio engine
- Three main methods: init(), updateParam(), loadBuffer()
- Method names encoded with package: `Java_com_example_rnbo_1test_MainActivity_*`

### description.json
- Parameter metadata file (from RNBO export)
- Defines: name, min, max, initialValue, visible
- Read by Java at startup
- Used to create UI sliders

### MSPLoader
- Wraps RNBO::CoreObject
- Manages Oboe audio stream
- Extracts parameter names
- Runs audio callback loop

### Audio Callback
- Called ~86 times/second (44.1kHz / 512 frames)
- ~11.6ms duration per callback
- Applies parameter changes
- Generates audio output

---

## Performance Characteristics

| Operation | Frequency | Latency | Notes |
|-----------|-----------|---------|-------|
| Audio callback | ~86/sec | 11.6ms | Real-time |
| Parameter update | 10-60/sec | <11.6ms | From slider drag |
| UI refresh | 60Hz | 16ms | Android UI thread |
| Buffer load | 1x | 100ms-1s | File-dependent |
| Init startup | 1x | 200ms-2s | One-time |

---

## Understanding the Code

### Start with these files (in order):
1. **rnbo_replacer_with_presets/main.py** - Entry point
2. **rnbo_replacer_with_presets/core/replacer.py** - Replacement logic
3. **MainActivity.java** - UI and JNI declarations
4. **native-lib.cpp** - JNI implementations
5. **MSPLoader.cpp** - RNBO engine wrapper

### Reference these for details:
- **description.json** - Parameter structure
- **PureDataSlider.java** - Custom slider widget
- **CMakeLists.txt** - C++ compilation config
- **build.gradle** - Java/Android build config

---

## Common Questions Answered

**Q: How does the app know what parameters to show?**
A: See Section 5.1 in ANDROID_RNBO_ANALYSIS.md - The app reads description.json at startup, which contains all parameter metadata.

**Q: How is parameter data passed to the C++ engine?**
A: See Section 5.3 in ANDROID_RNBO_ANALYSIS.md - When user moves slider, it calls updateParam() JNI method, which calls MSPLoader::updateParam(), which calls RNBO::CoreObject.setParameterValue().

**Q: How does the Python replacer generate the Android app?**
A: See Section 7 in ANDROID_RNBO_ANALYSIS.md - It reads the RNBO export, analyzes it with RNBOAnalyzer, and uses template generators to create all necessary files.

**Q: Why does the audio latency matter?**
A: See Section 6 in ANDROID_ARCHITECTURE_SUMMARY.md - The audio callback runs every 11.6ms, so parameter changes take 0-11.6ms to be heard.

**Q: How are audio buffers loaded?**
A: See Section 6 in ANDROID_RNBO_ANALYSIS.md - The UI shows a "LOAD FILE" button, which opens a file picker, then calls loadBuffer() JNI method.

---

## Extension & Customization

See Section 10 in ANDROID_ARCHITECTURE_SUMMARY.md for:
- Adding new parameter control types
- Adding new parameter types
- Adding buffer support
- Modifying UI appearance

---

## File Statistics

| Document | Size | Lines | Focus |
|----------|------|-------|-------|
| ANDROID_RNBO_ANALYSIS.md | 27 KB | 800+ | Detailed technical analysis |
| ANDROID_FILE_REFERENCE.md | 8.4 KB | 200+ | Quick file lookup |
| ANDROID_ARCHITECTURE_SUMMARY.md | 19 KB | 600+ | Visual diagrams & flows |
| **Total** | **54.4 KB** | **1600+** | Complete reference |

---

## Technology Stack

- **Java**: Android UI, JNI declarations
- **C++**: RNBO engine wrapper, JNI implementations
- **RNBO**: Audio DSP library from Cycling74
- **Oboe**: Android low-latency audio library
- **CMake**: C++ build system
- **Gradle**: Android build system
- **Python**: Code generation tools (replacer)

---

## Glossary

**RNBO** - Rapid Notation for Building Objects (Cycling74 audio DSP system)
**JNI** - Java Native Interface (Java-to-C++ bridge)
**MSPLoader** - Class wrapping RNBO::CoreObject with Oboe audio I/O
**Oboe** - Android low-latency audio library
**Assets** - App runtime resources (JSON files, audio samples)
**description.json** - Parameter metadata from RNBO export
**CoreObject** - Main RNBO audio engine class
**AudioStream** - Oboe low-latency audio stream

---

## Last Updated
November 15, 2025

## Document Version
1.0 - Complete Analysis

