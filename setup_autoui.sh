#!/bin/bash
# SwiftRNBO AutoUI - Setup Script
# Adds AutoUI files to Xcode project automatically

set -e

echo "🚀 SwiftRNBO AutoUI Setup Script"
echo "================================="
echo ""

PROJECT_DIR="SwiftRNBO-main/SwiftRNBO_Example_multiplatfrom_SwiftUI"
XCODEPROJ="$PROJECT_DIR/SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj"

# Check if we're in the right directory
if [ ! -d "$PROJECT_DIR" ]; then
    echo "❌ Error: Cannot find $PROJECT_DIR"
    echo "   Please run this script from the rnboid2 root directory"
    exit 1
fi

echo "✅ Found project at: $PROJECT_DIR"
echo ""

# Generate UUIDs for new files
echo "📝 Generating file references..."

# Function to generate a unique 24-character hex string (Xcode UUID format)
generate_uuid() {
    echo "$(LC_ALL=C tr -dc 'A-F0-9' < /dev/urandom | head -c 24)"
}

# Generate UUIDs
UUID_MODE_MANAGER_REF=$(generate_uuid)
UUID_MODE_MANAGER_BUILD=$(generate_uuid)
UUID_PARAM_FILTER_REF=$(generate_uuid)
UUID_PARAM_FILTER_BUILD=$(generate_uuid)
UUID_AUTOUI_VIEW_REF=$(generate_uuid)
UUID_AUTOUI_VIEW_BUILD=$(generate_uuid)
UUID_AUTO_SLIDER_REF=$(generate_uuid)
UUID_AUTO_SLIDER_BUILD=$(generate_uuid)
UUID_PARAM_ROW_REF=$(generate_uuid)
UUID_PARAM_ROW_BUILD=$(generate_uuid)

UUID_AUTOUI_GROUP=$(generate_uuid)
UUID_CORE_GROUP=$(generate_uuid)
UUID_WIDGETS_GROUP=$(generate_uuid)

echo "✅ Generated UUIDs for 5 Swift files"
echo ""

# Backup original project file
echo "💾 Creating backup..."
cp "$XCODEPROJ/project.pbxproj" "$XCODEPROJ/project.pbxproj.backup"
echo "✅ Backup created: project.pbxproj.backup"
echo ""

# Create modified project file
echo "🔧 Modifying Xcode project..."

PBXPROJ="$XCODEPROJ/project.pbxproj"

# Add PBXBuildFile entries (after line 35, before "/* End PBXBuildFile section */")
sed -i.tmp "/\/\* End PBXBuildFile section \*\//i\\
		$UUID_MODE_MANAGER_BUILD /* ModeManager.swift in Sources */ = {isa = PBXBuildFile; fileRef = $UUID_MODE_MANAGER_REF /* ModeManager.swift */; };\\
		$UUID_PARAM_FILTER_BUILD /* ParameterFilter.swift in Sources */ = {isa = PBXBuildFile; fileRef = $UUID_PARAM_FILTER_REF /* ParameterFilter.swift */; };\\
		$UUID_AUTOUI_VIEW_BUILD /* AutoUIView.swift in Sources */ = {isa = PBXBuildFile; fileRef = $UUID_AUTOUI_VIEW_REF /* AutoUIView.swift */; };\\
		$UUID_AUTO_SLIDER_BUILD /* AutoSlider.swift in Sources */ = {isa = PBXBuildFile; fileRef = $UUID_AUTO_SLIDER_REF /* AutoSlider.swift */; };\\
		$UUID_PARAM_ROW_BUILD /* ParameterRow.swift in Sources */ = {isa = PBXBuildFile; fileRef = $UUID_PARAM_ROW_REF /* ParameterRow.swift */; };\\
" "$PBXPROJ"

