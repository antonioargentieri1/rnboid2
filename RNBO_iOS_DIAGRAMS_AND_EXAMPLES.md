# iOS AutoUI System - Visual Diagrams & Code Examples

## PARAMETER UPDATE FLOW - DETAILED SEQUENCE DIAGRAM

```
┌─────────────────┐
│  User Interface │
│  (SwiftUI)      │
└────────┬────────┘
         │
         │ 1. User moves slider
         ↓
┌──────────────────────────────────────────┐
│  AutoSlider.sliderBinding.set()          │
│                                          │
│  Input: sliderPosition (0.0 - 1.0)       │
│  Compute: mappedValue =                  │
│    effectiveRange.min +                  │
│    sliderPosition * (range)              │
└──────────────────────────────────────────┘
         │
         │ 2. Call setParameterValue()
         ↓
┌──────────────────────────────────────────┐
│  RNBOAudioUnitHostModel                  │
│  @EnvironmentObject                      │
│                                          │
│  func setParameterValue(                 │
│    to value: Double,                     │
│    at parameterIndex: Int                │
│  ) {                                     │
│    audioUnit.setParameterValue(          │
│      parameterIndex,                     │
│      value: Float(value)                 │
│    )                                     │
│  }                                       │
└──────────────────────────────────────────┘
         │
         │ 3. Objective-C Bridge Call
         ↓
┌──────────────────────────────────────────┐
│  RNBOAudioUnit                           │
│  (Objective-C++ wrapper)                 │
│                                          │
│  - (void)setParameterValue:(int)index    │
│                     value:(float)value   │
│  {                                       │
│    rnbo::PatcherInterface *rnboPtr = ... │
│    rnboPtr->setParameterValue(           │
│      index, value                        │
│    );                                    │
│  }                                       │
└──────────────────────────────────────────┘
         │
         │ 4. C++ Method Call
         ↓
┌──────────────────────────────────────────┐
│  RNBO C++ Engine                         │
│  (rnbo::PatcherInterface)                │
│                                          │
│  Generated from RNBO export              │
│  - Updates internal parameter storage    │
│  - Parameter affects next process()      │
│  - May generate output events            │
└──────────────────────────────────────────┘
         │
         │ 5. Audio Processing
         ↓
┌──────────────────────────────────────────┐
│  AVAudioEngine                           │
│  (Next Audio Buffer ~23ms later)         │
│                                          │
│  process() with new parameter value      │
│  Output modified audio stream            │
└──────────────────────────────────────────┘
         │
         │ 6. Parameter Event (if internal change)
         ↓
┌──────────────────────────────────────────┐
│  RNBOEventHandler.handle()               │
│                                          │
│  Receives RNBOParameterEvent             │
│  Updates rnbo.parameters[index].value    │
│  (via DispatchQueue.main.async)          │
└──────────────────────────────────────────┘
         │
         │ 7. SwiftUI Reactive Update
         ↓
┌──────────────────────────────────────────┐
│  AutoSlider re-renders                   │
│  Slider position updates to show value   │
│  UI reflects change                      │
└──────────────────────────────────────────┘
```

---

## DESCRIPTION.JSON PARSING FLOW

```
Export Directory
├── description.json ────→ RNBOAnalyzer.analyze()
│                             │
│                             ├─ JSONDecoder()
│                             │
│                             ↓
│                         RNBODescription
│                         (Swift struct)
│                             │
│                             ├─ parameters: [ParameterInfo]
│                             ├─ numParameters: Int
│                             ├─ numInputChannels: Int
│                             ├─ numOutputChannels: Int
│                             └─ meta: Meta
│
├─ Packaged into Xcode project
│
├─ Bundled as "description.json"
│
└─ Loaded at Runtime
    │
    ├─ RNBOAudioUnitHostModel.init()
    │
    ├─ Bundle.main.url(forResource: "description", withExtension: "json")
    │
    ├─ JSONDecoder().decode(RNBODescription.self)
    │
    ├─ getParametersArray() → [RNBOParameter]
    │
    ├─ @Published var parameters
    │
    └─ AutoUI reads and displays each parameter
```

---

## STATE MACHINE: UI MODE TRANSITIONS

