# RNBO Replacer iOS/macOS

Automatic Xcode project generator for RNBO exports with AutoUI system.

## 🎯 What it does

Transforms an RNBO patch export into a ready-to-run iOS/macOS Xcode project with auto-generated UI.

**Before:**
```
1. Export RNBO patch from Max/MSP
2. Manually copy files to Xcode project
3. Write Swift UI code for each parameter
4. Configure build settings
5. Test and debug
```

**After:**
```bash
python main.py --template SwiftRNBO-main --export my_patch --output MyApp
open MyApp/SwiftRNBO_Example_multiplatfrom_SwiftUI/SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
# Press Cmd+R -> Done! ✅
```

## ✨ Features

- ✅ **Automatic UI Generation**: AutoUI system creates parameter UI automatically
- ✅ **3 UI Modes**: Live, All, Setup modes built-in
- ✅ **Persistent Configuration**: User selections saved with UserDefaults
- ✅ **Multi-platform**: iOS, macOS, tvOS support
- ✅ **Zero Swift coding**: Just run the script and open Xcode
- ✅ **Bundle ID management**: Unique identifiers auto-generated

## 📋 Requirements

- **Python 3.8+** (no external dependencies)
- **macOS** with Xcode installed
- **SwiftRNBO-main** template (with AutoUI integrated)
- **RNBO export** from Max/MSP

## 🚀 Quick Start

### 1. Export your RNBO patch from Max/MSP

In Max/MSP:
- Open your patch
- Select "Export" → "Export C++ Code"
- Choose output directory
- Wait for export to complete

Your export should contain:
```
my_patch_export/
├── description.json    ✅ Required
├── dependencies.json   (optional)
├── presets.json        (optional)
├── rnbo/              ✅ Required
├── *.cpp              ✅ Required
├── *.h                ✅ Required
└── *.dylib            (for macOS)
```

### 2. Run the replacer

```bash
cd rnbo_replacer_ios

python main.py \
  --template ../SwiftRNBO-main \
  --export /path/to/my_patch_export \
  --output ./MyRNBOApp
```

**Options:**
- `--template`: Path to SwiftRNBO-main directory (with AutoUI)
- `--export`: Path to RNBO export directory
- `--output`: Where to create the new project
- `--name`: (Optional) Custom app name
- `--log`: (Optional) Path to log file

### 3. Open in Xcode

```bash
cd MyRNBOApp/SwiftRNBO_Example_multiplatfrom_SwiftUI
open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
```

### 4. Configure signing (first time only)

In Xcode:
1. Select project in Navigator (blue icon)
2. Select target → "Signing & Capabilities"
3. Check ✅ "Automatically manage signing"
4. Select your Team (Apple ID)

### 5. Build and Run

Press **Cmd+R** or click ▶️

## 📱 AutoUI System

The generated project includes the complete AutoUI system:

### 🎛️ Three Modes:

**1. Live Mode**
- Shows only selected parameters
- Optimized for performance
- Perfect for live use

**2. All Mode** (default)
- Shows all RNBO parameters
- Full control
- Parameter exploration

**3. Setup Mode**
- Select which parameters appear in Live mode
- Checkboxes for each parameter
- "Select All" / "Deselect All" buttons
- Configuration persists between sessions

### 🎨 Features:

- ✅ Auto-generated sliders for all parameters
- ✅ Min/max labels with units
- ✅ Current value display
- ✅ Real-time value updates
- ✅ Normalized value handling (0.0-1.0)
- ✅ Discrete parameter support (steps)
- ✅ Persistent configuration (UserDefaults)

## 📂 Project Structure

After running the replacer:

```
MyRNBOApp/
├── SwiftRNBO_Example_multiplatfrom_SwiftUI/
│   ├── SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj  ← Open this!
│   └── SwiftRNBO_Example_multiplatfrom_SwiftUI/
│       ├── AutoUI/                    ✅ Auto-generated UI
│       │   ├── Core/
│       │   │   ├── ModeManager.swift
│       │   │   ├── ParameterFilter.swift
│       │   │   └── AutoUIView.swift
│       │   └── Widgets/
│       │       ├── AutoSlider.swift
│       │       └── ParameterRow.swift
│       ├── RNBO/
│       │   ├── Export/               ✅ Your patch files
│       │   │   ├── description.json
│       │   │   ├── *.cpp
│       │   │   ├── *.h
│       │   │   └── rnbo/
│       │   └── Interface/
│       │       ├── RNBOParameter.swift
│       │       └── RNBODescription.swift
│       ├── ContentView.swift
│       └── ...
```

