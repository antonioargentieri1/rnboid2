# iOS AutoUI Replacer System - Comprehensive Analysis

## 1. SYSTEM OVERVIEW

The iOS Replacer system is a Python-based code generator that transforms RNBO C++ patch exports into complete, ready-to-run iOS/macOS Xcode projects with an auto-generated SwiftUI parameter interface (AutoUI).

### Key Components:
- **Python Replacer** (rnbo_replacer_ios/): Generates Xcode projects from RNBO exports
- **SwiftUI AutoUI**: Auto-generated parameter UI with 3 modes (Live/All/Setup)
- **RNBO C++ Integration**: Parameter communication via RNBOAudioUnit bridge
- **Preset System**: Complete state save/load with interpolation

---

## 2. MAIN ENTRY POINT & WORKFLOW

### Entry Point: `main.py`

```python
# Usage:
python main.py --template SwiftRNBO-main --export my_export --output MyApp [--name "App Name"]
```

**Process Flow:**
```
1. Parse Command-Line Arguments
   ├── --template: Path to SwiftRNBO-main template
   ├── --export: Path to RNBO export directory
   ├── --output: Output Xcode project location
   └── --name: Optional custom app name

2. Create IOSReplacer instance
   ├── Initialize RNBOAnalyzer
   └── Set up logging

3. Execute Replacement Pipeline (5 Steps)
   ├── verify_paths()
   ├── analyzer.analyze()
   ├── copy_template()
   ├── replace_rnbo_export()
   └── update_project_config()

4. Output
   └── Ready-to-use Xcode project at --output path
```

---

## 3. RNBO EXPORT ANALYSIS SYSTEM

### `core/analyzer.py` - RNBOAnalyzer Class

**Purpose:** Parse RNBO export and extract metadata

**Key Files Analyzed:**

1. **description.json**
   - Contains all RNBO parameter definitions
   - Extracted fields:
     ```
     {
       "parameters": [
         {
           "index": 0,
           "paramId": "frequency",
           "name": "/frequency",
           "displayName": "Frequency",
           "minimum": 20.0,
           "maximum": 20000.0,
           "exponent": 1.0,
           "steps": 0,
           "initialValue": 440.0,
           "unit": "Hz",
           "visible": true,
           "type": "ParameterTypeNumber",
           "isEnum": false,
           "enumValues": []
         }
       ],
       "numParameters": 10,
       "numInputChannels": 2,
       "numOutputChannels": 2,
       // ... audio format info
     }
     ```

2. **dependencies.json** (optional)
   - List of external dependencies
   - Used for build configuration

3. **presets.json** (optional)
   - Factory presets from the RNBO patch
   - Can be either array or dict format

4. **Library Files**
   - `.dylib` files (macOS libraries)
   - `.cpp` source files (C++ patch code)
   - `.h` header files (C++ interfaces)
   - `rnbo/` directory (RNBO framework headers)

**Analyzer Methods:**
- `analyze()`: Main analysis entry point
- `get_patch_name()`: Extract patch name from description
- `get_visible_parameters()`: Filter visible parameters only
- `get_parameter_count()`: Return total parameter count

---

## 4. VERIFICATION LOGIC

### `core/replacer.py` - IOSReplacer.verify_paths()

**Validation Checks:**

1. **Template Validation**
   ```
   ✓ Template directory exists
   ✓ SwiftRNBO_Example_multiplatfrom_SwiftUI project directory found
   ```

2. **AutoUI Files Verification**
   ```
   ✓ Core/ModeManager.swift          - Mode switching (Live/All/Setup)
   ✓ Core/ParameterFilter.swift      - Parameter filtering logic
   ✓ Core/AutoUIView.swift           - Main UI orchestration
   ✓ Core/Preset.swift               - Preset data model
   ✓ Core/PresetManager.swift        - Save/load presets
   ✓ Core/XYPadMapping.swift         - XY Pad parameter mapping
   ✓ Core/XYPadManager.swift         - XY Pad state management
   ✓ Widgets/AutoSlider.swift        - Parameter slider widget
   ✓ Widgets/ParameterRow.swift      - Parameter container with checkbox
   ✓ Widgets/RangeSlider.swift       - Custom range adjustment
   ✓ Views/PresetsManagerView.swift  - Preset UI
   ✓ Views/QuickPresetsView.swift    - Quick preset buttons (8 presets)
   ✓ Views/XYPadView.swift           - XY Pad visualization
   ```

3. **Export Validation**
   ```
   ✓ Export directory exists
   ✓ description.json present
   ```

