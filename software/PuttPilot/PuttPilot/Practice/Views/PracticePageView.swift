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
            backgroundGradient
                .ignoresSafeArea()
            
            VStack(spacing: 20) {
                // Stats Card at the top
                statsCard
                
                // Simulation view in the center
                courseSimulation
                    .background(Color.black.opacity(0.3))
                    .cornerRadius(20)
                    .shadow(radius: 5)
                
                // Button to present the controls as a bottom sheet
                Button(action: {
                    showControlsSheet.toggle()
                }) {
                    Text("Show Controls")
                        .font(.headline)
                        .padding()
                        .frame(maxWidth: .infinity)
                        .background(Color.black.opacity(0.7))
                        .foregroundColor(.white)
                        .cornerRadius(10)
                        .padding(.horizontal)
                }
                
                courseModificationItems
                
                Spacer(minLength: 20)
            }
            .padding(.vertical)
        }
        .navigationTitle("PuttPilot")
        .navigationBarTitleDisplayMode(.inline)
        .onAppear { viewModel.startPolling() }
        .sheet(isPresented: $showControlsSheet) {
            BottomSheetControlPanel(viewModel: viewModel)
        }
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
                Text("Balls Missed")
                    .font(.headline)
                    .foregroundColor(Colours.accentNeon)
                Text("\(max(0, viewModel.ballsHit - viewModel.ballsInHole))")
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
}


import SwiftUI

struct BottomSheetControlPanel: View {
    @ObservedObject var viewModel: PracticePageViewModel
    @Environment(\.presentationMode) var presentationMode
    
    var body: some View {
        VStack(spacing: 20) {
            // Handle
            RoundedRectangle(cornerRadius: 3)
                .frame(width: 40, height: 6)
                .foregroundColor(.gray)
                .padding(.top, 10)
            
            ScrollView {
                VStack(spacing: 20) {
                    
                    // Control Panel Buttons
                    controlPanelButtons
                }
                .padding()
            }
            
            Button("Dismiss") {
                presentationMode.wrappedValue.dismiss()
            }
            .font(.headline)
            .padding()
            .frame(maxWidth: .infinity)
            .background(Color.black.opacity(0.7))
            .foregroundColor(.white)
            .cornerRadius(10)
            .padding([.horizontal, .bottom])
        }
        .background(Color.black.opacity(0.8))
        .cornerRadius(20)
        .padding()
    }
    

    private var controlPanelButtons: some View {
        VStack(spacing: 20) {
            HStack(spacing: 20) {
                Button(action: {
                    // TODO: implement save preset
                }) {
                    Label("Save Preset", systemImage: "star.fill")
                }
                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .small))
                
                Button(action: {
                    viewModel.clearBallsPressed()
                }) {
                    Label("Clear Balls", systemImage: "trash.fill")
                }
                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .small))
            }
            
            HStack(spacing: 20) {
                Button(action: {
                    viewModel.resetCoursePressed()
                }) {
                    Label("Reset Course", systemImage: "arrow.clockwise")
                }
                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .small))
                
                Button(action: {
                    // TODO: implement dispense ball action
                }) {
                    Label("Dispense Ball", systemImage: "play.fill")
                }
                .buttonStyle(Styles.Buttons.SportsButtonStyle(size: .small))
            }
            
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
        .padding()
    }
}
