//
//  AutoUIView.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  Main AutoUI orchestration view
//

import SwiftUI

/// Main AutoUI View - Orchestrates the entire auto-generated UI system
struct AutoUIView: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel
    @StateObject var modeManager = ModeManager()
    @StateObject var presetManager = PresetManager()

    @State private var sortOrder: ParameterSortOrder = .order
    @State private var searchQuery: String = ""

    var body: some View {
        VStack(spacing: 0) {
            // Mode selector
            ModeSelector(currentMode: $modeManager.currentMode)
                .padding(.horizontal)
                .padding(.top, 8)

            // Quick Presets (LIVE mode header)
            if modeManager.currentMode == .live {
                QuickPresetsView(parameters: $rnbo.parameters)
            }

            // Setup mode controls
            if modeManager.currentMode == .setup {
                setupModeControls
            }

            // Content (Presets Manager or Parameters List)
            if modeManager.currentMode == .presets {
                PresetsManagerView(parameters: $rnbo.parameters)
            } else {
                parametersList
            }
        }
        .environmentObject(modeManager)
        .environmentObject(presetManager)
        .onAppear {
            initializeModeManager()
        }
    }

    // MARK: - Setup Mode Controls

    private var setupModeControls: some View {
        VStack(spacing: 12) {
            // Instructions
            Text("Select parameters to show in Live mode")
                .font(.caption)
                .foregroundColor(.secondary)

            // Quick actions
            HStack(spacing: 16) {
                Button("Select All") {
                    modeManager.selectAllParameters(rnbo.parameters)
                }
                .buttonStyle(.bordered)

                Button("Deselect All") {
                    modeManager.deselectAllParameters()
                }
                .buttonStyle(.bordered)

                Spacer()

                Text("\(selectedCount) selected")
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
        .padding()
        .background(Color.blue.opacity(0.1))
    }

    // MARK: - Parameters List

    private var parametersList: some View {
        ScrollView {
            LazyVStack(spacing: 8) {
                ForEach($rnbo.parameters) { $parameter in
                    if shouldShowParameter(parameter) {
                        ParameterRow(
                            parameter: $parameter,
                            mode: modeManager.currentMode
                        )
                    }
                }

                // Empty state
                if filteredParameters.isEmpty {
                    emptyStateView
                }
            }
            .padding(.vertical)
        }
    }

    // MARK: - Empty State

    private var emptyStateView: some View {
        VStack(spacing: 16) {
            Image(systemName: "slider.horizontal.3")
                .font(.system(size: 60))
                .foregroundColor(.gray)

            Text(emptyStateMessage)
                .font(.headline)
                .foregroundColor(.secondary)

            if modeManager.currentMode == .live {
                Button("Go to Setup to select parameters") {
                    modeManager.switchMode(to: .setup)
                }
                .buttonStyle(.borderedProminent)
            }
        }
        .frame(maxWidth: .infinity)
        .padding(.top, 60)
    }

    // MARK: - Filtering Logic

    private func shouldShowParameter(_ parameter: RNBOParameter) -> Bool {
        filteredParameters.contains(where: { $0.id == parameter.id })
    }

    private var filteredParameters: [RNBOParameter] {
        let filtered = ParameterFilter.filterParameters(
            rnbo.parameters,
            mode: modeManager.currentMode,
            selectedIds: modeManager.selectedParameterIds
        )

        let sorted = sortOrder.sort(filtered)

        return searchQuery.isEmpty
            ? sorted
            : ParameterFilter.search(sorted, query: searchQuery)
    }

    private var selectedCount: Int {
        modeManager.selectedParameterIds.count
    }

    private var emptyStateMessage: String {
        switch modeManager.currentMode {
        case .live:
            return "No parameters selected for Live mode"
        case .all:
            return "No visible parameters found"
        case .presets:
            return "No presets saved yet"
        case .setup:
            return "No visible parameters to configure"
        }
    }

    // MARK: - Initialization

    private func initializeModeManager() {
        // Initialize with all parameters selected if first launch
        if modeManager.selectedParameterIds.isEmpty {
            modeManager.selectAllParameters(rnbo.parameters)
        }
    }
}

// MARK: - Mode Selector

struct ModeSelector: View {
    @Binding var currentMode: UIMode

    var body: some View {
        Picker("Mode", selection: $currentMode) {
            ForEach(UIMode.allCases) { mode in
                Label(mode.rawValue, systemImage: mode.icon)
                    .tag(mode)
            }
        }
        .pickerStyle(.segmented)
        .padding(.vertical, 8)
    }
}

// MARK: - Preview

#if DEBUG
struct AutoUIView_Previews: PreviewProvider {
    static var previews: some View {
        AutoUIView()
            .environmentObject(RNBOAudioUnitHostModel())
    }
}
#endif
