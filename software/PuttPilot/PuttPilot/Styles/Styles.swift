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
        enum Size {
            case small, medium, long

            var dimensions: CGSize {
                switch self {
                case .small: return CGSize(width: 145, height: 75)
                case .medium: return CGSize(width: 315, height: 75)
                case .long: return CGSize(width: 956, height: 65)
                }
            }
        }
        
        struct CustomButtonStyle: ButtonStyle {
            let size: Size
            
            func makeBody(configuration: Configuration) -> some View {
                configuration.label
                    .frame(width: size.dimensions.width, height: size.dimensions.height)
                    .background(Colours.brown)
                    .foregroundColor(.white)
                    .font(.system(size: 20, weight: .medium))
                    .cornerRadius(50)
                    .shadow(color: .black.opacity(0.3), radius: 5, x: 2, y: 2)
                    .scaleEffect(configuration.isPressed ? 0.95 : 1.0)
                    .animation(.easeOut(duration: 0.2), value: configuration.isPressed)
            }
        }
    }
}
