//
//  ParameterRow.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  Container row for parameter widget with optional selection checkbox
//

import SwiftUI

/// Parameter Row - Container for parameter widget with mode-specific UI
struct ParameterRow: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel
    @EnvironmentObject var modeManager: ModeManager

    @Binding var parameter: RNBOParameter
    let mode: UIMode

    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack(spacing: 12) {
                // Setup mode: Show checkbox for Live selection
                if mode == .setup {
                    selectionCheckbox
                }

                // Parameter widget (slider)
                AutoSlider(parameter: $parameter)
                    .opacity(mode == .setup ? (isSelected ? 1.0 : 0.5) : 1.0)
            }

            // Setup mode: Show custom range controls
            if mode == .setup {
                customRangeSection
                    .padding(.leading, mode == .setup ? 44 : 0) // Align with slider
            }
        }
        .padding(.horizontal)
        .padding(.vertical, 8)
        .background(rowBackground)
        .cornerRadius(8)
        .onAppear {
            // Ensure custom range exists for this parameter
            modeManager.ensureCustomRange(for: parameter)
        }
    }

    // MARK: - Selection Checkbox (Setup Mode)

    private var selectionCheckbox: some View {
        Button(action: {
            withAnimation(.spring(response: 0.3, dampingFraction: 0.7)) {
                modeManager.toggleParameterSelection(parameter.id)
            }
        }) {
            Image(systemName: isSelected ? "checkmark.circle.fill" : "circle")
                .font(.title2)
                .foregroundColor(isSelected ? .blue : .gray)
        }
        .buttonStyle(.plain)
    }

    // MARK: - Custom Range Section

    private var customRangeSection: some View {
        VStack(alignment: .leading, spacing: 8) {
            // Custom range toggle
            Toggle(isOn: Binding(
                get: { customRange?.enabled ?? false },
                set: { _ in modeManager.toggleCustomRange(parameter.id) }
            )) {
                Text("Custom Range")
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
            .toggleStyle(SwitchToggleStyle(tint: .blue))

            // Range display and slider (shown when enabled)
            if let range = customRange, range.enabled {
                VStack(alignment: .leading, spacing: 4) {
                    // Range label
                    Text(String(format: "[%.2f - %.2f]", range.customMin, range.customMax))
                        .font(.caption)
                        .foregroundColor(.blue)

                    // Range slider
                    RangeSlider(
                        minValue: Binding(
                            get: { range.customMin },
                            set: { newMin in
                                modeManager.updateCustomRange(parameter.id, min: newMin, max: range.customMax)
                            }
                        ),
                        maxValue: Binding(
                            get: { range.customMax },
                            set: { newMax in
                                modeManager.updateCustomRange(parameter.id, min: range.customMin, max: newMax)
                            }
                        ),
                        bounds: parameter.info.minimum...parameter.info.maximum,
                        onValueChanged: { newMin, newMax in
                            modeManager.updateCustomRange(parameter.id, min: newMin, max: newMax)
                        }
                    )
                    .frame(height: 30)

                    // Original range reference
                    HStack {
                        Text("Original:")
                            .font(.caption2)
                            .foregroundColor(.secondary)
                        Text(String(format: "[%.2f - %.2f]", parameter.info.minimum, parameter.info.maximum))
                            .font(.caption2)
                            .foregroundColor(.secondary)
                    }
                }
                .padding(.top, 4)
            }
        }
        .padding(.vertical, 8)
        .padding(.horizontal, 12)
        .background(
            RoundedRectangle(cornerRadius: 6)
                .fill(Color.gray.opacity(0.1))
        )
    }

    // MARK: - Background

    private var rowBackground: some View {
        Group {
            if mode == .setup {
                RoundedRectangle(cornerRadius: 8)
                    .fill(isSelected ? Color.blue.opacity(0.1) : Color.clear)
                    .overlay(
                        RoundedRectangle(cornerRadius: 8)
                            .stroke(isSelected ? Color.blue.opacity(0.3) : Color.clear, lineWidth: 1)
                    )
            } else {
                Color.clear
            }
        }
    }

    // MARK: - Computed Properties

    private var isSelected: Bool {
        modeManager.isParameterSelected(parameter.id)
    }

    private var customRange: CustomRange? {
        modeManager.customRanges[parameter.id]
    }
}

// MARK: - Preview

#if DEBUG
struct ParameterRow_Previews: PreviewProvider {
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

        return Group {
            // Setup mode preview
            ParameterRow(parameter: .constant(mockParameter), mode: .setup)
                .environmentObject(RNBOAudioUnitHostModel())
                .environmentObject(ModeManager())
                .padding()
                .previewDisplayName("Setup Mode")

            // Live mode preview
            ParameterRow(parameter: .constant(mockParameter), mode: .live)
                .environmentObject(RNBOAudioUnitHostModel())
                .environmentObject(ModeManager())
                .padding()
                .previewDisplayName("Live Mode")
        }
    }
}
#endif
