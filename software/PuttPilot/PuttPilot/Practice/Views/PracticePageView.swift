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
                    .frame(maxWidth: .infinity)
                    
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
            .font(.system(size: 20, weight: .bold))
            .frame(maxWidth: .infinity, alignment: .leading)
            .padding(.horizontal, 20)
    }
    
    private var courseModificationItems: some View {
        HStack(alignment: .top, spacing: 20) {
            CourseModificationItem(type: .leftLeaning,
                                   steepness: $viewModel.leftLeaningSteepness,
                                   position: $viewModel.leftLeaningPosition)
            .frame(maxWidth: .infinity, alignment: .leading)
            .layoutPriority(1)
            .onChange(of: viewModel.leftLeaningSteepness) { _ in
                viewModel.updatePreview()
            }
            .onChange(of: viewModel.leftLeaningPosition) { _ in
                viewModel.updatePreview()
            }
            
            CourseModificationItem(type: .rightLeaning,
                                   steepness: $viewModel.rightLeaningSteepness,
                                   position: $viewModel.rightLeaningPosition)
            .frame(maxWidth: .infinity, alignment: .leading)
            .layoutPriority(1)
            .onChange(of: viewModel.rightLeaningSteepness) { _ in
                viewModel.updatePreview()
            }
            .onChange(of: viewModel.rightLeaningPosition) { _ in
                viewModel.updatePreview()
            }
            
            CourseModificationItem(type: .upHill,
                                   steepness: $viewModel.uphillSteepness,
                                   position: $viewModel.uphillPosition)
            .frame(maxWidth: .infinity, alignment: .leading)
            .layoutPriority(1)
            .onChange(of: viewModel.uphillSteepness) { _ in
                viewModel.updatePreview()
            }
            .onChange(of: viewModel.uphillPosition) { _ in
                viewModel.updatePreview()
            }
        }
        .frame(maxWidth: .infinity, alignment: .leading)
        .padding(.horizontal, 20)
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
                    // TODO: still needs to be done
                }
                .buttonStyle(Styles.Buttons.CustomButtonStyle(size:.small))
                .padding(10)
                
                Button("Clear Balls") {
                    viewModel.clearBallsPressed()
                }
                .buttonStyle(Styles.Buttons.CustomButtonStyle(size:.small))
                .padding(10)
            }
            
            HStack {
                Button("Reset Course") {
                    viewModel.resetCoursePressed()
                }
                .buttonStyle(Styles.Buttons.CustomButtonStyle(size:.small))
                .padding(10)
                
                Button("Dispense Ball") {
                    
                }
                .buttonStyle(Styles.Buttons.CustomButtonStyle(size:.small))
                .padding(10)
            }
            
            Button("Send Course State") {
                viewModel.updateMotorPositions(
                    leftLeaning: (steepness: leftLeaningSteepness, position: leftLeaningPosition),
                    rightLeaning: (steepness: rightLeaningSteepness, position: rightLeaningPosition),
                    uphill: (steepness: uphillSteepness, position: uphillPosition)
                )
            }
            .buttonStyle(Styles.Buttons.CustomButtonStyle(size:.medium))
            .padding(10)

        }
        .padding(10)
    }
    
//    var courseSimulation: some View {
//        // TODO: update with simulation
//        Image(.coursePreview)
//            .imageScale(.small)
//            .padding(10)
//    }
}


extension PracticePageView {
    var courseSimulation: some View {
        SceneKitView(gridInputs: $viewModel.gridInputs,
                     numRows: Constants.numRowMotors,
                     numCols: Constants.numColsMotors)
            .frame(width: 550, height: 400) // Adjust the frame as needed
            .padding(10)
    }
}
