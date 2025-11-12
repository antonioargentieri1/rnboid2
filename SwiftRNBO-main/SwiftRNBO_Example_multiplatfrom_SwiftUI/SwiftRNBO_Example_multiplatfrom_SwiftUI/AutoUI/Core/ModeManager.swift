//
//  ModeManager.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  Mode management system (Live, All, Setup)
//

import SwiftUI

/// Custom Range - Restricts parameter range to a subset
struct CustomRange: Codable {
    var enabled: Bool
    var customMin: Double
    var customMax: Double

    init(min: Double, max: Double, enabled: Bool = false) {
        self.customMin = min
        self.customMax = max
        self.enabled = enabled
    }
}

/// UI Mode types
enum UIMode: String, CaseIterable, Identifiable {
    case live = "Live"
    case all = "All"
    case presets = "Presets"
    case setup = "Setup"

    var id: String { rawValue }

    var description: String {
        switch self {
        case .live:
            return "Show only selected parameters for performance"
        case .all:
            return "Show all visible parameters"
        case .presets:
            return "Manage presets and morphing"
        case .setup:
            return "Configure Live mode parameters"
        }
    }

    var icon: String {
        switch self {
        case .live:
            return "play.circle.fill"
        case .all:
            return "slider.horizontal.3"
        case .presets:
            return "square.stack.3d.up.fill"
        case .setup:
            return "gearshape.fill"
        }
    }
}

/// Mode Manager - Gestisce modalità corrente e configurazione
class ModeManager: ObservableObject {
    // Current mode
    @Published var currentMode: UIMode = .all

    // Selected parameters for Live mode (paramId set)
    @Published var selectedParameterIds: Set<String> = []

    // Custom ranges per parameter (paramId -> CustomRange)
    @Published var customRanges: [String: CustomRange] = [:]

    // Configuration store
    private let configStore = ConfigurationStore()

    init() {
        loadConfiguration()
    }

    // MARK: - Mode Switching

    /// Switch to a different mode
    func switchMode(to mode: UIMode) {
        withAnimation(.easeInOut(duration: 0.3)) {
            currentMode = mode
        }
        saveConfiguration()
    }

    // MARK: - Live Mode Configuration

    /// Toggle parameter selection for Live mode
    func toggleParameterSelection(_ paramId: String) {
        if selectedParameterIds.contains(paramId) {
            selectedParameterIds.remove(paramId)
        } else {
            selectedParameterIds.insert(paramId)
        }
        saveConfiguration()
    }

    /// Check if parameter is selected for Live mode
    func isParameterSelected(_ paramId: String) -> Bool {
        selectedParameterIds.contains(paramId)
    }

    /// Select all visible parameters
    func selectAllParameters(_ parameters: [RNBOParameter]) {
        selectedParameterIds = Set(parameters.filter { $0.info.visible }.map { $0.id })
        saveConfiguration()
    }

    /// Deselect all parameters
    func deselectAllParameters() {
        selectedParameterIds.removeAll()
        saveConfiguration()
    }

    /// Reset to default configuration (all visible parameters)
    func resetToDefaults(_ parameters: [RNBOParameter]) {
        selectAllParameters(parameters)
        currentMode = .all
        saveConfiguration()
    }

    // MARK: - Custom Range Management

    /// Get effective range for a parameter (custom if enabled, original otherwise)
    func getEffectiveRange(for parameter: RNBOParameter) -> (min: Double, max: Double) {
        if let customRange = customRanges[parameter.id], customRange.enabled {
            return (customRange.customMin, customRange.customMax)
        }
        return (parameter.info.minimum, parameter.info.maximum)
    }

    /// Set custom range for a parameter
    func setCustomRange(_ paramId: String, min: Double, max: Double, enabled: Bool) {
        customRanges[paramId] = CustomRange(min: min, max: max, enabled: enabled)
        saveConfiguration()
    }

    /// Toggle custom range enabled/disabled
    func toggleCustomRange(_ paramId: String) {
        if var range = customRanges[paramId] {
            range.enabled.toggle()
            customRanges[paramId] = range
            saveConfiguration()
        }
    }

    /// Update custom range values
    func updateCustomRange(_ paramId: String, min: Double, max: Double) {
        if var range = customRanges[paramId] {
            range.customMin = min
            range.customMax = max
            customRanges[paramId] = range
            saveConfiguration()
        }
    }

    /// Initialize custom range for a parameter if not exists
    func ensureCustomRange(for parameter: RNBOParameter) {
        if customRanges[parameter.id] == nil {
            customRanges[parameter.id] = CustomRange(
                min: parameter.info.minimum,
                max: parameter.info.maximum,
                enabled: false
            )
        }
    }

    // MARK: - Persistence

    private func saveConfiguration() {
        // Defer to avoid "Publishing changes from within view updates" warning
        DispatchQueue.main.async { [weak self] in
            guard let self = self else { return }
            self.configStore.saveMode(self.currentMode)
            self.configStore.saveSelectedParameters(self.selectedParameterIds)
            self.configStore.saveCustomRanges(self.customRanges)
        }
    }

    private func loadConfiguration() {
        currentMode = configStore.loadMode() ?? .all
        selectedParameterIds = configStore.loadSelectedParameters()
        customRanges = configStore.loadCustomRanges()
    }
}

/// Configuration Store - Persiste configurazione in UserDefaults
class ConfigurationStore {
    private let defaults = UserDefaults.standard

    // Keys
    private let modeKey = "autoui.currentMode"
    private let selectedParametersKey = "autoui.selectedParameters"
    private let customRangesKey = "autoui.customRanges"

    // MARK: - Mode

    func saveMode(_ mode: UIMode) {
        defaults.set(mode.rawValue, forKey: modeKey)
    }

    func loadMode() -> UIMode? {
        guard let rawValue = defaults.string(forKey: modeKey),
              let mode = UIMode(rawValue: rawValue) else {
            return nil
        }
        return mode
    }

    // MARK: - Selected Parameters

    func saveSelectedParameters(_ paramIds: Set<String>) {
        let array = Array(paramIds)
        defaults.set(array, forKey: selectedParametersKey)
    }

    func loadSelectedParameters() -> Set<String> {
        guard let array = defaults.stringArray(forKey: selectedParametersKey) else {
            return []
        }
        return Set(array)
    }

    // MARK: - Custom Ranges

    func saveCustomRanges(_ ranges: [String: CustomRange]) {
        let encoder = JSONEncoder()
        if let data = try? encoder.encode(ranges) {
            defaults.set(data, forKey: customRangesKey)
        }
    }

    func loadCustomRanges() -> [String: CustomRange] {
        guard let data = defaults.data(forKey: customRangesKey) else {
            return [:]
        }
        let decoder = JSONDecoder()
        return (try? decoder.decode([String: CustomRange].self, from: data)) ?? [:]
    }

    // MARK: - Reset

    func reset() {
        defaults.removeObject(forKey: modeKey)
        defaults.removeObject(forKey: selectedParametersKey)
        defaults.removeObject(forKey: customRangesKey)
    }
}
