//
//  PresetManager.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  Preset management with save/load and morphing interpolation
//  Matches Android's preset system logic
//

import Foundation
import Combine

/// Preset Manager - Manages preset save/load and interpolation
/// Replicates Android MainActivity preset logic
class PresetManager: ObservableObject {
    // MARK: - Published Properties

    @Published var savedPresets: [Preset] = []
    @Published var quickPresets: [Preset] = []

    // Interpolation settings
    @Published var interpolationEnabled: Bool = false
    @Published var interpolationTimeMs: Int = 1000 // 1-5000ms

    // Interpolation state
    @Published var isInterpolating: Bool = false
    @Published var interpolationProgress: Double = 0.0

    // MARK: - Constants

    private let maxQuickPresets = 8
    private let interpolationFps = 60.0 // Update rate (60fps like Android)

    // MARK: - Private State

    private var interpolationTimer: Timer?
    private var interpolationStartValues: [String: Double] = [:]
    private var interpolationTargetValues: [String: Double] = [:]
    private var interpolationParamIndices: [String: Int] = [:] // paramId -> parameter array index
    private var interpolationStartTime: Date?

    // Storage
    private let storage = PresetStorage()

    // MARK: - Initialization

    init() {
        loadConfiguration()
    }

    // MARK: - Save Current Configuration

    /// Save current parameter state as a new preset
    /// Matches Android: saveCurrentAsPreset()
    func saveCurrentAsPreset(
        name: String,
        parameters: [RNBOParameter],
        customRanges: [String: CustomRange],
        liveParameters: Set<String>,
        xyPadMappings: [String: XYPadMapping]
    ) {
        // Capture current parameter values
        var parameterValues: [String: Double] = [:]
        for param in parameters {
            parameterValues[param.id] = param.value
        }

        // Deep copy custom ranges
        let copiedRanges = customRanges.mapValues { CustomRange(min: $0.customMin, max: $0.customMax, enabled: $0.enabled) }

        // Deep copy XY Pad mappings
        let copiedMappings = xyPadMappings.mapValues { XYPadMapping(axis: $0.axis, invert: $0.invert) }

        // Create preset
        let preset = Preset(
            name: name,
            parameterValues: parameterValues,
            customRanges: copiedRanges,
            liveParameters: liveParameters,
            xyPadMappings: copiedMappings,
            isQuickPreset: false
        )

        savedPresets.append(preset)
        saveToStorage()
    }

    // MARK: - Load Preset

    /// Load preset (instant or with interpolation)
    /// Matches Android: loadPreset()
    func loadPreset(
        _ preset: Preset,
        parameters: inout [RNBOParameter],
        customRanges: inout [String: CustomRange],
        liveParameters: inout Set<String>,
        rnbo: RNBOAudioUnitHostModel
    ) {
        if interpolationEnabled && !isInterpolating {
            startInterpolation(
                preset: preset,
                parameters: parameters,
                customRanges: &customRanges,
                liveParameters: &liveParameters,
                rnbo: rnbo
            )
        } else {
            loadPresetInstant(preset, parameters: &parameters, customRanges: &customRanges, liveParameters: &liveParameters, rnbo: rnbo)
        }
    }

    /// Load preset instantly (no interpolation)
    private func loadPresetInstant(
        _ preset: Preset,
        parameters: inout [RNBOParameter],
        customRanges: inout [String: CustomRange],
        liveParameters: inout Set<String>,
        rnbo: RNBOAudioUnitHostModel
    ) {
        // Apply custom ranges
        customRanges = preset.customRanges.mapValues { CustomRange(min: $0.customMin, max: $0.customMax, enabled: $0.enabled) }

        // Apply live parameters selection
        liveParameters = preset.liveParameters

        // Apply parameter values
        for (paramId, value) in preset.parameterValues {
            if let index = parameters.firstIndex(where: { $0.id == paramId }) {
                parameters[index].value = value
                rnbo.setParameterValue(to: value, at: parameters[index].info.index)
            }
        }
    }

