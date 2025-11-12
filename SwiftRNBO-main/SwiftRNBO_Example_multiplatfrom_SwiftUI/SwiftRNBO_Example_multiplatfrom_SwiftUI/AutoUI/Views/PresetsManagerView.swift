//
//  PresetsManagerView.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  Presets Manager UI for PRESETS mode
//  Matches Android's preset manager layout
//

import SwiftUI

/// Presets Manager View - Shows in PRESETS mode
/// Replicates Android's preset manager UI (Lines 1370-1434)
struct PresetsManagerView: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel
    @EnvironmentObject var modeManager: ModeManager
    @EnvironmentObject var presetManager: PresetManager
    @EnvironmentObject var xyPadManager: XYPadManager

    @Binding var parameters: [RNBOParameter]

    @State private var showSaveDialog = false
    @State private var newPresetName = ""
    @State private var presetToRename: Preset?
    @State private var renameText = ""

    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 16) {
                // Header
                Text("PRESETS MANAGER")
                    .font(.title2)
                    .fontWeight(.bold)
                    .foregroundColor(.primary)
                    .padding(.horizontal)
                    .padding(.top, 8)

                // Save Current Configuration button
                Button(action: {
                    showSaveDialog = true
                }) {
                    Text("SAVE CURRENT CONFIGURATION")
                        .font(.headline)
                        .fontWeight(.bold)
                        .foregroundColor(.black)
                        .frame(maxWidth: .infinity)
                        .padding()
                        .background(Color.white)
                        .cornerRadius(8)
                }
                .padding(.horizontal)

                // Interpolation Settings
                interpolationSection

                Divider()
                    .padding(.horizontal)

                // Presets List
                if presetManager.savedPresets.isEmpty {
                    emptyStateView
                } else {
                    presetsListView
                }
            }
            .padding(.vertical)
        }
        .alert("Save Preset", isPresented: $showSaveDialog) {
            TextField("Preset Name", text: $newPresetName)
            Button("Cancel", role: .cancel) { }
            Button("Save") {
                saveCurrentPreset()
            }
        } message: {
            Text("Enter a name for this preset")
        }
        .alert("Rename Preset", isPresented: .constant(presetToRename != nil)) {
            TextField("New Name", text: $renameText)
            Button("Cancel", role: .cancel) {
                presetToRename = nil
            }
            Button("Rename") {
                if let preset = presetToRename {
                    presetManager.renamePreset(preset, newName: renameText)
                    presetToRename = nil
                }
            }
        }
    }

    // MARK: - Interpolation Section

    private var interpolationSection: some View {
        VStack(alignment: .leading, spacing: 12) {
            Text("MORPHING / INTERPOLATION")
                .font(.headline)
                .foregroundColor(.primary)

            // Enable toggle
            Toggle(isOn: $presetManager.interpolationEnabled) {
                Text("Enable morphing between presets")
                    .font(.subheadline)
            }
            .toggleStyle(SwitchToggleStyle(tint: .blue))

            // Time slider (shown when enabled)
            if presetManager.interpolationEnabled {
                VStack(alignment: .leading, spacing: 4) {
                    Text("Morph Time: \(presetManager.interpolationTimeMs)ms")
                        .font(.caption)
                        .foregroundColor(.secondary)

                    Slider(
                        value: Binding(
                            get: { Double(presetManager.interpolationTimeMs) },
                            set: { presetManager.interpolationTimeMs = Int($0) }
                        ),
                        in: 100...5000,
                        step: 100
                    )
                    .accentColor(.blue)

                    HStack {
                        Text("100ms")
                            .font(.caption2)
                            .foregroundColor(.secondary)
                        Spacer()
                        Text("5000ms")
                            .font(.caption2)
                            .foregroundColor(.secondary)
                    }
                }
            }
        }
        .padding()
        .background(
            RoundedRectangle(cornerRadius: 8)
                .fill(Color.gray.opacity(0.1))
        )
        .padding(.horizontal)
    }

    // MARK: - Presets List

    private var presetsListView: some View {
        VStack(spacing: 12) {
            ForEach(presetManager.savedPresets) { preset in
                PresetCard(
                    preset: preset,
                    onLoad: { loadPreset(preset) },
                    onRename: { startRename(preset) },
                    onDelete: { deletePreset(preset) },
                    onToggleQuick: { presetManager.toggleQuickPreset(preset) },
                    isQuick: presetManager.isQuickPreset(preset)
                )
            }
        }
        .padding(.horizontal)
    }

    private var emptyStateView: some View {
        VStack(spacing: 8) {
            Image(systemName: "square.stack.3d.up.slash")
                .font(.system(size: 48))
                .foregroundColor(.secondary)
            Text("No Presets Saved")
                .font(.headline)
                .foregroundColor(.secondary)
            Text("Save your first preset to get started")
                .font(.caption)
                .foregroundColor(.secondary)
        }
        .frame(maxWidth: .infinity)
        .padding(.vertical, 40)
    }

    // MARK: - Actions

    private func saveCurrentPreset() {
        guard !newPresetName.isEmpty else { return }

        presetManager.saveCurrentAsPreset(
            name: newPresetName,
            parameters: parameters,
            customRanges: modeManager.customRanges,
            liveParameters: modeManager.selectedParameterIds,
            xyPadMappings: xyPadManager.mappings
        )

        newPresetName = ""
    }

    private func loadPreset(_ preset: Preset) {
        presetManager.loadPreset(
            preset,
            parameters: &parameters,
            customRanges: &modeManager.customRanges,
            liveParameters: &modeManager.selectedParameterIds,
            xyPadManager: xyPadManager,
            rnbo: rnbo
        )

        // Switch to LIVE mode after loading (like Android)
        modeManager.switchMode(to: .live)
    }

    private func startRename(_ preset: Preset) {
        presetToRename = preset
        renameText = preset.name
    }

    private func deletePreset(_ preset: Preset) {
        presetManager.deletePreset(preset)
    }
}