# Add PBXFileReference entries
sed -i.tmp2 "/\/\* End PBXFileReference section \*\//i\\
		$UUID_MODE_MANAGER_REF /* ModeManager.swift */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.swift; path = ModeManager.swift; sourceTree = \"<group>\"; };\\
		$UUID_PARAM_FILTER_REF /* ParameterFilter.swift */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.swift; path = ParameterFilter.swift; sourceTree = \"<group>\"; };\\
		$UUID_AUTOUI_VIEW_REF /* AutoUIView.swift */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.swift; path = AutoUIView.swift; sourceTree = \"<group>\"; };\\
		$UUID_AUTO_SLIDER_REF /* AutoSlider.swift */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.swift; path = AutoSlider.swift; sourceTree = \"<group>\"; };\\
		$UUID_PARAM_ROW_REF /* ParameterRow.swift */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.swift; path = ParameterRow.swift; sourceTree = \"<group>\"; };\\
" "$PBXPROJ"

# Add to PBXGroup (folder structure)
# Find the main group and add AutoUI folder reference
sed -i.tmp3 "/ContentView.swift/a\\
				$UUID_AUTOUI_GROUP /* AutoUI */,\\
" "$PBXPROJ"

# Add AutoUI group definition (before "/* End PBXGroup section */")
sed -i.tmp4 "/\/\* End PBXGroup section \*\//i\\
		$UUID_AUTOUI_GROUP /* AutoUI */ = {\\
			isa = PBXGroup;\\
			children = (\\
				$UUID_CORE_GROUP /* Core */,\\
				$UUID_WIDGETS_GROUP /* Widgets */,\\
			);\\
			path = AutoUI;\\
			sourceTree = \"<group>\";\\
		};\\
		$UUID_CORE_GROUP /* Core */ = {\\
			isa = PBXGroup;\\
			children = (\\
				$UUID_MODE_MANAGER_REF /* ModeManager.swift */,\\
				$UUID_PARAM_FILTER_REF /* ParameterFilter.swift */,\\
				$UUID_AUTOUI_VIEW_REF /* AutoUIView.swift */,\\
			);\\
			path = Core;\\
			sourceTree = \"<group>\";\\
		};\\
		$UUID_WIDGETS_GROUP /* Widgets */ = {\\
			isa = PBXGroup;\\
			children = (\\
				$UUID_AUTO_SLIDER_REF /* AutoSlider.swift */,\\
				$UUID_PARAM_ROW_REF /* ParameterRow.swift */,\\
			);\\
			path = Widgets;\\
			sourceTree = \"<group>\";\\
		};\\
" "$PBXPROJ"

# Add to PBXSourcesBuildPhase (compilation)
sed -i.tmp5 "/SliderView.swift in Sources/a\\
				$UUID_MODE_MANAGER_BUILD /* ModeManager.swift in Sources */,\\
				$UUID_PARAM_FILTER_BUILD /* ParameterFilter.swift in Sources */,\\
				$UUID_AUTOUI_VIEW_BUILD /* AutoUIView.swift in Sources */,\\
				$UUID_AUTO_SLIDER_BUILD /* AutoSlider.swift in Sources */,\\
				$UUID_PARAM_ROW_BUILD /* ParameterRow.swift in Sources */,\\
" "$PBXPROJ"

# Clean up temp files
rm -f "$PBXPROJ.tmp" "$PBXPROJ.tmp2" "$PBXPROJ.tmp3" "$PBXPROJ.tmp4" "$PBXPROJ.tmp5"

echo "✅ Xcode project modified successfully!"
echo ""
echo "================================================"
echo "✅ SETUP COMPLETATO!"
echo "================================================"
echo ""
echo "Prossimi step:"
echo "1. Apri Xcode:"
echo "   cd $PROJECT_DIR"
echo "   open SwiftRNBO_Example_multiplatfrom_SwiftUI.xcodeproj"
echo ""
echo "2. Seleziona device (iPhone Simulator o My Mac)"
echo ""
echo "3. Premi ▶️ (Cmd+R) per compilare ed eseguire"
echo ""
echo "4. Testa le 3 modalità: [Live] [All] [Setup]"
echo ""
echo "📚 Guida completa: SwiftRNBO-main/TESTING.md"
echo ""
echo "🔙 Backup disponibile in:"
echo "   $XCODEPROJ/project.pbxproj.backup"
echo ""
