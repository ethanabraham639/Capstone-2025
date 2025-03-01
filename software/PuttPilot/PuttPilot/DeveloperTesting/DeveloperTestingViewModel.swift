//
//  DeveloperTestingViewModel.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-02.
//

import Foundation
import Combine

class DeveloperTestingViewModel: ObservableObject {
    @Published var statsDescription: String = "Fetching stats..."
    @Published var errorCodesDescription: String = "Fetching Error Codes..."
    @Published var debugMessage: String = "Fetching debug message..."
    // Consider updating the APIManager function to take an input like this, and then convert to what it needs
    @Published var gridInputs: [[String]] = Array(repeating: Array(repeating: "0", count: Constants.numColsMotors), count: Constants.numRowMotors)
    @Published var ballDispensingMode: BallDispensingMode = .manual
    @Published var motorMode: Mode = .staticMode
    @Published var dispenseBallsInput: String = ""
    @Published var setAll: String = ""

    private let apiManager = APIManager.shared
    private var cancellables: Set<AnyCancellable> = []
    
    func startPolling() {
        
        apiManager.startPollingStats(interval: 1)
        apiManager.startPollingErrorCodes(interval: 1)
        apiManager.startPollingDebugMessage(interval: 0.5)
        
        apiManager.$stats
            .receive(on: DispatchQueue.main)
            .map { $0.map(String.init).joined(separator: ",")}
            .assign(to: &$statsDescription)
        
        apiManager.$errorCodes
            .receive(on: DispatchQueue.main)
            .map { $0.map(String.init).joined(separator: ",")}
            .assign(to: &$errorCodesDescription)
        
        apiManager.$debugMessage
            .receive(on: DispatchQueue.main)
            .assign(to: &$debugMessage)
    }
    
    func dispenseBalls() {
        // TODO: only allow for number inputs, this wont send calls that arent numbers but doesnt prevent users from inputting not numeric values in the field
        guard let count = Int(dispenseBallsInput) else { return }
        apiManager.dispenseBallsPublisher(numberBalls: count)
            .sink(receiveCompletion: { completion in
                if case let .failure(error) = completion {
                    print("Error dispensing balls: \(error)")
                }
            }, receiveValue: { success in
                print("Balls dispensed successfully: \(success)")
                
            })
            .store(in: &cancellables)
    }
    
    func sendSettings() {
        apiManager.sendSettingsPublisher(ballDispensingMode: ballDispensingMode)
            .sink(receiveCompletion: { completion in
                if case let .failure(error) = completion {
                    print("Error sending settings: \(error)")
                }
            }, receiveValue: { success in
                print("Settings sent successfully: \(success)")
            })
            .store(in: &cancellables)
    }
    
    func apiManagerSendCourseState(mode: Mode, motorPositions: [String]) {
        apiManager.sendCourseStatePublisher(mode: mode, motorPositions: motorPositions)
            .sink(receiveCompletion: { completion in
                if case let .failure(error) = completion {
                    print("Error sending course state: \(error)")
                }
            }, receiveValue: { success in
                print("Course state sent successfully: \(success)")
            })
            .store(in: &cancellables)
    }

    func sendCourseState() {
        var motorPositions = gridInputs.flatMap { $0 }
        // Duplicate contents (testing purposes)
        let temp = motorPositions
        motorPositions.append(contentsOf: temp)
        motorPositions.append(contentsOf: temp)
        apiManagerSendCourseState(mode: motorMode, motorPositions: motorPositions)
    }
    
    func sendCourseState(presetMotorPositions: [String]?) {
        if let presetMotorPositions = presetMotorPositions {
            apiManagerSendCourseState(mode: motorMode, motorPositions: presetMotorPositions)
        }
    }
    
    // Set all  motors to one position
    func setAllToVal(motorPos: String) {
        let repeatedMotorPositions = Array(repeating: motorPos, count: Constants.numMotors)
        apiManagerSendCourseState(mode: motorMode, motorPositions: repeatedMotorPositions)
    }
    
    func resetStats() {
        apiManager.resetStatsPublisher()
            .sink(receiveCompletion: { completion in
                if case let .failure(error) = completion {
                    print("Error resseting stats \(error)")
                }
            }, receiveValue: { success in
                print("Stats reset successfully: \(success)")
            })
            .store(in: &cancellables)
    }
    
    func resetGridInputs() {
        for row in 0..<gridInputs.count {
            for col in 0..<gridInputs[row].count {
                gridInputs[row][col] = "0"
            }
        }
    }
}
