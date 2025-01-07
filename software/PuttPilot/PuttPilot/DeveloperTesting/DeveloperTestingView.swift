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
                
                // Fetch Debug Message
                // TODO: put this on its own page
                VStack(alignment: .leading) {
                    Text("Fetch Debug Message")
                        .font(.headline)
                    Text(viewModel.debugMessage)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding()
                        .background(Color.gray.opacity(0.1))
                        .cornerRadius(8)
                }
                
                // Ball Dispensing Mode Toggle
                VStack(alignment: .leading) {
                    Text("Select Ball Dispensing Mode")
                        .font(.headline)
                    
                    Picker("Ball Dispensing Mode", selection: $viewModel.ballDispensingMode) {
                        Text("Automatic").tag(BallDispensingMode.automatic)
                        Text("Manual").tag(BallDispensingMode.manual)
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
                    LazyVGrid(columns: Array(repeating: GridItem(.flexible()), count: 3), spacing: 10) {
                        ForEach(0..<3, id: \.self) { row in
                            ForEach(0..<3, id: \.self) { col in
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

                    Button("Send Course State") {
                        viewModel.sendCourseState()
                    }
                    .buttonStyle(.borderedProminent)
                    .padding()
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
