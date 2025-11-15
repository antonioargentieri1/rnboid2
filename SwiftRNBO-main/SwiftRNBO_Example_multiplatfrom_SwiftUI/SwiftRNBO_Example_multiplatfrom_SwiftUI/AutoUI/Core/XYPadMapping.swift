//
//  XYPadMapping.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  XY Pad parameter mapping configuration
//  Matches Android's SensorControl with special types: "X Axis", "Y Axis", "Pad trigger"
//

import Foundation

/// XY Pad axis type
enum XYPadAxis: String, Codable, CaseIterable {
    case none = "None"
    case xAxis = "X Axis"
    case yAxis = "Y Axis"
    case padTrigger = "Pad trigger"

    var displayName: String {
        rawValue
    }
}

/// XY Pad parameter mapping
/// Maps a parameter to an XY Pad axis
struct XYPadMapping: Codable {
    var axis: XYPadAxis
    var invert: Bool  // Invert axis (useful for Y: top=max, bottom=min)

    init(axis: XYPadAxis = .none, invert: Bool = false) {
        self.axis = axis
        self.invert = invert
    }
}

// MARK: - Extensions

extension XYPadMapping {
    /// Check if mapping is active (not none)
    var isActive: Bool {
        axis != .none
    }
}
