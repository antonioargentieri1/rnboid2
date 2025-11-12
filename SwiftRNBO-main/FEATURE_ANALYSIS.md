# RNBOID2 Feature Analysis: Android vs iOS

Comprehensive comparison of features implemented in Android RNBOID2 vs iOS SwiftRNBO AutoUI.

## 📊 Feature Comparison Matrix

| Feature | Android | iOS | Priority | Complexity |
|---------|---------|-----|----------|------------|
| **Core UI** |
| Parameter sliders | ✅ | ✅ | - | - |
| Auto-generated UI | ✅ | ✅ | - | - |
| Real-time value display | ✅ | ✅ | - | - |
| Min/max labels | ✅ | ✅ | - | - |
| Unit display | ✅ | ✅ | - | - |
| **Modes** |
| Live mode | ✅ | ✅ | - | - |
| All mode | ✅ | ✅ | - | - |
| Setup mode | ✅ | ✅ | - | - |
| Presets mode | ✅ | ❌ | 🔥 HIGH | Medium |
| **Custom Range** |
| Per-parameter range override | ✅ | ❌ | 🔥 HIGH | Medium |
| Range slider UI (dual thumb) | ✅ | ❌ | 🔥 HIGH | Medium |
| Enable/disable custom range | ✅ | ❌ | 🔥 HIGH | Low |
| Persistent range storage | ✅ | ❌ | 🔥 HIGH | Low |
| **Presets** |
| Save/load presets | ✅ | ❌ | 🔥 HIGH | High |
| Quick presets (8 slots) | ✅ | ❌ | 🟡 MEDIUM | Medium |
| Preset interpolation | ✅ | ❌ | 🟡 MEDIUM | High |
| Configurable interpolation time | ✅ | ❌ | 🟢 LOW | Low |
| Preset metadata (name, timestamp) | ✅ | ❌ | 🟢 LOW | Low |
| **Sensors** |
| Accelerometer mapping | ✅ | ❌ | 🟡 MEDIUM | High |
| Gyroscope mapping | ✅ | ❌ | 🟡 MEDIUM | High |
| Per-parameter sensor assignment | ✅ | ❌ | 🟡 MEDIUM | Medium |
| Sensor value smoothing | ✅ | ❌ | 🟢 LOW | Medium |
| Global sensor on/off | ✅ | ❌ | 🟢 LOW | Low |
| **XY Pad** |
| 2D touch control | ✅ | ❌ | 🟡 MEDIUM | Medium |
| Dual parameter mapping (X/Y) | ✅ | ❌ | 🟡 MEDIUM | Low |
| Visual feedback | ✅ | ❌ | 🟢 LOW | Low |
| **Buffer Management** |
| Load audio files to buffers | ✅ | ❌ | 🟢 LOW | High |
| File picker integration | ✅ | ❌ | 🟢 LOW | Medium |
| **Persistence** |
| Selected parameters (Live mode) | ✅ | ✅ | - | - |
| Custom ranges | ✅ | ❌ | 🔥 HIGH | Low |
| Sensor mappings | ✅ | ❌ | 🟡 MEDIUM | Low |
| Presets | ✅ | ❌ | 🔥 HIGH | Medium |
| Slider positions | ✅ | ✅ (implicit) | - | - |

---

## 🔥 Priority 1: HIGH (Must-Have)

### 1. Custom Range System

**What it does:**
- Allows users to restrict parameter range to a subset of the full range
- Example: Parameter has range 20-20000 Hz, user sets custom range 200-2000 Hz
- Makes it easier to control parameters with large ranges

**Android Implementation:**
```java
private static class CustomRange {
    boolean enabled;
    double customMin;
    double customMax;
}

private Map<String, CustomRange> customRangeMap = new HashMap<>();
```

**UI Components:**
- ✅ Checkbox to enable/disable custom range per parameter
- ✅ RangeSlider (dual-thumb) to set min/max
- ✅ Label showing current range: `[200.00 - 2000.00]`
- ✅ Persistent storage with SharedPreferences

**Benefits:**
- 🎯 Precision: Fine control over large ranges
- 🎛️ UX: Focus on musically relevant range
- 💾 Persistent: Saved per-patch, per-parameter