```
                    ┌─────────────┐
                    │    START    │
                    └──────┬──────┘
                           │
                           ↓
                    ┌─────────────┐
         ┌─────────→│   ALL MODE  │←─────────┐
         │          └─────────────┘          │
         │          Shows all visible        │
         │          parameters               │
         │          (default state)          │
         │                                   │
    Tap Setup        Tap All              Tap Live
         │                                   │
         ↓          ┌─────────────┐         ↑
    ┌─────────────┐ │  SETUP MODE │         │
    │  LIVE MODE  │ └─────────────┘     (Select
    └─────────────┘ Shows all params   at least
    Shows selected   with checkboxes    one param)
    parameters only  (Setup selection)
    (Performance)
         ↑                │
         │              Select     ← Cannot go to
         │              All/None     LIVE if none
         │                │         selected
         │                ↓
         │            Parameter
         │            Selection
         │            Changed
         └────────────────┘


SETUP MODE Details:
┌─────────────────────────────────────────┐
│ For each parameter:                     │
├─────────────────────────────────────────┤
│ ☐ Parameter Name          [○━━━━•━━━━]  │
│   └ Custom Range (toggle)              │
│      [────•────] (min-max slider)       │
│   └ XY Pad Assignment                  │
│      X Axis ⦿ Y Axis ◯ None ◯          │
│      ☐ Invert                          │
└─────────────────────────────────────────┘

LIVE MODE Details:
┌─────────────────────────────────────────┐
│ Quick Presets (8 buttons)               │
├─────────────────────────────────────────┤
│ [Preset1] [Preset2] ...                │
│                                         │
│ XY Pad (if enabled) or Parameters       │
│                                         │
│ Show only selected parameters           │
│ Optimized for performance               │
└─────────────────────────────────────────┘

PRESETS MODE Details:
┌─────────────────────────────────────────┐
│ [Save Current] [Clear All]              │
├─────────────────────────────────────────┤
│ Preset Name       Date       [Load] [X] │
│ ─────────────────────────────────────── │
│ "MyPreset1"       Jan 1...   [Load] [X] │
│ "MyPreset2"       Jan 2...   [Load] [X] │
│ ─────────────────────────────────────── │
│                                         │
│ ☐ Interpolation:        [1000ms slider] │
└─────────────────────────────────────────┘
```

---

## PRESET SYSTEM: SAVE & LOAD FLOW

### Save Preset Flow
```
Current Application State
│
├─ Parameter Values
│  └─ [frequency: 440.0, resonance: 0.5, ...]
│
├─ Custom Ranges
│  └─ [frequency: (20, 20000, enabled), ...]
│
├─ Live Parameters Selection
│  └─ Set<String> {frequency, resonance}
│
├─ XY Pad Mappings
│  └─ [resonance: (axis: .x, invert: false), ...]
│
└─ UI Mode State
   └─ selectedParameterIds: Set<String>

            ↓ saveCurrentAsPreset("My Setup")

      Create Preset Object
      {
        id: UUID(),
        name: "My Setup",
        timestamp: Date(),
        parameterValues: [String: Double],
        customRanges: [String: CustomRange],
        liveParameters: Set<String>,
        xyPadMappings: [String: XYPadMapping],
        isQuickPreset: false
      }

            ↓ Add to savedPresets array

      PresetManager.savedPresets
            │
            ↓ JSONEncoder + UserDefaults

      "autoui.presets" key in UserDefaults
            │
            └─ Binary JSON data stored on device
```

### Load Preset Flow
```
User Taps "Load Preset" or Quick Preset Button
            │
            ↓ Load Preset from savedPresets

       Found: Preset {
         parameterValues: [String: Double],
         customRanges: [String: CustomRange],
         liveParameters: Set<String>,
         xyPadMappings: [String: XYPadMapping]
       }

            ↓ if interpolationEnabled

       ┌─────────────────────────────────┐
       │ Start Interpolation (60fps)      │
       │                                 │
       │ Timer: 0ms → 1000ms             │
       │ Progress: 0.0 → 1.0             │
       │ Easing: easeInOutQuad()         │
       │                                 │
       │ Each frame:                     │
       │ ├─ interpolated =               │
       │ │   start +                     │
       │ │   (target - start) * eased    │
       │ │                               │
       │ └─ setParameterValue(           │
       │     interpolated, index)        │
       └─────────────────────────────────┘
            │
            ↓ else (instant load)

       Apply all values immediately:
       ├─ customRanges ← preset ranges
       ├─ liveParameters ← preset selection
       ├─ xyPadMappings ← preset mappings
       └─ parameters[i].value ← preset values

            ↓ Update RNBO Engine

       For each parameter:
       ├─ rnbo.setParameterValue(value, index)
       └─ Internal C++ parameter storage updated

            ↓ Next Audio Buffer

       RNBO processes with preset values
            │
            └─ Audio output changes
```

