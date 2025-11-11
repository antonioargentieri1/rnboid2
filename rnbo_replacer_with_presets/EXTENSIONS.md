# 🚀 Guida alle Future Estensioni

Questa architettura modulare rende facile aggiungere nuove funzionalità.

## 💡 Idee per Estensioni

### 1. Config Manager
**File**: `core/config.py`

```python
"""
Gestione configurazione persistente
"""
import json
from pathlib import Path

class ConfigManager:
    def __init__(self):
        self.config_file = Path.home() / ".rnbo_replacer" / "config.json"
        self.config = self.load_config()
    
    def load_config(self):
        if self.config_file.exists():
            with open(self.config_file) as f:
                return json.load(f)
        return {}
    
    def save_config(self, config):
        self.config_file.parent.mkdir(exist_ok=True)
        with open(self.config_file, 'w') as f:
            json.dump(config, f, indent=2)
    
    def get_last_template(self):
        return self.config.get('last_template_path')
    
    def set_last_template(self, path):
        self.config['last_template_path'] = str(path)
        self.save_config(self.config)
```

**Uso in GUI**:
```python
from core.config import ConfigManager

# In __init__
self.config = ConfigManager()

# Carica ultimo percorso
last_template = self.config.get_last_template()
if last_template:
    self.template_path.set(last_template)
```

---

### 2. Batch Processor
**File**: `core/batch_processor.py`

```python
"""
Processamento batch di multiple export
"""
from pathlib import Path
from core.replacer import RNBOReplacer

class BatchProcessor:
    def __init__(self, template_path, logger):
        self.template = Path(template_path)
        self.logger = logger
    
    def process_multiple_exports(self, export_dirs):
        """Processa multiple directory export"""
        results = []
        
        for export_dir in export_dirs:
            self.logger.log(f"\n🔄 Processing {export_dir.name}...")
            
            replacer = RNBOReplacer(self.template, export_dir, self.logger)
            success = replacer.replace_files()
            
            results.append({
                'export': export_dir,
                'success': success
            })
        
        return results
```

---

### 3. Preview Mode
**File**: `gui/preview_window.py`

```python
"""
Finestra preview modifiche prima di applicarle
"""
import tkinter as tk
from tkinter import scrolledtext

class PreviewWindow(tk.Toplevel):
    def __init__(self, parent, changes):
        super().__init__(parent)
        self.title("Preview Modifiche")
        self.geometry("800x600")
        
        # Lista modifiche
        self.text = scrolledtext.ScrolledText(self, wrap=tk.WORD)
        self.text.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Mostra modifiche
        for change in changes:
            self.text.insert(tk.END, f"📝 {change['file']}\n")
            self.text.insert(tk.END, f"   {change['action']}\n\n")
        
        # Pulsanti
        btn_frame = tk.Frame(self)
        btn_frame.pack(pady=10)
        
        tk.Button(btn_frame, text="✓ Applica", 
                 command=self.apply).pack(side=tk.LEFT, padx=5)
        tk.Button(btn_frame, text="✗ Annulla", 
                 command=self.cancel).pack(side=tk.LEFT, padx=5)
```

---

### 4. Validator
**File**: `core/validator.py`

```python
"""
Validazione approfondita export RNBO
"""
import json
from pathlib import Path

class RNBOValidator:
    def __init__(self, export_path, logger):
        self.export = Path(export_path)
        self.logger = logger
        self.errors = []
        self.warnings = []
    
    def validate(self):
        """Esegue validazione completa"""
        self._check_required_files()
        self._validate_description_json()
        self._check_cpp_compatibility()
        
        return len(self.errors) == 0
    
    def _check_required_files(self):
        required = ['description.json', 'dependencies.json', 
                   'presets.json', 'rnbo']
        
        for item in required:
            if not (self.export / item).exists():
                self.errors.append(f"Missing: {item}")
    
    def _validate_description_json(self):
        desc = self.export / "description.json"
        if not desc.exists():
            return
        
        try:
            with open(desc) as f:
                data = json.load(f)
            
            # Check structure
            if 'parameters' not in data:
                self.warnings.append("No parameters in description.json")
            
            # Check parameter validity
            for param in data.get('parameters', []):
                if 'name' not in param:
                    self.errors.append("Parameter without name")
                if 'minimum' not in param or 'maximum' not in param:
                    self.warnings.append(f"Parameter {param.get('name')} missing min/max")
        
        except json.JSONDecodeError as e:
            self.errors.append(f"Invalid JSON: {e}")
```

---

### 5. Undo/Redo System
**File**: `core/history.py`

