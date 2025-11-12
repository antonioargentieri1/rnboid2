# AutoUI - Sistema Auto-Generazione UI per SwiftRNBO

Sistema di generazione automatica dell'interfaccia utente per parametri RNBO, ispirato a RNBOID2 Android.

## Struttura Cartelle

```
AutoUI/
├── Core/           # Core logic & state management
├── Widgets/        # UI components (slider, knob, toggle, etc.)
├── Modes/          # Mode system (Live, All, Setup)
└── Themes/         # Theme system (Dark, Light, Custom)
```

---

## Core/

**Funzione**: Logica centrale per filtraggio, sorting, grouping e gestione stato.

### File Pianificati:
- `ParameterFilter.swift` - Filtra parametri (visible, custom filters)
- `ParameterGrouper.swift` - Raggruppa parametri per categoria
- `ParameterSorter.swift` - Ordina parametri (order, nome, tipo)
- `ModeManager.swift` - Gestisce modalità corrente e transizioni
- `ConfigurationStore.swift` - Persistenza configurazione (UserDefaults)
- `AutoUIBuilder.swift` - Builder principale che orchestra tutto
- `ParameterGrid.swift` - Layout griglia responsive
- `ParameterList.swift` - Layout lista verticale

### Responsabilità:
- Filtro parametri basato su `visible`, modalità corrente, filtri custom
- Ordinamento parametri (rispetta `order` da JSON)
- Grouping per categoria (future: supporto tag custom)
- State management centralizzato
- Persistenza selezione parametri "Live mode"

---

## Widgets/

**Funzione**: Componenti UI riutilizzabili auto-generati da `ParameterInfo`.

### File Pianificati:
- `AutoSlider.swift` - Slider orizzontale standard
- `AutoKnob.swift` - Knob rotante (alternativa slider)
- `AutoToggle.swift` - Toggle/Switch per parametri bool
- `AutoPicker.swift` - Picker per parametri enum
- `AutoLabel.swift` - Label con nome + unità + valore formattato
- `ParameterRow.swift` - Container riga singola (nome + widget + valore)

### Caratteristiche Widget:
- ✅ Binding bidirezionale con `RNBOParameter`
- ✅ Normalizzazione automatica (usa `valueNormalized`)
- ✅ Supporto min/max/exponent da `ParameterInfo`
- ✅ Formatting automatico (unit, decimali)
- ✅ Theming dinamico
- ✅ Accessibilità (VoiceOver, Dynamic Type)

### Tipo Widget per Parametro:
| Tipo Parametro | Widget Suggerito |
|----------------|------------------|
| Continuo (steps=0) | `AutoSlider` o `AutoKnob` |
| Discreto (steps>0) | `AutoSlider` con snap |
| Enum (isEnum=true) | `AutoPicker` |
| Boolean (min=0, max=1, steps=1) | `AutoToggle` |

---

## Modes/

**Funzione**: Sistema modalità ispirato a RNBOID2 Android.

### File Pianificati:
- `LiveMode.swift` - Mostra solo parametri selezionati
- `AllMode.swift` - Mostra tutti i parametri visibili
- `SetupMode.swift` - UI per selezionare parametri Live
- `ModeSelector.swift` - Segmented control per switch modalità

### Modalità:

#### Live Mode
- **Scopo**: Performance live, controlli essenziali
- **Parametri**: Solo quelli selezionati in Setup
- **Layout**: Compatto, grandi widget
- **Persistenza**: Selezione salvata in UserDefaults

#### All Mode
- **Scopo**: Editing completo, sound design
- **Parametri**: Tutti i parametri con `visible: true`
- **Layout**: Lista scrollabile
- **Grouping**: Opzionale per categoria

#### Setup Mode
- **Scopo**: Configura parametri per Live mode
- **UI**: Checkboxes + preview
- **Azioni**:
  - Toggle visibilità parametro in Live
  - Riordina parametri (drag & drop)
  - Reset configurazione

---

## Themes/

**Funzione**: Sistema theming dinamico per customizzazione UI.

### File Pianificati:
- `ThemeProtocol.swift` - Protocol che definisce tema
- `DarkTheme.swift` - Tema dark mode
- `LightTheme.swift` - Tema light mode
- `CustomTheme.swift` - Tema personalizzabile

### Theme Properties:
```swift
protocol ThemeProtocol {
    // Colors
    var backgroundColor: Color { get }
    var foregroundColor: Color { get }
    var accentColor: Color { get }
    var sliderTrackColor: Color { get }
    var sliderThumbColor: Color { get }

    // Typography
    var labelFont: Font { get }
    var valueFont: Font { get }
    var titleFont: Font { get }

    // Spacing
    var itemSpacing: CGFloat { get }
    var sectionSpacing: CGFloat { get }
    var padding: CGFloat { get }

    // Sizing
    var sliderHeight: CGFloat { get }
    var knobSize: CGFloat { get }
    var cornerRadius: CGFloat { get }
}
```

---

## Flusso di Utilizzo

### 1. App Startup
```swift
@main
struct SwiftRNBOApp: App {
    @ObservedObject var rnbo = RNBOAudioUnitHostModel()
    @StateObject var autoUI = AutoUIBuilder()

    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(rnbo)
                .environmentObject(autoUI)
                .onAppear {
                    rnbo.connectEventHandler()
                    autoUI.loadConfiguration()  // Load saved config
                }
        }
    }
}
```

### 2. ContentView (Semplificato)
```swift
struct ContentView: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel
    @EnvironmentObject var autoUI: AutoUIBuilder

    var body: some View {
        VStack {
            ModeSelector(currentMode: $autoUI.mode)

            AutoUIView(
                parameters: autoUI.filteredParameters,
                mode: autoUI.mode,
                theme: autoUI.theme
            )
        }
    }
}
```

### 3. AutoUIBuilder (Core)
```swift
class AutoUIBuilder: ObservableObject {
    @Published var mode: UIMode = .all
    @Published var selectedParameters: Set<String> = []
    @Published var theme: ThemeProtocol = DarkTheme()

    var filteredParameters: [RNBOParameter] {
        switch mode {
        case .live:
            return parameters.filter { selectedParameters.contains($0.id) }
        case .all:
            return parameters.filter { $0.info.visible }
        case .setup:
            return parameters.filter { $0.info.visible }
        }
    }

    func loadConfiguration() { /* UserDefaults */ }
    func saveConfiguration() { /* UserDefaults */ }
}
```

---

## Vantaggi Rispetto a UI Manuale

| Aspetto | UI Manuale | AutoUI |
|---------|------------|--------|
| **Codice per parametro** | ~20 righe | 0 righe |
| **Aggiornamento patch** | Riscrivere UI | Automatico |
| **Modalità Live/All/Setup** | Non supportato | Built-in |
| **Theming** | Hardcoded | Dinamico |
| **Persistenza config** | Manuale | Automatico |
| **Accessibilità** | Manuale | Automatico |
| **Multi-platform** | Duplicare codice | Condiviso |

---

## Prossimi Step

### Fase 2: Core Implementation
1. Creare `ModeManager.swift`
2. Creare `ParameterFilter.swift`
3. Creare `ConfigurationStore.swift`

### Fase 3: Primo Widget
1. Implementare `AutoSlider.swift`
2. Implementare `ParameterRow.swift`
3. Test con parametri reali

### Fase 4: Mode System
1. Implementare `LiveMode.swift`
2. Implementare `AllMode.swift`
3. Implementare `SetupMode.swift`
4. Implementare `ModeSelector.swift`

---

**Status**: Setup completato ✅
**Next**: Implementazione Core system
