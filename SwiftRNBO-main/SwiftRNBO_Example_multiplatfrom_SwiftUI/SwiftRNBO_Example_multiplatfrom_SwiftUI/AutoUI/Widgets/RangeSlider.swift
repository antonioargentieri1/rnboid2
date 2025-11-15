//
//  RangeSlider.swift
//  SwiftRNBO AutoUI
//
//  Created by AutoUI System
//  Dual-thumb range slider for custom parameter ranges
//

import SwiftUI

/// Dual-thumb Range Slider
struct RangeSlider: View {
    // Binding to range values
    @Binding var minValue: Double
    @Binding var maxValue: Double

    // Range limits
    let bounds: ClosedRange<Double>

    // UI customization
    var trackColor: Color = Color.gray.opacity(0.3)
    var rangeColor: Color = .blue
    var thumbColor: Color = .white
    var thumbSize: CGFloat = 20
    var trackHeight: CGFloat = 4

    // Callback for value changes
    var onValueChanged: ((Double, Double) -> Void)?

    @State private var isDraggingMin = false
    @State private var isDraggingMax = false

    var body: some View {
        GeometryReader { geometry in
            let trackWidth = geometry.size.width - thumbSize

            ZStack(alignment: .leading) {
                // Background track
                Capsule()
                    .fill(trackColor)
                    .frame(height: trackHeight)
                    .frame(width: geometry.size.width)

                // Active range track
                Capsule()
                    .fill(rangeColor)
                    .frame(height: trackHeight)
                    .frame(width: max(0, calculateWidth(for: maxValue, in: trackWidth) - calculateWidth(for: minValue, in: trackWidth)))
                    .offset(x: calculateWidth(for: minValue, in: trackWidth))

                // Min thumb
                Circle()
                    .fill(thumbColor)
                    .frame(width: thumbSize, height: thumbSize)
                    .shadow(color: isDraggingMin ? .blue.opacity(0.5) : .gray.opacity(0.3), radius: isDraggingMin ? 8 : 4)
                    .overlay(
                        Circle()
                            .stroke(rangeColor, lineWidth: 2)
                    )
                    .offset(x: calculateWidth(for: minValue, in: trackWidth))
                    .gesture(
                        DragGesture(minimumDistance: 0)
                            .onChanged { value in
                                isDraggingMin = true
                                let newValue = calculateValue(for: value.location.x, in: trackWidth)
                                minValue = min(newValue, maxValue - 0.01 * (bounds.upperBound - bounds.lowerBound))
                                onValueChanged?(minValue, maxValue)
                            }
                            .onEnded { _ in
                                isDraggingMin = false
                            }
                    )

                // Max thumb
                Circle()
                    .fill(thumbColor)
                    .frame(width: thumbSize, height: thumbSize)
                    .shadow(color: isDraggingMax ? .blue.opacity(0.5) : .gray.opacity(0.3), radius: isDraggingMax ? 8 : 4)
                    .overlay(
                        Circle()
                            .stroke(rangeColor, lineWidth: 2)
                    )
                    .offset(x: calculateWidth(for: maxValue, in: trackWidth))
                    .gesture(
                        DragGesture(minimumDistance: 0)
                            .onChanged { value in
                                isDraggingMax = true
                                let newValue = calculateValue(for: value.location.x, in: trackWidth)
                                maxValue = max(newValue, minValue + 0.01 * (bounds.upperBound - bounds.lowerBound))
                                onValueChanged?(minValue, maxValue)
                            }
                            .onEnded { _ in
                                isDraggingMax = false
                            }
                    )
            }
            .frame(height: thumbSize)
        }
        .frame(height: thumbSize)
    }

    // MARK: - Helper Functions

    private func calculateWidth(for value: Double, in trackWidth: CGFloat) -> CGFloat {
        let normalized = (value - bounds.lowerBound) / (bounds.upperBound - bounds.lowerBound)
        return CGFloat(normalized) * trackWidth
    }

    private func calculateValue(for xPosition: CGFloat, in trackWidth: CGFloat) -> Double {
        let normalized = max(0, min(1, Double(xPosition / trackWidth)))
        return bounds.lowerBound + normalized * (bounds.upperBound - bounds.lowerBound)
    }
}

// MARK: - Preview

#if DEBUG
struct RangeSlider_Previews: PreviewProvider {
    static var previews: some View {
        VStack(spacing: 30) {
            VStack(alignment: .leading) {
                Text("Frequency Range")
                    .font(.headline)
                Text("20 Hz - 20000 Hz")
                    .font(.caption)
                    .foregroundColor(.secondary)

                RangeSliderPreviewContainer(
                    bounds: 20...20000,
                    initialMin: 200,
                    initialMax: 2000
                )
            }

            VStack(alignment: .leading) {
                Text("Volume Range")
                    .font(.headline)
                Text("0.0 - 1.0")
                    .font(.caption)
                    .foregroundColor(.secondary)

                RangeSliderPreviewContainer(
                    bounds: 0...1,
                    initialMin: 0.2,
                    initialMax: 0.8
                )
            }
        }
        .padding()
    }
}

private struct RangeSliderPreviewContainer: View {
    let bounds: ClosedRange<Double>
    @State var min: Double
    @State var max: Double

    init(bounds: ClosedRange<Double>, initialMin: Double, initialMax: Double) {
        self.bounds = bounds
        self._min = State(initialValue: initialMin)
        self._max = State(initialValue: initialMax)
    }

    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            RangeSlider(
                minValue: $min,
                maxValue: $max,
                bounds: bounds,
                onValueChanged: { newMin, newMax in
                    print("Range: \(newMin) - \(newMax)")
                }
            )

            HStack {
                Text("Min: \(String(format: "%.2f", min))")
                    .font(.caption)
                    .foregroundColor(.secondary)
                Spacer()
                Text("Max: \(String(format: "%.2f", max))")
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
    }
}
#endif
