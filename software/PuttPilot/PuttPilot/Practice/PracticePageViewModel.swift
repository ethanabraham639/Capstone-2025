//
//  PracticePageViewModel.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-18.
//

import Foundation
import Combine

class PracticePageViewModel: ObservableObject {
    @Published var ballsHit: Int = 0
    @Published var ballsInHole: Int = 0
    @Published var accuracy: String = "0%"
    
    @Published var gridInputs: [[Double]] = Array(repeating: Array(repeating: 0.0, count: Constants.numColsMotors), count: Constants.numRowMotors)
    
    private let apiManager = APIManager.shared
    private var cancellables: Set<AnyCancellable> = []
    
    func startPolling() {
        apiManager.startPollingStats(interval: 1)
        
        apiManager.$stats
            .receive(on: DispatchQueue.main)
            .sink { stats in
                if !stats.isEmpty {
                    self.ballsHit = stats[0]
                    self.ballsInHole = stats[1]
                    
                    if self.ballsHit > 0 {
                        let accuracyValue = Double(self.ballsInHole) / Double(self.ballsHit)
                        self.accuracy = String(format: "%.f%%", accuracyValue)
                    } else {
                        self.accuracy = "0%"
                    }
                }
            }
            .store(in: &cancellables)
    }
    
    func updateMotorPositions(
         leftLeaning: (steepness: Double, position: Double),
         rightLeaning: (steepness: Double, position: Double),
         uphill: (steepness: Double, position: Double)
     ) {
         // Reset gridInputs
         gridInputs = Array(repeating: Array(repeating: 0.0, count: Constants.numColsMotors), count: Constants.numRowMotors)

         applyModification(type: .leftLeaning, steepness: leftLeaning.steepness, position: leftLeaning.position)

         applyModification(type: .rightLeaning, steepness: rightLeaning.steepness, position: rightLeaning.position)

         applyModification(type: .upHill, steepness: uphill.steepness, position: uphill.position)
         
         sendCourseState()
     }
    
    func sendCourseState() {
        let flattenedPositions = gridInputs.flatMap { $0.map { String(format: "%.0f", $0.rounded()) } }
        APIManager.shared.sendCourseStatePublisher(mode: .staticMode, motorPositions: flattenedPositions)
            .sink(receiveCompletion: { completion in
                if case let .failure(error) = completion {
                    print("Error sending course state: \(error)")
                }
            }, receiveValue: { success in
                print("Course state sent successfully: \(success)")
            })
            .store(in: &cancellables)
    }
    
    private func applyModification(type: CourseModificationItemType, steepness: Double, position: Double) {
        let startCol = Int((position / 100) * Double(Constants.numColsMotors))
        let steepnessValue = steepness / 100

        for row in 0..<Constants.numRowMotors {
            for col in 0..<Constants.numColsMotors {
                let distance = abs(col - startCol)
                let adjustment = max(0.0, steepnessValue - (Double(distance) * 0.1)) // Gradual fade effect
                gridInputs[row][col] += adjustment * 90 // Scale to 0-90 range
            }
        }
    }

}
