//
//  QuickPresetsView.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  Quick Presets buttons for LIVE mode fixed header
//  Matches Android's quick presets layout (Lines 2271-2331)
//

import SwiftUI

/// Quick Presets View - Shows 8 quick preset buttons in LIVE mode
/// Replicates Android's quick presets in fixed header
struct QuickPresetsView: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel
    @EnvironmentObject var modeManager: ModeManager
    @EnvironmentObject var presetManager: PresetManager
    @EnvironmentObject var xyPadManager: XYPadManager

    @Binding var parameters: [RNBOParameter]

    // Grid layout: 4 columns (like Android)
    private let columns = [
        GridItem(.flexible()),
        GridItem(.flexible()),
        GridItem(.flexible()),
        GridItem(.flexible())
    ]

    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            // XY Pad toggle button (matches Android Lines 331-338)
            xyPadToggleButton

            // Quick Presets section
            if !presetManager.quickPresets.isEmpty {
                quickPresetsSection
            }

            // Interpolation progress (shown when morphing)
            if presetManager.isInterpolating {
                interpolationProgressView
            }
        }
        .padding(.vertical, 8)
        .background(
            RoundedRectangle(cornerRadius: 8)
                .fill(Color.gray.opacity(0.1))
        )
        .padding(.horizontal)
    }

    // MARK: - XY Pad Toggle Button

    private var xyPadToggleButton: some View {
        Button(action: {
            xyPadManager.isEnabled.toggle()
        }) {
            Text(xyPadManager.isEnabled ? "XY PAD ON" : "XY PAD OFF")
                .font(.caption)
                .fontWeight(.bold)
                .foregroundColor(xyPadManager.isEnabled ? .black : .white)
                .frame(maxWidth: .infinity)
                .padding(.vertical, 10)
                .background(xyPadManager.isEnabled ? Color.white : Color.black)
                .cornerRadius(6)
        }
        .buttonStyle(.plain)
        .padding(.horizontal)
    }

    // MARK: - Quick Presets Section

    private var quickPresetsSection: some View {
        VStack(alignment: .leading, spacing: 8) {
            Text("QUICK PRESETS")
                .font(.caption)
                .fontWeight(.semibold)
                .foregroundColor(.secondary)
                .padding(.horizontal)

            LazyVGrid(columns: columns, spacing: 8) {
                ForEach(Array(presetManager.quickPresets.enumerated()), id: \.element.id) { index, preset in
                    QuickPresetButton(
                        number: index + 1,
                        preset: preset,
                        onTap: {
                            loadQuickPreset(preset)
                        }
                    )
                }
            }
            .padding(.horizontal)
        }
    }

    // MARK: - Interpolation Progress

    private var interpolationProgressView: some View {
        VStack(spacing: 4) {
            HStack {
                Text("Morphing...")
                    .font(.caption)
                    .foregroundColor(.blue)
                Spacer()
                Text("\(Int(presetManager.interpolationProgress * 100))%")
                    .font(.caption)
                    .foregroundColor(.blue)
            }

            ProgressView(value: presetManager.interpolationProgress, total: 1.0)
                .progressViewStyle(LinearProgressViewStyle(tint: .blue))
        }
        .padding(.horizontal)
    }

    // MARK: - Actions

    private func loadQuickPreset(_ preset: Preset) {
        presetManager.loadPreset(
            preset,
            parameters: &parameters,
            customRanges: &modeManager.customRanges,
            liveParameters: &modeManager.selectedParameterIds,
            rnbo: rnbo
        )
    }
}

// MARK: - Quick Preset Button

/// Individual quick preset button
/// Matches Android's button format: "1. PresetName"
struct QuickPresetButton: View {
    let number: Int
    let preset: Preset
    let onTap: () -> Void

    var body: some View {
        Button(action: onTap) {
            Text("\(number). \(preset.truncatedName)")
                .font(.caption)
                .fontWeight(.semibold)
                .foregroundColor(.white)
                .frame(maxWidth: .infinity)
                .padding(.vertical, 12)
                .background(Color(white: 0.2)) // Dark gray #333333
                .cornerRadius(6)
        }
        .buttonStyle(.plain)
    }
}

// MARK: - Preview

#if DEBUG
struct QuickPresetsView_Previews: PreviewProvider {
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

        // Create mock PresetManager with quick presets
        let mockPresetManager = PresetManager()
        mockPresetManager.quickPresets = [
            Preset(name: "Bass Heavy", parameterValues: [:], customRanges: [:], liveParameters: [], isQuickPreset: true),
            Preset(name: "Bright Pad", parameterValues: [:], customRanges: [:], liveParameters: [], isQuickPreset: true),
            Preset(name: "Deep Sub", parameterValues: [:], customRanges: [:], liveParameters: [], isQuickPreset: true),
            Preset(name: "High Pass", parameterValues: [:], customRanges: [:], liveParameters: [], isQuickPreset: true)
        ]

        return QuickPresetsView(parameters: .constant([mockParameter]))
            .environmentObject(RNBOAudioUnitHostModel())
            .environmentObject(ModeManager())
            .environmentObject(mockPresetManager)
            .previewLayout(.sizeThatFits)
    }
}
#endif
