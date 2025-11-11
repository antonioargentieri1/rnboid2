"""
RNBO Analyzer - Extract information from RNBO export
"""

import json
import re
from pathlib import Path


class RNBOAnalyzer:
    def __init__(self, export_path, logger):
        self.export = Path(export_path)
        self.logger = logger
    
    def read_parameters(self):
        """Legge i parametri dal description.json"""
        desc_file = self.export / "description.json"

        if not desc_file.exists():
            self.logger.log("[WARN] description.json non trovato!")
            return []

        try:
            with open(desc_file, 'r', encoding='utf-8') as f:
                data = json.load(f)
                parameters = data.get('parameters', [])

                self.logger.log(f"  [INFO] Trovati {len(parameters)} parametri")
                return parameters

        except Exception as e:
            self.logger.log(f"[WARN] Errore lettura description.json: {e}")
            return []

    def find_package_name(self, java_dir):
        """Trova il package name dal MainActivity.java esistente"""
        for java_file in java_dir.rglob("MainActivity.java"):
            try:
                with open(java_file, 'r', encoding='utf-8') as f:
                    content = f.read()
                    match = re.search(r'package\s+([\w\.]+);', content)
                    if match:
                        package_name = match.group(1)
                        self.logger.log(f"  [PACKAGE] Package trovato: {package_name}")
                        return package_name
            except Exception as e:
                self.logger.log(f"[WARN] Errore lettura {java_file}: {e}")

        # Default fallback
        default_package = "com.example.rnbo_test"
        self.logger.log(f"  [PACKAGE] Uso package default: {default_package}")
        return default_package

    def find_mainactivity_dir(self, java_dir, package_name):
        """Trova la directory dove si trova MainActivity.java"""
        # Cerca prima MainActivity.java esistente
        for java_file in java_dir.rglob("MainActivity.java"):
            return java_file.parent

        # Se non esiste, crea la struttura dal package name
        package_path = package_name.replace('.', '/')
        main_activity_dir = java_dir / package_path
        main_activity_dir.mkdir(parents=True, exist_ok=True)

        self.logger.log(f"  [DIR] Directory MainActivity: {main_activity_dir}")
        return main_activity_dir
    
    def get_parameter_info(self, param):
        """Estrae informazioni da un parametro"""
        return {
            'name': param.get('name', 'unknown'),
            'min': param.get('min', 0.0),
            'max': param.get('max', 1.0),
            'initialValue': param.get('initialValue', 0.0),
            'visible': param.get('visible', True)
        }
