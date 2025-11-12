# 🎯 Custom Range - Comandi per MacInCloud

## ✅ FIX COMPLETATO!

**Problema risolto:** Slider ora mappa correttamente sul custom range.

**Esempio:** Range custom 200-2000 Hz, slider a 50% → **1100 Hz** ✅ (NON 10000 Hz!)

---

## 📦 STEP 1: Aggiorna Repository

```bash
cd ~/Desktop/rnboid2
git pull origin claude/swiftrnbo-autoui-setup-011CV2oURwyLUPLc1aKZaEfc
```

**Output atteso:**
```
From ...
 * branch            claude/swiftrnbo-autoui-setup-011CV2oURwyLUPLc1aKZaEfc -> FETCH_HEAD
Updating 699d966..a464d3e
Fast-forward
 AutoUI/Widgets/AutoSlider.swift | 42 ++++++++++++---
 CUSTOM_RANGE_COMMANDS.md        | 31 +++++++----
 2 files changed, 57 insertions(+), 16 deletions(-)
```

---

## 🔧 STEP 2: Rigenera Progetto con Python Replacer

```bash
cd rnbo_replacer_ios

python3 main.py \
  --template ../SwiftRNBO-main \
  --export ../SwiftRNBO-main/SwiftRNBO_Example_multiplatfrom_SwiftUI/SwiftRNBO_Example_multiplatfrom_SwiftUI/RNBO/Export \
  --output ~/Desktop/CustomRangeFixed
```

**Output atteso:**
```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 RNBO iOS/macOS REPLACER
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

[VERIFYING PATHS]
✓ Template found: ../SwiftRNBO-main
✓ Template project structure found
  ✓ Core/ModeManager.swift
  ✓ Core/ParameterFilter.swift
  ✓ Core/AutoUIView.swift
  ✓ Widgets/AutoSlider.swift     ← ✅ FIXED!
  ✓ Widgets/ParameterRow.swift
  ✓ Widgets/RangeSlider.swift
✓ Export found: ...

[COPYING TEMPLATE]
✓ Template copied to: /Users/user288431/Desktop/CustomRangeFixed

[REPLACING RNBO EXPORT]
✓ Old export removed
✓ New export copied

[ANALYZING RNBO EXPORT]
✓ Found 7 parameters

[SUCCESS] ✅
Project generated: /Users/user288431/Desktop/CustomRangeFixed
```

---

## 📂 STEP 3: Apri in Xcode

```bash
cd ~/Desktop/CustomRangeFixed/SwiftRNBO_Example_multiplatfrom_SwiftUI
open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
```

---

## ⚙️ STEP 4: Setup Xcode (Prima volta)

### 4.1 Aggiungi RangeSlider.swift al Target (se non già fatto)

1. Navigator (sinistra) → `AutoUI/Widgets/RangeSlider.swift`
2. Click sul file
3. Inspector (destra) → **Target Membership**
4. ✅ Spunta `SwiftRNBO_Example_multiplatfrom_SwiftUI`

### 4.2 Configura Signing

1. Click progetto (icona blu)
2. Target → Tab **Signing & Capabilities**
3. ✅ **Automatically manage signing**
4. Seleziona **Team** (il tuo Apple ID)

### 4.3 Build & Run

Premi **⌘ Cmd + R** per buildare e eseguire ▶️

---

## 🧪 STEP 5: Test del Custom Range MAPPING

### Test 1: Imposta Custom Range

1. App si apre in modalità **All**
2. Click **Setup** (in alto)
3. Seleziona parametro **frequency_1** (checkbox)
4. Scroll giù → Trova **"Custom Range"** toggle
5. Attiva toggle: ☐ → ✅
6. Appare dual-thumb slider
7. **Drag left thumb** → Imposta a **200**
8. **Drag right thumb** → Imposta a **2000**
9. Verifica label mostra: **[200.00 - 2000.00]** ✅

### Test 2: ⭐ VERIFICA MAPPING (CRITICO!)

1. Click **All** (in alto)
2. Trova **frequency_1** con custom range attivo
3. **Labels dovrebbero mostrare:**
   - Min: `200.00 Hz`
   - Max: `2000.00 Hz`

