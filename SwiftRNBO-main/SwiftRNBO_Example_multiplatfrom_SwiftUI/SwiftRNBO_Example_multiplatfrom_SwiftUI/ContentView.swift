//
//  ContentView.swift
//  SwiftRNBO_Example_multiplatfrom_SwiftUI
//
//  Created by Эльдар Садыков on 19.02.2023.
//  Updated with AutoUI System
//

import SwiftUI

struct ContentView: View {
    @EnvironmentObject var rnbo: RNBOAudioUnitHostModel

    var body: some View {
        VStack(spacing: 0) {
            Buttons()
            #if !os(tvOS)
                if rnbo.showDescription {
                    DescriptionView()
                } else {
                    // AutoUI System - Auto-generated parameter UI
                    AutoUIView()
                    AudioKitKeyboard()
                }
            #endif
        }
        .padding()
    }
}
