//
//  ParameterFilter.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  Parameter filtering and sorting logic
//

import Foundation

/// Parameter Filter - Filtra e ordina parametri in base alla modalità
struct ParameterFilter {
    // MARK: - Filtering

    /// Filter parameters based on current mode
    static func filterParameters(
        _ parameters: [RNBOParameter],
        mode: UIMode,
        selectedIds: Set<String>
    ) -> [RNBOParameter] {
        switch mode {
        case .live:
            return filterForLiveMode(parameters, selectedIds: selectedIds)
        case .all:
            return filterForAllMode(parameters)
        case .presets:
            // Presets mode doesn't show parameter list (has its own UI)
            return []
        case .setup:
            return filterForSetupMode(parameters)
        }
    }

    /// Filter for Live mode - only selected parameters
    private static func filterForLiveMode(
        _ parameters: [RNBOParameter],
        selectedIds: Set<String>
    ) -> [RNBOParameter] {
        parameters.filter { param in
            param.info.visible && selectedIds.contains(param.id)
        }
    }

    /// Filter for All mode - all visible parameters
    private static func filterForAllMode(_ parameters: [RNBOParameter]) -> [RNBOParameter] {
        parameters.filter { $0.info.visible }
    }

    /// Filter for Setup mode - all visible parameters (for selection)
    private static func filterForSetupMode(_ parameters: [RNBOParameter]) -> [RNBOParameter] {
        parameters.filter { $0.info.visible }
    }

    // MARK: - Sorting

    /// Sort parameters by order field from description.json
    static func sortByOrder(_ parameters: [RNBOParameter]) -> [RNBOParameter] {
        parameters.sorted { $0.info.order < $1.info.order }
    }

    /// Sort parameters alphabetically by displayName
    static func sortByName(_ parameters: [RNBOParameter]) -> [RNBOParameter] {
        parameters.sorted { param1, param2 in
            let name1 = param1.info.displayName.isEmpty ? param1.info.name : param1.info.displayName
            let name2 = param2.info.displayName.isEmpty ? param2.info.name : param2.info.displayName
            return name1 < name2
        }
    }

    /// Sort parameters by index
    static func sortByIndex(_ parameters: [RNBOParameter]) -> [RNBOParameter] {
        parameters.sorted { $0.info.index < $1.info.index }
    }

    // MARK: - Grouping (Future feature)

    /// Group parameters by a custom category field
    /// NOTE: Requires custom field in description.json metadata
    static func groupByCategory(_ parameters: [RNBOParameter]) -> [String: [RNBOParameter]] {
        // Future implementation: requires "category" field in JSON
        // For now, return single group
        return ["All": parameters]
    }

    // MARK: - Search

    /// Search parameters by name/displayName
    static func search(
        _ parameters: [RNBOParameter],
        query: String
    ) -> [RNBOParameter] {
        guard !query.isEmpty else { return parameters }

        let lowercaseQuery = query.lowercased()
        return parameters.filter { param in
            let name = param.info.name.lowercased()
            let displayName = param.info.displayName.lowercased()
            let paramId = param.info.paramId.lowercased()

            return name.contains(lowercaseQuery) ||
                   displayName.contains(lowercaseQuery) ||
                   paramId.contains(lowercaseQuery)
        }
    }
}

// MARK: - Sort Order Enum

enum ParameterSortOrder: String, CaseIterable, Identifiable {
    case order = "Order (JSON)"
    case name = "Name (A-Z)"
    case index = "Index"

    var id: String { rawValue }

    func sort(_ parameters: [RNBOParameter]) -> [RNBOParameter] {
        switch self {
        case .order:
            return ParameterFilter.sortByOrder(parameters)
        case .name:
            return ParameterFilter.sortByName(parameters)
        case .index:
            return ParameterFilter.sortByIndex(parameters)
        }
    }
}
