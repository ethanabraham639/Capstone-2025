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
    
    private let apiManager = APIManager.shared
    private var cancellables: Set<AnyCancellable> = []
    
    init() {
        startPolling()
    }
    
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
                        self.accuracy = String(format: "%.f%%")
                    } else {
                        self.accuracy = "0%"
                    }
                }
            }
            .store(in: &cancellables)
    }
}
