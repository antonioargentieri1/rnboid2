# SwiftRNBO - Analisi Progetto per AutoUI System

**Data**: 2025-11-12
**Obiettivo**: Estendere SwiftRNBO con sistema di auto-generazione UI simile a RNBOID2 Android

---

## 1. PANORAMICA PROGETTO

### Descrizione
SwiftRNBO è un layer di compatibilità Swift per RNBO (Cycling '74) che permette di utilizzare codice C++ esportato da Max/MSP in applicazioni iOS/macOS/tvOS.

### Piattaforme Supportate
- ✅ iOS (audio input + MIDI)
- ✅ macOS (audio input + MIDI)
- ✅ tvOS (limitato: no audio input, no MIDI)

### Architettura Attuale
```
SwiftRNBO_Example_multiplatfrom_SwiftUI/
├── RNBO/
│   ├── Export/                    # Export RNBO (rnbo_source.cpp + JSON)
│   │   ├── description.json       # ⭐ Parametri RNBO
│   │   ├── dependencies.json
│   │   ├── presets.json
│   │   ├── media/                 # Audio samples (wav/aiff)
│   │   └── rnbo/                  # C++ RNBO SDK
│   ├── Bindings/                  # Objective-C++ bridging layer
│   │   ├── RNBOAudioUnit.h/.mm
│   │   ├── RNBOEventHandlerProtocol.h
│   │   └── RNBO-Bridging-Header.h
│   └── Interface/                 # ⭐ Swift API pubbliche
│       ├── RNBOParameter.swift
│       ├── RNBODescription.swift
│       ├── RNBOAudioUnitHostModel.swift
│       ├── RNBOAudioEngine.swift
│       ├── RNBOEventHandler.swift
│       └── MIDIHelpers.swift
├── AutoUI/                        # 🆕 NUOVO - Sistema Auto-UI
│   ├── Core/
│   ├── Widgets/
│   ├── Modes/
│   └── Themes/
├── ContentView.swift              # ⚠️ UI Manuale attuale
├── Sliders.swift                  # ⚠️ UI Manuale attuale
└── SliderView.swift               # ⚠️ UI Manuale attuale
```

---

## 2. FILE CHIAVE - ANALISI DETTAGLIATA

### 2.1 RNBODescription.swift
**Percorso**: `RNBO/Interface/RNBODescription.swift`

**Funzione**: Strutture Codable per deserializzare `description.json` di RNBO.

**Strutture Principali**:
```swift
struct RNBODescription: Codable {
    let parameters: [ParameterInfo]      // ⭐ Array parametri
    let numParameters: Int
    let numInputChannels: Int
    let numOutputChannels: Int
    let externalDataRefs: [ExternalDataRef]  // Buffer audio
    let presetid: String
    let meta: Meta
    // ...
}

struct ParameterInfo: Codable, Identifiable {
    let type: String                  // "ParameterTypeNumber"
    let index: Int                    // Indice parametro
    let name, paramId: String         // Nome e ID
    let minimum, maximum: Double      // Range
    let exponent: Double              // Normalizzazione
    let steps: Int                    // Quantizzazione
    let initialValue: Double
    let isEnum: Bool
    let enumValues: [EnumValue]
    let displayName, unit: String
    let order: Int
    let visible: Bool                 // ⭐ Visibilità parametro!
    let signalIndex: Int?
}
```

**Punti Chiave**:
- ✅ Tutti i metadati parametri disponibili da JSON
- ✅ Campo `visible` per filtrare parametri invisibili
- ✅ Campo `order` per ordinamento UI
- ✅ Supporto enum values per parametri discreti
- ✅ `isEnum` flag per identificare parametri discreti
- ✅ `displayName` e `unit` per UI

**Extension disponibile**:
```swift
extension RNBODescription {
    func getParametersArray() -> [RNBOParameter]  // Converte a array Swift
}
```

---

### 2.2 RNBOParameter.swift
**Percorso**: `RNBO/Interface/RNBOParameter.swift`

**Funzione**: Wrapper Swift per parametri RNBO con normalizzazione.

**Struttura**:
```swift
struct RNBOParameter {
    var value: Double                    // Valore attuale
    let info: ParameterInfo              // Metadati (da Description)

    var valueNormalized: Double {        // ⭐ Normalizzato 0.0-1.0
        get { ... }  // toNormalised()
        set { ... }  // fromNormalised()
    }
}
```

**Features**:
- ✅ Normalizzazione automatica (0.0-1.0) con exponent curve
- ✅ Clipping automatico min/max
- ✅ Conforme a `Identifiable` (per ForEach SwiftUI)
- ✅ Conforme a `Equatable`

**Extension Double**:
```swift
extension Double {
    func clip(from: Double, to: Double) -> Double
    func fromNormalised(minValue, maxValue, factor) -> Double
    func toNormalised(minValue, maxValue, factor) -> Double
}
```

---

### 2.3 RNBOAudioUnitHostModel.swift
**Percorso**: `RNBO/Interface/RNBOAudioUnitHostModel.swift`

**Funzione**: Controller principale dell'app, gestisce audio engine e parametri.

**Classe**:
```swift
class RNBOAudioUnitHostModel: ObservableObject {
    @Published var parameters: [RNBOParameter]  // ⭐ Array parametri
    @Published var showDescription: Bool
    let description: RNBODescription?

    private let audioEngine: RNBOAudioEngine
    private let audioUnit: RNBOAudioUnit
    private let eventHandler: RNBOEventHandler

    init() {
        // Carica description.json dal Bundle
        // Inizializza audioEngine
        // Crea array parameters
    }
}
```

**API Parametri**:
```swift
// Lettura
func refreshParameterValue(at index: Int)

// Scrittura (valore assoluto)
func setParameterValue(to value: Double, at index: Int)
func setParameterValueHot(to value: Double, at index: Int)

// Scrittura (normalizzato 0.0-1.0)
func setParameterValueNormalized(to value: Double, at index: Int)
func setParameterValueNormalizedHot(to value: Double, at index: Int)
```

**API MIDI**:
```swift
func sendNoteOn(pitch: UInt8, velocity: UInt8, channel: UInt8)
func sendNoteOff(pitch: UInt8, releaseVelocity: UInt8, channel: UInt8)
func sendAftertouch(pitch: UInt8, pressure: UInt8, channel: UInt8)
func sendContinuousController(number: UInt8, value: UInt8, channel: UInt8)
func sendPatchChange(program: UInt8, channel: UInt8)
func sendChannelPressure(pressure: UInt8, channel: UInt8)
func sendPitchBend(value: UInt16, channel: UInt8)
```

**API Audio**:
```swift
func playAudioFile()
func pauseAudioFile()
func toggleMic(_ on: Bool)
```

**API Messaggi**:
```swift
func sendMessage(_ message: [Double])
```

**Uso tipico**:
```swift
@main
struct SwiftRNBOApp: App {
    @ObservedObject var rnbo = RNBOAudioUnitHostModel()  // ⭐ Singleton

    var body: some Scene {
        WindowGroup {
            ContentView()
                .onAppear { rnbo.connectEventHandler() }
                .environmentObject(rnbo)  // ⭐ Inietta in tutta l'app
        }
    }
}
```

---

## 3. UI ATTUALE - IMPLEMENTAZIONE MANUALE

### 3.1 ContentView.swift
**Percorso**: `ContentView.swift`

**Struttura UI attuale**:
```swift
struct ContentView: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel

    var body: some View {
        VStack(spacing: 0) {
            Buttons()                    // Play/Pause/Mic buttons
            if rnbo.showDescription {
                DescriptionView()        // Mostra description.json
            } else {
                Sliders()                // ⚠️ TUTTI gli slider hardcoded
                AudioKitKeyboard()       // MIDI keyboard
            }
        }
        .padding()
    }
}
```

**Problema**: UI completamente statica, developer deve:
1. Aprire `description.json`
2. Leggere manualmente i parametri
3. Scrivere codice UI per ogni parametro
4. Nessun sistema di modalità (Live/All/Setup)

---

### 3.2 Sliders.swift
**Percorso**: `Sliders.swift`

**Implementazione attuale**:
```swift
struct Sliders: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel

    var body: some View {
        ScrollView {
            VStack {
                // ⚠️ ForEach su TUTTI i parametri
                ForEach($rnbo.parameters) { $parameter in
                    SliderView(parameter: $parameter)
                }
            }
            .padding()
            .background()
            .padding(.bottom)
        }
        .frame(minHeight: 100)
        .padding(.bottom)
    }
}
```

**Caratteristiche**:
- ✅ ForEach automatico (bene!)
- ⚠️ Mostra TUTTI i parametri (anche `visible: false`)
- ⚠️ Nessun filtro per modalità (Live/All/Setup)
- ⚠️ Nessun ordinamento custom
- ⚠️ Nessun grouping per categoria

---

### 3.3 SliderView.swift
**Percorso**: `SliderView.swift`

**Widget slider singolo**:
```swift
struct SliderView: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel
    @Binding var parameter: RNBOParameter

    var body: some View {
        HStack {
            let displayName = parameter.info.paramId

            #if os(iOS)
                SliderNameLabel(name: displayName)
            #endif

            Slider(value: $parameter.valueNormalized) {
                SliderNameLabel(name: displayName)
            } minimumValueLabel: {
                SliderValueLabel(value: parameter.info.minimum)
            } maximumValueLabel: {
                SliderValueLabel(value: parameter.info.maximum)
            }
            .onChange(of: parameter.valueNormalized) {
                rnbo.setParameterValueNormalized(to: $0, at: parameter.info.index)
            }

            SliderValueLabel(value: parameter.value)
        }
    }
}
```

**Caratteristiche**:
- ✅ Usa `valueNormalized` (0.0-1.0)
- ✅ Mostra min/max labels
- ✅ Mostra valore corrente
- ✅ Binding bidirezionale con RNBO
- ⚠️ Design fisso (nessun theming)
- ⚠️ Usa `paramId` invece di `displayName`
- ⚠️ Nessun supporto per parametri enum
- ⚠️ Nessun supporto per knobs/toggle/picker

---

## 4. PUNTI DI INTEGRAZIONE PER AUTOUI

### 4.1 Dati Disponibili da RNBO

#### description.json
```json
{
  "parameters": [
    {
      "type": "ParameterTypeNumber",
      "index": 0,
      "name": "/freq",
      "paramId": "freq",
      "minimum": 20.0,
      "maximum": 20000.0,
      "exponent": 1.0,
      "steps": 0,
      "initialValue": 440.0,
      "isEnum": false,
      "enumValues": [],
      "displayName": "Frequency",
      "unit": "Hz",
      "order": 0,
      "visible": true,
      "debug": false,
      "signalIndex": -1,
      "ioType": "IOTypeUndefined"
    }
  ],
  "numParameters": 8,
  "externalDataRefs": [
    {
      "id": "drums",
      "file": "Plaits_20200805_10.wav",
      "type": "audiobuffer",
      "tag": "drums"
    }
  ]
}
```

**Campi Utili AutoUI**:
- ✅ `visible`: bool - filtra parametri UI
- ✅ `order`: int - ordinamento custom
- ✅ `displayName`: string - nome UI localizzato
- ✅ `unit`: string - unità di misura (Hz, dB, %, etc.)
- ✅ `isEnum`: bool - parametro discreto
- ✅ `enumValues`: array - valori discreti
- ✅ `steps`: int - quantizzazione (0 = continuo)
- ✅ `type`: string - tipo parametro
- ⚠️ **Manca**: categoria/group (serve estensione custom)
- ⚠️ **Manca**: widget type hint (serve estensione custom)

---

### 4.2 API SwiftRNBO Disponibili

#### RNBOAudioUnitHostModel
```swift
class RNBOAudioUnitHostModel: ObservableObject {
    @Published var parameters: [RNBOParameter]  // ⭐ Fonte dati
    let description: RNBODescription?           // ⭐ Metadati completi

    // API controllo parametri (già disponibile)
    func setParameterValueNormalized(to: Double, at: Int)
    func setParameterValueNormalizedHot(to: Double, at: Int)
    func refreshParameterValue(at: Int)
}
```

**Cosa abbiamo**:
- ✅ Array parametri observable (`@Published`)
- ✅ Metadati completi da JSON
- ✅ API normalizzazione automatica
- ✅ Binding bidirezionale funzionante
- ✅ Supporto SwiftUI nativo (`ObservableObject`)

**Cosa ci serve per AutoUI**:
- 🆕 Filtro parametri per visibilità
- 🆕 Ordinamento parametri
- 🆕 Grouping parametri per categoria
- 🆕 Configurazione "Live mode" (parametri selezionati)
- 🆕 Persistenza configurazione (UserDefaults)
- 🆕 Sistema temi (colors, fonts, spacing)

---

### 4.3 Compatibilità con RNBOID2 Android

#### Similitudini
| Feature | Android RNBOID2 | SwiftRNBO | Compatibilità |
|---------|----------------|-----------|---------------|
| **Fonte dati** | `description.json` | `description.json` | ✅ 100% |
| **Parametri** | `MainActivity.parameters` | `RNBOAudioUnitHostModel.parameters` | ✅ Stesso concetto |
| **Normalizzazione** | Java custom | `valueNormalized` computed property | ✅ Stesso range 0.0-1.0 |
| **UI dinamica** | `ForEach` in Java | `ForEach` in SwiftUI | ✅ Stesso pattern |
| **Binding** | Manual updateParam() | `@Binding` SwiftUI | ✅ Più elegante in Swift |
| **Modes** | Live/All/Setup | ❌ Non implementato | 🆕 Da implementare |
| **Themes** | Dark mode statico | ❌ Non implementato | 🆕 Da implementare |

#### Differenze Architetturali
| Aspetto | Android | iOS/Swift |
|---------|---------|----------|
| **UI Framework** | XML layouts + Java | SwiftUI (declarative) |
| **State Management** | Manual listeners | `@State`, `@Published`, `@Binding` |
| **Lifecycle** | Activity onCreate/onResume | View onAppear/onDisappear |
| **Storage** | SharedPreferences | UserDefaults |
| **Threading** | Handler/runOnUiThread | @MainActor/DispatchQueue.main |
| **Widgets** | Custom View classes | SwiftUI Views (struct) |

**Vantaggio SwiftUI**: AutoUI sarà PIÙ SEMPLICE da implementare grazie a:
- ✅ Declarative syntax (meno boilerplate)
- ✅ Native binding system
- ✅ Automatic state updates
- ✅ Composable views
- ✅ Built-in animations

---

## 5. ROADMAP IMPLEMENTAZIONE AUTOUI

### FASE 1: Setup & Analisi ✅ **COMPLETATO**
- [x] Analisi struttura progetto
- [x] Mapping file chiave
- [x] Identificazione API disponibili
- [x] Creazione struttura cartelle `AutoUI/`
- [x] Documentazione `ANALYSIS.md`

### FASE 2: Core System (PROSSIMO)
**File**: `AutoUI/Core/`
- [ ] `ParameterFilter.swift` - Filtraggio parametri (visible, custom filters)
- [ ] `ParameterGrouper.swift` - Grouping per categoria
- [ ] `ParameterSorter.swift` - Ordinamento custom
- [ ] `ModeManager.swift` - Gestione modalità (Live/All/Setup)
- [ ] `ConfigurationStore.swift` - Persistenza UserDefaults

### FASE 3: Modes System
**File**: `AutoUI/Modes/`
- [ ] `LiveMode.swift` - Parametri selezionati
- [ ] `AllMode.swift` - Tutti i parametri
- [ ] `SetupMode.swift` - Selezione parametri per Live

### FASE 4: Widgets Auto-Generated
**File**: `AutoUI/Widgets/`
- [ ] `AutoSlider.swift` - Slider generato automaticamente
- [ ] `AutoKnob.swift` - Knob rotante (alternativa slider)
- [ ] `AutoToggle.swift` - Toggle per parametri bool
- [ ] `AutoPicker.swift` - Picker per parametri enum
- [ ] `AutoLabel.swift` - Label con unità + valore
- [ ] `ParameterRow.swift` - Container singola riga parametro

### FASE 5: Layout Auto-Generated
**File**: `AutoUI/Core/`
- [ ] `AutoUIBuilder.swift` - Builder principale
- [ ] `ParameterGrid.swift` - Layout griglia responsive
- [ ] `ParameterList.swift` - Layout lista verticale

### FASE 6: Theming System
**File**: `AutoUI/Themes/`
- [ ] `ThemeProtocol.swift` - Protocol theme
- [ ] `DarkTheme.swift` - Tema scuro
- [ ] `LightTheme.swift` - Tema chiaro
- [ ] `CustomTheme.swift` - Tema personalizzabile

### FASE 7: Integration & Testing
- [ ] Integrazione in `ContentView.swift`
- [ ] Testing su iOS/macOS/tvOS
- [ ] Documentazione utente

---

## 6. ESEMPI CODICE PROPOSTI

### 6.1 Uso Atteso AutoUI (Goal)

```swift
import SwiftUI

struct ContentView: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel
    @StateObject var autoUI = AutoUIBuilder()

    var body: some View {
        VStack {
            // Selezione modalità
            ModeSelector(currentMode: $autoUI.mode)

            // UI auto-generata in base alla modalità
            AutoUIView(
                rnbo: rnbo,
                mode: autoUI.mode,
                theme: .dark
            )
        }
    }
}
```

**Risultato**: Developer NON deve più scrivere UI manualmente!

---

### 6.2 Confronto Prima/Dopo

#### Prima (Manuale) ❌
```swift
// Developer deve scrivere questo per ogni patch RNBO:
struct Sliders: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel

    var body: some View {
        ScrollView {
            VStack {
                ForEach($rnbo.parameters) { $parameter in
                    SliderView(parameter: $parameter)
                }
            }
        }
    }
}

// E poi anche SliderView, labels, formatting, etc...
```

#### Dopo (AutoUI) ✅
```swift
// Developer scrive solo questo:
struct ContentView: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel

    var body: some View {
        AutoUIView(rnbo: rnbo)  // ⭐ Una riga!
    }
}
```

---

## 7. FILE DA MODIFICARE/CREARE

### File Esistenti da NON Modificare
- ✅ `RNBO/Interface/*.swift` - API core RNBO (stabile)
- ✅ `RNBO/Bindings/*.mm` - Bridging layer (stabile)
- ✅ `RNBO/Export/*` - Export RNBO (sostituito ad ogni export)

### File Esistenti da Modificare (Fase 7)
- 🔧 `ContentView.swift` - Sostituire `Sliders()` con `AutoUIView()`
- 🔧 `SwiftRNBOApp.swift` - Inizializzare `AutoUIBuilder`

### File Nuovi da Creare (Fase 2-6)
**AutoUI/Core/**
- 🆕 `ParameterFilter.swift`
- 🆕 `ParameterGrouper.swift`
- 🆕 `ParameterSorter.swift`
- 🆕 `ModeManager.swift`
- 🆕 `ConfigurationStore.swift`
- 🆕 `AutoUIBuilder.swift`
- 🆕 `ParameterGrid.swift`
- 🆕 `ParameterList.swift`

**AutoUI/Modes/**
- 🆕 `LiveMode.swift`
- 🆕 `AllMode.swift`
- 🆕 `SetupMode.swift`
- 🆕 `ModeSelector.swift`

**AutoUI/Widgets/**
- 🆕 `AutoSlider.swift`
- 🆕 `AutoKnob.swift`
- 🆕 `AutoToggle.swift`
- 🆕 `AutoPicker.swift`
- 🆕 `AutoLabel.swift`
- 🆕 `ParameterRow.swift`

**AutoUI/Themes/**
- 🆕 `ThemeProtocol.swift`
- 🆕 `DarkTheme.swift`
- 🆕 `LightTheme.swift`
- 🆕 `CustomTheme.swift`

---

## 8. NOTE TECNICHE

### 8.1 SwiftUI Best Practices per AutoUI
- ✅ Usare `@State` per stato locale UI
- ✅ Usare `@Binding` per passare stato ai child
- ✅ Usare `@EnvironmentObject` per `RNBOAudioUnitHostModel`
- ✅ Usare `@Published` per dati observable
- ✅ Preferire `struct` View per performance
- ✅ Usare `@ViewBuilder` per composizione flessibile

### 8.2 Gestione Stato
```swift
// Pattern proposto
class AutoUIBuilder: ObservableObject {
    @Published var mode: UIMode = .all
    @Published var selectedParameters: Set<String> = []
    @Published var theme: ThemeProtocol = DarkTheme()

    func filterParameters(_ params: [RNBOParameter]) -> [RNBOParameter]
    func sortParameters(_ params: [RNBOParameter]) -> [RNBOParameter]
    func groupParameters(_ params: [RNBOParameter]) -> [String: [RNBOParameter]]
}
```

### 8.3 Persistenza Configurazione
```swift
// UserDefaults keys
extension UserDefaults {
    static let selectedParametersKey = "autoui.selectedParameters"
    static let currentModeKey = "autoui.currentMode"
    static let themeKey = "autoui.theme"
}

// Save/Load
func saveConfiguration()
func loadConfiguration()
```

### 8.4 Compatibilità Multi-Platform
```swift
// iOS-specific
#if os(iOS)
    // iPhone/iPad layout
#endif

// macOS-specific
#if os(macOS)
    // Desktop layout
#endif

// tvOS-specific (limitato)
#if os(tvOS)
    // TV remote navigation
#endif
```

---

## 9. CONCLUSIONI

### Stato Attuale
- ✅ SwiftRNBO ha un'architettura solida e ben strutturata
- ✅ API RNBO complete e funzionali
- ✅ SwiftUI permette implementazione elegante AutoUI
- ⚠️ UI attualmente manuale (richiede codice per ogni parametro)
- ⚠️ Nessun sistema di modalità (Live/All/Setup)
- ⚠️ Nessun theming dinamico

### Fattibilità AutoUI
**MOLTO ALTA (95%)**

**Vantaggi**:
- ✅ `description.json` identico ad Android (dati disponibili)
- ✅ SwiftUI più semplice di XML+Java Android
- ✅ Binding system nativo (meno boilerplate)
- ✅ Architettura già pulita e modulare

**Complessità Stimata**: **MEDIA-BASSA**
- 📅 Fase 2-3 (Core + Modes): 2-3 giorni
- 📅 Fase 4 (Widgets): 2-3 giorni
- 📅 Fase 5 (Layout): 1-2 giorni
- 📅 Fase 6 (Theming): 1-2 giorni
- 📅 Fase 7 (Integration): 1 giorno
- **TOTALE**: ~7-11 giorni sviluppo

### Prossimi Step
1. ✅ Setup struttura completato
2. 🔜 Implementare `ParameterFilter.swift`
3. 🔜 Implementare `ModeManager.swift`
4. 🔜 Creare primo widget `AutoSlider.swift`
5. 🔜 Test su patch RNBO reale

---

## APPENDICE: Risorse Utili

### Documentazione RNBO
- [RNBO Documentation](https://rnbo.cycling74.com/learn)
- [RNBO C++ API](https://rnbo.cycling74.com/cpp)

### SwiftUI Resources
- [Apple SwiftUI Documentation](https://developer.apple.com/documentation/swiftui/)
- [SwiftUI Binding](https://developer.apple.com/documentation/swiftui/binding)
- [SwiftUI Property Wrappers](https://developer.apple.com/documentation/swiftui/state-and-data-flow)

### RNBOID2 Android Reference
- Repository: `rnboid2/Max-MSP-RNBO-CPP-Native-Android-master`
- File chiave: `MainActivity.java` (2757 righe)
- Sistema modalità: righe 946-1086

---

**Fine Analisi** | Generato: 2025-11-12 | SwiftRNBO AutoUI Project
