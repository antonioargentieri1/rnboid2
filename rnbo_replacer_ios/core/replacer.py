"""
Core Replacer iOS/macOS - File Replacement Logic
"""

import shutil
import uuid
from pathlib import Path
from core.analyzer import RNBOAnalyzer


class IOSReplacer:
    """Main replacer class for iOS/macOS projects"""

    def __init__(self, template_path, export_path, output_path, logger):
        self.template = Path(template_path)
        self.export = Path(export_path)
        self.output = Path(output_path)
        self.logger = logger
        self.analyzer = RNBOAnalyzer(export_path, logger)

        # Template directories (relative to SwiftRNBO-main)
        self.template_project_dir = (
            self.template / "SwiftRNBO_Example_multiplatfrom_SwiftUI" /
            "SwiftRNBO_Example_multiplatfrom_SwiftUI"
        )
        self.template_rnbo_dir = self.template_project_dir / "RNBO"

    def verify_paths(self):
        """Verify that all required paths exist"""
        self.logger.section("VERIFYING PATHS")

        all_valid = True

        # Check template
        if not self.template.exists():
            self.logger.error(f"Template not found: {self.template}")
            all_valid = False
        else:
            self.logger.success(f"Template found: {self.template}")

        # Check template project structure
        if not self.template_project_dir.exists():
            self.logger.error(f"Template project not found: {self.template_project_dir}")
            all_valid = False
        else:
            self.logger.success("Template project structure found")

        # Check AutoUI files
        autoui_dir = self.template_project_dir / "AutoUI"
        if not autoui_dir.exists():
            self.logger.error("AutoUI directory not found in template!")
            all_valid = False
        else:
            autoui_files = [
                "Core/ModeManager.swift",
                "Core/ParameterFilter.swift",
                "Core/AutoUIView.swift",
                "Core/Preset.swift",
                "Core/PresetManager.swift",
                "Core/XYPadMapping.swift",
                "Core/XYPadManager.swift",
                "Widgets/AutoSlider.swift",
                "Widgets/ParameterRow.swift",
                "Widgets/RangeSlider.swift",
                "Views/PresetsManagerView.swift",
                "Views/QuickPresetsView.swift",
                "Views/XYPadView.swift"
            ]
            for file in autoui_files:
                if (autoui_dir / file).exists():
                    self.logger.success(f"  ✓ {file}")
                else:
                    self.logger.error(f"  ✗ {file} missing!")
                    all_valid = False

        # Check export
        if not self.export.exists():
            self.logger.error(f"Export not found: {self.export}")
            all_valid = False
        else:
            self.logger.success(f"Export found: {self.export}")

        # Check output (should not exist)
        if self.output.exists():
            self.logger.warning(f"Output directory already exists: {self.output}")
            response = input("  Overwrite? (y/n): ")
            if response.lower() != 'y':
                self.logger.error("Operation cancelled by user")
                all_valid = False

        if all_valid:
            self.logger.success("All paths verified!")
        else:
            self.logger.error("Path verification failed!")

        return all_valid

    def copy_template(self):
        """Copy template to output directory"""
        self.logger.section("COPYING TEMPLATE")

        try:
            # Remove output if exists
            if self.output.exists():
                shutil.rmtree(self.output)

            # Copy only the Xcode project directory (not the entire SwiftRNBO-main folder)
            project_dir = self.template / "SwiftRNBO_Example_multiplatfrom_SwiftUI"

            if not project_dir.exists():
                self.logger.error(f"Project directory not found: {project_dir}")
                return False

            shutil.copytree(project_dir, self.output)
            self.logger.success(f"Template copied to: {self.output}")

            return True

        except Exception as e:
            self.logger.error(f"Failed to copy template: {e}")
            return False

    def replace_rnbo_export(self):
        """Replace RNBO export files in the output project"""
        self.logger.section("REPLACING RNBO EXPORT")

        try:
            # Target RNBO directory in output
            output_rnbo_dir = (
                self.output / "SwiftRNBO_Example_multiplatfrom_SwiftUI" / "RNBO" / "Export"
            )

            if not output_rnbo_dir.exists():
                self.logger.error(f"Output RNBO directory not found: {output_rnbo_dir}")
                return False

            # Clear existing export
            for item in output_rnbo_dir.glob("*"):
                if item.is_file():
                    item.unlink()
                elif item.is_dir():
                    shutil.rmtree(item)

            self.logger.info("Cleared existing RNBO export")

            # Copy new export files
            copied_files = []
            for item in self.export.glob("*"):
                dest = output_rnbo_dir / item.name
                if item.is_file():
                    shutil.copy2(item, dest)
                    copied_files.append(item.name)
                elif item.is_dir():
                    shutil.copytree(item, dest)
                    copied_files.append(f"{item.name}/")

            self.logger.success(f"Copied {len(copied_files)} items:")
            for file in copied_files:
                self.logger.info(f"  ✓ {file}")

            return True

        except Exception as e:
            self.logger.error(f"Failed to replace RNBO export: {e}")
            return False

    def update_project_config(self, app_name=None):
        """Update Xcode project configuration"""
        self.logger.section("UPDATING PROJECT CONFIGURATION")

        try:
            # Generate unique bundle identifier
            if not app_name:
                app_name = self.analyzer.get_patch_name()

            bundle_id = f"com.rnbo.{app_name.lower().replace(' ', '').replace('_', '')}"
            self.logger.info(f"Bundle Identifier: {bundle_id}")

            # Find project.pbxproj file
            project_file = (
                self.output / "SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj" / "project.pbxproj"
            )

            if not project_file.exists():
                self.logger.error("project.pbxproj not found!")
                return False

            # Read project file
            with open(project_file, 'r') as f:
                content = f.read()

            # Replace bundle identifier (find and replace pattern)
            # Note: This is a simple replacement, might need adjustment
            old_bundle_id = "com.EldarSadykov.SwiftRNBO-Example-multiplatfrom-SwiftUI"
            if old_bundle_id in content:
                content = content.replace(old_bundle_id, bundle_id)
                self.logger.success(f"Updated bundle identifier to: {bundle_id}")
            else:
                self.logger.warning("Could not find bundle identifier to replace")

            # Write back
            with open(project_file, 'w') as f:
                f.write(content)

            self.logger.success("Project configuration updated!")
            return True

        except Exception as e:
            self.logger.error(f"Failed to update project config: {e}")
            return False

    def run(self, app_name=None):
        """Run the complete replacement process"""
        self.logger.section("🚀 STARTING IOS REPLACER")

        # Step 1: Verify paths
        if not self.verify_paths():
            return False

        # Step 2: Analyze export
        if not self.analyzer.analyze():
            return False

        # Step 3: Copy template
        if not self.copy_template():
            return False

        # Step 4: Replace RNBO export
        if not self.replace_rnbo_export():
            return False

        # Step 5: Update project config
        if not self.update_project_config(app_name):
            return False

        # Success!
        self.logger.section("✅ SUCCESS!")
        self.logger.success(f"Xcode project created: {self.output}")
        self.logger.info("\nNext steps:")
        self.logger.info("1. cd " + str(self.output))
        self.logger.info("2. open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj")
        self.logger.info("3. Select device and press Cmd+R to build & run")

        return True
