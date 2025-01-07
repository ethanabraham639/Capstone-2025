//
//  DebugMessageView.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-07.
//

import Foundation
import SwiftUI

struct DebugMessageView: View {
    @ObservedObject var viewModel: DeveloperTestingViewModel

    var body: some View {
        VStack {
            Text("Debug Messages")
                .font(.largeTitle)
                .bold()
                .padding()

            ScrollView {
                Text(viewModel.debugMessage)
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .padding()
                    .background(Color.gray.opacity(0.1))
                    .cornerRadius(8)
            }
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            .padding()
        }
        .onAppear {
            viewModel.startPolling()
        }
        .padding()
    }
}
