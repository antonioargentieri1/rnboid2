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
    @EnvironmentObject var modeManager: ModeManager
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
                    ValueLabel(value: effectiveRange.min, unit: parameter.info.unit)
                        .frame(minWidth: 50, alignment: .trailing)
                }

                // Slider
                Group {
                    if let step = stepSize {
                        Slider(
                            value: sliderBinding,
                            in: 0...1,
                            step: step
                        ) {
                            Text(displayName)
                        } minimumValueLabel: {
                            if !showMinMaxLabels {
                                ValueLabel(value: effectiveRange.min, unit: parameter.info.unit)
                            } else {
                                EmptyView()
                            }
                        } maximumValueLabel: {
                            if !showMinMaxLabels {
                                ValueLabel(value: effectiveRange.max, unit: parameter.info.unit)
                            } else {
                                EmptyView()
                            }
                        }
                        .accentColor(accentColor)
                    } else {
                        Slider(
                            value: sliderBinding,
                            in: 0...1
                        ) {
                            Text(displayName)
                        } minimumValueLabel: {
                            if !showMinMaxLabels {
                                ValueLabel(value: effectiveRange.min, unit: parameter.info.unit)
                            } else {
                                EmptyView()
                            }
                        } maximumValueLabel: {
                            if !showMinMaxLabels {
                                ValueLabel(value: effectiveRange.max, unit: parameter.info.unit)
                            } else {
                                EmptyView()
                            }
                        }
                        .accentColor(accentColor)
                    }
                }

                // Max label
                if showMinMaxLabels {
                    ValueLabel(value: effectiveRange.max, unit: parameter.info.unit)
                        .frame(minWidth: 50, alignment: .leading)
                }

                // Current value
                if showCurrentValue {
                    CurrentValueLabel(value: displayValue, unit: parameter.info.unit)
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

    private var effectiveRange: (min: Double, max: Double) {
        modeManager.getEffectiveRange(for: parameter)
    }

    private var displayValue: Double {
        parameter.value
    }

    /// Custom binding that maps slider (0-1) to effective range (custom or original)
    /// This matches Android's SliderListener.onProgressChanged logic
    private var sliderBinding: Binding<Double> {
        Binding(
            get: {
                // Get current value from parameter
                let currentValue = parameter.value
                let range = effectiveRange

                // Normalize to effective range: (value - min) / (max - min)
                let normalized = (currentValue - range.min) / (range.max - range.min)
                return max(0, min(1, normalized))
            },
            set: { sliderPosition in
                // Map slider position (0-1) to effective range
                let range = effectiveRange
                let mappedValue = range.min + sliderPosition * (range.max - range.min)

                // Set to RNBO using actual value (not normalized)
                rnbo.setParameterValue(to: mappedValue, at: parameter.info.index)

                // Update parameter binding
                parameter.value = mappedValue
            }
        )
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
