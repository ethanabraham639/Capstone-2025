//
//  SplashScreenView.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-02.
//

import Foundation
import SwiftUI

struct SplashScreenView: View {
    @State private var isActive = false
    
    var body: some View {
        Group {
            if isActive {
                ContentView()
            } else {
                ZStack {
                    Color.white
                        .edgesIgnoringSafeArea(.all)
                    Image("Logo")
                        .resizable()
                        .scaledToFit()
                        .frame(width: 274, height: 157)
                }
                .onAppear {
                    // Simulate splash screen delay
                    DispatchQueue.main.asyncAfter(deadline: .now() + 1) {
                        withAnimation {
                            isActive = true
                        }
                    }
                }
            }
        }
    }
}
