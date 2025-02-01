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
                        Spacer()
                        controlPanel
                    }
                    .padding(20)
                    
                    VStack(alignment: .leading) {
                        modifyCourseTitle
                        
                        courseModificationItems
                    }

                    sendCourseStateButton
                    
                    // TODO: Saved Courses
                    
                    Spacer()
                }
            }
            .padding(20)


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
                .frame(width: 536, height: 40)
                .font(.system(size: 28, weight: .regular))
                .padding()
                .background(Color.white.opacity(0.85))
                .cornerRadius(25)
                .shadow(color: .black.opacity(0.3), radius: 5, x: 2, y: 2)
    }
    
    
    private var controlPanel: some View {
        VStack {
            HStack {
                Button("Save Preset") {
                    
                }
                .buttonStyle(Styles.Buttons.CustomButtonStyle(size:.small))
                .padding(10)
                
                Button("Clear Balls") {
                    
                }
                .buttonStyle(Styles.Buttons.CustomButtonStyle(size:.small))
                .padding(10)
            }
            
            HStack {
                Button("Reset Course") {
                    
                }
                .buttonStyle(Styles.Buttons.CustomButtonStyle(size:.small))
                .padding(10)
                
                Button("Dispense Ball") {
                    
                }
                .buttonStyle(Styles.Buttons.CustomButtonStyle(size:.small))
                .padding(10)
            }
            
            Button("Send Course State") {
                
            }
            .buttonStyle(Styles.Buttons.CustomButtonStyle(size:.medium))
            .padding(10)
        }
        .padding(10)
    }
    
    var courseSimulation: some View {
        // TODO: update with simulation
        Image(.coursePreview)
            .imageScale(.small)
            .padding(10)
    }
}

