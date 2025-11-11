# RNBO Android Replacer v2.2 - MULTI-MODE

Tool per sostituire automaticamente i file RNBO in un template Android Studio.

## 🏗️ Struttura Modulare

```
rnbo_replacer/
├── main.py                      # 🚀 Entry point - avvia l'applicazione
│
├── gui/                         # 🎨 Interfaccia Grafica
│   ├── __init__.py
│   ├── main_window.py          # Finestra principale con tutti i widget
│   └── widgets.py              # Widget personalizzati (future)
│
├── core/                        # 🔧 Logica Business
│   ├── __init__.py
│   ├── replacer.py             # Logica sostituzione file
│   ├── analyzer.py             # Analisi export RNBO
│   └── config.py               # Gestione configurazione (future)
│
├── templates/                   # 📄 Template Codice
│   ├── __init__.py
│   ├── mainactivity.py         # Generatore MainActivity.java
│   ├── layout.py               # Template activity_main.xml
│   └── msploader.py            # Template MSPLoader.cpp ottimizzato
│
├── utils/                       # 🛠️ Utility
│   ├── __init__.py
│   └── logger.py               # Sistema logging per GUI
│
├── mainactivity_extracted.txt   # Template MainActivity completo
└── README.md                    # Questo file
```

## 🎯 Responsabilità dei Moduli

### GUI Layer (`gui/`)
- **main_window.py**: Interfaccia utente, gestione eventi, threading
- Separata dalla logica business per facilitare testing e manutenzione

### Core Layer (`core/`)
- **replacer.py**: Orchestrazione sostituzione file, backup, copia
- **analyzer.py**: Lettura description.json, parsing parametri RNBO
- Logica pura, indipendente dall'UI

### Templates Layer (`templates/`)
- **mainactivity.py**: Genera MainActivity.java con Multi-Mode System
- **msploader.py**: MSPLoader.cpp ottimizzato anti-click
- **layout.py**: Layout XML per Android
- Template modulari e facilmente modificabili

### Utils Layer (`utils/`)
- **logger.py**: Gestione logging thread-safe per GUI
- Utility riutilizzabili

## 🚀 Come Usare

### Esecuzione
```bash
python3 main.py
```

O da qualsiasi directory:
```bash
cd rnbo_replacer
python3 -m main
```

### Workflow
1. Seleziona Template Android
2. Seleziona Export RNBO
3. Verifica Percorsi
4. Sostituisci File

## ✨ Features v2.2

- ✅ **Multi-Mode System**: Live, All, Setup modes
- ✅ **Parameter Management**: Custom ranges, sensor control
- ✅ **MSPLoader Ottimizzato**: Anti-click fix
- ✅ **Persistent Configuration**: Salvataggio settings
- ✅ **Real Parameter Values**: Lettura da description.json

## 🔧 Estendibilità

### Aggiungere nuove funzionalità:

**Nuovo template:**
```python
# templates/new_template.py
class NewTemplate:
    @staticmethod
    def get_content():
        return '''template content'''
```

**Nuova analisi:**
```python
# core/new_analyzer.py
class NewAnalyzer:
    def analyze(self, data):
        # analisi
        return results
```

**Nuovo widget GUI:**
```python
# gui/widgets.py
class CustomWidget(tk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        # widget implementation
```

## 📊 Vantaggi Architettura

1. **Separazione Concerns**: GUI ≠ Logic ≠ Templates
2. **Testabilità**: Ogni modulo testabile isolatamente
3. **Manutenibilità**: Modifiche localizzate
4. **Estendibilità**: Facile aggiungere features
5. **Leggibilità**: File ~300 righe invece di 1800

## 🔍 Debug

Ogni modulo ha logging integrato:

```python
# core/replacer.py logga operazioni
self.logger.log("Step X: Doing something...")

# Errori con traceback completo
except Exception as e:
    self.logger.log(f"❌ Error: {e}")
    import traceback
    self.logger.log(traceback.format_exc())
```

## 📝 Note Tecniche

- **Threading**: Sostituzione file in thread separato per non bloccare GUI
- **Backup**: Automatico prima di ogni modifica
- **Error Handling**: Gestione errori granulare per ogni step
- **Path Management**: Uso di `pathlib.Path` per compatibilità cross-platform

## 🐛 Troubleshooting

**Import Error:**
```bash
# Assicurati di essere nella directory corretta
cd path/to/rnbo_replacer
python3 main.py
```

**Template Non Trovato:**
```bash
# Verifica che mainactivity_extracted.txt esista
ls -la mainactivity_extracted.txt
```

## 📄 Licenza

RNBO Android Replacer v2.2
