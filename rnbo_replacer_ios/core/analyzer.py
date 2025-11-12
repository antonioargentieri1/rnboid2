"""
RNBO Export Analyzer - Reads and analyzes RNBO export files
"""

import json
from pathlib import Path


class RNBOAnalyzer:
    """Analyzes RNBO export directory and extracts metadata"""

    def __init__(self, export_path, logger):
        self.export_path = Path(export_path)
        self.logger = logger
        self.description = None
        self.parameters = []
        self.dependencies = None
        self.presets = None

    def analyze(self):
        """Analyze the RNBO export directory"""
        self.logger.section("ANALYZING RNBO EXPORT")

        # Read description.json
        desc_file = self.export_path / "description.json"
        if desc_file.exists():
            with open(desc_file, 'r') as f:
                self.description = json.load(f)

            # Extract parameters
            if 'parameters' in self.description:
                self.parameters = self.description['parameters']
                self.logger.success(f"Found {len(self.parameters)} parameters")

                # Log visible parameters
                visible = [p for p in self.parameters if p.get('visible', True)]
                self.logger.info(f"  - {len(visible)} visible parameters")
            else:
                self.logger.warning("No parameters found in description.json")
        else:
            self.logger.error("description.json not found!")
            return False

        # Read dependencies.json
        dep_file = self.export_path / "dependencies.json"
        if dep_file.exists():
            with open(dep_file, 'r') as f:
                self.dependencies = json.load(f)
            self.logger.success("Found dependencies.json")
        else:
            self.logger.info("dependencies.json not found (optional)")

        # Read presets.json
        preset_file = self.export_path / "presets.json"
        if preset_file.exists():
            with open(preset_file, 'r') as f:
                self.presets = json.load(f)

            # Handle both list and dict formats
            if isinstance(self.presets, list):
                preset_count = len(self.presets)
            elif isinstance(self.presets, dict):
                preset_count = len(self.presets.get('presets', []))
            else:
                preset_count = 0

            self.logger.success(f"Found {preset_count} presets")
        else:
            self.logger.info("presets.json not found (optional)")

        # Find .dylib file (macOS) or other library files
        dylib_files = list(self.export_path.glob("*.dylib"))
        if dylib_files:
            self.logger.success(f"Found library: {dylib_files[0].name}")
        else:
            self.logger.warning("No .dylib file found")

        # Find C++ source files
        cpp_files = list(self.export_path.glob("*.cpp"))
        if cpp_files:
            self.logger.success(f"Found C++ source: {cpp_files[0].name}")
        else:
            self.logger.warning("No .cpp file found")

        # Find header files
        h_files = list(self.export_path.glob("*.h"))
        if h_files:
            self.logger.success(f"Found {len(h_files)} header files")

        self.logger.success("Export analysis complete!")
        return True

    def get_patch_name(self):
        """Get the patch name from description"""
        if self.description:
            return self.description.get('desc', {}).get('name', 'RNBOPatch')
        return 'RNBOPatch'

    def get_visible_parameters(self):
        """Get list of visible parameters"""
        return [p for p in self.parameters if p.get('visible', True)]

    def get_parameter_count(self):
        """Get total parameter count"""
        return len(self.parameters)
