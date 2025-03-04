//
//  CourseModificationItem.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-18.
//

import Foundation
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
            Text(type.rawValue)
                .font(.system(size: 15, weight: .bold))
                .padding(.vertical)
            
            Image(type.rawValue)
                .resizable()
                .scaledToFit()
                .frame(width: 215, height: 215)
                .frame(maxWidth: .infinity, alignment: .center)
            
            Text("Steepness: \(Int(steepness))%")
            
            Slider(value: $steepness, in: 0...100, step: 1)
                .frame(maxWidth: .infinity)
            
            Text("Position: \(Int(position))%")

            Slider(value: $position, in: 0...100, step: 1)
                .frame(maxWidth: .infinity)

        }
        .frame(maxWidth: .infinity)
        .padding()
        .background(Color.white.opacity(0.1))
        .cornerRadius(15)
    }
}
