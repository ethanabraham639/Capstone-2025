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
    @State private var scaleEffect = 0.8
    @State private var opacity = 0.5

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
                        .frame(width: Constants.logoWidth, height: Constants.logoHeight)
                        .scaleEffect(scaleEffect)
                        .opacity(opacity)
                        .onAppear {
                            withAnimation(.easeInOut(duration: 1.0)) {
                                scaleEffect = 1.0
                                opacity = 1.0
                            }
                            DispatchQueue.main.asyncAfter(deadline: .now() + Constants.splashDelay) {
                                withAnimation {
                                    isActive = true
                                }
                            }
                        }
                }
            }
        }
    }
}

struct Constants {
    static let logoWidth: CGFloat = 274
    static let logoHeight: CGFloat = 157
    static let splashDelay: TimeInterval = 1.0
}
