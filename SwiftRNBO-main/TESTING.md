# SwiftRNBO AutoUI - Testing Guide

**Fase 2 Completata**: Core AutoUI implementato e pronto per test!

---

## 📦 FILE IMPLEMENTATI

### Core System
- ✅ `AutoUI/Core/ModeManager.swift` - Gestione modalità (Live/All/Setup)
- ✅ `AutoUI/Core/ParameterFilter.swift` - Filtraggio e sorting parametri
- ✅ `AutoUI/Core/AutoUIView.swift` - Vista principale orchestrator

### Widgets
- ✅ `AutoUI/Widgets/AutoSlider.swift` - Slider auto-generato con binding
- ✅ `AutoUI/Widgets/ParameterRow.swift` - Container riga parametro

### Integration
- ✅ `ContentView.swift` - Aggiornato per usare `AutoUIView()`

---

## 🧪 COME TESTARE IN XCODE

### 1. Aprire Progetto
```bash
cd SwiftRNBO-main/SwiftRNBO_Example_multiplatfrom_SwiftUI
open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj
```

### 2. Aggiungere File al Target
In Xcode:
1. Seleziona la cartella `AutoUI/` nel Navigator
2. Per ogni file `.swift`:
   - Apri il file
   - Inspector (right panel) → **Target Membership**
   - ✅ Abilita il checkbox per il target principale

**File da abilitare**:
- `AutoUI/Core/ModeManager.swift`
- `AutoUI/Core/ParameterFilter.swift`
- `AutoUI/Core/AutoUIView.swift`
- `AutoUI/Widgets/AutoSlider.swift`
- `AutoUI/Widgets/ParameterRow.swift`

### 3. Build & Run
```
Cmd+R
```

**Piattaforme supportate**:
- ✅ iOS Simulator
- ✅ macOS
- ✅ tvOS (limitato)

---

## 🎯 COSA ASPETTARSI

### All Mode (Default)
- Mostra **tutti** i parametri visibili
- Slider auto-generati per ogni parametro
- Nomi da `displayName` (o `paramId` se vuoto)
- Valori min/max/current visibili
- Unità di misura mostrate (Hz, dB, etc.)

### Setup Mode
- Mostra tutti i parametri con **checkbox**
- Clicca checkbox per selezionare parametri per Live mode
- Pulsanti "Select All" / "Deselect All"
- Contatore parametri selezionati
- Parametri non selezionati = semi-trasparenti

### Live Mode
- Mostra **solo** parametri selezionati in Setup
- UI compatta per performance
- Empty state se nessun parametro selezionato
  - Pulsante "Go to Setup" per configurare

---

## 🔄 FLUSSO TESTING

### Test 1: All Mode
1. ✅ App si apre in "All" mode (default)
2. ✅ Vedi tutti i parametri RNBO della patch
3. ✅ Muovi slider → parametro RNBO si aggiorna
4. ✅ Valore corrente aggiornato in tempo reale

### Test 2: Setup Mode
1. ✅ Clicca segmented control → "Setup"
2. ✅ Vedi checkbox su ogni parametro
3. ✅ Clicca checkbox → parametro selezionato (blu)
4. ✅ Contatore si aggiorna
5. ✅ Slider funzionano anche in Setup mode

### Test 3: Live Mode
1. ✅ Setup mode → seleziona 2-3 parametri
2. ✅ Clicca segmented control → "Live"
3. ✅ Vedi SOLO i parametri selezionati
4. ✅ Slider funzionano normalmente

### Test 4: Persistenza
1. ✅ Seleziona parametri in Setup
2. ✅ Chiudi app (Cmd+Q)
3. ✅ Riapri app (Cmd+R)
4. ✅ Selezione parametri salvata (UserDefaults)

---

## 🐛 POSSIBILI ERRORI

### Error: "Cannot find type 'RNBOParameter'"
**Causa**: File AutoUI non aggiunti al target
**Fix**: Abilita Target Membership per tutti i file AutoUI

### Error: "No such module 'SwiftUI'"
**Causa**: Target iOS deployment < iOS 13
**Fix**: Build Settings → iOS Deployment Target = 13.0+

### Warning: "Missing preview"
**Ignora**: I preview sono opzionali, l'app funziona

### Crash: "Index out of range"
**Causa**: description.json mancante o malformato
**Fix**: Verifica che `RNBO/Export/description.json` esista e sia valido

---

## 📊 COSA VERIFICARE

### Binding Funzionante
- [ ] Muovi slider → valore numerico si aggiorna
- [ ] Valore numerico = valore reale del parametro RNBO
- [ ] Audio si modifica (se patch ha audio output)

### Normalizzazione Corretta
- [ ] Slider va da 0.0 a 1.0 (normalizzato)
- [ ] Valore mostrato = valore reale (min-max)
- [ ] Parametri con exponent funzionano (curve log/exp)

### Filtraggio Parametri
- [ ] Parametri con `visible: false` NON mostrati
- [ ] All mode mostra tutti i `visible: true`
- [ ] Live mode mostra solo selezionati

### Persistenza
- [ ] Selezione parametri salvata
- [ ] Modalità corrente salvata
- [ ] Configurazione persiste tra run

---

## 📝 FEEDBACK DA RACCOGLIERE

### UI/UX
- Layout responsive? (landscape/portrait)
- Font leggibili?
- Colori accessibili?
- Spacing adeguato?

### Performance
- Smooth scrolling?
- Lag quando muovi slider?
- Memoria stabile?

### Funzionalità
- Tutte le modalità funzionano?
- Checkbox reattivi?
- Empty state chiaro?

---

## 🚀 PROSSIMI STEP (Dopo Test)

### Se funziona ✅
1. Creare Python replacer
2. Automatizzare generazione file Swift
3. Test end-to-end con export RNBO reale

### Se ci sono problemi ⚠️
1. Annotare errori specifici
2. Fixare bug critici
3. Re-test

---

## 💡 TIPS XCODE

### Quick Test Parameter Changes
1. Cambia `description.json` in `RNBO/Export/`
2. Clean Build Folder (Cmd+Shift+K)
3. Build & Run (Cmd+R)

### Debug Print
Aggiungi in `AutoUIView.swift`:
```swift
.onAppear {
    print("AutoUI: \(filteredParameters.count) parameters")
    print("Mode: \(modeManager.currentMode)")
}
```

### Live Preview (SwiftUI Canvas)
1. Apri `AutoSlider.swift`
2. Canvas (Editor → Canvas)
3. Resume preview
4. Vedi widget isolato

---

## 📞 SUPPORTO

**Issues comuni**: Vedi sezione "POSSIBILI ERRORI"
**Domande**: Controlla `ANALYSIS.md` per architettura
**Bug**: Annota errore + steps to reproduce

---

**Status**: ✅ Fase 2 completata, pronto per test Xcode!
**Next**: Raccogliere feedback → Implementare Python replacer
