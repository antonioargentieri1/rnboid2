# Usage Examples

## Basic Usage

### Example 1: Simple Synth

You exported a simple synth from Max/MSP with 5 parameters:
- Frequency
- Cutoff
- Resonance
- Volume
- Attack

```bash
python main.py \
  --template ../SwiftRNBO-main \
  --export ~/Documents/Max/exports/simple_synth \
  --output ~/Desktop/SimpleSynth
```

**Result:**
- Xcode project at `~/Desktop/SimpleSynth/`
- 5 sliders auto-generated
- 3 modes (Live/All/Setup) working
- Ready to build & run

---

### Example 2: Complex Effect with Custom Name

You exported a complex reverb with 20+ parameters:

```bash
python main.py \
  --template ../SwiftRNBO-main \
  --export ~/Music/RNBO/CloudReverb \
  --output ~/Projects/CloudReverbApp \
  --name "Cloud Reverb"
```

**Result:**
- App named "Cloud Reverb"
- Bundle ID: `com.rnbo.cloudreverb`
- All 20+ parameters visible in "All" mode
- Use "Setup" mode to select your favorites
- Switch to "Live" mode for streamlined UI

---

### Example 3: With Logging

Track the entire process with a log file:

```bash
python main.py \
  --template ../SwiftRNBO-main \
  --export ~/Desktop/my_patch_export \
  --output ~/Desktop/MyPatchApp \
  --log ~/Desktop/replacer.log
```

Check the log:
```bash
cat ~/Desktop/replacer.log
```

---

## Typical Workflow

### For Sound Designers:

```bash
# 1. Export from Max/MSP
# (Menu: Export → Export C++ Code)

# 2. Generate app
cd ~/Development/rnboid2/rnbo_replacer_ios
python main.py \
  --template ../SwiftRNBO-main \
  --export ~/Desktop/my_latest_export \
  --output ~/Desktop/MyApp

# 3. Open & test
cd ~/Desktop/MyApp/SwiftRNBO_Example_multiplatfrom_SwiftUI
open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj

# In Xcode: Cmd+R
```

### For Developers:

```bash
# Clone the repo
git clone https://github.com/yourusername/rnboid2.git
cd rnboid2

# Get RNBO export from client
scp user@server:/path/to/export ./client_patch_export

# Generate project
cd rnbo_replacer_ios
python main.py \
  --template ../SwiftRNBO-main \
  --export ../client_patch_export \
  --output ../ClientApp \
  --name "Client App v1.0"

# Test
cd ../ClientApp/SwiftRNBO_Example_multiplatfrom_SwiftUI
open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
```

---

## Advanced: Batch Processing

Create multiple apps from multiple exports:

```bash
#!/bin/bash
# batch_generate.sh

EXPORTS_DIR=~/Music/RNBO/exports
OUTPUT_DIR=~/Desktop/RNBOApps
TEMPLATE=../SwiftRNBO-main

for export in $EXPORTS_DIR/*; do
    name=$(basename "$export")
    echo "Generating $name..."

    python main.py \
        --template $TEMPLATE \
        --export "$export" \
        --output "$OUTPUT_DIR/$name" \
        --name "$name"
done

echo "✅ All apps generated in $OUTPUT_DIR"
```

Run:
```bash
chmod +x batch_generate.sh
./batch_generate.sh
```

---

## Testing Different Patches

### Minimal Patch (1 parameter)

Good for testing:
```bash
python main.py \
  --template ../SwiftRNBO-main \
  --export ./test_exports/minimal \
  --output ./test_output/minimal_test
```

### Maximum Patch (50+ parameters)

Stress test:
```bash
python main.py \
  --template ../SwiftRNBO-main \
  --export ./test_exports/maximal \
  --output ./test_output/maximal_test
```

---

## Common Scenarios

### Scenario 1: Quick Prototype

You're at a hackathon and need a working app FAST:

```bash
# 1 minute to export
# 10 seconds to run replacer
# 30 seconds to build in Xcode
# = Working app in < 2 minutes! 🚀

python main.py --template ../SwiftRNBO-main --export ./patch --output ./App && \
cd App/SwiftRNBO_Example_multiplatfrom_SwiftUI && \
open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
```

### Scenario 2: Client Demo

Client wants to see their patch on iPad:

```bash
python main.py \
  --template ../SwiftRNBO-main \
  --export ./client_patch \
  --output ./ClientDemo \
  --name "Client Demo - v1.2"

# Open Xcode, select iPad target, build to device
# Hand iPad to client ✅
```

### Scenario 3: A/B Testing

Compare two versions of same patch:

```bash
python main.py --template ../SwiftRNBO-main --export ./patch_v1 --output ./TestV1
python main.py --template ../SwiftRNBO-main --export ./patch_v2 --output ./TestV2

# Open both in Xcode, run side-by-side
```

---

## Tips & Tricks

### 1. Keep Template Clean

Always work from the main SwiftRNBO-main template:
```bash
git clone https://github.com/.../SwiftRNBO-main.git
# Never modify this directly
# Always generate into separate output folders
```

### 2. Unique Output Names

Use descriptive output names:
```bash
# Good ✅
--output ./MyPatch_v1.2_2024-01-15

# Bad ❌
--output ./output
```

### 3. Log Everything

Always use `--log` for debugging:
```bash
python main.py ... --log ./logs/$(date +%Y%m%d_%H%M%S).log
```

### 4. Test on Device

After generating:
1. Connect iPhone/iPad
2. Select device in Xcode
3. Build to device (may need Apple Developer account)
4. Test touch interaction, real audio latency

---

## Troubleshooting Examples

### Export Missing Files

```bash
$ python main.py --template ../SwiftRNBO-main --export ./bad_export --output ./Test

[ERROR] Export analysis failed!
[ERROR] description.json not found!

# Solution: Re-export from Max/MSP with all files
```

### Template Not Found

```bash
$ python main.py --template ./wrong_path --export ./export --output ./Test

[ERROR] Template not found: ./wrong_path

# Solution: Use correct path to SwiftRNBO-main
$ python main.py --template ../SwiftRNBO-main --export ./export --output ./Test
```

### Output Exists

```bash
$ python main.py --template ../SwiftRNBO-main --export ./export --output ./ExistingApp

[WARNING] Output directory already exists: ./ExistingApp
  Overwrite? (y/n): y

# Replacer will remove old and create new
```

---

**Ready to create your app? Start with the [Quick Start Guide](QUICKSTART.md)!**
