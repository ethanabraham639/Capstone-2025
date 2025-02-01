//
//  CourseModificationItem.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-18.
//

import Foundation
import SwiftUI

enum CourseModificationItemType: String {
    case leftLeaning = "Left-leaning"
    case rightLeaning = "Right-Leaning"
    case upHill = "Uphill"
}

struct CourseModificationItem: View {
    var type: CourseModificationItemType
    @Binding var steepness: Double
    @Binding var position: Double
    
    var body: some View {
        VStack(alignment: .leading) {
            Text(type.rawValue)
                .font(.title3)
            ZStack {
                Image(type.rawValue)
                    .resizable()
                    .scaledToFit()
                    .frame(height: 216.76)
                
            }
            
            Text("Steepness: \(Int(steepness))%")
            Slider(value: $steepness, in: 0...Double(Constants.maxMotorPosition), step: 1)
            
            Text("Position: \(Int(position))%")
            Slider(value: $position, in: 0...Double(Constants.maxMotorPosition), step: 1)

        }
        .padding()
//        .background(RoundedRectangle(cornerRadius: 10).fill(Color.gray.opacity(0.2)))
//        .shadow(radius: isSelected ? 4 : 0)
//        
    }
}