4. **Output Directory Check**
   ```
   ? If exists: Ask user to confirm overwrite
   ```

---

## 5. FILE REPLACEMENT PIPELINE

### Step 1: Copy Template
```python
# Copy SwiftRNBO_Example_multiplatfrom_SwiftUI to output
shutil.copytree(template_project_dir, output_path)
```

### Step 2: Replace RNBO Export
```
Target: output/SwiftRNBO_Example_multiplatfrom_SwiftUI/RNBO/Export/

Actions:
├── Clear existing export files
├── Copy new RNBO files:
│   ├── description.json ✓ CRITICAL
│   ├── dependencies.json (optional)
│   ├── presets.json (optional)
│   ├── *.cpp (patch source code)
│   ├── *.h (patch headers)
│   └── rnbo/ (framework headers)
└── Log all copied items
```

### Step 3: Update Project Configuration
```
Actions:
├── Generate unique bundle ID
│   └── Format: com.rnbo.{appname}
├── Find project.pbxproj file
├── Replace bundle identifier
│   └── Old: com.EldarSadykov.SwiftRNBO-Example-multiplatfrom-SwiftUI
│   └── New: com.rnbo.{custom-name}
└── Write updated project file
```

---

## 6. RNBO C++ CODE INTERACTION

### Audio Pipeline Architecture

```
┌─────────────────────────────────────────────────────────┐
│ SwiftUI AutoUI (Parameter Control)                      │
│                                                         │
│  AutoSlider ──> ParameterRow ──> AutoUIView           │
│      ↓                ↓                ↓                │
│  User drags    Mode filtering    Live/All/Setup        │
│  slider        parameter display  switching             │
└─────────────────────────────────────────────────────────┘
         ↓
         ↓ setParameterValue()
         ↓
┌─────────────────────────────────────────────────────────┐
│ RNBOAudioUnitHostModel (Bridge/State Manager)          │
│                                                         │
│  @Published var parameters: [RNBOParameter]            │
│  @Published var audioUnit: RNBOAudioUnit               │
│                                                         │
│  Functions:                                            │
│  ├── setParameterValue(value, at: index)              │
│  ├── refreshParameterValue(at: index)                 │
│  └── sendNoteOn/Off, sendMIDI()                       │
└─────────────────────────────────────────────────────────┘
         ↓
         ↓ Direct C++ call
         ↓
┌─────────────────────────────────────────────────────────┐
│ RNBOAudioUnit (Objective-C Bridge)                      │
│                                                         │
│  Functions (Swift↔C++):                                │
│  ├── setParameterValue(index, value: Float)           │
│  ├── getParameterValue(index) → Float                 │
│  ├── setParameterValueNormalized(index, value: Float) │
│  ├── sendMessage(tag, list: [Double])                │
│  ├── sendNoteOn/Off(pitch, velocity, channel)         │
│  └── setEventHandler(RNBOEventHandler)                │
└─────────────────────────────────────────────────────────┘
         ↓
         ↓ Out-of-process AUAudioUnit
         ↓
┌─────────────────────────────────────────────────────────┐
│ RNBO C++ Engine (Patched Code)                          │
│                                                         │
│  Generated from RNBO description.json:                 │
│  ├── class rnbo::PatcherInterface                      │
│  ├── Parameter setters: setParameterValue()           │
│  ├── Parameter getters: getParameterValue()           │
│  ├── Audio processing: process()                      │
│  ├── MIDI input handling                              │
│  └── Event output callbacks                           │
└─────────────────────────────────────────────────────────┘
         ↓
         ↓ Audio samples + events
         ↓
┌─────────────────────────────────────────────────────────┐
│ AVAudioEngine (macOS/iOS Audio Framework)              │
│                                                         │
│  ├── AudioUnit input bus (microphone)                  │
│  ├── AudioUnit processing                             │
│  └── Main output mixer (speaker/headphone)            │
└─────────────────────────────────────────────────────────┘
```

### Parameter Normalization

**RNBO uses exponential parameter scaling:**

```swift
// From RNBOParameter.swift
// Convert 0.0-1.0 slider position to actual parameter value

valueNormalized (0.0-1.0) ↔ value (min-max range)

// With exponent (power) for exponential curves:
fromNormalised(normalized, min, max, exponent):
    exponentiatedValue = pow(normalized, exponent)
    return min + (max - min) * exponentiatedValue

toNormalised(value, min, max, exponent):
    normalizedValue = (value - min) / (max - min)
    return pow(normalizedValue, 1/exponent)
```

