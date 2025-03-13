//
//  CourseModificationItem.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-18.
//

import SwiftUI

enum CourseModificationItemType: String {
    case leftLeaning = "Left-Leaning"
    case rightLeaning = "Right-Leaning"
    case upHill = "Uphill"
}

struct CourseModificationItem: View {
    var type: CourseModificationItemType
    @Binding var steepness: Double
    @Binding var position: Double
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            // Title with better contrast.
            Text(type.rawValue)
                .font(.system(size: 15, weight: .bold))
                .foregroundColor(.white)
                .padding(.vertical, 4)
            
            // Image area with a subtle background to help it pop.
            Image(type.rawValue)
                .resizable()
                .scaledToFit()
                .frame(width: 215, height: 215)
                .frame(maxWidth: .infinity, alignment: .center)
                .background(Color.white.opacity(0.2))
                .cornerRadius(10)
            
            // Steepness label and slider.
            Text("Steepness: \(Int(steepness))%")
                .foregroundColor(.white)
            Slider(value: $steepness, in: 0...100, step: 1)
                .accentColor(Colours.accentNeon)
            
            // Position label and slider.
            Text("Position: \(Int(position))%")
                .foregroundColor(.white)
            Slider(value: $position, in: 0...100, step: 1)
                .accentColor(Colours.accentNeon)
        }
        .padding()
        .background(Color.black.opacity(0.6))
        .cornerRadius(15)
        .shadow(color: Color.black.opacity(0.5), radius: 5, x: 2, y: 2)
    }
}
