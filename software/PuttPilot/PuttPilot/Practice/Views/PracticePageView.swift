//
//  PracticePageView.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-18.
//

import Foundation
import SwiftUI

struct PracticePageView: View {
    @StateObject private var viewModel = PracticePageViewModel()
    
    var body: some View {
        VStack {
            // TODO: move image to nav bar (make custom nav bar?) or remove from design
            Image("Logo")
                .resizable()
                .scaledToFit()
                .frame(width: Constants.logoWidth, height: Constants.logoHeight)
            
            AccuracyIndicator(accuracy: viewModel.accuracy)
                .padding()
            
            HStack {
                CourseSimulation()
                ControlPanel()
            }
            
            Text("Modify Course")
                .font(.title3)
            
            HStack {
                CourseModificationItem() // Left Leaning
                
                CourseModificationItem() // Right Leaning
                
                CourseModificationItem() // Uphill
            }
            
            // TODO: Saved Courses
            
            Spacer()
        }
    

    }
}


struct AccuracyIndicator: View {
    let accuracy: String

    var body: some View {
        Text("Accuracy: \(accuracy)")
            .font(.title3)
            .padding()
            .background(Color.green.opacity(0.2))
            .cornerRadius(10)
    }
}

struct ControlPanel: View {
    var body: some View {
        VStack {
            HStack {
                Button("Save Preset") {
                    
                }
                
                Button("Clear Balls") {
                    
                }
            }
            
            HStack {
                Button("Reset Course") {
                    
                }
                
                Button("Dispense Ball") {
                    
                }
            }
            
            Button("Update Course") {
                
            }
        }
    }
}

struct CourseSimulation: View {
    var body: some View {
        Text("Placeholder")
    }
}
