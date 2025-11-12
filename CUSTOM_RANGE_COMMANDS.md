# Custom Range System - Comandi per Aggiornare e Testare

## ✅ Implementazione Completata!

Il sistema Custom Range è stato implementato con successo, simile a quello Android RNBOID2.

---

## 📦 Su MacInCloud - Comandi per Aggiornare

### 1. Aggiorna il repository

```bash
cd ~/Desktop/rnboid2
git pull origin claude/swiftrnbo-autoui-setup-011CV2oURwyLUPLc1aKZaEfc
```

### 2. Rigenera progetto con Python Replacer

```bash
cd rnbo_replacer_ios

python3 main.py \
  --template ../SwiftRNBO-main \
  --export ../SwiftRNBO-main/SwiftRNBO_Example_multiplatfrom_SwiftUI/SwiftRNBO_Example_multiplatfrom_SwiftUI/RNBO/Export \
  --output ~/Desktop/TestCustomRange
```

### 3. Apri in Xcode

```bash
cd ~/Desktop/TestCustomRange/SwiftRNBO_Example_multiplatfrom_SwiftUI
open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
```

---

## 🔧 In Xcode - Setup (Prima volta)

### 1. Aggiungi NUOVO file al target

**Nuovo file da aggiungere:**
- `AutoUI/Widgets/RangeSlider.swift` ✨ (NUOVO!)

**File già aggiunti (dalla volta scorsa):**
- AutoUI/Core/ModeManager.swift ✅
- AutoUI/Core/ParameterFilter.swift ✅
- AutoUI/Core/AutoUIView.swift ✅
- AutoUI/Widgets/AutoSlider.swift ✅
- AutoUI/Widgets/ParameterRow.swift ✅

**Come aggiungere:**
1. Nel Navigator a sinistra, trova `AutoUI/Widgets/RangeSlider.swift`
2. Click sul file
3. Inspector a destra → Sezione "Target Membership"
4. ✅ Spunta `SwiftRNBO_Example_multiplatfrom_SwiftUI`

### 2. Configura signing (se necessario)

1. Click progetto (icona blu) → Target
2. Tab "Signing & Capabilities"
3. ✅ "Automatically manage signing"
4. Seleziona Team (Apple ID)

### 3. Build & Run

Premi **Cmd+R** ▶️

---

## 🧪 Testing del Custom Range

### Test 1: Setup Mode - Abilitare Custom Range

1. **Apri app** → Dovrebbe aprirsi in modalità "All"
2. **Click "Setup"** in alto
3. **Seleziona un parametro** (es: frequency_1)
4. **Scroll giù** sotto lo slider
5. **Dovresti vedere:**
   ```
   ┌─────────────────────────────────┐
   │ ☐ Custom Range                  │
   └─────────────────────────────────┘
   ```

6. **Attiva il toggle** ☐ → ✅
7. **Dovrebbe apparire:**
   ```
   ┌─────────────────────────────────┐
   │ ✅ Custom Range                 │
   │                                 │
   │ [20.00 - 20000.00]             │  ← Range attuale
   │ ├──────●━━━━━━━━●──────┤        │  ← Dual-thumb slider
   │                                 │
   │ Original: [20.00 - 20000.00]   │  ← Riferimento
   └─────────────────────────────────┘
   ```

### Test 2: Modificare il Range

1. **Drag thumb sinistro** (min) → Imposta a 200
2. **Drag thumb destro** (max) → Imposta a 2000
3. **Dovresti vedere aggiornamento in tempo reale:**
   ```
   [200.00 - 2000.00]
   ```

### Test 3: Usare Custom Range in All Mode

1. **Click "All"** in alto
2. **Trova il parametro con custom range**
3. **Labels min/max dovrebbero mostrare:**
   - Min label: `200.00 Hz` (invece di 20.00)
   - Max label: `2000.00 Hz` (invece di 20000.00)
4. **Muovi slider** → Controlla solo range 200-2000!

### Test 4: Persistenza