// MARK: - Preset Card

/// Individual preset card
/// Matches Android's preset item layout (Lines 1439-1523)
struct PresetCard: View {
    let preset: Preset
    let onLoad: () -> Void
    let onRename: () -> Void
    let onDelete: () -> Void
    let onToggleQuick: () -> Void
    let isQuick: Bool

    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            // Preset name and timestamp
            VStack(alignment: .leading, spacing: 4) {
                Text(preset.name)
                    .font(.headline)
                    .foregroundColor(.primary)

                Text(preset.formattedTimestamp)
                    .font(.caption)
                    .foregroundColor(.secondary)
            }

            // Action buttons
            HStack(spacing: 12) {
                // LOAD button
                Button(action: onLoad) {
                    Text("LOAD")
                        .font(.caption)
                        .fontWeight(.semibold)
                        .foregroundColor(.white)
                        .padding(.horizontal, 12)
                        .padding(.vertical, 6)
                        .background(Color.blue)
                        .cornerRadius(6)
                }

                // RENAME button
                Button(action: onRename) {
                    Text("RENAME")
                        .font(.caption)
                        .fontWeight(.semibold)
                        .foregroundColor(.white)
                        .padding(.horizontal, 12)
                        .padding(.vertical, 6)
                        .background(Color.orange)
                        .cornerRadius(6)
                }

                // DELETE button
                Button(action: onDelete) {
                    Text("DELETE")
                        .font(.caption)
                        .fontWeight(.semibold)
                        .foregroundColor(.white)
                        .padding(.horizontal, 12)
                        .padding(.vertical, 6)
                        .background(Color.red)
                        .cornerRadius(6)
                }

                Spacer()

                // Q (Quick) button
                Button(action: onToggleQuick) {
                    Text("Q")
                        .font(.caption)
                        .fontWeight(.bold)
                        .foregroundColor(isQuick ? .white : .primary)
                        .frame(width: 28, height: 28)
                        .background(isQuick ? Color.green : Color.gray.opacity(0.3))
                        .cornerRadius(6)
                }
            }
        }
        .padding()
        .background(
            RoundedRectangle(cornerRadius: 8)
                .fill(Color.gray.opacity(0.15))
        )
    }
}

// MARK: - Preview

#if DEBUG
struct PresetsManagerView_Previews: PreviewProvider {
    static var previews: some View {
        let mockParameterInfo = ParameterInfo(
            type: "ParameterTypeNumber",
            index: 0,
            name: "/frequency",
            paramId: "frequency",
            minimum: 20.0,
            maximum: 20000.0,
            exponent: 1.0,
            steps: 0,
            initialValue: 440.0,
            isEnum: false,
            enumValues: [],
            displayName: "Frequency",
            unit: "Hz",
            order: 0,
            debug: false,
            visible: true,
            signalIndex: nil,
            ioType: "IOTypeUndefined"
        )

        let mockParameter = RNBOParameter(mockParameterInfo)

        return PresetsManagerView(parameters: .constant([mockParameter]))
            .environmentObject(RNBOAudioUnitHostModel())
            .environmentObject(ModeManager())
            .environmentObject(PresetManager())
    }
}
#endif