```python
"""
Sistema undo/redo per operazioni
"""
from pathlib import Path
import shutil

class HistoryManager:
    def __init__(self, max_history=10):
        self.history = []
        self.current = -1
        self.max_history = max_history
    
    def save_snapshot(self, description, files):
        """Salva snapshot dei file modificati"""
        snapshot = {
            'description': description,
            'files': {},
            'timestamp': datetime.now()
        }
        
        # Copia file
        for file_path in files:
            if Path(file_path).exists():
                snapshot['files'][file_path] = Path(file_path).read_bytes()
        
        # Aggiungi a history
        self.history = self.history[:self.current+1]
        self.history.append(snapshot)
        self.current += 1
        
        # Limita history
        if len(self.history) > self.max_history:
            self.history.pop(0)
            self.current -= 1
    
    def undo(self):
        """Ripristina snapshot precedente"""
        if self.current > 0:
            self.current -= 1
            return self._restore_snapshot(self.history[self.current])
        return False
    
    def redo(self):
        """Riapplica snapshot successivo"""
        if self.current < len(self.history) - 1:
            self.current += 1
            return self._restore_snapshot(self.history[self.current])
        return False
    
    def _restore_snapshot(self, snapshot):
        for file_path, content in snapshot['files'].items():
            Path(file_path).write_bytes(content)
        return True
```

---

### 6. Custom Widgets
**File**: `gui/widgets.py`

```python
"""
Widget personalizzati per GUI
"""
import tkinter as tk
from tkinter import ttk

class PathSelector(tk.Frame):
    """Widget combinato Entry + Button per selezione path"""
    def __init__(self, parent, label, on_select):
        super().__init__(parent)
        self.on_select = on_select
        
        tk.Label(self, text=label, font=('Arial', 10, 'bold')).pack(anchor=tk.W)
        
        entry_frame = tk.Frame(self)
        entry_frame.pack(fill=tk.X, pady=5)
        
        self.path_var = tk.StringVar()
        self.entry = tk.Entry(entry_frame, textvariable=self.path_var)
        self.entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0, 10))
        
        tk.Button(entry_frame, text="Browse...", 
                 command=self._browse).pack(side=tk.LEFT)
    
    def _browse(self):
        path = filedialog.askdirectory()
        if path:
            self.path_var.set(path)
            self.on_select(path)
    
    def get_path(self):
        return self.path_var.get()

class StatusBar(tk.Frame):
    """Barra stato con icone colorate"""
    def __init__(self, parent):
        super().__init__(parent, bg='#E3F2FD', height=40)
        
        self.label = tk.Label(self, bg='#E3F2FD', 
                             font=('Arial', 10, 'bold'))
        self.label.pack(pady=10)
    
    def set_status(self, message, status='info'):
        colors = {
            'info': ('#1976D2', '#E3F2FD'),
            'success': ('#2E7D32', '#C8E6C9'),
            'warning': ('#F57C00', '#FFE0B2'),
            'error': ('#C62828', '#FFCDD2')
        }
        
        fg, bg = colors.get(status, colors['info'])
        self.config(bg=bg)
        self.label.config(text=message, fg=fg, bg=bg)
```

---

## 🎯 Come Aggiungere un'Estensione

1. **Crea il file nel modulo appropriato**
   ```bash
   # Esempio: nuovo analyzer
   touch core/advanced_analyzer.py
   ```

2. **Implementa la classe**
   ```python
   class AdvancedAnalyzer:
       def __init__(self):
           pass
       
       def analyze(self):
           pass
   ```

3. **Aggiungi a __init__.py**
   ```python
   # core/__init__.py
   from .advanced_analyzer import AdvancedAnalyzer
   __all__ = [..., 'AdvancedAnalyzer']
   ```

4. **Usa dove serve**
   ```python
   from core.advanced_analyzer import AdvancedAnalyzer
   analyzer = AdvancedAnalyzer()
   ```

## 📝 Best Practices

1. **Un file = Una responsabilità**
2. **Mantieni file < 300 righe**
3. **Documenta con docstring**
4. **Aggiungi test quando possibile**
5. **Segui naming conventions Python**
6. **Usa type hints quando utile**

## 🧪 Testing Extensions

```python
# tests/test_new_feature.py
import unittest
from core.new_feature import NewFeature

class TestNewFeature(unittest.TestCase):
    def test_something(self):
        feature = NewFeature()
        result = feature.do_something()
        self.assertTrue(result)
```

---

**L'architettura modulare è pronta per crescere! 🚀**
