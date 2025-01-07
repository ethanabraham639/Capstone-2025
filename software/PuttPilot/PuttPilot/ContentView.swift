//
//  ContentView.swift
//  PuttPilot
//
//  Created by Ria Narang on 2024-12-27.
//

import SwiftUI
import SwiftData

struct ContentView: View {
    @State private var ipAddress: String = ""
    @State private var isConnected = true // Flip to false if you want to block users from going further without connection
    @State private var errorMessage: String?

    var body: some View {
        NavigationStack {
            VStack(spacing: 20) {
                Text("PuttPilot")
                    .font(.largeTitle)
                    .bold()

                TextField("Enter IP Address", text: $ipAddress)
                    .textFieldStyle(.roundedBorder)
                    .keyboardType(.URL)

                Button("Test Connection") {
                    testConnection()
                }
                .buttonStyle(.borderedProminent)

                if let errorMessage = errorMessage {
                    Text(errorMessage)
                        .foregroundColor(.red)
                        .multilineTextAlignment(.center)
                }

                if isConnected {
                    NavigationLink("Open Developer Testing Mode", destination: DeveloperTestingView())
                        .buttonStyle(.borderedProminent)
                }
            }
            .padding()
        }
    }

    private func testConnection() {
        guard !ipAddress.isEmpty else {
            errorMessage = "Please enter a valid IP address."
            isConnected = false
            return
        }

        APIManager.shared.updateIPAddress(ipAddress)

        APIManager.shared.ping { success, error in
            if success {
                isConnected = true
                errorMessage = nil
                APIManager.shared.sendDefaultStates()
                print("Connection Success")
            } else {
                isConnected = false
                errorMessage = error
                print("Connection Failed: \(String(describing: error))")
            }
        }
    }


}


#Preview {
    ContentView()
}
