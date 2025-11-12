//
//  XYPadView.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  XY Pad touch control UI
//  Matches Android XYPadView.java visual and interaction
//

import SwiftUI

/// XY Pad View - 2D touch control for dual parameter mapping
/// Replicates Android XYPadView.java (237 lines)
struct XYPadView: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel
    @EnvironmentObject var xyPadManager: XYPadManager
    @EnvironmentObject var modeManager: ModeManager

    let xLabel: String
    let yLabel: String

    @State private var currentPosition: CGPoint = CGPoint(x: 0.5, y: 0.5)

    var body: some View {
        GeometryReader { geometry in
            ZStack {
                // Background
                Rectangle()
                    .fill(Color(white: 0.1)) // Dark background #1a1a1a

                // Grid (4x4)
                gridView

                // Center crosshair
                crosshairView

                // Cursor (filled when touching, stroke when not)
                cursorView
                    .position(
                        x: currentPosition.x * geometry.size.width,
                        y: currentPosition.y * geometry.size.height
                    )

                // Labels overlay
                labelsView(size: geometry.size)

                // Position values at top
                positionValuesView
                    .position(x: geometry.size.width / 2, y: 20)
            }
            .gesture(
                DragGesture(minimumDistance: 0)
                    .onChanged { value in
                        handleTouch(value.location, in: geometry.size, touching: true)
                    }
                    .onEnded { _ in
                        xyPadManager.updateTouchState(false)
                        updateParameters()
                    }
            )
            .onAppear {
                // Initialize position from manager
                currentPosition = CGPoint(
                    x: xyPadManager.currentX,
                    y: xyPadManager.currentY
                )
            }
        }
        .frame(maxWidth: .infinity)
        .frame(height: 400) // Fixed height like Android (800dp scaled)
        .cornerRadius(8)
        .padding()
    }

    // MARK: - Grid View

    private var gridView: some View {
        GeometryReader { geometry in
            Path { path in
                let width = geometry.size.width
                let height = geometry.size.height

                // Vertical lines (4 divisions = 5 lines)
                for i in 0...4 {
                    let x = CGFloat(i) * width / 4
                    path.move(to: CGPoint(x: x, y: 0))
                    path.addLine(to: CGPoint(x: x, y: height))
                }

                // Horizontal lines (4 divisions = 5 lines)
                for i in 0...4 {
                    let y = CGFloat(i) * height / 4
                    path.move(to: CGPoint(x: 0, y: y))
                    path.addLine(to: CGPoint(x: width, y: y))
                }
            }
            .stroke(Color.gray.opacity(0.3), lineWidth: 1)
        }
    }

    // MARK: - Crosshair View

    private var crosshairView: some View {
        GeometryReader { geometry in
            Path { path in
                let centerX = geometry.size.width / 2
                let centerY = geometry.size.height / 2
                let crosshairSize: CGFloat = 20

                // Horizontal line
                path.move(to: CGPoint(x: centerX - crosshairSize, y: centerY))
                path.addLine(to: CGPoint(x: centerX + crosshairSize, y: centerY))

                // Vertical line
                path.move(to: CGPoint(x: centerX, y: centerY - crosshairSize))
                path.addLine(to: CGPoint(x: centerX, y: centerY + crosshairSize))
            }
            .stroke(Color.gray.opacity(0.5), lineWidth: 2)
        }
    }

    // MARK: - Cursor View

    private var cursorView: some View {
        Circle()
            .stroke(Color.white, lineWidth: xyPadManager.isTouching ? 0 : 2) // Stroke when not touching
            .fill(xyPadManager.isTouching ? Color.white : Color.clear) // Fill when touching
            .frame(width: 30, height: 30)
    }

    // MARK: - Labels View

    private func labelsView(size: CGSize) -> some View {
        ZStack {
            // X label (bottom center)
            Text(xLabel)
                .font(.caption)
                .foregroundColor(.white)
                .position(x: size.width / 2, y: size.height - 15)

            // Y label (left middle, rotated)
            Text(yLabel)
                .font(.caption)
                .foregroundColor(.white)
                .rotationEffect(.degrees(-90))
                .position(x: 15, y: size.height / 2)
        }
    }

    // MARK: - Position Values View

    private var positionValuesView: some View {
        Text(String(format: "X: %.2f  Y: %.2f", xyPadManager.currentX, xyPadManager.currentY))
            .font(.caption)
            .foregroundColor(.white)
            .padding(.horizontal, 8)
            .padding(.vertical, 4)
            .background(Color.black.opacity(0.5))
            .cornerRadius(4)
    }

    // MARK: - Touch Handling

    private func handleTouch(_ location: CGPoint, in size: CGSize, touching: Bool) {
        // Normalize to [0-1]
        let normalizedX = max(0, min(1, location.x / size.width))
        let normalizedY = max(0, min(1, location.y / size.height))

        // Update position
        currentPosition = CGPoint(x: normalizedX, y: normalizedY)

        // Update manager
        xyPadManager.updatePosition(x: normalizedX, y: normalizedY)
        xyPadManager.updateTouchState(touching)

        // Update parameters in real-time
        updateParameters()
    }

    private func updateParameters() {
        xyPadManager.updateParameters(
            parameters: rnbo.parameters,
            customRanges: modeManager.customRanges,
            rnbo: rnbo
        )
    }
}

// MARK: - Preview

#if DEBUG
struct XYPadView_Previews: PreviewProvider {
    static var previews: some View {
        XYPadView(xLabel: "Frequency", yLabel: "Resonance")
            .environmentObject(RNBOAudioUnitHostModel())
            .environmentObject(XYPadManager())
            .environmentObject(ModeManager())
            .previewLayout(.sizeThatFits)
    }
}
#endif