---

## XCODE PROJECT GENERATION STEPS

### Step 1: Analyze RNBO Export
```
my_synth_export/
├── description.json
│   ├── Read & parse
│   ├── Extract: 12 parameters, 2-in/2-out audio
│   ├── Extract: Visible parameters list
│   └── Store in analyzer memory
│
├── rnbo/
│   └── Verify framework headers present
│
├── my_synth.cpp
│   └── Verify source code present
│
└── presets.json (optional)
    └── Count: 5 factory presets
```

### Step 2: Verify Template
```
SwiftRNBO-main/
├── SwiftRNBO_Example_multiplatfrom_SwiftUI/
│   └── SwiftRNBO_Example_multiplatfrom_SwiftUI/
│       ├── RNBO/
│       │   ├── Export/ ← Will replace contents
│       │   └── Interface/ ← Verify all files
│       │
│       └── AutoUI/ ← Verify structure
│           ├── Core/ (8 files)
│           ├── Widgets/ (3 files)
│           └── Views/ (3 files)
│
└── result: Template valid ✓
```

### Step 3: Copy & Replace
```
Step 3a: Copy template
    Template → /tmp/MyApp (full copy)

Step 3b: Clear old export
    /tmp/MyApp/.../RNBO/Export/*
    └─ Delete all files

Step 3c: Copy new export
    my_synth_export/* 
    → /tmp/MyApp/.../RNBO/Export/
    
    Copies:
    ├── description.json ✓
    ├── rnbo/ ✓
    ├── my_synth.cpp ✓
    ├── my_synth.h ✓
    └── presets.json ✓

Step 3d: Update Xcode configuration
    Find: com.EldarSadykov.SwiftRNBO-Example-multiplatfrom-SwiftUI
    Replace: com.rnbo.mysynth
    In: project.pbxproj
```

### Step 4: Ready to Build
```
Output Project:
MyApp/
├── SwiftRNBO_Example_multiplatfrom_SwiftUI/
│   ├── SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
│   │   └── project.pbxproj (✓ updated bundle ID)
│   │
│   └── SwiftRNBO_Example_multiplatfrom_SwiftUI/
│       ├── RNBO/
│       │   ├── Export/
│       │   │   ├── description.json (✓ from your patch)
│       │   │   ├── my_synth.cpp (✓ from your patch)
│       │   │   ├── my_synth.h (✓ from your patch)
│       │   │   ├── rnbo/ (✓ from your patch)
│       │   │   └── presets.json (✓ from your patch)
│       │   └── Interface/
│       │       └── ... (unchanged)
│       │
│       └── AutoUI/ (✓ ready to use)

Next:
$ open MyApp/.../SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
$ Cmd+R (Build & Run)
```

---

## CODE EXAMPLE: PARAMETER BINDING

### AutoSlider Binding Implementation

```swift
private var sliderBinding: Binding<Double> {
    Binding(
        get: {
            // GET: Convert parameter value to normalized 0-1
            let currentValue = parameter.value
            let range = effectiveRange
            
            // Normalize to 0-1 based on custom/original range
            let normalized = (currentValue - range.min) / (range.max - range.min)
            
            // Clamp to valid range
            return max(0, min(1, normalized))
        },
        set: { sliderPosition in
            // SET: User moved slider
            // sliderPosition is 0.0-1.0 from SwiftUI Slider
            
            let range = effectiveRange
            
            // Map slider position to actual parameter value
            let mappedValue = range.min + sliderPosition * (range.max - range.min)
            
            // Send to RNBO C++ engine
            // This is the critical call that changes the audio
            rnbo.setParameterValue(to: mappedValue, at: parameter.info.index)
            
            // Update local binding (UI immediate feedback)
            parameter.value = mappedValue
        }
    )
}
```

### Custom Range Effect

```
Example: Frequency parameter
  Original range: 20 Hz - 20000 Hz
  User selects custom range: 440 Hz - 8000 Hz
  
  Slider now maps:
  0%   → 440 Hz (custom min)
  50%  → 4220 Hz (midpoint)
  100% → 8000 Hz (custom max)
  
  Instead of:
  0%   → 20 Hz
  50%  → 10010 Hz
  100% → 20000 Hz
  
  This allows fine-tuning without scrolling the full range.
```

