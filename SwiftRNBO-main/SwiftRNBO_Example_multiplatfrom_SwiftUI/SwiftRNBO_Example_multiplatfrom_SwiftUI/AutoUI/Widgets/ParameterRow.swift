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
        HStack(spacing: 12) {
            // Setup mode: Show checkbox for Live selection
            if mode == .setup {
                selectionCheckbox
            }

            // Parameter widget (slider)
            AutoSlider(parameter: $parameter)
                .opacity(mode == .setup ? (isSelected ? 1.0 : 0.5) : 1.0)
        }
        .padding(.horizontal)
        .padding(.vertical, 8)
        .background(rowBackground)
        .cornerRadius(8)
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
