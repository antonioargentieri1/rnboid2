//
//  AudioMeterView.swift
//  SwiftRNBO_Example_multiplatfrom_SwiftUI
//
//  Audio level meter for visual feedback
//

import SwiftUI

struct AudioMeterView: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel

    var body: some View {
        VStack(spacing: 8) {
            Text("Output Level")
                .font(.caption)
                .foregroundColor(.secondary)

            HStack(spacing: 4) {
                // Left channel
                VStack {
                    Text("L")
                        .font(.caption2)
                        .foregroundColor(.secondary)

                    MeterBar(level: rnbo.audioLevelLeft)
                        .frame(width: 20, height: 100)
                }

                // Right channel
                VStack {
                    Text("R")
                        .font(.caption2)
                        .foregroundColor(.secondary)

                    MeterBar(level: rnbo.audioLevelRight)
                        .frame(width: 20, height: 100)
                }
            }

            // Numeric display
            HStack(spacing: 16) {
                Text(String(format: "L: %.2f dB", amplitudeToDb(rnbo.audioLevelLeft)))
                    .font(.caption2)
                    .monospacedDigit()

                Text(String(format: "R: %.2f dB", amplitudeToDb(rnbo.audioLevelRight)))
                    .font(.caption2)
                    .monospacedDigit()
            }
            .foregroundColor(.secondary)
        }
        .padding()
        .background(Color.secondary.opacity(0.1))
        .cornerRadius(8)
    }

    private func amplitudeToDb(_ amplitude: Float) -> Float {
        guard amplitude > 0 else { return -60 }
        return 20 * log10(amplitude)
    }
}

struct MeterBar: View {
    let level: Float

    var body: some View {
        GeometryReader { geometry in
            ZStack(alignment: .bottom) {
                // Background
                Rectangle()
                    .fill(Color.gray.opacity(0.3))
                    .cornerRadius(4)

                // Level bar
                Rectangle()
                    .fill(levelColor)
                    .frame(height: geometry.size.height * CGFloat(level))
                    .cornerRadius(4)
                    .animation(.linear(duration: 0.05), value: level)
            }
        }
    }

    private var levelColor: Color {
        if level > 0.9 {
            return .red
        } else if level > 0.7 {
            return .yellow
        } else {
            return .green
        }
    }
}

#Preview {
    AudioMeterView()
        .environmentObject(RNBOAudioUnitHostModel())
}
