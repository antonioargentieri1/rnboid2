# Quick Start Guide

## 🚀 3 Steps to Your iOS/macOS RNBO App

### Step 1: Export from Max/MSP

In Max/MSP:
- Export → Export C++ Code
- Choose destination folder
- Wait for completion

### Step 2: Run Replacer

```bash
cd rnbo_replacer_ios

python main.py \
  --template ../SwiftRNBO-main \
  --export /path/to/your/rnbo_export \
  --output ./MyApp
```

### Step 3: Open & Run

```bash
cd MyApp/SwiftRNBO_Example_multiplatfrom_SwiftUI
open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
```

In Xcode:
1. Select device (iPhone Simulator or My Mac)
2. Press **Cmd+R** ▶️
3. Done! 🎉

---

## First Time Setup (Xcode Signing)

In Xcode:
1. Click project (blue icon) in Navigator
2. Select target → "Signing & Capabilities" tab
3. Check ✅ "Automatically manage signing"
4. Select Team → Add Apple ID if needed

---

## Test AutoUI Modes

**All Mode** (default):
- See all parameters
- Full control

**Setup Mode**:
- Check parameters you want
- Click "Select All" or individual checkboxes

**Live Mode**:
- See only selected parameters
- Streamlined performance UI

Configuration persists between app launches!

---

## Common Issues

**"Template not found"**
→ Check path to SwiftRNBO-main: `ls ../SwiftRNBO-main`

**"Export not found"**
→ Check description.json exists: `ls /path/to/export/description.json`

**Xcode signing error**
→ Add Apple ID in Xcode → Preferences → Accounts

---

**Need more details?** See [README.md](README.md)
