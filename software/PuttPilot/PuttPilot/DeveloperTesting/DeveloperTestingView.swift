//
//  DeveloperTestingView.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-02.
//

import Foundation
import SwiftUI

struct DeveloperTestingView: View {
    // StateObject 
    @StateObject private var viewModel = DeveloperTestingViewModel()
    
    // TODO: can make each item its own view variable to make this body smaller
    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 20) {
                
                // Fetch Stats
                VStack(alignment: .leading) {
                    Text("Fetch Stats")
                        .font(.headline)
                    Text(viewModel.statsDescription)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding()
                        .background(Color.gray.opacity(0.1))
                        .cornerRadius(8)
                }
                
                // Fetch Error Codes
                VStack(alignment: .leading) {
                    Text("Fetch Error Codes")
                        .font(.headline)
                    Text(viewModel.errorCodesDescription)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding()
                        .background(Color.gray.opacity(0.1))
                        .cornerRadius(8)
                }

                // Navigate to Debug Message View
                VStack(alignment: .leading) {
                    Text("Debug Messages")
                        .font(.headline)
                    
                    NavigationLink("View Debug Messages", destination: DebugMessageView(viewModel: viewModel))
                        .buttonStyle(.borderedProminent)
                        .padding(.top, 10)
                }
                
                // Ball Dispensing Mode Toggle
                VStack(alignment: .leading) {
                    Text("Select Ball Dispensing Mode")
                        .font(.headline)
                    
                    Picker("Ball Dispensing Mode", selection: $viewModel.ballDispensingMode) {
                        Text("Manual").tag(BallDispensingMode.manual)
                        Text("Automatic").tag(BallDispensingMode.automatic)
                    }
                    .pickerStyle(.segmented)
                    .onChange(of: viewModel.ballDispensingMode) { newValue in
                        viewModel.sendSettings()
                    }

                }

                // Course Mode
                VStack(alignment: .leading) {
                    Text("Select Course Mode")
                        .font(.headline)
    
                    Picker("Course Mode", selection: $viewModel.motorMode) {
                        Text("Static").tag(Mode.staticMode)
                        Text("Wave").tag(Mode.wave)
                        Text("Tsunami").tag(Mode.tsunami)
                        Text("Ball Return").tag(Mode.ballReturn)
                    }
                    .pickerStyle(.segmented)
                    .onChange(of: viewModel.motorMode) { newValue in
                        viewModel.sendCourseState()
                    }
                }

                // Grid Input for Course State
                Text("Send Course State")
                    .font(.headline)
                
                VStack {
                    LazyVGrid(columns: Array(repeating: GridItem(.flexible()), count: Constants.numColsMotors), spacing: 10) {
                        ForEach(0..<Constants.numRowMotors, id: \.self) { row in
                            ForEach(0..<Constants.numColsMotors, id: \.self) { col in
                                TextField("0", text: $viewModel.gridInputs[row][col])
                                    .textFieldStyle(.roundedBorder)
                                    .frame(width: 50, height: 40)
                                    .keyboardType(.numberPad)
                                    .id("\(row)-\(col)")
                            }
                        }
                        .frame(maxWidth: .infinity)
                        .padding()
                    }

                    HStack {
                        Button("Send Course State") {
                            viewModel.sendCourseState()
                        }
                        .buttonStyle(.borderedProminent)
                        .padding()
                        
                        Button("Clear Course State") {
                            viewModel.resetGridInputs()
                        }
                        .buttonStyle(.borderedProminent)
                        .padding()
                    }
                }
                
                // Default motor states

                VStack(alignment: .leading) {
                    Text("Default Motor Positions")
                        .font(.headline)
                    
                    HStack {
                        Button("Left Leaning") {
                            viewModel.sendCourseState(presetMotorPositions: ["0", "25", "50", "75", "90",
                                                                             "0", "25", "50", "75", "90",
                                                                             "0", "25", "50", "75", "90",
                                                                             "0", "25", "50", "75", "90",
                                                                             "0", "25", "50", "75", "90",
                                                                             "0", "25", "50", "75", "90",
                                                                             "0", "25", "50", "75", "90",
                                                                             "0", "25", "50", "75", "90",
                                                                             "0", "25", "50", "75", "90"])
                        }
                        .buttonStyle(.bordered)
                        .padding()
                        
                        Button("Right Leaning") {
                            viewModel.sendCourseState(presetMotorPositions: ["90", "75", "50", "25", "0",
                                                                             "90", "75", "50", "25", "0",
                                                                             "90", "75", "50", "25", "0",
                                                                             "90", "75", "50", "25", "0",
                                                                             "90", "75", "50", "25", "0",
                                                                             "90", "75", "50", "25", "0",
                                                                             "90", "75", "50", "25", "0",
                                                                             "90", "75", "50", "25", "0",
                                                                             "90", "75", "50", "25", "0"
                                                                            ])
                        }
                        .buttonStyle(.bordered)
                        .padding()
                        
                        Button("Uphill") {
                            viewModel.sendCourseState(presetMotorPositions: ["10", "10", "10", "10", "10",
                                                                             "20", "20", "20", "20", "20",
                                                                             "30", "30", "30", "30", "30",
                                                                             "40", "40", "40", "40", "40",
                                                                             "50", "50", "50", "50", "50",
                                                                             "60", "60", "60", "60", "60",
                                                                             "70", "70", "70", "70", "70",
                                                                             "80", "80", "80", "80", "80",
                                                                             "90", "90", "90", "90", "90",
                                                                            ])
                        }
                        .buttonStyle(.bordered)
                        .padding()
                        
                        Button("Flat") {
                            viewModel.sendCourseState(presetMotorPositions: ["0", "0", "0", "0", "0",
                                                                             "0", "0", "0", "0", "0",
                                                                             "0", "0", "0", "0", "0",
                                                                             "0", "0", "0", "0", "0",
                                                                             "0", "0", "0", "0", "0",
                                                                             "0", "0", "0", "0", "0",
                                                                             "0", "0", "0", "0", "0",
                                                                             "0", "0", "0", "0", "0",
                                                                             "0", "0", "0", "0", "0",
                                                                            ])
                        }
                        .buttonStyle(.bordered)
                        .padding()

                    }
                    
                    
                    Text("Set All Motors")
                        .font(.headline
                        )
                    HStack {
                        TextField("0", text: $viewModel.setAll)
                            .textFieldStyle(.roundedBorder)
                            .keyboardType(.numberPad)
                        
                        Button("Send") {
                            viewModel.setAllToVal(motorPos: viewModel.setAll)
                        }
                        .buttonStyle(.borderedProminent)
                    }
                }
                
                // Dispense Balls
                VStack(alignment: .leading) {
                    Text("Dispense Balls")
                        .font(.headline)
                    HStack {
                        TextField("Enter Count", text: $viewModel.dispenseBallsInput)
                            .textFieldStyle(.roundedBorder)
                            .keyboardType(.numberPad)
                        Button("Dispense") {
                            viewModel.dispenseBalls()
                        }
                        .buttonStyle(.borderedProminent)
                    }
                }
                
                // Reset Stats
                VStack(alignment: .leading) {
                    Text("Reset Stats")
                        .font(.headline)
                    Button("Reset Stats") {
                        viewModel.resetStats()
                    }
                    .buttonStyle(.borderedProminent)
                }
            }
            .padding()
        }
        .onAppear {
            viewModel.startPolling()
        }
    }
}
