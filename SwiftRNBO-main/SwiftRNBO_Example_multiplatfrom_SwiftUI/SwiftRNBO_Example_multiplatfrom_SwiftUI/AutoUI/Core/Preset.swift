//
//  Preset.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  Preset data model for saving/loading parameter configurations
//

import Foundation

/// Preset - Complete snapshot of parameter configuration
/// Matches Android's Preset class structure
struct Preset: Codable, Identifiable, Equatable {
    var id: UUID
    var name: String
    var timestamp: Date

    // Parameter values (paramId -> actual value)
    var parameterValues: [String: Double]

    // Custom ranges per parameter
    var customRanges: [String: CustomRange]

    // Selected parameters for Live mode
    var liveParameters: Set<String>

    // Quick preset flag (for 8 quick buttons in LIVE mode)
    var isQuickPreset: Bool

    // XY Pad mappings per parameter (paramId -> XYPadMapping)
    var xyPadMappings: [String: XYPadMapping]

    // Future: Sensor controls (when sensors are implemented)
    // var sensorControls: [String: SensorControl] = [:]
    // var sensorsGloballyEnabled: Bool = false

    init(
        name: String,
        parameterValues: [String: Double],
        customRanges: [String: CustomRange],
        liveParameters: Set<String>,
        xyPadMappings: [String: XYPadMapping] = [:],
        isQuickPreset: Bool = false
    ) {
        self.id = UUID()
        self.name = name
        self.timestamp = Date()
        self.parameterValues = parameterValues
        self.customRanges = customRanges
        self.liveParameters = liveParameters
        self.xyPadMappings = xyPadMappings
        self.isQuickPreset = isQuickPreset
    }

    // MARK: - Equatable

    static func == (lhs: Preset, rhs: Preset) -> Bool {
        lhs.id == rhs.id
    }
}

// MARK: - Preset Extensions

extension Preset {
    /// Format timestamp for display
    var formattedTimestamp: String {
        let formatter = DateFormatter()
        formatter.dateFormat = "MMM d, yyyy HH:mm"
        return formatter.string(from: timestamp)
    }

    /// Truncated name for quick preset buttons (max 10 chars like Android)
    var truncatedName: String {
        if name.count <= 10 {
            return name
        }
        return String(name.prefix(10))
    }
}