Example: Frequency parameter
```
Minimum: 20.0 Hz
Maximum: 20000.0 Hz
Exponent: 2.0 (exponential curve)

Slider at 50%:
  normalized = 0.5
  exponentiatedValue = pow(0.5, 2.0) = 0.25
  actual = 20 + (20000-20) * 0.25 = 5005 Hz
```

---

## 7. AUTOUI FILE STRUCTURE & INTEGRATION

### Core Data Models

#### RNBODescription.swift
```swift
struct RNBODescription: Codable {
    let parameters: [ParameterInfo]
    let numParameters, numInputChannels, numOutputChannels: Int
    // ... metadata
}

struct ParameterInfo: Codable {
    let index: Int
    let paramId: String
    let name, displayName: String
    let minimum, maximum, exponent: Double
    let steps: Int
    let initialValue: Double
    let unit: String
    let visible: Bool
    let isEnum: Bool
    let enumValues: [EnumValue]
    // ... other fields
}
```

#### RNBOParameter.swift
```swift
struct RNBOParameter {
    var value: Double
    let info: ParameterInfo
    
    var valueNormalized: Double { /* 0.0-1.0 */ }
    
    static func == (lhs: RNBOParameter, rhs: RNBOParameter) -> Bool
}
```

### UI Orchestration Hierarchy

```
AutoUIView (Main Orchestrator)
├── @StateObject var modeManager = ModeManager()
├── @StateObject var presetManager = PresetManager()
├── @StateObject var xyPadManager = XYPadManager()
│
├── ModeSelector
│   └── Picker(UIMode: .live, .all, .setup)
│
├── QuickPresetsView (LIVE mode header)
│   └── 8 buttons for quick preset loading
│
├── SetupModeControls (SETUP mode)
│   ├── "Select All" / "Deselect All" buttons
│   └── Parameter count display
│
├── PresetsManagerView (PRESETS mode)
│   ├── Saved presets list
│   ├── Save current as preset
│   └── Load/Delete/Rename presets
│
├── XYPadView (LIVE mode with XY Pad enabled)
│   └── Interactive 2D parameter mapper
│
└── ScrollView (Parameters List)
    └── LazyVStack of ParameterRows
        └── ParameterRow
            ├── AutoSlider (parameter widget)
            ├── Custom Range Controls (SETUP mode)
            └── XY Pad Assignment Section (SETUP mode)
```

### State Management Flow

```
RNBOAudioUnitHostModel (Root Observable)
├── @Published parameters: [RNBOParameter]
│   └── Direct connection to RNBO C++ engine
├── audioEngine: RNBOAudioEngine
│   └── AVAudioEngine + RNBOAudioUnit
└── eventHandler: RNBOEventHandler
    └── Receives parameter change events from C++

ModeManager (Mode Selection)
├── @Published currentMode: UIMode (.live, .all, .setup)
├── selectedParameterIds: Set<String>
│   └── Which parameters show in LIVE mode
├── customRanges: [String: CustomRange]
│   └── User-defined min/max per parameter
└── Functions:
    ├── toggleParameterSelection()
    ├── toggleCustomRange()
    └── getEffectiveRange() → (min, max)

PresetManager (Save/Load State)
├── @Published savedPresets: [Preset]
├── @Published quickPresets: [Preset] (max 8)
├── interpolationEnabled: Bool
├── interpolationTimeMs: Int
└── Functions:
    ├── saveCurrentAsPreset()
    ├── loadPreset()
    ├── toggleQuickPreset()
    └── startInterpolation()

XYPadManager (2D Parameter Control)
├── @Published isEnabled: Bool
├── xyPadMappings: [String: XYPadMapping]
│   └── paramId → {axis: .x/.y/.none, invert: Bool}
└── Functions:
    ├── setMapping()
    ├── toggleInvert()
    └── loadMappings()
```

### Parameter Flow: UI to C++ Engine

```
1. User Interaction (AutoSlider)
   └─> onChange: sliderBinding

2. Binding Update
   └─> rnbo.setParameterValue(mappedValue, at: index)

3. RNBOAudioUnitHostModel
   └─> audioUnit.setParameterValue(index, Float(value))

4. RNBOAudioUnit (Objective-C Bridge)
   └─> Calls C++ method via AudioUnit protocol

5. RNBO C++ Engine
   ├─> Parameter memory updated
   ├─> Next audio buffer processes with new value
   └─> May trigger output events

6. Event Feedback (Bidirectional)
   ├─> C++ engine sends RNBOParameterEvent
   ├─> RNBOEventHandler.handle(_ event)
   ├─> Updates rnbo.parameters[event.index].value
   └─> UI re-renders via @Published trigger
```

