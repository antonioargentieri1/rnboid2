# Presets System - Comandi per MacInCloud

## ✅ Implementazione Completata!

Sistema Presets completo, identico ad Android RNBOID2:
- ✅ Save/Load presets
- ✅ 8 Quick Presets (bottoni in LIVE mode)
- ✅ Morphing/Interpolation (transizioni smooth)
- ✅ PRESETS mode UI
- ✅ Persistence con UserDefaults

**Commit:** 73882c5 (+938 lines, 4 nuovi files)

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
  --output ~/Desktop/PresetsTest
```

**Output atteso:**
```
✓ Core/Preset.swift
✓ Core/PresetManager.swift
✓ Views/PresetsManagerView.swift
✓ Views/QuickPresetsView.swift
```

---

## 📂 STEP 3: Apri in Xcode

```bash
cd ~/Desktop/PresetsTest
open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
```

---

## ⚙️ STEP 4: Setup Xcode

### 4.1 Aggiungi NUOVI Files al Target

**NUOVI files da aggiungere:**
- ✨ `AutoUI/Core/Preset.swift`
- ✨ `AutoUI/Core/PresetManager.swift`
- ✨ `AutoUI/Views/PresetsManagerView.swift`
- ✨ `AutoUI/Views/QuickPresetsView.swift`

**Files già aggiunti (dalla volta scorsa):**
- ✅ AutoUI/Core/ModeManager.swift
- ✅ AutoUI/Core/ParameterFilter.swift
- ✅ AutoUI/Core/AutoUIView.swift
- ✅ AutoUI/Widgets/AutoSlider.swift
- ✅ AutoUI/Widgets/ParameterRow.swift
- ✅ AutoUI/Widgets/RangeSlider.swift

**Come aggiungere:**
1. Navigator (sinistra) → Trova file in `AutoUI/Core` o `AutoUI/Views`
2. Click sul file
3. Inspector (destra) → **Target Membership**
4. ✅ Spunta `SwiftRNBO_Example_multiplatfrom_SwiftUI`

**Ripeti per tutti e 4 i nuovi files!**

### 4.2 Configura Signing (se necessario)

1. Click progetto → Target → **Signing & Capabilities**
2. ✅ **Automatically manage signing**
3. Seleziona **Team**

### 4.3 Build & Run

Premi **⌘ Cmd + R** ▶️

---

## 🧪 TESTING DEL PRESETS SYSTEM

### Test 1: Interface - Verifica 4 Mode Buttons

1. **App si apre** → Dovrebbe essere in modalità "All"
2. **In alto vedi 4 bottoni:**
   ```
   [Live] [All] [Presets] [Setup]
   ```
3. ✅ Se vedi **4 bottoni** (incluso Presets), l'aggiornamento è riuscito!

### Test 2: PRESETS Mode - Save Current Configuration

1. **Click "All"** → Muovi alcuni sliders (es: frequency_1, cutoff)
2. **Click "Presets"** in alto
3. **Dovresti vedere:**
   ```
   PRESETS MANAGER

   ┌─────────────────────────────────────────┐
   │ SAVE CURRENT CONFIGURATION              │ ← Bottone bianco
   └─────────────────────────────────────────┘

   MORPHING / INTERPOLATION
   ☐ Enable morphing between presets

   ───────────────────────────────────────────

   No Presets Saved
   Save your first preset to get started
   ```

4. **Click "SAVE CURRENT CONFIGURATION"**
5. **Dialog appare** → Inserisci nome: "Bass Heavy"
6. **Click "Save"**
7. **Dovresti vedere una card:**
   ```
   ┌─────────────────────────────────────────┐
   │ Bass Heavy                              │
   │ Nov 12, 2025 16:30                     │
   │                                         │
   │ [LOAD] [RENAME] [DELETE]           [Q] │
   └─────────────────────────────────────────┘
   ```

### Test 3: Load Preset (Instant)

1. **Click "All"** → Muovi sliders a valori diversi
2. **Click "Presets"** → Trova preset "Bass Heavy"
3. **Click "LOAD"**
4. **App switcha a "Live" mode** (come Android!)
5. **Parametri tornano ai valori salvati** ✅

### Test 4: Quick Presets (LIVE Mode Header)

1. **Click "Presets"** → Trova preset "Bass Heavy"
2. **Click "Q" (Quick button)** → Diventa verde ✅
3. **Salva altri 2 presets:**
   - "Bright Pad"
   - "Deep Sub"
4. **Imposta entrambi come Quick** (click Q su ciascuno)
5. **Click "Live"** in alto
6. **Dovresti vedere header con bottoni:**
   ```
   QUICK PRESETS
   ┌──────┬──────┬──────┬──────┐
   │1.Bass│2.Brig│3.Deep│      │
   │ Heavy│ht Pad│  Sub │      │
   └──────┴──────┴──────┴──────┘
   ```
7. **Click "1. Bass Heavy"** → Parametri cambiano istantaneamente!

### Test 5: Morphing/Interpolation

1. **Click "Presets"** in alto
2. **Abilita morphing:**
   - ✅ Toggle "Enable morphing between presets"
   - **Slider appare**: "Morph Time: 1000ms"
3. **Muovi slider** → Imposta a 2000ms (2 secondi)
4. **Click "All"** → Muovi parametri a valori estremi
5. **Click "Live"** → Click "1. Bass Heavy"
6. **Dovresti vedere:**
   - Parametri si muovono **smooth** (NON istantaneo!)
   - Progress bar appare: "Morphing... 50%"
   - Transizione dura ~2 secondi
   - ✅ Smooth interpolation funziona!

### Test 6: Morph Speed Test

1. **Click "Presets"** → Imposta morph time a **100ms** (veloce)
2. **Click "Live"** → Switch tra quick presets
3. **Transizioni dovrebbero essere rapide** (100ms)
4. **Torna a Presets** → Imposta **5000ms** (5 secondi)
5. **Torna a Live** → Switch preset
6. **Transizione dovrebbe durare 5 secondi lenti** ✅

### Test 7: Rename Preset

1. **Click "Presets"**
2. **Trova preset "Bass Heavy"**
3. **Click "RENAME"** → Dialog appare
4. **Inserisci:** "Fat Bass"
5. **Click "Rename"**
6. **Nome aggiornato nella card** ✅
7. **Se è Quick Preset, anche bottone in Live si aggiorna!**

### Test 8: Delete Preset

1. **Click "Presets"**
2. **Salva un preset temporaneo:** "TestDelete"
3. **Click "DELETE"**
4. **Preset scompare dalla lista** ✅

### Test 9: Persistence (IMPORTANTE!)

1. **Salva 3 presets** con nomi unici
2. **Imposta 2 come Quick** (Q verde)
3. **Abilita morphing** → Imposta 1500ms
4. **CHIUDI APP** (Stop in Xcode)
5. **RIAPRI APP** (Cmd+R)
6. **Click "Presets"** → Verifica:
   - ✅ I 3 presets sono ancora lì
   - ✅ Quick flags mantenuti (Q verdi)
   - ✅ Morphing ancora abilitato con 1500ms
7. **Click "Live"** → Verifica:
   - ✅ Quick presets appaiono nell'header
   - ✅ Funzionano correttamente
8. **✅ Persistence funziona perfettamente!**

### Test 10: Max 8 Quick Presets

1. **Click "Presets"**
2. **Crea 10 presets** (Preset1, Preset2, ... Preset10)
3. **Imposta primi 8 come Quick** → ✅ Verde
4. **Prova a impostare il 9° come Quick**
5. **Il bottone Q NON dovrebbe cambiare** (max 8 raggiunto)
6. **Click "Live"** → Verifica:
   - Solo 8 bottoni appaiono (2 righe x 4 colonne)
   - Formato: "1. Preset1", "2. Preset2", etc.

---

## 🎯 Cosa Aspettarsi (UI Layout)

### PRESETS Mode:
```
┌─────────────────────────────────────────────┐
│ [Live] [All] [PRESETS] [Setup]             │
├─────────────────────────────────────────────┤
│ PRESETS MANAGER                             │
│                                             │
│ ┌─────────────────────────────────────────┐ │
│ │ SAVE CURRENT CONFIGURATION              │ │
│ └─────────────────────────────────────────┘ │
│                                             │
│ MORPHING / INTERPOLATION                    │
│ ✅ Enable morphing between presets          │
│ Morph Time: 1000ms                         │
│ ├──────●────────────────────────┤          │
│ 100ms                        5000ms         │
│                                             │
│ ─────────────────────────────────────────── │
│                                             │
│ ┌─────────────────────────────────────────┐ │
│ │ Bass Heavy                              │ │
│ │ Nov 12, 2025 16:30                     │ │
│ │ [LOAD] [RENAME] [DELETE]           [Q] │ │ ← Q verde se quick
│ └─────────────────────────────────────────┘ │
│                                             │
│ ┌─────────────────────────────────────────┐ │
│ │ Bright Pad                              │ │
│ │ Nov 12, 2025 16:35                     │ │
│ │ [LOAD] [RENAME] [DELETE]           [Q] │ │
│ └─────────────────────────────────────────┘ │
└─────────────────────────────────────────────┘
```

### LIVE Mode (con Quick Presets):
```
┌─────────────────────────────────────────────┐
│ [LIVE] [All] [Presets] [Setup]             │
├─────────────────────────────────────────────┤
│ QUICK PRESETS                               │
│ ┌─────┬─────┬─────┬─────┐                  │
│ │1.Bas│2.Bri│3.Dee│4.Hig│                  │
│ │s Hea│ght P│p Sub│h Pas│                  │
│ └─────┴─────┴─────┴─────┘                  │
│ ┌─────┬─────┬─────┬─────┐                  │
│ │5.Met│6.War│7.Coo│8.Cry│                  │
│ │allic│m Pad│l Amb│stal │                  │
│ └─────┴─────┴─────┴─────┘                  │
│                                             │
│ Morphing... 75%                            │
│ ████████████████████░░░░░                   │
│                                             │
├─────────────────────────────────────────────┤
│ Frequency 1                                 │
│ [━━━━━━●━━━━━━] 440.00 Hz                  │
│ ...                                         │
└─────────────────────────────────────────────┘
```

---

## 📊 Files Implementati

| File | Lines | Description |
|------|-------|-------------|
| **Core/Preset.swift** | 68 | Preset model (Codable) |
| **Core/PresetManager.swift** | 320 | Save/Load + Interpolation engine |
| **Views/PresetsManagerView.swift** | 271 | PRESETS mode UI |
| **Views/QuickPresetsView.swift** | 154 | LIVE mode quick buttons |
| **Core/ModeManager.swift** | +7 | Added .presets case |
| **Core/AutoUIView.swift** | +15 | Integrated presets UI |
| **replacer.py** | +4 | Added files verification |
| **TOTAL** | **+938** | **New functionality** |

---

## 🐛 Troubleshooting

### Errore: "Cannot find 'PresetManager' in scope"

**Causa:** Files non aggiunti al target

**Fix:**
1. In Xcode, trova i 4 nuovi files
2. Click su ciascuno → Inspector → Target Membership
3. ✅ Spunta il target per tutti

### Errore: "Cannot find 'PresetsManagerView' in scope"

**Causa:** File Views/ non aggiunti

**Fix:** Come sopra, aggiungi tutti i files in AutoUI/Views/

### PRESETS button non appare

**Causa:** ModeManager.swift non aggiornato

**Fix:**
```bash
cd ~/Desktop/rnboid2
git pull origin claude/swiftrnbo-autoui-setup-011CV2oURwyLUPLc1aKZaEfc
# Rigenera progetto
```

### Quick Presets non appaiono in LIVE mode

**Causa:** QuickPresetsView.swift non aggiunto al target

**Fix:** Aggiungi file al target come descritto sopra

### Morphing non funziona

**Verifica:**
1. ✅ Morphing abilitato in PRESETS mode?
2. ✅ Morph time > 100ms?
3. Se sì, dovrebbe funzionare

---

## ✅ Success Criteria

Presets System implementato correttamente se:

- ✅ **4 mode buttons** appaiono: Live, All, Presets, Setup
- ✅ **PRESETS mode** mostra manager UI
- ✅ **Save/Load** funziona correttamente
- ✅ **Quick Presets** (max 8) appaiono in LIVE mode
- ✅ **Q button** toggle funziona
- ✅ **Morphing** con smooth interpolation
- ✅ **Progress bar** appare durante morphing
- ✅ **Morph time slider** (100-5000ms) funziona
- ✅ **Rename/Delete** funzionano
- ✅ **Persistence** (chiudi/riapri app) ✅
- ✅ **Nessun crash** o errore di compilazione

---

## 🚀 Logica Android Replicata

| Feature | Android Lines | iOS Implementation |
|---------|--------------|-------------------|
| Preset Model | 109-129 | Preset.swift |
| Save/Load | 1554-1658 | PresetManager.saveCurrentAsPreset/loadPreset |
| Quick Toggle | 2067-2095 | PresetManager.toggleQuickPreset |
| Interpolation | 1749-1832 | PresetManager.startInterpolation |
| Easing | 1875-1877 | PresetManager.easeInOutQuad |
| Manager UI | 1370-1434 | PresetsManagerView.swift |
| Quick UI | 2271-2331 | QuickPresetsView.swift |

---

## 🎉 Next Steps (Opzionali)

Dopo aver testato Presets, possiamo implementare:

1. **Sensors Integration** (~12-15 ore)
   - CoreMotion (accelerometer, gyroscope)
   - Map sensors → parameters

2. **XY Pad** (~8-10 ore)
   - 2D touch control
   - Dual parameter mapping

3. **MIDI CC Mapping** (~10 ore)
   - Map MIDI controllers to parameters

---

**Buon testing!** 🎉