1. **Imposta custom range** per 2-3 parametri
2. **Chiudi app** (stop in Xcode)
3. **Riapri app** (Cmd+R)
4. **Click "Setup"**
5. **Verifica:** Custom ranges ancora attivi ✅

### Test 5: Disabilitare Custom Range

1. **Setup mode** → Trova parametro con custom range attivo
2. **Toggle OFF** ✅ → ☐
3. **Range slider scompare**
4. **Click "All"**
5. **Verifica:** Labels tornano a range originale (20-20000)

---

## 🎯 Cosa Aspettarsi

### Visual Design:

```
┌──────────────────────────────────────────────────┐
│ SETUP MODE                                       │
├──────────────────────────────────────────────────┤
│                                                  │
│ ☑  Frequency 1                                  │
│    [━━━━━━━━●━━━━━━━━━] 440.00 Hz              │
│    20.00 Hz                    20000.00 Hz       │
│                                                  │
│    ┌──────────────────────────────────────────┐ │
│    │ ✅ Custom Range                          │ │
│    │                                          │ │
│    │ [200.00 - 2000.00]                      │ │
│    │ ├─────●━━━━━━━━━━━━━━●─────┤            │ │
│    │                                          │ │
│    │ Original: [20.00 - 20000.00]            │ │
│    └──────────────────────────────────────────┘ │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Funzionalità:

- ✅ **Toggle** per abilitare/disabilitare custom range
- ✅ **Dual-thumb slider** per impostare min e max
- ✅ **Live update** del range label durante drag
- ✅ **Range originale** sempre visibile per riferimento
- ✅ **Persistenza** con UserDefaults (JSON encoding)
- ✅ **Slider normale** usa range custom quando abilitato
- ✅ **Smooth UX** con animazioni

---

## 🐛 Troubleshooting

### Errore: "RangeSlider not found"

**Causa:** File non aggiunto al target

**Fix:**
1. In Xcode, click su `AutoUI/Widgets/RangeSlider.swift`
2. Inspector → Target Membership
3. ✅ Spunta il target

### Errore: "Cannot find 'getEffectiveRange' in scope"

**Causa:** ModeManager non aggiornato

**Fix:**
```bash
cd ~/Desktop/rnboid2
git pull origin claude/swiftrnbo-autoui-setup-011CV2oURwyLUPLc1aKZaEfc
# Rigenera progetto
```

### Custom Range Section non appare

**Causa:** File ParameterRow.swift non aggiornato

**Fix:** Stesso del punto precedente (git pull + rigenera)

### Slider non rispetta custom range

**Causa:** AutoSlider.swift non aggiornato

**Fix:** Git pull + rigenera progetto

---

## 📊 Files Modificati (Summary)

| File | Changes | Lines Added |
|------|---------|-------------|
| ModeManager.swift | Added CustomRange model + methods | +110 |
| RangeSlider.swift | NEW - Dual-thumb slider | +185 |
| AutoSlider.swift | Use effective ranges | +5 |
| ParameterRow.swift | Added custom range UI | +69 |
| **TOTAL** | | **+369 lines** |

---

## 🚀 Next Steps (Opzionale)

Dopo aver testato Custom Range, possiamo implementare:

1. **Presets System** (~15-20 ore)
   - Save/load parameter states
   - Quick presets (8 slots)
   - Smooth interpolation

2. **Sensors Integration** (~12-15 ore)
   - Map accelerometer to parameters
   - Tilt to control

3. **XY Pad** (~8-10 ore)
   - 2D touch control
   - Dual parameter mapping

---

## ✅ Success Criteria

Custom Range è implementato correttamente se:

- ✅ Toggle appare in Setup mode
- ✅ Range slider con 2 thumbs funziona
- ✅ Range label si aggiorna live
- ✅ Slider in All mode usa custom range
- ✅ Persistenza funziona (chiudi/riapri app)
- ✅ Disabling toggle ripristina range originale
- ✅ Nessun crash o errore di compilazione

---

**Buon testing!** 🎉