---

## CODE EXAMPLE: PRESET INTERPOLATION

```swift
// Example: Load preset with 1-second morphing

let targetPreset = Preset(
    name: "Lush Pad",
    parameterValues: [
        "cutoff": 8000.0,
        "resonance": 0.8,
        "decay": 2.0,
        "dry": 0.2,
        "wet": 0.8
    ],
    // ... other fields
)

// Load with interpolation (if enabled)
presetManager.loadPreset(
    targetPreset,
    parameters: &rnbo.parameters,
    customRanges: &modeManager.customRanges,
    liveParameters: &modeManager.selectedParameterIds,
    xyPadManager: xyPadManager,
    rnbo: rnbo
)

// Timeline during interpolation:
// 0ms:    Start values captured
// 16ms:   0.2% progress, eased to ~0.0%
// 250ms:  25% progress, eased to ~6.25%
// 500ms:  50% progress, eased to ~25%
// 750ms:  75% progress, eased to ~56.25%
// 1000ms: 100% progress → complete

// For each parameter, every 16ms:
let progress = elapsed / 1000.0
let eased = easeInOutQuad(progress)
let value = startValue + (targetValue - startValue) * eased
rnbo.setParameterValue(to: value, at: index)
```

---

## PARAMETER INDEX MAPPING

```
description.json
{
  "parameters": [
    {
      "index": 0,
      "paramId": "frequency",
      "name": "/frequency"
    },
    {
      "index": 1,
      "paramId": "resonance",
      "name": "/resonance"
    },
    ...
  ]
}

↓ Loaded and indexed in Swift

rnbo.parameters[0] → RNBOParameter(frequency)
rnbo.parameters[1] → RNBOParameter(resonance)

↓ UI reads parameter info

AutoSlider(parameter: $rnbo.parameters[0])
  │
  └─ Shows "Frequency"
  └─ Range: 20 - 20000
  └─ Unit: "Hz"

↓ When changed

rnbo.setParameterValue(to: 8000.0, at: 0)
         │
         └─ index 0 → RNBO C++ knows which parameter
         └─ value 8000.0 → Sets cutoff frequency

↓ C++ engine

rnbo::PatcherInterface::setParameterValue(0, 8000.0)
  └─ Updates: parameters[0] = 8000.0
  └─ Next audio buffer uses new value
```

---

## MODE MANAGER STATE EXAMPLE

```swift
class ModeManager: ObservableObject {
    @Published var currentMode: UIMode = .all
    @Published var selectedParameterIds: Set<String> = []
    @Published var customRanges: [String: CustomRange] = [:]
    
    // State transitions
    
    // LIVE → SETUP
    func switchToSetup() {
        currentMode = .setup
        // Show all parameters with checkboxes
        // Allow user to select which appear in LIVE
    }
    
    // SETUP → LIVE (only if at least one selected)
    func switchToLive() {
        if !selectedParameterIds.isEmpty {
            currentMode = .live
            // Show only selected parameters
            // Hide setup controls
        }
    }
    
    // Toggle parameter visibility
    func toggleParameterSelection(_ paramId: String) {
        if selectedParameterIds.contains(paramId) {
            selectedParameterIds.remove(paramId)
        } else {
            selectedParameterIds.insert(paramId)
        }
        // If all deselected and in LIVE, revert to ALL
        if selectedParameterIds.isEmpty && currentMode == .live {
            currentMode = .all
        }
    }
    
    // Custom range management
    func toggleCustomRange(_ paramId: String) {
        if let range = customRanges[paramId] {
            customRanges[paramId]?.enabled.toggle()
        } else {
            // Create default custom range (same as original)
            customRanges[paramId] = CustomRange(
                min: param.info.minimum,
                max: param.info.maximum,
                enabled: true
            )
        }
    }
    
    // Effective range (considers custom vs original)
    func getEffectiveRange(for parameter: RNBOParameter) -> (min: Double, max: Double) {
        if let custom = customRanges[parameter.id], custom.enabled {
            return (min: custom.customMin, max: custom.customMax)
        } else {
            return (min: parameter.info.minimum, max: parameter.info.maximum)
        }
    }
}
```

