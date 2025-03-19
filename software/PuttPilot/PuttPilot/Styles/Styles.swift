//
//  Styles.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-18.
//

import Foundation
import SwiftUI

struct Styles {
    struct Buttons {
        struct SportsButtonStyle: ButtonStyle {
            enum Size {
                case small, medium, large
                
                var dimensions: CGSize {
                    switch self {
                    case .small:
                        return CGSize(width: 140, height: 60)
                    case .medium:
                        return CGSize(width: 180, height: 60)
                    case .large:
                        return CGSize(width: 300, height: 60)
                    }
                }
            }
            
            var size: Size = .medium
            
            func makeBody(configuration: Configuration) -> some View {
                configuration.label
                    .frame(width: size.dimensions.width, height: size.dimensions.height)
                    .background(
                        LinearGradient(
                            gradient: Gradient(colors: [Colours.accentNeon, Color.blue]),
                            startPoint: .topLeading,
                            endPoint: .bottomTrailing
                        )
                    )
                    .foregroundColor(.white)
                    .font(.system(size: 20, weight: .bold))
                    .cornerRadius(size.dimensions.height / 2)
                    .shadow(color: Color.black.opacity(0.3), radius: 5, x: 2, y: 2)
                    .scaleEffect(configuration.isPressed ? 0.95 : 1.0)
                    .animation(.easeOut(duration: 0.2), value: configuration.isPressed)
            }
        }
    }
}
