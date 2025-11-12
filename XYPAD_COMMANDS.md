# XY Pad System - Comandi per MacInCloud

## ✅ Sistema XY Pad Completo!

**Commit:** ee00af8 (pronto per testing)

---

## 📦 STEP 1: Aggiorna Repository

```bash
cd ~/Desktop/rnboid2
git pull origin claude/swiftrnbo-autoui-setup-011CV2oURwyLUPLc1aKZaEfc
```

---

## 🔧 STEP 2: Rigenera Progetto

```bash
cd rnbo_replacer_ios

python3 main.py \
  --template ../SwiftRNBO-main \
  --export ../SwiftRNBO-main/SwiftRNBO_Example_multiplatfrom_SwiftUI/SwiftRNBO_Example_multiplatfrom_SwiftUI/RNBO/Export \
  --output ~/Desktop/XYPadTest
```

**Output atteso:**
```
✓ Core/XYPadMapping.swift
✓ Core/XYPadManager.swift
✓ Views/XYPadView.swift
```

---

## 📂 STEP 3: Apri in Xcode

```bash
cd ~/Desktop/XYPadTest
open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
```

---

## ⚙️ STEP 4: Aggiungi Files al Target

**NUOVI files da aggiungere (3 files):**
1. ✨ `AutoUI/Core/XYPadMapping.swift`
2. ✨ `AutoUI/Core/XYPadManager.swift`
3. ✨ `AutoUI/Views/XYPadView.swift`

**Come aggiungere:**
1. Navigator (sinistra) → Trova file in `AutoUI/Core` o `AutoUI/Views`
2. Click sul file
3. Inspector (destra) → **Target Membership**
4. ✅ Spunta `SwiftRNBO_Example_multiplatfrom_SwiftUI`

**Ripeti per tutti e 3 i nuovi files!**

---

## 🏗️ STEP 5: Build & Run

Premi **⌘ Cmd + R** ▶️

**Dovrebbe compilare senza errori!** ✅

---

## 🧪 TESTING XY PAD

### Test 1: Verifica Toggle Button

1. **App si apre** → Modalità "All"
2. **Click "Live"** in alto
3. **Vedi header con bottone:**
   ```
   ┌─────────────────────────────┐
   │     XY PAD OFF              │ ← Bottone nero con testo bianco
   └─────────────────────────────┘
   ```
4. ✅ Se vedi il bottone, l'integrazione è riuscita!

### Test 2: Attiva XY Pad

1. **Click "XY PAD OFF"**
2. **Bottone diventa:**
   ```
   ┌─────────────────────────────┐
   │     XY PAD ON               │ ← Bottone bianco con testo nero
   └─────────────────────────────┘
   ```
3. **Vista cambia** → Sliders scompaiono
4. **Appare XY Pad:**
   ```
   ┌─────────────────────────────┐
   │  X: 0.50  Y: 0.50          │
   │                             │
   │       ╋                     │ ← Grid 4x4
   │                             │
   │         ●                   │ ← Cursore (cerchio vuoto)
   │                             │
   │   Y                         │
   │   │                         │
   │   └────── X                 │
   └─────────────────────────────┘
   ```

### Test 3: Touch Control (IMPORTANTE!)

1. **XY Pad è ON**
2. **Touch lo schermo sul pad**
3. **Osserva:**
   - ✅ Cursore si muove dove tocchi
   - ✅ Cursore diventa **pieno** (filled circle)
   - ✅ Valori in alto si aggiornano: "X: 0.23 Y: 0.78"
4. **Rilascia touch**
5. **Osserva:**
   - ✅ Cursore torna **vuoto** (stroke circle)
   - ✅ Valori rimangono aggiornati

### Test 4: Assign Parameters (Setup Mode)

1. **Click "Setup"** in alto
2. **Scroll ad un parametro** (es: frequency_1)
3. **Dopo "Custom Range", vedi:**
   ```
   XY Pad Assignment
   ┌────────────────────────────────┐
   │ [None] [X Axis] [Y Axis] [Pad]│ ← Segmented picker
   └────────────────────────────────┘
   ```
4. **Click "X Axis"**
5. **Appare toggle:**
   ```
   ☐ Invert
   ```
6. **Assign altro parametro** (es: cutoff):
   - Click su cutoff
   - Seleziona "Y Axis"
   - ✅ Abilita "Invert" (per Y è comune)

### Test 5: Use XY Pad with Assigned Parameters

1. **Click "Live"**
2. **Click "XY PAD ON"**
3. **Labels dovrebbero mostrare:**
   ```
   Y: Cutoff (verticale, ruotato)
   X: Frequency 1 (orizzontale, sotto)
   ```
4. **Touch e trascina sul pad**
5. **ASCOLTA L'AUDIO** (se possibile):
   - X (orizzontale) → frequency cambia
   - Y (verticale) → cutoff cambia
6. ✅ Controllo simultaneo di 2 parametri!

### Test 6: Invert Test

1. **Setup mode** → cutoff ha "Invert" ☐
2. **Live mode** → XY Pad ON
3. **Touch in ALTO (Y = 0)**
   - Con invert OFF → cutoff = minimum
   - Con invert ON → cutoff = maximum ✅