**iOS Implementation Required:**
```swift
// 1. Data model
struct CustomRange {
    var enabled: Bool
    var customMin: Double
    var customMax: Double
}

// 2. Add to ModeManager
@Published var customRanges: [String: CustomRange] = [:]

// 3. Update AutoSlider to use custom range instead of info.min/max
// 4. Add UI in Setup mode:
//    - Toggle for "Use Custom Range"
//    - Dual-thumb slider (iOS doesn't have native, need custom or library)
```

**Estimated Implementation:**
- SwiftUI RangeSlider custom component: 3-4 hours
- Integration in AutoSlider: 1-2 hours
- Setup mode UI: 2-3 hours
- Persistence (UserDefaults): 1 hour
- **Total: ~8-10 hours**

---

### 2. Presets System

**What it does:**
- Save current state of all parameters as a named preset
- Load presets to recall parameter configurations
- Optional smooth interpolation between presets

**Android Implementation:**
```java
private static class Preset {
    String name;
    long timestamp;
    Map<String, CustomRange> customRanges;    // Includes ranges
    Set<String> liveParameters;               // Includes Live mode config
    Map<String, Float> parameterValues;       // All parameter values
    boolean isQuickPreset;                    // Quick vs named preset
}

private List<Preset> savedPresets = new ArrayList<>();
private List<Preset> quickPresets = new ArrayList<>();
```

**Features:**
- ✅ Save preset with custom name
- ✅ Load preset (instant or interpolated)
- ✅ Quick presets (8 slots, one-tap save/load)
- ✅ Preset browser with delete option
- ✅ Interpolation time configurable (0-5000ms)
- ✅ Metadata: timestamp, preset name

**Presets Mode UI (Android):**
```
┌─────────────────────────────┐
│ PRESETS MODE                │
├─────────────────────────────┤
│ [Interpolation: ON  1000ms] │
│ [Save New Preset]           │
│                             │
│ Quick Presets:              │
│ [1] [2] [3] [4]            │
│ [5] [6] [7] [8]            │
│                             │
│ Saved Presets:              │
│ ┌─────────────────────────┐│
│ │ "Bright Lead"       [🗑]││
│ │ 2024-11-10          [⬇]││
│ ├─────────────────────────┤│
│ │ "Dark Pad"          [🗑]││
│ │ 2024-11-09          [⬇]││
│ └─────────────────────────┘│
└─────────────────────────────┘
```

**Benefits:**
- 🎭 Instant sound switching
- 🎹 Performance tool (quick presets)
- 💾 Patch management
- 🎨 Experimentation (save variations)

**iOS Implementation Required:**
```swift
// 1. Data model
struct RNBOPreset: Codable {
    var id: UUID
    var name: String
    var timestamp: Date
    var parameterValues: [String: Double]
    var customRanges: [String: CustomRange]
    var liveParameters: Set<String>
    var isQuickPreset: Bool
}

// 2. Preset Manager
class PresetManager: ObservableObject {
    @Published var presets: [RNBOPreset] = []
    @Published var quickPresets: [RNBOPreset?] = Array(repeating: nil, count: 8)
    @Published var interpolationEnabled: Bool = false
    @Published var interpolationTime: Double = 1.0 // seconds

    func savePreset(name: String, parameters: [RNBOParameter], ...) { }
    func loadPreset(_ preset: RNBOPreset, animated: Bool) { }
    func deletePreset(_ preset: RNBOPreset) { }
}

// 3. New View: PresetsView.swift
struct PresetsView: View {
    // Preset browser, interpolation settings, quick slots
}

// 4. Add "Presets" mode to UIMode enum
enum UIMode {
    case live, all, setup, presets
}
```

**Estimated Implementation:**
- Data models + PresetManager: 3-4 hours
- PresetsView UI: 4-6 hours
- Interpolation animation: 3-4 hours
- Quick presets slots: 2-3 hours
- Persistence: 1-2 hours
- **Total: ~15-20 hours**

---

## 🟡 Priority 2: MEDIUM (Nice-to-Have)

### 3. Sensor Integration

**What it does:**
- Map device sensors (accelerometer, gyroscope) to parameters
- Tilt phone to control cutoff, shake to control resonance, etc.

**Android Implementation:**
- Per-parameter sensor assignment in Setup mode
- Sensor type dropdown (accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z)
- Works with custom ranges
- Global on/off switch
- Auto-smoothing with configurable window

**Benefits:**
- 🎸 Expressive control
- 🎤 Performance tool
- 🎮 Novel interaction

