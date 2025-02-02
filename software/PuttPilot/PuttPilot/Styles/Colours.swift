//
//  Colours.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-21.
//

import Foundation
import SwiftUI

struct Colours {
    static let lightGreen = Color(hex: 0x93C781)
    static let mediumGreen = Color(hex: 0x7AC64F)
    static let brown = Color(hex: 0x795757)

}

extension Color {
    init(hex: Int, opacity: Double = 1.0) {
        let red = Double((hex & 0xff0000) >> 16) / 255.0
        let green = Double((hex & 0xff00) >> 8) / 255.0
        let blue = Double((hex & 0xff) >> 0) / 255.0
        self.init(.sRGB, red: red, green: green, blue: blue, opacity: opacity)
    }
}
