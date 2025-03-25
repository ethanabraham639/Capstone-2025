import SwiftUI

struct PracticePageView: View {
    @StateObject private var viewModel = PracticePageViewModel()
    @State private var showControlsSheet: Bool = false
    
    // Local states for the slider values
    @State private var leftLeaningSteepness = 0.0
    @State private var leftLeaningPosition = 0.0
    
    @State private var rightLeaningSteepness = 0.0
    @State private var rightLeaningPosition = 0.0
    
    @State private var uphillSteepness = 0.0
    @State private var uphillPosition = 0.0
    
    var body: some View {
        ZStack {
            // Background gradient now applies to the whole screen
            backgroundGradient
                .ignoresSafeArea()

            ScrollView {
                VStack(spacing: 20) {
                    statsCard
                    
                    // SceneKit Simulation View (Make sure it has a fixed height)
                    HStack(spacing: 20) {
                        courseSimulation
                            .frame(height: 400)
                            .background(Color.black.opacity(0.3))
                            .cornerRadius(20)
                            .shadow(radius: 5)
                        
                        VStack(spacing: 20) {
                            clearStatsButton
                            clearBallsButton
                            dispenseBallButton
                        }
                    }
                    .frame(maxWidth: .infinity)

//                    controlPanelButtons
                    Text("Custom Slopes")
                        .font(.title2)
                    
                    courseModificationItems
                    
                    HStack(spacing: 20) {
                        resetCourseButton
                        sendCourseStateButton
                    }
                    
                    Divider()
                    
                    presetCourseItems
                    
                    Spacer()
                }
                .padding(.vertical)
            }
        }
        .navigationTitle("PuttPilot")
        .navigationBarTitleDisplayMode(.inline)
        .onAppear { viewModel.startPolling() }
    }

    private var presetCourseItems: some View {
        VStack(alignment: .leading){
            Text("Presets")
                .font(.title2)
                .foregroundColor(.white)
                .padding(.leading, 10)
                
            
            HStack(spacing: 15) {
                Button(action: {
                    viewModel.gridInputs = PresetsDouble.leftLeaning
                    viewModel.sendCourseState(presetMotorPositions: Presets.leftLeaning)
                }) {
                    Text("Left Leaning")
                }
                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
                
                Button(action: {
                    viewModel.sendCourseState(presetMotorPositions: Presets.rightLeaning)
                }) {
                    Text("Right Leaning")
                }
                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
                
                Button(action: {
                    viewModel.sendCourseState(presetMotorPositions: Presets.uphill)
                }) {
                    Text("Uphill")
                }
                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
                
                Button(action: {
                    viewModel.sendCourseState(presetMotorPositions: Presets.mountain)
                }) {
                    Text("Mountain")
                }
                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
                
                Button(action: {
                    viewModel.sendCourseState(presetMotorPositions: Presets.valley)
                }) {
                    Text("Valley")
                }
                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
                                
            }
            .frame(maxWidth: .infinity)
            
//            HStack(spacing: 15) {
//                Button(action: {
//                    viewModel.sendCourseState(presetMotorPositions: Presets.mountain)
//                }) {
//                    Text("Mountain")
//                }
//                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
//                
//                Button(action: {
//                    viewModel.sendCourseState(presetMotorPositions: Presets.valley)
//                }) {
//                    Text("Valley")
//                }
//                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
//                
//                Button(action: {
//                    viewModel.sendCourseState(presetMotorPositions: Presets.flat)
//                }) {
//                    Text("Flat")
//                }
//                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
//                
//                
//                Button(action: {
//                    viewModel.sendCourseState(presetMotorPositions: Presets.max)
//                }) {
//                    Text("Max")
//                }
//                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
//            }
//            .frame(maxWidth: .infinity)

        }
        .frame(maxWidth: .infinity, alignment: .leading)
        .padding(10)
    }