### Preset System Integration

```
Preset Structure:
┌─────────────────────────────────────────────┐
│ Preset {                                    │
│   id: UUID                                  │
│   name: String                              │
│   timestamp: Date                           │
│   parameterValues: [String: Double]         │
│   customRanges: [String: CustomRange]       │
│   liveParameters: Set<String>               │
│   xyPadMappings: [String: XYPadMapping]    │
│   isQuickPreset: Bool                       │
│ }                                           │
└─────────────────────────────────────────────┘

Persistence Layer:
┌──────────────────────────────────────────┐
│ PresetStorage {                          │
│   UserDefaults.standard                  │
│   ├── "autoui.presets"                   │
│   ├── "autoui.interpolation.enabled"     │
│   └── "autoui.interpolation.timeMs"      │
│ }                                        │
└──────────────────────────────────────────┘

Save Flow:
Current State ──> Preset ──> JSON Encode ──> UserDefaults

Load Flow:
UserDefaults ──> JSON Decode ──> Preset ──> Apply to Engine
                                    ├── Update parameter values
                                    ├── Restore custom ranges
                                    ├── Restore live selection
                                    └── Restore XY mappings
```

### Interpolation System

```
When loading preset with interpolation enabled:

1. Capture start values
   └─> interpolationStartValues[paramId] = current value

2. Set target values
   └─> interpolationTargetValues[paramId] = preset value

3. Start 60fps timer
   ├─> Calculate progress (0.0 → 1.0)
   ├─> Apply easing: easeInOutQuad()
   └─> Update all parameters

4. Interpolation equation:
   interpolated = start + (target - start) * easedProgress

5. Apply to engine every frame:
   └─> rnbo.setParameterValue(interpolated, at: index)

Example: 1-second morph
   0%   → value unchanged
   50%  → ~25% of the way (quadratic easing)
   100% → value at preset target
```

---

## 8. INTEGRATION POINTS & KEY FILES

### File Organization in Generated Project

```
MyRNBOApp/
├── SwiftRNBO_Example_multiplatfrom_SwiftUI/
│   ├── SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
│   │   └── project.pbxproj (bundle ID modified here)
│   │
│   └── SwiftRNBO_Example_multiplatfrom_SwiftUI/
│       ├── ContentView.swift (entry point)
│       ├── SwiftRNBO_Example_multiplatfrom_SwiftUIApp.swift (app delegate)
│       │
│       ├── AutoUI/ (AUTO-GENERATED UI SYSTEM)
│       │   ├── Core/
│       │   │   ├── AutoUIView.swift ✓ Main orchestrator
│       │   │   ├── ModeManager.swift ✓ Mode switching
│       │   │   ├── ParameterFilter.swift ✓ Filtering logic
│       │   │   ├── PresetManager.swift ✓ Save/load system
│       │   │   ├── Preset.swift ✓ Data model
│       │   │   ├── XYPadManager.swift ✓ 2D control state
│       │   │   └── XYPadMapping.swift ✓ Parameter mapping
│       │   ├── Widgets/
│       │   │   ├── AutoSlider.swift ✓ Parameter slider
│       │   │   ├── ParameterRow.swift ✓ Container
│       │   │   └── RangeSlider.swift ✓ Custom range editor
│       │   └── Views/
│       │       ├── PresetsManagerView.swift ✓ Preset UI
│       │       ├── QuickPresetsView.swift ✓ Quick buttons
│       │       └── XYPadView.swift ✓ 2D display
│       │
│       ├── RNBO/ (YOUR PATCH INTEGRATION)
│       │   ├── Export/ ✓ REPLACED BY REPLACER
│       │   │   ├── description.json ✓ Parameter metadata
│       │   │   ├── rnbo/ ✓ Framework headers
│       │   │   ├── *.cpp ✓ Patch source code
│       │   │   ├── *.h ✓ Patch headers
│       │   │   ├── dependencies.json (optional)
│       │   │   └── presets.json (optional)
│       │   │
│       │   └── Interface/ (BRIDGE LAYER)
│       │       ├── RNBOParameter.swift ✓ Parameter wrapper
│       │       ├── RNBODescription.swift ✓ Metadata parsing
│       │       ├── RNBOAudioUnitHostModel.swift ✓ State manager
│       │       ├── RNBOAudioEngine.swift ✓ Audio setup
│       │       ├── RNBOEventHandler.swift ✓ Event receiver
│       │       └── MIDIHelpers.swift ✓ MIDI utilities
```