4. **🎯 TEST DEL MAPPING:**
   - Muovi slider **tutto a SINISTRA** → Number box: **~200 Hz** ✅
   - Muovi slider **a METÀ** → Number box: **~1100 Hz** ✅ (NON 10000!)
   - Muovi slider **tutto a DESTRA** → Number box: **~2000 Hz** ✅

5. **✅ SE IL NUMERO BOX MOSTRA VALORI CORRETTI = MAPPING FUNZIONA!**

### Test 3: Persistenza

1. Imposta custom range per 2-3 parametri
2. Stop app in Xcode (⏹)
3. Riavvia app (**⌘ Cmd + R**)
4. Click **Setup**
5. Verifica: Custom ranges ancora attivi ✅

### Test 4: Disabilita Custom Range

1. **Setup mode** → Trova parametro con custom range
2. **Toggle OFF**: ✅ → ☐
3. Range slider scompare
4. Click **All**
5. Labels tornano a range originale: `20.00 Hz - 20000.00 Hz` ✅

---

## ✅ Cosa Aspettarsi (CORRETTO!)

**Prima del fix:**
```
Custom Range: 200-2000 Hz
Slider a 50% → 10000 Hz  ❌ SBAGLIATO!
(Mappava ancora sul range originale 20-20000)
```

**Dopo il fix:**
```
Custom Range: 200-2000 Hz
Slider a 50% → 1100 Hz   ✅ CORRETTO!
(Mappa correttamente sul custom range)
```

---

## 🐛 Troubleshooting

### Errore: "Cannot find 'sliderBinding' in scope"

**Causa:** AutoSlider.swift non aggiornato

**Fix:**
```bash
cd ~/Desktop/rnboid2
git pull origin claude/swiftrnbo-autoui-setup-011CV2oURwyLUPLc1aKZaEfc
# Rigenera progetto
```

### Slider ancora mappa sul range originale

**Verifica:**
```bash
cd ~/Desktop/rnboid2/SwiftRNBO-main/SwiftRNBO_Example_multiplatfrom_SwiftUI/SwiftRNBO_Example_multiplatfrom_SwiftUI/AutoUI/Widgets
grep "sliderBinding" AutoSlider.swift
```

**Output atteso:**
```
    private var sliderBinding: Binding<Double> {
            value: sliderBinding,
            value: sliderBinding,
```

Se NON vedi queste righe → Git pull + rigenera

---

## 📊 File Modificato

**AutoSlider.swift** (commit: e9dc763)
```swift
/// Custom binding that maps slider (0-1) to effective range (custom or original)
private var sliderBinding: Binding<Double> {
    Binding(
        get: {
            let currentValue = parameter.value
            let range = effectiveRange

            // Normalize to effective range: (value - min) / (max - min)
            let normalized = (currentValue - range.min) / (range.max - range.min)
            return max(0, min(1, normalized))
        },
        set: { sliderPosition in
            // Map slider position (0-1) to effective range
            let range = effectiveRange
            let mappedValue = range.min + sliderPosition * (range.max - range.min)

            // Set to RNBO
            rnbo.setParameterValue(to: mappedValue, at: parameter.info.index)
            parameter.value = mappedValue
        }
    )
}
```

**Cosa fa:**
- **GET:** Valore RNBO → Normalizza su custom range → 0-1 per slider
- **SET:** Slider 0-1 → Mappa su custom range → Imposta valore RNBO

**Esattamente come Android:** `MainActivity.java` SliderListener.onProgressChanged()

---

## 🎉 Success!

Se il test 2 (mapping) funziona correttamente, il Custom Range è implementato perfettamente!

**Comportamento atteso:**
- ✅ Toggle abilita/disabilita custom range
- ✅ Dual-thumb slider imposta min/max
- ✅ **Slider mappa correttamente su custom range (critico!)**
- ✅ Persistenza con UserDefaults
- ✅ Funziona in All e Live mode

---

**Domande? Problemi?** Scrivi nel chat!