    // MARK: - Interpolation

    /// Start interpolation to preset values
    /// Matches Android: startInterpolation() - Line 1749
    private func startInterpolation(
        preset: Preset,
        parameters: [RNBOParameter],
        customRanges: inout [String: CustomRange],
        liveParameters: inout Set<String>,
        rnbo: RNBOAudioUnitHostModel
    ) {
        // Capture start values and parameter indices
        interpolationStartValues.removeAll()
        interpolationParamIndices.removeAll()

        for (arrayIndex, param) in parameters.enumerated() {
            interpolationStartValues[param.id] = param.value
            interpolationParamIndices[param.id] = arrayIndex
        }

        // Set target values
        interpolationTargetValues = preset.parameterValues

        // Load preset configuration (ranges, live params) IMMEDIATELY
        // Matches Android Lines 1768-1771
        // This ensures UI shows new ranges while values interpolate
        customRanges = preset.customRanges.mapValues { CustomRange(min: $0.customMin, max: $0.customMax, enabled: $0.enabled) }
        liveParameters = preset.liveParameters

        // Start interpolation
        isInterpolating = true
        interpolationProgress = 0.0
        interpolationStartTime = Date()

        // Start timer (60fps = ~16ms interval)
        let interval = 1.0 / interpolationFps
        interpolationTimer = Timer.scheduledTimer(withTimeInterval: interval, repeats: true) { [weak self] _ in
            self?.updateInterpolation(rnbo: rnbo)
        }
    }

    /// Update interpolation progress
    /// Matches Android: updateInterpolatedValues()
    private func updateInterpolation(rnbo: RNBOAudioUnitHostModel) {
        guard let startTime = interpolationStartTime else {
            finishInterpolation()
            return
        }

        // Calculate progress (0.0 to 1.0)
        let elapsed = Date().timeIntervalSince(startTime) * 1000.0 // ms
        let duration = Double(interpolationTimeMs)
        let rawProgress = elapsed / duration

        if rawProgress >= 1.0 {
            // Interpolation complete
            interpolationProgress = 1.0
            applyInterpolatedValues(progress: 1.0, rnbo: rnbo)
            finishInterpolation()
        } else {
            // Update with easing
            interpolationProgress = rawProgress
            applyInterpolatedValues(progress: rawProgress, rnbo: rnbo)
        }
    }

    /// Apply interpolated values with easing
    private func applyInterpolatedValues(
        progress: Double,
        rnbo: RNBOAudioUnitHostModel
    ) {
        let easedProgress = easeInOutQuad(progress)

        for (paramId, startValue) in interpolationStartValues {
            guard let targetValue = interpolationTargetValues[paramId],
                  let arrayIndex = interpolationParamIndices[paramId] else {
                continue
            }

            // Linear interpolation with easing
            let interpolatedValue = startValue + (targetValue - startValue) * easedProgress

            // Update RNBO engine
            let paramIndex = rnbo.parameters[arrayIndex].info.index
            rnbo.setParameterValue(to: interpolatedValue, at: paramIndex)

            // Update @Published parameters array for UI update
            rnbo.parameters[arrayIndex].value = interpolatedValue
        }
    }

    /// Finish interpolation
    private func finishInterpolation() {
        interpolationTimer?.invalidate()
        interpolationTimer = nil
        isInterpolating = false
        interpolationProgress = 0.0
        interpolationStartTime = nil
        interpolationStartValues.removeAll()
        interpolationTargetValues.removeAll()
        interpolationParamIndices.removeAll()
    }

    /// Quadratic easing function (matches Android)
    private func easeInOutQuad(_ t: Double) -> Double {
        if t < 0.5 {
            return 2.0 * t * t
        } else {
            return 1.0 - pow(-2.0 * t + 2.0, 2.0) / 2.0
        }
    }

