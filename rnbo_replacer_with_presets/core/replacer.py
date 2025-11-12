"""
Core Replacer - File Replacement Logic
"""

import shutil
from pathlib import Path
from core.analyzer import RNBOAnalyzer
from templates.mainactivity import MainActivityTemplate
from templates.layout import LayoutTemplate
from templates.msploader import MSPLoaderTemplate
from templates.msploader_h import MSPLoaderHeaderTemplate
from templates.buffer_loader_h import BufferLoaderHeaderTemplate
from templates.buffer_loader_cpp import BufferLoaderCppTemplate
from templates.nativelib import NativeLibTemplate
from templates.build_gradle import BuildGradleTemplate
from templates.androidmanifest import AndroidManifestTemplate
from templates.cmakelists import CMakeListsTemplate
from templates.themes import ThemesTemplate
from templates.colors import ColorsTemplate



class RNBOReplacer:
    def __init__(self, template_path, export_path, logger):
        self.template = Path(template_path)
        self.export = Path(export_path)
        self.logger = logger
        self.analyzer = RNBOAnalyzer(export_path, logger)
        
        # Directories
        self.cpp_dir = self.template / "app" / "src" / "main" / "cpp"
        self.java_dir = self.template / "app" / "src" / "main" / "java"
        self.res_dir = self.template / "app" / "src" / "main" / "res" / "layout"
        self.assets_dir = self.template / "app" / "src" / "main" / "assets"
    
    def verify_paths(self):
        """Verifica che tutti i percorsi siano corretti"""
        self.logger.log("\n" + "=" * 80)
        self.logger.log("[VERIFICA] VERIFICA PERCORSI")
        self.logger.log("=" * 80)

        # Verifica esistenza
        if not self.template.exists():
            self.logger.log("[ERROR] Template non trovato!")
            return False

        if not self.export.exists():
            self.logger.log("[ERROR] Export non trovato!")
            return False

        # Verifica export
        self.logger.log("\n[CHECK] Verifica Export RNBO:")
        required_export = ['description.json', 'dependencies.json', 'presets.json', 'rnbo']

        all_found = True
        for item in required_export:
            path = self.export / item
            if path.exists():
                self.logger.log(f"  [OK] {item}")
            else:
                self.logger.log(f"  [ERROR] {item} - MANCANTE!")
                all_found = False

        # Trova file .cpp
        cpp_files = list(self.export.glob("*.cpp"))
        if cpp_files:
            self.logger.log(f"  [OK] File C++: {cpp_files[0].name}")
        else:
            self.logger.log("  [ERROR] Nessun file .cpp trovato!")
            all_found = False

        # Verifica template
        self.logger.log("\n[CHECK] Verifica Template Android:")

        if self.cpp_dir.exists():
            self.logger.log(f"  [OK] Directory cpp: {self.cpp_dir}")
        else:
            self.logger.log(f"  [ERROR] Directory cpp non trovata!")
            all_found = False

        if self.java_dir.exists():
            self.logger.log(f"  [OK] Directory java: {self.java_dir}")
        else:
            self.logger.log(f"  [ERROR] Directory java non trovata!")
            all_found = False

        if self.res_dir.exists():
            self.logger.log(f"  [OK] Directory layout: {self.res_dir}")
        else:
            self.logger.log(f"  [ERROR] Directory layout non trovata!")
            all_found = False

        if not self.assets_dir.exists():
            self.logger.log(f"  [INFO] Directory assets non esiste (verra creata)")

        if all_found:
            self.logger.log("\n[OK] Verifica completata!")
        else:
            self.logger.log("\n[ERROR] Verifica fallita - Alcuni file mancano")

        return all_found
    
    def replace_files(self, rename_cpp=True):
        """Esegue la sostituzione completa dei file"""
        try:
            self.logger.log("\n" + "=" * 80)
            self.logger.log("[START] INIZIO SOSTITUZIONE")
            self.logger.log("=" * 80)
            
            # Crea assets se non esiste
            self.assets_dir.mkdir(parents=True, exist_ok=True)
            
            # Step 1: Backup cpp
            self._backup_cpp()
            
            # Step 2: Sostituisci file cpp export
            self._replace_cpp_files(rename_cpp)
            
            # Step 3: Sostituisci JSON in cpp/
            self._replace_json_files()
            
            # Step 4: Copia description.json in assets/
            self._copy_description_to_assets()
            
            # Step 5: Sostituisci cartella rnbo
            self._replace_rnbo_folder()
            
            # Step 5a: Copia media/ in assets/ (BUFFER SUPPORT)
            self._copy_media_to_assets()
            
            # Step 5b: Genera BufferLoader files (BUFFER SUPPORT)
            self._generate_buffer_loader()
            
            # Step 5c: Genera MSPLoader.h (BUFFER SUPPORT)
            self._generate_msploader_header()
            
            # Step 5d: Genera native-lib.cpp (BUFFER SUPPORT)
            self._generate_nativelib()
            
            # Step 6: Applica MSPLoader ottimizzato
            self._apply_msploader()
            
            # Step 7: Genera MainActivity
            self._generate_mainactivity()

            # Step 8: Genera activity_main.xml
            self._generate_layout()

            # Step 9: Genera build.gradle
            self._generate_build_gradle()

            # Step 10: Genera AndroidManifest
            self._generate_android_manifest()

            # Step 11: Genera CMakeLists.txt con Oboe
            self._generate_cmakelists()

            # Step 12: Genera themes e colors
            self._generate_resources()

            self._log_completion()
            return True
            
        except Exception as e:
            self.logger.log(f"\n[ERROR] ERRORE: {str(e)}")
            import traceback
            self.logger.log(traceback.format_exc())
            return False

    def _backup_cpp(self):
        """Crea backup della directory cpp"""
        self.logger.log("\n[BACKUP] Step 1: Backup directory cpp...")
        backup_dir = self.cpp_dir.parent / "cpp_backup"

        if backup_dir.exists():
            shutil.rmtree(backup_dir)

        shutil.copytree(self.cpp_dir, backup_dir)
        self.logger.log(f"  [OK] Backup creato: {backup_dir}")

    def _backup_java(self, java_dir):
        """Crea backup dei file Java"""
        backup_dir = self.template / "app" / "src" / "main" / "java_backup"

        if backup_dir.exists():
            shutil.rmtree(backup_dir)

        shutil.copytree(java_dir, backup_dir / java_dir.name)
        self.logger.log(f"  [OK] Backup Java creato: {backup_dir}")

    def _replace_cpp_files(self, rename_cpp):
        """Sostituisce i file .cpp dell'export"""
        self.logger.log("\n[REPLACE] Step 2: Sostituzione file export C++...")
        cpp_files = list(self.export.glob("*.cpp"))

        if not cpp_files:
            self.logger.log("  [WARN] Nessun file .cpp trovato nell'export")
            return

        export_cpp = cpp_files[0]

        if rename_cpp:
            # Trova e sostituisci il vecchio file cpp
            old_cpp_files = [f for f in self.cpp_dir.glob("*.cpp")
                            if f.name not in ['MSPLoader.cpp', 'native-lib.cpp']]

            if old_cpp_files:
                old_name = old_cpp_files[0].name
                target_cpp = self.cpp_dir / old_name

                self.logger.log(f"  [DELETE] Rimuovo: {old_name}")
                old_cpp_files[0].unlink()

                self.logger.log(f"  [COPY] Copio: {export_cpp.name} -> {old_name}")
                shutil.copy2(export_cpp, target_cpp)
            else:
                self.logger.log(f"  [COPY] Copio: {export_cpp.name}")
                shutil.copy2(export_cpp, self.cpp_dir / export_cpp.name)
        else:
            self.logger.log(f"  [COPY] Copio: {export_cpp.name}")
            shutil.copy2(export_cpp, self.cpp_dir / export_cpp.name)

    def _replace_json_files(self):
        """Sostituisce i file JSON nella directory cpp"""
        self.logger.log("\n[JSON] Step 3: Sostituzione file JSON in cpp/...")
        json_files = ['description.json', 'dependencies.json', 'presets.json']

        for json_file in json_files:
            src = self.export / json_file
            dst = self.cpp_dir / json_file

            if src.exists():
                shutil.copy2(src, dst)
                self.logger.log(f"  [OK] {json_file}")
            else:
                self.logger.log(f"  [WARN] {json_file} non trovato")

    def _copy_description_to_assets(self):
        """Copia description.json e dependencies.json nella directory assets"""
        self.logger.log("\n[ASSETS] Step 4: Copia JSON files in assets/...")

        # Copy description.json
        desc_src = self.export / "description.json"
        desc_dst = self.assets_dir / "description.json"

        if desc_src.exists():
            shutil.copy2(desc_src, desc_dst)
            self.logger.log(f"  [OK] description.json -> assets/")
        else:
            self.logger.log(f"  [WARN] description.json non trovato!")

        # Copy dependencies.json (required by BufferLoader)
        deps_src = self.export / "dependencies.json"
        deps_dst = self.assets_dir / "dependencies.json"

        if deps_src.exists():
            shutil.copy2(deps_src, deps_dst)
            self.logger.log(f"  [OK] dependencies.json -> assets/")
        else:
            self.logger.log(f"  [WARN] dependencies.json non trovato!")

    def _replace_rnbo_folder(self):
        """Sostituisce la cartella rnbo"""
        self.logger.log("\n[RNBO] Step 5: Sostituzione cartella rnbo/...")
        rnbo_dst = self.cpp_dir / "rnbo"

        if rnbo_dst.exists():
            shutil.rmtree(rnbo_dst)

        shutil.copytree(self.export / "rnbo", rnbo_dst)
        self.logger.log("  [OK] Cartella rnbo sostituita")

    def _copy_media_to_assets(self):
        """Copia cartella media/ in assets/ per buffer support"""
        self.logger.log("\n[MEDIA] Step 5a: Copia cartella media/ in assets/ (BUFFER SUPPORT)...")
        media_src = self.export / "media"

        if media_src.exists() and media_src.is_dir():
            media_dst = self.assets_dir / "media"

            if media_dst.exists():
                shutil.rmtree(media_dst)

            shutil.copytree(media_src, media_dst)

            # Lista file copiati
            media_files = list(media_dst.glob("*"))
            self.logger.log(f"  [OK] Cartella media/ copiata: {len(media_files)} file")
            for f in media_files:
                size_kb = f.stat().st_size / 1024
                self.logger.log(f"    - {f.name} ({size_kb:.1f} KB)")
        else:
            self.logger.log("  [INFO] Nessuna cartella media/ trovata (nessun buffer da caricare)")

    def _generate_buffer_loader(self):
        """Genera BufferLoader.h e BufferLoader.cpp"""
        self.logger.log("\n[GENERATE] Step 5b: Generazione BufferLoader files (BUFFER SUPPORT)...")

        # BufferLoader.h
        buffer_h_path = self.cpp_dir / "BufferLoader.h"
        content_h = BufferLoaderHeaderTemplate.get_content()
        with open(buffer_h_path, 'w', encoding='utf-8') as f:
            f.write(content_h)
        self.logger.log("  [OK] BufferLoader.h generato")

        # BufferLoader.cpp
        buffer_cpp_path = self.cpp_dir / "BufferLoader.cpp"
        content_cpp = BufferLoaderCppTemplate.get_content()
        with open(buffer_cpp_path, 'w', encoding='utf-8') as f:
            f.write(content_cpp)
        self.logger.log("  [OK] BufferLoader.cpp generato")

    def _generate_msploader_header(self):
        """Genera MSPLoader.h aggiornato"""
        self.logger.log("\n[GENERATE] Step 5c: Generazione MSPLoader.h aggiornato (BUFFER SUPPORT)...")
        msploader_h_path = self.cpp_dir / "MSPLoader.h"

        content = MSPLoaderHeaderTemplate.get_content()
        with open(msploader_h_path, 'w', encoding='utf-8') as f:
            f.write(content)

        self.logger.log("  [OK] MSPLoader.h aggiornato con AssetManager support")

    def _generate_nativelib(self):
        """Genera native-lib.cpp con AssetManager support"""
        self.logger.log("\n[GENERATE] Step 5d: Generazione native-lib.cpp (BUFFER SUPPORT)...")

        package_name = self.analyzer.find_package_name(self.java_dir)
        nativelib_path = self.cpp_dir / "native-lib.cpp"

        content = NativeLibTemplate.generate(package_name)
        with open(nativelib_path, 'w', encoding='utf-8') as f:
            f.write(content)

        self.logger.log(f"  [OK] native-lib.cpp generato con package: {package_name}")

    def _apply_msploader(self):
        """Applica MSPLoader.cpp ottimizzato"""
        self.logger.log("\n[OPTIMIZE] Step 6: Applicazione MSPLoader.cpp ottimizzato...")
        msploader_path = self.cpp_dir / "MSPLoader.cpp"

        content = MSPLoaderTemplate.get_content()

        with open(msploader_path, 'w', encoding='utf-8') as f:
            f.write(content)

        self.logger.log("  [OK] MSPLoader.cpp ottimizzato applicato")

    def _generate_mainactivity(self):
        """Genera MainActivity.java con i parametri corretti"""
        self.logger.log("\n[GENERATE] Step 7: Generazione MainActivity con valori parametri corretti...")

        # Leggi parametri dall'export
        parameters = self.analyzer.read_parameters()
        package_name = self.analyzer.find_package_name(self.java_dir)

        # Trova directory MainActivity
        main_activity_dir = self.analyzer.find_mainactivity_dir(self.java_dir, package_name)

        # Backup
        self._backup_java(main_activity_dir)

        # Genera nuovo MainActivity
        content = MainActivityTemplate.generate(package_name, parameters)

        original_file = main_activity_dir / "MainActivity.java"
        with open(original_file, 'w', encoding='utf-8') as f:
            f.write(content)

        self.logger.log("  [OK] MainActivity.java aggiornato")

    def _generate_layout(self):
        """Genera activity_main.xml"""
        self.logger.log("\n[GENERATE] Step 8: Generazione activity_main.xml...")

        # Backup
        xml_file = self.res_dir / "activity_main.xml"
        if xml_file.exists():
            backup_dir = self.template / "app" / "src" / "main" / "java_backup"
            backup_dir.mkdir(parents=True, exist_ok=True)
            backup_file = backup_dir / "activity_main.xml.backup"
            shutil.copy2(xml_file, backup_file)
            self.logger.log(f"  [BACKUP] Backup XML: java_backup/activity_main.xml.backup")

        # Genera nuovo XML
        content = LayoutTemplate.get_content()

        with open(xml_file, 'w', encoding='utf-8') as f:
            f.write(content)

        self.logger.log("  [OK] activity_main.xml aggiornato")

    def _generate_build_gradle(self):
        """Genera build.gradle"""
        self.logger.log("\n[GENERATE] Step 9: Generazione build.gradle...")

        gradle_file = self.template / "app" / "build.gradle"

        # Backup
        if gradle_file.exists():
            backup_dir = self.template / "app" / "gradle_backup"
            backup_dir.mkdir(parents=True, exist_ok=True)
            backup_file = backup_dir / "build.gradle.backup"
            shutil.copy2(gradle_file, backup_file)
            self.logger.log(f"  [BACKUP] Backup Gradle: gradle_backup/build.gradle.backup")

        # Genera nuovo build.gradle
        content = BuildGradleTemplate.get_app_build_gradle()

        with open(gradle_file, 'w', encoding='utf-8') as f:
            f.write(content)

        self.logger.log("  [OK] build.gradle aggiornato")

    def _generate_android_manifest(self):
        """Genera AndroidManifest.xml"""
        self.logger.log("\n[GENERATE] Step 10: Generazione AndroidManifest.xml...")

        manifest_file = self.template / "app" / "src" / "main" / "AndroidManifest.xml"

        # Backup
        if manifest_file.exists():
            backup_dir = self.template / "app" / "src" / "main" / "manifest_backup"
            backup_dir.mkdir(parents=True, exist_ok=True)
            backup_file = backup_dir / "AndroidManifest.xml.backup"
            shutil.copy2(manifest_file, backup_file)
            self.logger.log(f"  [BACKUP] Backup Manifest: manifest_backup/AndroidManifest.xml.backup")

        # Genera nuovo AndroidManifest
        content = AndroidManifestTemplate.get_content()

        with open(manifest_file, 'w', encoding='utf-8') as f:
            f.write(content)

        self.logger.log("  [OK] AndroidManifest.xml aggiornato")

    def _generate_cmakelists(self):
        """Genera CMakeLists.txt con Oboe configurato"""
        self.logger.log("\n[GENERATE] Step 11: Generazione CMakeLists.txt...")

        cmake_file = self.cpp_dir / "CMakeLists.txt"

        # Backup
        if cmake_file.exists():
            backup_dir = self.cpp_dir.parent / "cpp_backup"
            backup_dir.mkdir(parents=True, exist_ok=True)
            backup_file = backup_dir / "CMakeLists.txt.backup"
            shutil.copy2(cmake_file, backup_file)
            self.logger.log(f"  [BACKUP] Backup CMake: cpp_backup/CMakeLists.txt.backup")

        # Genera nuovo CMakeLists
        content = CMakeListsTemplate.get_content()

        with open(cmake_file, 'w', encoding='utf-8') as f:
            f.write(content)

        self.logger.log("  [OK] CMakeLists.txt aggiornato con Oboe")

    def _generate_resources(self):
        """Genera themes.xml e colors.xml"""
        self.logger.log("\n[GENERATE] Step 12: Generazione resources (themes & colors)...")

        values_dir = self.template / "app" / "src" / "main" / "res" / "values"
        values_dir.mkdir(parents=True, exist_ok=True)

        # themes.xml
        themes_file = values_dir / "themes.xml"
        if themes_file.exists():
            backup_dir = self.template / "app" / "src" / "main" / "res_backup"
            backup_dir.mkdir(parents=True, exist_ok=True)
            shutil.copy2(themes_file, backup_dir / "themes.xml.backup")

        with open(themes_file, 'w', encoding='utf-8') as f:
            f.write(ThemesTemplate.get_content())

        self.logger.log("  [OK] themes.xml")

        # colors.xml
        colors_file = values_dir / "colors.xml"
        if colors_file.exists():
            backup_dir = self.template / "app" / "src" / "main" / "res_backup"
            backup_dir.mkdir(parents=True, exist_ok=True)
            shutil.copy2(colors_file, backup_dir / "colors.xml.backup")

        with open(colors_file, 'w', encoding='utf-8') as f:
            f.write(ColorsTemplate.get_content())

        self.logger.log("  [OK] colors.xml")

    def _log_completion(self):
        """Log di completamento"""
        self.logger.log("\n" + "=" * 80)
        self.logger.log("[SUCCESS] SOSTITUZIONE COMPLETATA CON SUCCESSO!")
        self.logger.log("=" * 80)
        self.logger.log("\n[SUMMARY] Riepilogo:")
        self.logger.log(f"  - Backup cpp: cpp_backup/")
        self.logger.log(f"  - Backup java/xml: java_backup/")
        self.logger.log(f"  - Backup gradle: gradle_backup/")
        self.logger.log(f"  - Backup manifest: manifest_backup/")
        self.logger.log("  - File C++ sostituiti")
        self.logger.log("  - description.json in assets/")
        self.logger.log("  - MSPLoader ottimizzato applicato")
        self.logger.log("  - MainActivity generato")
        self.logger.log("  - activity_main.xml generato")
        self.logger.log("  - build.gradle aggiornato")
        self.logger.log("  - AndroidManifest aggiornato")
        self.logger.log("  - CMakeLists.txt con Oboe configurato")
        self.logger.log("\n[BUILD] Compila ora:")
        self.logger.log("   gradlew clean")
        self.logger.log("   gradlew build")
        self.logger.log("\n[TEST] Test:")
        self.logger.log("   1. Run app su device")
        self.logger.log("   2. Verifica parametri RNBO funzionanti")