## 🔧 Customization

### Change Bundle Identifier

The script auto-generates a bundle ID like `com.rnbo.mypatch`.

To customize:
1. Open Xcode project
2. Select project → target → "General"
3. Change "Bundle Identifier"

### Change App Name

Use the `--name` flag:

```bash
python main.py \
  --template ../SwiftRNBO-main \
  --export ./my_patch \
  --output ./MyApp \
  --name "My Amazing Synth"
```

## 🐛 Troubleshooting

### "Template not found"

Make sure you're pointing to the correct SwiftRNBO-main directory:

```bash
ls -la ../SwiftRNBO-main/SwiftRNBO_Example_multiplatfrom_SwiftUI/
```

Should show the Xcode project.

### "AutoUI directory not found"

The template must have the AutoUI system integrated. Clone the latest version from the branch with AutoUI.

### "Export not found"

Check that your export path contains `description.json`:

```bash
ls -la /path/to/export/description.json
```

### Xcode signing error

1. Xcode → Preferences → Accounts
2. Add your Apple ID
3. In project settings, select your team

### Build errors

1. Clean build folder: Xcode → Product → Clean Build Folder
2. Restart Xcode
3. Check that all AutoUI files are added to target

## 📊 Example Workflow

```bash
# 1. Clone repository
git clone https://github.com/yourusername/rnboid2.git
cd rnboid2

# 2. Export patch from Max/MSP to /tmp/my_synth_export

# 3. Generate project
cd rnbo_replacer_ios
python main.py \
  --template ../SwiftRNBO-main \
  --export /tmp/my_synth_export \
  --output ~/Desktop/MySynthApp

# 4. Open in Xcode
cd ~/Desktop/MySynthApp/SwiftRNBO_Example_multiplatfrom_SwiftUI
open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj

# 5. Build & Run (Cmd+R)
```

## 🎓 Architecture

### Replacer Flow:

```
1. Verify Paths
   ├── Check template exists
   ├── Check AutoUI files present
   └── Check export valid

2. Analyze Export
   ├── Read description.json
   ├── Extract parameters
   ├── Find library files
   └── Count presets

3. Copy Template
   └── Clone entire SwiftRNBO-main to output

4. Replace RNBO Export
   ├── Clear old export files
   └── Copy new export files

5. Update Configuration
   ├── Generate bundle identifier
   └── Update project.pbxproj

6. Done! ✅
```

### Key Files:

- `main.py`: Entry point and CLI
- `core/replacer.py`: Main replacement logic
- `core/analyzer.py`: RNBO export analysis
- `utils/logger.py`: Logging utilities

## 🆚 Comparison: Android vs iOS

| Feature | Android (Java) | iOS (Swift) |
|---------|---------------|-------------|
| **Lines of code** | ~2700 | ~990 (63% less!) |
| **UI System** | Custom XML layouts | SwiftUI declarative |
| **Modes** | Live/All/Setup | Live/All/Setup |
| **Persistence** | SharedPreferences | UserDefaults |
| **Build time** | ~2 min | ~30 sec |
| **Platforms** | Android only | iOS, macOS, tvOS |

## 🔮 Future Enhancements

- [ ] Support for additional widgets (Knob, Toggle, Picker)
- [ ] Theme system (Dark/Light modes)
- [ ] Parameter grouping/categorization
- [ ] Preset management UI
- [ ] MIDI integration
- [ ] Audio buffer support
- [ ] Export to App Store workflow

## 📄 License

Same as parent project.

## 🤝 Contributing

Contributions welcome! Please test on both iOS and macOS before submitting.

## 📞 Support

For issues, please provide:
- Python version: `python --version`
- macOS version: `sw_vers`
- Xcode version
- Export structure: `tree -L 2 /path/to/export`
- Error messages/logs

---

**Made with ❤️ for the RNBO community**
