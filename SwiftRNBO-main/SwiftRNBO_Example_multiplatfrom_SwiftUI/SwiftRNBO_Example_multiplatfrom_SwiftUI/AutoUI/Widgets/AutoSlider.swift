//
//  AutoSlider.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  Auto-generated slider widget for RNBO parameters
//

import SwiftUI

/// Auto-generated Slider component
struct AutoSlider: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel
    @Binding var parameter: RNBOParameter

    // Optional customization
    var showMinMaxLabels: Bool = true
    var showCurrentValue: Bool = true
    var accentColor: Color = .blue

    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            // Parameter name
            Text(displayName)
                .font(.subheadline)
                .foregroundColor(.primary)

            // Slider with optional labels
            HStack(spacing: 12) {
                // Min label
                if showMinMaxLabels {
                    ValueLabel(value: parameter.info.minimum, unit: parameter.info.unit)
                        .frame(minWidth: 50, alignment: .trailing)
                }

                // Slider
                Slider(
                    value: $parameter.valueNormalized,
                    in: 0...1,
                    step: stepSize
                ) {
                    Text(displayName)
                } minimumValueLabel: {
                    if !showMinMaxLabels {
                        ValueLabel(value: parameter.info.minimum, unit: parameter.info.unit)
                    } else {
                        EmptyView()
                    }
                } maximumValueLabel: {
                    if !showMinMaxLabels {
                        ValueLabel(value: parameter.info.maximum, unit: parameter.info.unit)
                    } else {
                        EmptyView()
                    }
                }
                .accentColor(accentColor)
                .onChange(of: parameter.valueNormalized) { newValue in
                    rnbo.setParameterValueNormalized(to: newValue, at: parameter.info.index)
                }

                // Max label
                if showMinMaxLabels {
                    ValueLabel(value: parameter.info.maximum, unit: parameter.info.unit)
                        .frame(minWidth: 50, alignment: .leading)
                }

                // Current value
                if showCurrentValue {
                    CurrentValueLabel(value: parameter.value, unit: parameter.info.unit)
                        .frame(minWidth: 70, alignment: .leading)
                }
            }
        }
        .padding(.vertical, 4)
    }

    // MARK: - Computed Properties

    private var displayName: String {
        parameter.info.displayName.isEmpty
            ? parameter.info.paramId
            : parameter.info.displayName
    }

    private var stepSize: Double? {
        // If steps > 0, calculate step size for discrete parameters
        guard parameter.info.steps > 0 else { return nil }
        return 1.0 / Double(parameter.info.steps)
    }
}

// MARK: - Value Label

private struct ValueLabel: View {
    let value: Double
    let unit: String

    var body: some View {
        Text(formattedValue)
            .font(.caption)
            .foregroundColor(.secondary)
            .monospacedDigit()
    }

    private var formattedValue: String {
        let formatted = String(format: "%.2f", value)
        return unit.isEmpty ? formatted : "\(formatted) \(unit)"
    }
}

// MARK: - Current Value Label

private struct CurrentValueLabel: View {
    let value: Double
    let unit: String

    var body: some View {
        Text(formattedValue)
            .font(.system(.body, design: .monospaced))
            .foregroundColor(.primary)
            .padding(.horizontal, 8)
            .padding(.vertical, 4)
            .background(
                RoundedRectangle(cornerRadius: 6)
                    .fill(Color.secondary.opacity(0.2))
            )
    }

    private var formattedValue: String {
        let formatted: String
        if value >= 1000 {
            formatted = String(format: "%.1f", value)
        } else if value >= 100 {
            formatted = String(format: "%.2f", value)
        } else {
            formatted = String(format: "%.3f", value)
        }
        return unit.isEmpty ? formatted : "\(formatted)\(unit)"
    }
}

// MARK: - Preview

#if DEBUG
struct AutoSlider_Previews: PreviewProvider {
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

        return AutoSlider(parameter: .constant(mockParameter))
            .environmentObject(RNBOAudioUnitHostModel())
            .padding()
            .previewLayout(.sizeThatFits)
    }
}
#endif