**iOS Implementation:**
- CoreMotion framework
- Similar UI in Setup mode
- Sensor data smoothing/filtering

**Estimated Implementation:** ~12-15 hours

---

### 4. XY Pad

**What it does:**
- 2D touch surface for simultaneous control of 2 parameters
- X axis → param1, Y axis → param2
- Visual crosshair feedback

**Android Implementation:**
```java
public class XYPadView extends View {
    private PointF currentPosition = new PointF(0.5f, 0.5f);

    public interface XYPadListener {
        void onValueChanged(float x, float y);
        void onTouchStateChanged(boolean isTouching);
    }
}
```

**Benefits:**
- 🎹 Filter sweep + resonance
- 🎛️ Panning + volume
- 🎨 Creative control combinations

**iOS Implementation:**
- Custom SwiftUI view with DragGesture
- Canvas for drawing grid + cursor
- Parameter assignment dropdowns

**Estimated Implementation:** ~8-10 hours

---

### 5. Quick Presets

**What it does:**
- 8 dedicated slots for instant save/recall
- Long-press to save, tap to load
- Visual indication of filled slots

**Benefits:**
- ⚡ Fastest preset workflow
- 🎭 Live performance tool
- 🔢 Numbered (1-8) for easy recall

**iOS Implementation:**
- Part of Presets system (see above)
- HStack of 8 buttons in Presets mode

**Estimated Implementation:** Included in Presets system (~3 hours standalone)

---

## 🟢 Priority 3: LOW (Future Enhancements)

### 6. Interpolation Settings

**What it does:**
- Configure smooth transition time between presets
- Range: 0ms (instant) to 5000ms (5 seconds)

**iOS Implementation:**
- Slider in Presets mode
- Use withAnimation in SwiftUI

**Estimated Implementation:** ~2-3 hours

---

### 7. Buffer Management

**What it does:**
- Load audio files into RNBO buffers
- File picker → sample selection
- Replace buffer content at runtime

**Benefits:**
- 🎵 User-provided samples
- 🎸 Custom instrument sounds
- 🔊 Dynamic audio loading

**Complexity:**
- Requires RNBO C++ buffer API
- File picker (DocumentPicker in SwiftUI)
- Audio format conversion

**Estimated Implementation:** ~15-20 hours

---

### 8. Sensor Value Smoothing

**What it does:**
- Moving average filter on sensor data
- Prevents jittery parameter control

**iOS Implementation:**
- Ring buffer for sensor history
- Configurable window size

**Estimated Implementation:** ~3-4 hours

---

## 📈 Implementation Roadmap

### Phase 1: Essential Features (HIGH Priority)
**Goal:** Feature parity with Android on core functionality

1. **Custom Range System** (~8-10 hours)
   - Week 1: RangeSlider component
   - Week 1-2: Integration + UI

2. **Presets System** (~15-20 hours)
   - Week 2-3: Data models + manager
   - Week 3-4: UI + interpolation
   - Week 4: Testing + polish

**Total Phase 1:** ~25-30 hours (3-4 weeks part-time)

---

### Phase 2: Enhanced Interaction (MEDIUM Priority)
**Goal:** Add advanced control methods

3. **XY Pad** (~8-10 hours)
   - Week 5: Custom view + gestures
   - Week 5-6: Integration

4. **Sensor Integration** (~12-15 hours)
   - Week 6-7: CoreMotion setup
   - Week 7-8: UI + mapping

**Total Phase 2:** ~20-25 hours (2-3 weeks part-time)

---

### Phase 3: Polish & Extensions (LOW Priority)
**Goal:** Nice-to-have features

5. **Buffer Management** (~15-20 hours)
6. **Advanced Smoothing** (~3-4 hours)

**Total Phase 3:** ~18-24 hours (2-3 weeks part-time)

---

## 🎯 Recommended Next Steps

### Option A: Custom Range (Quick Win)
**Why:**
- Most requested feature
- Relatively simple to implement
- Big UX improvement for large ranges
- Foundation for other features (presets use it)

**Implementation Order:**
1. Add CustomRange model to ModeManager
2. Create RangeSliderView (custom SwiftUI component)
3. Add toggle + slider in Setup mode
4. Update AutoSlider to use custom range
5. Add UserDefaults persistence

**Timeline:** 1-2 weeks

---

