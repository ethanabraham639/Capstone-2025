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
    
    @State private var leftLeaningSteepness = 0.0
    @State private var leftLeaningPosition = 0.0
    
    @State private var rightLeaningSteepness = 0.0
    @State private var rightLeaningPosition = 0.0
    
    @State private var uphillSteepness = 0.0
    @State private var uphillPosition = 0.0
    
    var body: some View {
        ZStack {
            backgroundGradient
            VStack {
                ScrollView {
                    // TODO: move image to nav bar (make custom nav bar?) or remove from design
                    Image("Logo")
                        .resizable()
                        .scaledToFit()
                        .frame(width: Constants.logoWidth, height: Constants.logoHeight)
                    
                    accuracyIndicator
                        .padding()
                    
                    HStack {
                        courseSimulation
                        controlPanel
                    }
                    
                    modifyCourseTitle
                    
                    courseModificationItems
                    
                    sendCourseStateButton
                    
                    // TODO: Saved Courses
                    
                    Spacer()
                }
                }


        }
        .onAppear {
            viewModel.startPolling()
        }
        .ignoresSafeArea()

    

    }
}

let backgroundGradient = LinearGradient(
    colors: [Colours.lightGreen, Colours.mediumGreen],
    startPoint: .top, endPoint: .bottom)

extension PracticePageView {
    private var logoImage: some View {
        Image("Logo")
            .resizable()
            .scaledToFit()
    }
    
    private var modifyCourseTitle: some View {
        Text("Modify Course")
            .font(.title3)
    }
    
    private var courseModificationItems: some View {
        HStack {
            CourseModificationItem(type: .leftLeaning,
                                   steepness: $leftLeaningSteepness,
                                   position: $leftLeaningPosition)
            
            CourseModificationItem(type: .rightLeaning,
                                   steepness: $rightLeaningSteepness,
                                   position: $rightLeaningPosition)
            
            CourseModificationItem(type: .upHill,
                                   steepness: $uphillSteepness,
                                   position: $uphillPosition)
        }
    }
    
    private var sendCourseStateButton: some View {
        Button("Send Course State") {
            viewModel.updateMotorPositions(
                leftLeaning: (steepness: leftLeaningSteepness, position: leftLeaningPosition),
                rightLeaning: (steepness: rightLeaningSteepness, position: rightLeaningPosition),
                uphill: (steepness: uphillSteepness, position: uphillPosition)
            )
        }
        .buttonStyle(.borderedProminent)
    }
    
    private var accuracyIndicator: some View {
            Text("Accuracy: \(viewModel.accuracy)")
                .font(.title3)
                .padding()
                .background(Color.white.opacity(0.85))
                .cornerRadius(10)
        
    }
    
    
    private var controlPanel: some View {
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
            
            Button("Send Course State") {
 
            }
            .buttonStyle(.borderedProminent)
        }
    }
    
    var courseSimulation: some View {
        // TODO: update with simulation
        Image(.coursePreview)
            .imageScale(.small)
    }
}