### Critical Integration Points

1. **Description Loading** (RNBOAudioUnitHostModel.init)
   ```swift
   let url = Bundle.main.url(forResource: "description", withExtension: "json")!
   let data = try Data(contentsOf: url)
   description = try JSONDecoder().decode(RNBODescription.self, from: data)
   parameters = description?.getParametersArray() ?? []
   ```

2. **Parameter Binding** (AutoSlider)
   ```swift
   private var sliderBinding: Binding<Double> {
       Binding(
           get: { /* normalize to 0-1 */ },
           set: { sliderPosition in
               let mappedValue = effectiveRange.min + sliderPosition * range
               rnbo.setParameterValue(to: mappedValue, at: parameter.info.index)
               parameter.value = mappedValue
           }
       )
   }
   ```

3. **Event Handling** (RNBOEventHandler)
   ```swift
   func handle(_ event: RNBOParameterEvent) {
       DispatchQueue.main.async {
           rnbo.parameters[event.index].value = event.value
       }
   }
   ```

4. **Mode Filtering** (ParameterFilter)
   ```
   LIVE mode:  Show only modeManager.selectedParameterIds
   ALL mode:   Show all parameters where visible == true
   SETUP mode: Show all parameters for selection
   PRESETS:    Show preset management UI
   ```

---

## 9. COMMUNICATION PROTOCOL SUMMARY

### Swift → C++ Parameter Update

```
Timeline Event:
1. User moves slider  ──→ onChange callback
2. Compute mapped value ──→ range.min + position * (range.max - range.min)
3. Call setParameterValue ──→ @escaping closure to audioUnit
4. Objective-C bridge ──→ RNBOAudioUnit.setParameterValue(index, Float)
5. C++ setParameterValue ──→ rnbo::PatcherInterface::setParameterValue()
6. Next audio buffer ──→ C++ processes with new parameter value
7. Audio output ──→ Modified audio to speaker/headphones
```

### C++ → Swift Parameter Change

```
Timeline Event:
1. C++ code changes parameter (internal event)
2. Sends RNBOParameterEvent ──→ via event handler protocol
3. RNBOEventHandler.handle(_ event) ──→ Main thread dispatch
4. Update @Published rnbo.parameters[index].value ──→ SwiftUI reactive
5. AutoSlider re-renders ──→ Displays new value
6. UI reflects change ──→ Slider position updates
```

### Preset Load with Interpolation

```
Timeline Event (over 1000ms):
0ms:  StartInterpolation called
      ├─ Capture current values
      ├─ Set target values from preset
      └─ Start 60fps timer

16ms: First frame update
      ├─ elapsed = 16ms, progress = 1.6%
      ├─ eased = easeInOutQuad(0.016)
      └─ interpolated = start + (target-start) * eased

500ms: Halfway point
      ├─ elapsed = 500ms, progress = 50%
      ├─ eased ≈ 0.25 (quadratic slowdown)
      └─ Apply to all parameters

1000ms: Complete
      ├─ Progress = 100%
      ├─ Final values applied
      └─ Timer stopped
```

---

## 10. MISSING COMPONENTS

**Note:** The user mentioned `verifier.py` but this file does not exist in the current codebase. Verification is handled by:
- `IOSReplacer.verify_paths()` method in replacer.py
- `RNBOAnalyzer.analyze()` method validates export structure

---

## 11. KEY TAKEAWAYS

1. **Separation of Concerns:**
   - Python replacer: Project generation
   - Swift AutoUI: Parameter UI and state management
   - RNBO C++: Audio processing and signal flow

2. **Parameter Flow:**
   - UI changes → RNBOAudioUnitHostModel → RNBOAudioUnit → C++ engine
   - C++ events → RNBOEventHandler → @Published update → SwiftUI re-render

3. **Value Mapping:**
   - UI: 0.0-1.0 normalized slider position
   - RNBO: Custom range with exponential exponent scaling
   - Bidirectional conversion via toNormalised()/fromNormalised()

4. **State Persistence:**
   - Presets saved as JSON in UserDefaults
   - Includes parameter values, custom ranges, live selection, XY mappings
   - Interpolation smooths preset transitions at 60fps

5. **Three UI Modes:**
   - LIVE: Optimized performance, user-selected parameters only
   - ALL: Full parameter visibility and exploration
   - SETUP: Configuration and parameter selection

