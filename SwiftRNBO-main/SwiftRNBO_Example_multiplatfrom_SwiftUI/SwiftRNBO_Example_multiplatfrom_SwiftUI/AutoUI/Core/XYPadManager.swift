//
//  XYPadManager.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  XY Pad state management and parameter mapping
//  Matches Android's XY Pad update logic (Lines 521-571)
//

import Foundation
import Combine

/// XY Pad Manager - Manages XY Pad state and parameter mapping
class XYPadManager: ObservableObject {
    // MARK: - Published Properties

    @Published var isEnabled: Bool = false
    @Published var currentX: Double = 0.5  // [0-1]
    @Published var currentY: Double = 0.5  // [0-1]
    @Published var isTouching: Bool = false

    // Parameter mappings (paramId -> XYPadMapping)
    @Published var mappings: [String: XYPadMapping] = [:]

    // MARK: - Computed Properties

    /// Get parameter names assigned to X axis
    var xAxisParameters: [String] {
        mappings.filter { $0.value.axis == .xAxis }.map { $0.key }
    }

    /// Get parameter names assigned to Y axis
    var yAxisParameters: [String] {
        mappings.filter { $0.value.axis == .yAxis }.map { $0.key }
    }

    /// Get parameter names assigned to Pad trigger
    var padTriggerParameters: [String] {
        mappings.filter { $0.value.axis == .padTrigger }.map { $0.key }
    }

    /// Check if any parameters are mapped
    var hasActiveMappings: Bool {
        mappings.values.contains { $0.isActive }
    }

    // MARK: - Position Update

    /// Update XY position (from touch input)
    /// Matches Android: XYPadView.updatePosition()
    func updatePosition(x: Double, y: Double) {
        currentX = max(0, min(1, x))
        currentY = max(0, min(1, y))
    }

    /// Update touch state
    func updateTouchState(_ touching: Bool) {
        isTouching = touching
    }

    // MARK: - Parameter Mapping

    /// Update RNBO parameters based on current XY position
    /// Matches Android: MainActivity.updateXyPadParameters() (Lines 521-571)
    func updateParameters(
        parameters: [RNBOParameter],
        customRanges: [String: CustomRange],
        rnbo: RNBOAudioUnitHostModel
    ) {
        for param in parameters {
            guard let mapping = mappings[param.id], mapping.isActive else {
                continue
            }

            // 1. Get normalized value [0-1] based on axis type
            var normalizedValue: Double

            switch mapping.axis {
            case .none:
                continue

            case .xAxis:
                normalizedValue = currentX

            case .yAxis:
                normalizedValue = currentY

            case .padTrigger:
                normalizedValue = isTouching ? 1.0 : 0.0
            }

            // 2. Apply inversion if enabled
            if mapping.invert {
                normalizedValue = 1.0 - normalizedValue
            }

            // 3. Get effective range (custom or original)
            let customRange = customRanges[param.id]
            let minValue = (customRange?.enabled == true) ? customRange!.customMin : param.info.minimum
            let maxValue = (customRange?.enabled == true) ? customRange!.customMax : param.info.maximum

            // 4. Map [0-1] to parameter range
            let paramValue = minValue + normalizedValue * (maxValue - minValue)

            // 5. Update RNBO parameter
            rnbo.setParameterValue(to: paramValue, at: param.info.index)

            // 6. Update @Published parameters array for UI
            if let index = parameters.firstIndex(where: { $0.id == param.id }) {
                rnbo.parameters[index].value = paramValue
            }
        }
    }

    // MARK: - Mapping Management

    /// Set mapping for a parameter
    func setMapping(for paramId: String, axis: XYPadAxis, invert: Bool = false) {
        mappings[paramId] = XYPadMapping(axis: axis, invert: invert)
    }

    /// Clear mapping for a parameter
    func clearMapping(for paramId: String) {
        mappings[paramId] = XYPadMapping(axis: .none, invert: false)
    }

    /// Get mapping for a parameter
    func getMapping(for paramId: String) -> XYPadMapping {
        mappings[paramId] ?? XYPadMapping(axis: .none, invert: false)
    }

    /// Toggle invert for a parameter
    func toggleInvert(for paramId: String) {
        if var mapping = mappings[paramId] {
            mapping.invert.toggle()
            mappings[paramId] = mapping
        }
    }

    // MARK: - Initialization

    /// Load mappings from dictionary
    func loadMappings(_ newMappings: [String: XYPadMapping]) {
        mappings = newMappings
    }

    /// Get first X and Y parameter names for labels
    func getAxisLabels(parameters: [RNBOParameter]) -> (xLabel: String, yLabel: String) {
        let xParam = parameters.first { xAxisParameters.contains($0.id) }
        let yParam = parameters.first { yAxisParameters.contains($0.id) }

        let xLabel = xParam?.info.displayName.isEmpty == false
            ? xParam!.info.displayName
            : (xParam?.info.paramId ?? "X")

        let yLabel = yParam?.info.displayName.isEmpty == false
            ? yParam!.info.displayName
            : (yParam?.info.paramId ?? "Y")

        return (xLabel, yLabel)
    }
}