    // MARK: - Quick Presets

    /// Toggle quick preset status (max 8)
    /// Matches Android: toggleQuickPreset()
    func toggleQuickPreset(_ preset: Preset) {
        if let index = quickPresets.firstIndex(of: preset) {
            // Remove from quick presets
            quickPresets.remove(at: index)
            if let savedIndex = savedPresets.firstIndex(where: { $0.id == preset.id }) {
                savedPresets[savedIndex].isQuickPreset = false
            }
        } else {
            // Add to quick presets (max 8)
            if quickPresets.count >= maxQuickPresets {
                return // Max reached
            }
            quickPresets.append(preset)
            if let savedIndex = savedPresets.firstIndex(where: { $0.id == preset.id }) {
                savedPresets[savedIndex].isQuickPreset = true
            }
        }
        saveToStorage()
    }

    /// Check if preset is a quick preset
    func isQuickPreset(_ preset: Preset) -> Bool {
        quickPresets.contains(preset)
    }

    // MARK: - Preset Management

    /// Delete a preset
    func deletePreset(_ preset: Preset) {
        savedPresets.removeAll { $0.id == preset.id }
        quickPresets.removeAll { $0.id == preset.id }
        saveToStorage()
    }

    /// Rename a preset
    func renamePreset(_ preset: Preset, newName: String) {
        if let index = savedPresets.firstIndex(where: { $0.id == preset.id }) {
            savedPresets[index].name = newName
            if let quickIndex = quickPresets.firstIndex(of: preset) {
                quickPresets[quickIndex].name = newName
            }
            saveToStorage()
        }
    }

    // MARK: - Persistence

    private func saveToStorage() {
        storage.savePresets(savedPresets)
        storage.saveInterpolationSettings(enabled: interpolationEnabled, timeMs: interpolationTimeMs)
    }

    private func loadConfiguration() {
        savedPresets = storage.loadPresets()
        quickPresets = savedPresets.filter { $0.isQuickPreset }

        let settings = storage.loadInterpolationSettings()
        interpolationEnabled = settings.enabled
        interpolationTimeMs = settings.timeMs
    }
}

// MARK: - Preset Storage

/// Handles UserDefaults persistence for presets
/// Matches Android's SharedPreferences logic
class PresetStorage {
    private let defaults = UserDefaults.standard

    // Keys
    private let presetsKey = "autoui.presets"
    private let interpolationEnabledKey = "autoui.interpolation.enabled"
    private let interpolationTimeKey = "autoui.interpolation.timeMs"

    // MARK: - Presets

    func savePresets(_ presets: [Preset]) {
        let encoder = JSONEncoder()
        encoder.dateEncodingStrategy = .iso8601
        if let data = try? encoder.encode(presets) {
            defaults.set(data, forKey: presetsKey)
        }
    }

    func loadPresets() -> [Preset] {
        guard let data = defaults.data(forKey: presetsKey) else {
            return []
        }
        let decoder = JSONDecoder()
        decoder.dateDecodingStrategy = .iso8601
        return (try? decoder.decode([Preset].self, from: data)) ?? []
    }

    // MARK: - Interpolation Settings

    func saveInterpolationSettings(enabled: Bool, timeMs: Int) {
        defaults.set(enabled, forKey: interpolationEnabledKey)
        defaults.set(timeMs, forKey: interpolationTimeKey)
    }

    func loadInterpolationSettings() -> (enabled: Bool, timeMs: Int) {
        let enabled = defaults.bool(forKey: interpolationEnabledKey)
        let timeMs = defaults.integer(forKey: interpolationTimeKey)
        return (enabled, timeMs > 0 ? timeMs : 1000) // Default 1000ms
    }

    // MARK: - Reset

    func reset() {
        defaults.removeObject(forKey: presetsKey)
        defaults.removeObject(forKey: interpolationEnabledKey)
        defaults.removeObject(forKey: interpolationTimeKey)
    }
}