    private var courseModificationItems: some View {
        HStack(alignment: .top, spacing: 20) {
            CourseModificationItem(type: .leftLeaning,
                                   steepness: $viewModel.leftLeaningSteepness,
                                   position: $viewModel.leftLeaningPosition)
                .onChange(of: viewModel.leftLeaningSteepness) { _ in viewModel.updatePreview() }
                .onChange(of: viewModel.leftLeaningPosition) { _ in viewModel.updatePreview() }
            
            CourseModificationItem(type: .rightLeaning,
                                   steepness: $viewModel.rightLeaningSteepness,
                                   position: $viewModel.rightLeaningPosition)
                .onChange(of: viewModel.rightLeaningSteepness) { _ in viewModel.updatePreview() }
                .onChange(of: viewModel.rightLeaningPosition) { _ in viewModel.updatePreview() }
            
            CourseModificationItem(type: .upHill,
                                   steepness: $viewModel.uphillSteepness,
                                   position: $viewModel.uphillPosition)
                .onChange(of: viewModel.uphillSteepness) { _ in viewModel.updatePreview() }
                .onChange(of: viewModel.uphillPosition) { _ in viewModel.updatePreview() }
        }
        .padding(.horizontal, 10)
    }
    
}


let backgroundGradient = LinearGradient(
    colors: [Colours.darkGreen, Color.black],
    startPoint: .topLeading,
    endPoint: .bottomTrailing
)

extension PracticePageView {
    
    private var statsCard: some View {
        HStack(spacing: 20) {
            VStack {
                Text("Balls Hit")
                    .font(.headline)
                    .foregroundColor(Colours.accentNeon)
                Text("\(viewModel.ballsHit)")
                    .font(.largeTitle)
                    .fontWeight(.bold)
                    .foregroundColor(.white)
            }
            
            VStack {
                Text("Puts Made")
                    .font(.headline)
                    .foregroundColor(Colours.accentNeon)
                Text("\(max(0, viewModel.ballsInHole))")
                    .font(.largeTitle)
                    .fontWeight(.bold)
                    .foregroundColor(.white)
            }
            
            VStack {
                Text("Accuracy")
                    .font(.headline)
                    .foregroundColor(Colours.accentNeon)
                Text(viewModel.accuracy)
                    .font(.largeTitle)
                    .fontWeight(.bold)
                    .foregroundColor(.white)
            }
        }
        .padding()
        .frame(maxWidth: .infinity)
        .background(
            RoundedRectangle(cornerRadius: 25)
                .fill(Color.black.opacity(0.6))
                .overlay(
                    RoundedRectangle(cornerRadius: 25)
                        .stroke(Colours.accentNeon, lineWidth: 2)
                )
        )
        .shadow(color: Color.black.opacity(0.5), radius: 5, x: 2, y: 2)
        .padding(.horizontal)
    }
    
    private var courseSimulation: some View {
        SceneKitView(animationState: $viewModel.animationState,
                     numRows: Constants.numRowMotors,
                     numCols: Constants.numColsMotors)
            .frame(width: 550, height: 400)
            .padding(10)
    }
    
    private var clearStatsButton: some View {
        Button(action: {
            viewModel.resetStatsPressed()
        }) {
            Label("Clear Stats", systemImage: "star.fill")
        }
        .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
    }
    
    private var clearBallsButton: some View {
        Button(action: {
            Task {
                await viewModel.clearBallsPressed()
            }
        }) {
            Label("Clear Balls", systemImage: "trash.fill")
        }
        .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
    }
    
    private var sendCourseStateButton: some View {
        Button(action: {
            viewModel.updateMotorPositions(
                leftLeaning: (steepness: viewModel.leftLeaningSteepness, position: viewModel.leftLeaningPosition),
                rightLeaning: (steepness: viewModel.rightLeaningSteepness, position: viewModel.rightLeaningPosition),
                uphill: (steepness: viewModel.uphillSteepness, position: viewModel.uphillPosition)
            )
        }) {
            Text("Send Course State")
                .frame(maxWidth: .infinity)
        }
        .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .large))
    }
    
    private var resetCourseButton: some View {
        Button(action: {
            viewModel.resetCoursePressed()
        }) {
            Label("Reset Course", systemImage: "arrow.clockwise")
        }
        .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .large))
    }
    
    private var dispenseBallButton: some View {
        Button(action: {
            viewModel.dispenseBallsPressed()
        }) {
            Label("Dispense Ball", systemImage: "play.fill")
        }
        .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .medium))
    }
    
    private var controlPanelButtons: some View {
        VStack(spacing: 20) {
            HStack(spacing: 20) {
                clearStatsButton
                
                clearBallsButton
                
                sendCourseStateButton

                resetCourseButton

                dispenseBallButton
            }
        }
        .padding()
    }

}