4. **Touch in BASSO (Y = 1)**
   - Con invert OFF → cutoff = maximum
   - Con invert ON → cutoff = minimum ✅

### Test 7: Pad Trigger

1. **Setup mode** → Trova parametro "play" o "trigger"
2. **Assign a "Pad trigger"**
3. **Live mode** → XY Pad ON
4. **Touch pad** → Parametro = 1.0 (ON)
5. **Rilascia** → Parametro = 0.0 (OFF)
6. ✅ Funziona come pulsante momentaneo!

### Test 8: Save Preset with XY Pad Mappings

1. **Setup mode:**
   - frequency_1 → X Axis
   - cutoff → Y Axis (Invert ON)
   - play → Pad trigger
2. **Click "Presets"**
3. **SAVE CURRENT CONFIGURATION**
   - Nome: "XY Control"
4. **Preset salvato** ✅
5. **Setup mode** → Cambia assignments:
   - frequency_1 → None
   - cutoff → None
6. **Presets mode** → Carica "XY Control"
7. **Setup mode** → Verifica:
   - ✅ frequency_1 = X Axis
   - ✅ cutoff = Y Axis (Invert ON)
   - ✅ Mappings ripristinati!

### Test 9: XY Pad Toggle OFF

1. **Live mode** → XY Pad ON (con mappings attivi)
2. **Click "XY PAD OFF"**
3. **Vista cambia** → XY Pad scompare
4. **Sliders riappaiono** ✅
5. **Click "XY PAD ON"**
6. **XY Pad riappare** con mappings ancora attivi ✅

---

## ✅ Cosa Dovrebbe Funzionare

1. ✅ **Toggle button** in LIVE mode (ON/OFF)
2. ✅ **XY Pad view** con grid, crosshair, cursor
3. ✅ **Touch handling** (cursor filled/stroke, position update)
4. ✅ **Assignment UI** in Setup mode (axis picker + invert)
5. ✅ **Parameter mapping** (X/Y/Trigger → parameters)
6. ✅ **Multiple params** per axis (tutti si muovono insieme)
7. ✅ **Custom range integration** (rispetta i range custom)
8. ✅ **Invert** funziona correttamente
9. ✅ **Preset persistence** (save/load XY Pad mappings)
10. ✅ **Morphing** preserva XY Pad mappings

---

## 🐛 Troubleshooting

### Errore: "Cannot find 'XYPadManager' in scope"

**Causa:** Files non aggiunti al target

**Fix:**
1. Aggiungi i 3 nuovi files al target (vedi Step 4)
2. Clean Build Folder (**⌘ Cmd + Shift + K**)
3. Rebuild (**⌘ Cmd + R**)

### XY Pad non appare quando attivo

**Verifica:**
1. Console Xcode → Cerca errori
2. AutoUIView.swift → Verifica logica:
   ```swift
   else if modeManager.currentMode == .live && xyPadManager.isEnabled {
       xyPadView
   }
   ```

### Touch non funziona

**Possibili cause:**
- Simulator iOS ha problemi touch complessi
- **Prova su device fisico** se possibile

### Parameters non si muovono

**Verifica in Setup:**
1. Parametri hanno axis assignment diverso da "None"?
2. Console → Cerca errori in `updateParameters()`

---

## 📊 Files Implementati

| File | Lines | Status |
|------|-------|--------|
| **XYPadMapping.swift** | 41 | ✅ NEW |
| **XYPadManager.swift** | 164 | ✅ NEW |
| **XYPadView.swift** | 189 | ✅ NEW |
| Preset.swift | +2 | ✅ Modified |
| PresetManager.swift | +6 | ✅ Modified |
| AutoUIView.swift | +9 | ✅ Modified |
| QuickPresetsView.swift | +20 | ✅ Modified |
| ParameterRow.swift | +37 | ✅ Modified |
| replacer.py | +3 | ✅ Modified |
| **TOTAL** | **+471** | **✅ Complete** |

---

## 🎯 Matching Android

| Feature | Android | iOS | Status |
|---------|---------|-----|--------|
| XY Pad View | XYPadView.java (237 lines) | XYPadView.swift (189 lines) | ✅ |
| Toggle Button | Lines 331-338 | QuickPresetsView | ✅ |
| Touch Handling | Lines 141-188 | DragGesture | ✅ |
| Mapping Logic | Lines 521-571 | XYPadManager | ✅ |
| Assignment UI | Lines 1150-1180 | ParameterRow | ✅ |
| Grid 4x4 | ✅ | ✅ | ✅ |
| Crosshair | ✅ | ✅ | ✅ |
| Cursor (filled/stroke) | ✅ | ✅ | ✅ |
| Labels (X/Y) | ✅ | ✅ | ✅ |
| Position values | ✅ | ✅ | ✅ |
| Invert | ✅ | ✅ | ✅ |
| Custom range support | ✅ | ✅ | ✅ |
| Preset persistence | ✅ | ✅ | ✅ |

---

**Buon testing!** 🎉

Fammi sapere se:
- ✅ Compila correttamente
- ✅ XY Pad appare e funziona
- ❓ Problemi o errori
