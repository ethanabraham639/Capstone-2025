import Foundation
import Combine

class PracticePageViewModel: ObservableObject {
    @Published var ballsHit: Int = 0
    @Published var ballsInHole: Int = 0
    @Published var accuracy: String = "0%"
    
    // 2D array representing actuator positions (0 to 90 degrees)
    @Published var gridInputs: [[Double]] = Array(
        repeating: Array(repeating: 0.0, count: Constants.numColsMotors),
        count: Constants.numRowMotors
    )
    
    // Slider values for each modification
    @Published var leftLeaningSteepness = 0.0
    @Published var leftLeaningPosition = 0.0
    
    @Published var rightLeaningSteepness = 0.0
    @Published var rightLeaningPosition = 0.0
    
    @Published var uphillSteepness = 0.0
    @Published var uphillPosition = 0.0 
    
    private let apiManager = APIManager.shared
    private var cancellables: Set<AnyCancellable> = []
    
    // MARK: - Polling & Accuracy Updates
    
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
    
    // MARK: - Grid Calculation Helper
    
    /// Calculates a new grid of motor positions based on the provided slider values.
    /// - Parameters:
    ///   - leftSteep: Left-leaning steepness (0-100)
    ///   - leftPos: Left-leaning position (0-100)
    ///   - rightSteep: Right-leaning steepness (0-100)
    ///   - rightPos: Right-leaning position (0-100)
    ///   - uphillSteep: Uphill steepness (0-100)
    ///   - uphillPos: Uphill position (0-100)
    /// - Returns: A new 2D array of motor positions scaled to 0-90%
    private func calculateGridInputs(
        leftSteep: Double, leftPos: Double,
        rightSteep: Double, rightPos: Double,
        uphillSteep: Double, uphillPos: Double
    ) -> [[Double]] {
        let numRows = Constants.numRowMotors
        let numCols = Constants.numColsMotors

        // Create an empty grid.
        var newGrid = Array(
            repeating: Array(repeating: 0.0, count: numCols),
            count: numRows
        )

        // Convert slider positions (0-100) to normalized values (0.0 to 0.9)
        let leftPosNorm = leftPos / 100.0
        let rightPosNorm = rightPos / 100.0
        let uphillPosNorm = uphillPos / 100.0
        
        let maxMotorPos = Double(Constants.maxMotorPosition)

        for row in 0..<numRows {
            // Normalize row: 0 at the top, 1 at the bottom.
            let normRow = Double(numRows - 1) / Double(row)
            for col in 0..<numCols {
                // Normalize column: 0 at the left, 1 at the right.
                let normCol = Double(col) / Double(numCols - 1)

                // -----------------------------
                // Left-Leaning Effect:
                // If the cell is left of the slider (normCol < leftPosNorm),
                // apply a gradient from full effect on the far left (normCol = 0)
                // to zero effect at the slider's position.
                var leftEffect: Double = 0.0
                if leftSteep > 0 && normCol > leftPosNorm {
                    leftEffect = (leftSteep / 100.0) * ((normCol - leftPosNorm) / (1 - leftPosNorm))
                }

                // -----------------------------
                // Right-Leaning Effect:
                // If the cell is right of the slider (normCol > rightPosNorm),
                // apply a gradient from zero at the slider's position
                // to full effect on the far right (normCol = 1).
                var rightEffect: Double = 0.0
                if rightSteep > 0 && normCol < rightPosNorm {
                    rightEffect = (rightSteep / 100.0) * ((normCol - rightPosNorm) / (1 - rightPosNorm))
                }

                // -----------------------------
                // Uphill Effect:
                // If the cell is below the uphill threshold (normRow >= uphillPosNorm),
                // apply an effect that ramps linearly from 0 at uphillPosNorm to full effect at the bottom.
                var uphillEffect: Double = 0.0
                if uphillSteep > 0 && normRow >= uphillPosNorm {
                    uphillEffect = (uphillSteep / 100.0) * ((normRow - uphillPosNorm) / (1 - uphillPosNorm))
                }

                // -----------------------------
                // Combine the Effects:
                // Count how many modifications are active and average their contributions.
                var activeCount = 0.0
                if leftSteep > 0 { activeCount += 1 }
                if rightSteep > 0 { activeCount += 1 }
                if uphillSteep > 0 { activeCount += 1 }
                
                let combinedEffect = leftEffect + rightEffect + uphillEffect
                let averagedEffect = activeCount > 0 ? combinedEffect / activeCount : 0.0

                // Scale the averaged effect to the motor range (0–90°) and clamp if necessary.
                newGrid[row][col] = min(averagedEffect * 90.0, 90.0)
            }
        }
        
        return newGrid
    }


    // MARK: - Updating Preview & Sending Motor Positions
    
    /// Updates the gridInputs for real-time preview without sending a command.
    func updatePreview() {
        // Use the current slider values from the view model.
        gridInputs = calculateGridInputs(
            leftSteep: leftLeaningSteepness,
            leftPos: leftLeaningPosition,
            rightSteep: rightLeaningSteepness,
            rightPos: rightLeaningPosition,
            uphillSteep: uphillSteepness,
            uphillPos: uphillPosition
        )
    }
    
    /// Updates gridInputs and sends the new state to the firmware.
    func updateMotorPositions(
         leftLeaning: (steepness: Double, position: Double),
         rightLeaning: (steepness: Double, position: Double),
         uphill: (steepness: Double, position: Double)
     ) {
         // Calculate new grid using the passed-in values.
         gridInputs = calculateGridInputs(
             leftSteep: leftLeaning.steepness,
             leftPos: leftLeaning.position,
             rightSteep: rightLeaning.steepness,
             rightPos: rightLeaning.position,
             uphillSteep: uphill.steepness,
             uphillPos: uphillPosition
         )
         
         // Send updated course state to firmware.
         sendCourseState()
     }
    
    /// Sends the current gridInputs to the firmware.
    func sendCourseState() {
        let flattenedPositions = gridInputs.flatMap { row in
            row.map { String(format: "%.0f", $0.rounded()) }
        }
        print("GRID INPUTS: ", flattenedPositions)
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
    
    // MARK: - Other Actions
    
    func resetCoursePressed() {
        // Reset gridInputs to all zeros.
        gridInputs = Array(
            repeating: Array(repeating: 0.0, count: Constants.numColsMotors),
            count: Constants.numRowMotors
        )
        sendCourseState()
        
        // Reset slider values.
        leftLeaningSteepness = 0.0
        leftLeaningPosition = 0.0
        rightLeaningSteepness = 0.0
        rightLeaningPosition = 0.0
        uphillSteepness = 0.0
        uphillPosition = 0.0
    }
    
    func clearBallsPressed() {
        let flattenedPositions = gridInputs.flatMap { row in
            row.map { String(format: "%.0f", $0.rounded()) }
        }
        APIManager.shared.sendCourseStatePublisher(mode: .ballReturn, motorPositions: flattenedPositions)
            .sink(receiveCompletion: { completion in
                if case let .failure(error) = completion {
                    print("Error sending course state: \(error)")
                }
            }, receiveValue: { success in
                print("Course state sent successfully: \(success)")
            })
            .store(in: &cancellables)
    }
    
    func dispenseBallsPressed() {
        // TODO: Batch the request, or change input field type
        apiManager.dispenseBallsPublisher(numberBalls: 1)
            .sink(receiveCompletion: { completion in
                if case let .failure(error) = completion {
                    print("Error dispensing balls: \(error)")
                }
            }, receiveValue: { success in
                print("Balls dispensed successfully: \(success)")
            })
            .store(in: &cancellables)
    }
}