### Option B: Presets Mode (High Impact)
**Why:**
- Most powerful feature from Android
- Professional workflow tool
- Enables experimentation
- High user value

**Implementation Order:**
1. Create RNBOPreset model + PresetManager
2. Build PresetsView UI (list, save, load)
3. Add interpolation animations
4. Add quick preset slots
5. Integrate with existing modes

**Timeline:** 3-4 weeks

---

### Option C: Both (Optimal)
**Why:**
- Custom ranges are used BY presets
- Logical dependency chain
- Best long-term architecture

**Implementation Order:**
1. Phase 1a: Custom Range (week 1-2)
2. Phase 1b: Presets (week 3-5)
3. Integration (week 6)

**Timeline:** 6 weeks

---

## 💡 Technical Considerations

### SwiftUI vs Android Equivalents

| Android | SwiftUI | Notes |
|---------|---------|-------|
| `SharedPreferences` | `UserDefaults` | Same concept |
| `RangeSlider` | Custom view | No native component |
| `SeekBar` | `Slider` | Native equivalent |
| `CheckBox` | `Toggle` | Native equivalent |
| `LinearLayout` | `VStack`/`HStack` | Similar layout |
| `SensorManager` | `CoreMotion` | Different API |
| `Handler.postDelayed` | `DispatchQueue.asyncAfter` | Similar pattern |
| `Animation` | `withAnimation` | SwiftUI is easier |

### Challenges

1. **RangeSlider**: iOS doesn't have native dual-thumb slider
   - **Solutions:**
     - Custom SwiftUI component
     - Third-party library
     - Two separate sliders (less UX)

2. **Sensor Integration**: CoreMotion API different from Android
   - **Solutions:**
     - CMMotionManager for accelerometer/gyro
     - Similar mapping logic
     - Platform-specific calibration

3. **Interpolation**: SwiftUI animations work differently
   - **Solutions:**
     - Use `withAnimation { }` for smooth transitions
     - Timer-based for custom easing
     - Combine framework for reactive updates

---

## 🔍 Code Examples Preview

### Custom Range in Swift

```swift
// ModeManager.swift
struct CustomRange: Codable {
    var enabled: Bool = false
    var customMin: Double
    var customMax: Double

    init(min: Double, max: Double) {
        self.customMin = min
        self.customMax = max
    }
}

class ModeManager: ObservableObject {
    @Published var customRanges: [String: CustomRange] = [:]

    func getEffectiveRange(for parameter: RNBOParameter) -> (Double, Double) {
        if let customRange = customRanges[parameter.id], customRange.enabled {
            return (customRange.customMin, customRange.customMax)
        }
        return (parameter.info.minimum, parameter.info.maximum)
    }
}
```

### Preset Model in Swift

```swift
// PresetManager.swift
struct RNBOPreset: Codable, Identifiable {
    let id: UUID
    var name: String
    var timestamp: Date
    var parameterValues: [String: Double]
    var customRanges: [String: CustomRange]
    var liveParameters: Set<String>
    var isQuickPreset: Bool

    init(name: String, from rnbo: RNBOAudioUnitHostModel, modeManager: ModeManager) {
        self.id = UUID()
        self.name = name
        self.timestamp = Date()
        self.isQuickPreset = false

        // Capture current state
        self.parameterValues = Dictionary(
            uniqueKeysWithValues: rnbo.parameters.map { ($0.id, $0.value) }
        )
        self.customRanges = modeManager.customRanges
        self.liveParameters = modeManager.selectedParameterIds
    }
}
```

---

## 📊 Summary

**Features implemented in Android but missing in iOS:**
- ✅ Already have: 3 modes, parameter UI, persistence
- 🔥 **HIGH Priority** (must-have): Custom Ranges, Presets
- 🟡 **MEDIUM Priority** (nice-to-have): Sensors, XY Pad, Quick Presets
- 🟢 **LOW Priority** (future): Buffers, Advanced settings

**Recommended implementation:**
1. Start with **Custom Range** (1-2 weeks)
2. Add **Presets Mode** (3-4 weeks)
3. Evaluate user feedback before sensors/XY pad

**Total effort for feature parity:**
- Core features (HIGH): ~25-30 hours
- All features (HIGH+MEDIUM): ~45-55 hours
- Complete parity (ALL): ~65-80 hours

---

**Next question:** Which feature do you want to implement first? 🚀
