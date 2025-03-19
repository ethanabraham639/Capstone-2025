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
    
    // 2D array used to calculate animation sate in SceneKitView
    @Published var animationState: [[Double]] = Array(
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
    
    private func calculateAnimationState(
        leftSteep: Double, leftPos: Double,
        rightSteep: Double, rightPos: Double,
        uphillSteep: Double, uphillPos: Double
    ) -> [[Double]] {
        let nRows = Constants.numRowMotors  // 9
        let nCols = Constants.numColsMotors // 5
        
        // Convert 0–100 user inputs into 0.0–1.0 scale for easier math
        let lSteep = leftSteep / 100.0
        let lPos   = leftPos   / 100.0
        
        let rSteep = rightSteep / 100.0
        let rPos   = rightPos   / 100.0
        
        let uSteep = uphillSteep / 100.0
        let uPos   = uphillPos   / 100.0
        
        // Initialize a 2D array (rows × cols) to store results
        var output = Array(
            repeating: Array(repeating: 0.0, count: nCols),
            count: nRows
        )
        
        // For each row & column, compute the final displacement based on
        // left leaning + right leaning + uphill
        for row in 0..<nRows {
            // fraction [0..1] for how "far down" we are in the rows
            let rowFraction = Double(row) / Double(nRows - 1)
            
            for col in 0..<nCols {
                // fraction [0..1] for how "far right" we are in the columns
                let colFraction = Double(col) / Double(nCols - 1)
                
                //---- LEFT-LEANING EFFECT ----
                // a) "position" defines the fraction at which slope starts
                // b) from that point to the far right, it ramps from 0 up to lSteep
                var leftEffect: Double = 0.0
                if colFraction > lPos {
                    // from lPos..1, we linearly go from 0..lSteep
                    let slopeFrac = (colFraction - lPos) / (1.0 - lPos)
                    leftEffect = slopeFrac * lSteep
                }
                
                //---- RIGHT-LEANING EFFECT ----
                // This is basically the opposite direction.
                // We can invert colFraction → colRev = 1 - colFraction
                let colRev = 1.0 - colFraction
                var rightEffect: Double = 0.0
                if colRev > rPos {
                    // from rPos..1, we linearly go from 0..rSteep
                    let slopeFrac = (colRev - rPos) / (1.0 - rPos)
                    rightEffect = slopeFrac * rSteep
                }
                
                //---- UPHILL EFFECT ----
                // This goes from row 0..(nRows-1).
                // If rowFraction < uPos => 0. else it ramps from 0..uSteep
                let revRowFraction = 1 - rowFraction   // Reversed: row 0 → 1, last row → 0
                var uphillEffect: Double = 0.0
                if revRowFraction > uPos {
                    let slopeFrac = (revRowFraction - uPos) / (1.0 - uPos)
                    uphillEffect = slopeFrac * uSteep
                }
                //---- COMBINE ALL SLOPES ----
                let combined = leftEffect + rightEffect + uphillEffect
                
                // Scale the 0..1 slope to 0..90 motor range
                let motorPos = combined * 90.0
                
                // Optionally clamp to [0..90]
                output[row][col] = max(0.0, min(90.0, motorPos))
            }
        }
        
        return output
    }
    
    
    /// Calculates a new grid of motor positions based on the provided slider values.
    private func calculateGridInputs(
        leftSteep: Double, leftPos: Double,
        rightSteep: Double, rightPos: Double,
        uphillSteep: Double, uphillPos: Double
    ) -> [[Double]] {
        // Could literally be the *same* as calculateAnimationState,
        // or just call `calculateAnimationState` from here
        return calculateAnimationState(
            leftSteep: leftSteep,
            leftPos:   leftPos,
            rightSteep: rightSteep,
            rightPos:   rightPos,
            uphillSteep: uphillSteep,
            uphillPos:   uphillPos
        )
    }




    // MARK: - Updating Preview & Sending Motor Positions
    
    /// Updates the gridInputs for real-time preview without sending a command.
    func updatePreview() {
        DispatchQueue.main.async {
            self.animationState = self.calculateAnimationState(
                leftSteep:       self.leftLeaningSteepness,
                leftPos:         self.leftLeaningPosition,
                rightSteep:      self.rightLeaningSteepness,
                rightPos:        self.rightLeaningPosition,
                uphillSteep:     self.uphillSteepness,
                uphillPos:       self.uphillPosition
            )
        }
    }

    /// Updates gridInputs and sends the new state to the firmware.
    func updateMotorPositions(
         leftLeaning: (steepness: Double, position: Double),
         rightLeaning: (steepness: Double, position: Double),
         uphill: (steepness: Double, position: Double)
    ) {
        // 1) Recompute final grid
        gridInputs = calculateGridInputs(
            leftSteep:    leftLeaning.steepness,
            leftPos:      leftLeaning.position,
            rightSteep:   rightLeaning.steepness,
            rightPos:     rightLeaning.position,
            uphillSteep:  uphill.steepness,
            uphillPos:    uphill.position
        )
        
        // 2) Then push to firmware
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
    
    func sendCourseState(presetMotorPositions: [String]?) {
        if let presetMotorPositions = presetMotorPositions {
            APIManager.shared.sendCourseStatePublisher(mode: .staticMode, motorPositions: presetMotorPositions)
                .sink(receiveCompletion: { completion in
                    if case let .failure(error) = completion {
                        print("Error sending course state: \(error)")
                    }
                }, receiveValue: { success in
                    print("Course state sent successfully: \(success)")
                })
                .store(in: &cancellables)
        }
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
    
    func resetStatsPressed() {
        // TODO: may have to manually set values to 0 if they don't sync
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
    
    func clearBallsPressed() {
        let flattenedPositions = gridInputs.flatMap { row in
            row.map { String(format: "%.0f", $0.rounded()) }
        }
        APIManager.shared.ballReturnPublisher()
            .sink(receiveCompletion: { completion in
                if case let .failure(error) = completion {
                    print("Error clearing balls: \(error)")
                }
            }, receiveValue: { success in
                print("Balls cleared successfully: \(success)")
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
