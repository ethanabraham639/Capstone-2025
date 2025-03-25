////
////  APIManager.swift
////  PuttPilot
////
////  Created by Ria Narang on 2024-12-27.
////

import Foundation
import Combine

enum Mode: Int {
    case staticMode = 0
    case ballReturn = 1
    case wave = 2
    case tsunami = 3

    var asciiCharacter: Character? {
        guard let scalar = UnicodeScalar(rawValue) else { return nil }
        return Character(scalar)
    }
}

enum BallDispensingMode: Int {
    case automatic = 1
    case manual = 0

    var asciiCharacter: Character? {
        guard let scalar = UnicodeScalar(rawValue) else { return nil }
        return Character(scalar)
    }
}

class APIManager {
    static let shared = APIManager()
    
    private var esp8266IP: String = "192.168.4.1" // Default value
    private var cancellables: Set<AnyCancellable> = []

    private var baseURL: URL {
        guard let url = URL(string: "http://\(esp8266IP)/") else {
            fatalError("Invalid ESP8266 IP Address")
        }
        return url
    }
    
    @Published var stats: [Int] = []
    @Published var errorCodes: [Int] = []
    @Published var debugMessage: String = ""
    @Published var apiError: String?
    
    init() {}
    
    // MARK: - Combine Publishers
    
    func startPollingStats(interval: TimeInterval) {
        Timer.publish(every: interval, on: .main, in: .common)
            .autoconnect()
            .flatMap { [weak self] _ in
                self?.fetchStatsPublisher() ?? Empty().eraseToAnyPublisher()
            }
            .sink(receiveCompletion: { completion in
                if case let .failure(error) = completion {
                    self.apiError = error.localizedDescription
                }
            }, receiveValue: { stats in
                self.stats = stats
            })
            .store(in: &cancellables)
    }
    
    func startPollingErrorCodes(interval: TimeInterval) {
        Timer.publish(every: interval, on: .main, in: .common)
            .autoconnect()
            .flatMap { [weak self] _ in
                self?.fetchErrorCodesPublisher() ?? Empty().eraseToAnyPublisher()
            }
            .sink(receiveCompletion: { completion in
                if case let .failure(error) = completion {
                    self.apiError = error.localizedDescription
                }
            }, receiveValue: { errorCodes in
                self.errorCodes = errorCodes
            })
            .store(in: &cancellables)
    }
    
    func startPollingDebugMessage(interval: TimeInterval) {
        Timer.publish(every: interval, on: .main, in: .common)
            .autoconnect()
            .flatMap { [weak self] _ in
                self?.fetchDebugMessagePublisher() ?? Empty().eraseToAnyPublisher()
            }
            .sink(receiveCompletion: { completion in
                if case let .failure(error) = completion {
                    self.apiError = error.localizedDescription
                }
            }, receiveValue: { [weak self] debugMessage in
                self?.debugMessage = debugMessage
                
            })
            .store(in: &cancellables)
    }
    
    // MARK: - API Calls with Combine
    
    func fetchStatsPublisher() -> AnyPublisher<[Int], Error> {
        let endpoint = "stats"
        guard let url = URL(string: "\(baseURL)\(endpoint)") else {
            return Fail(error: URLError(.badURL)).eraseToAnyPublisher()
        }
        
        return URLSession.shared.dataTaskPublisher(for: url)
            .map(\.data)
            .tryMap { data in
                data.compactMap { byte -> Int? in
                    return Int(byte)
                }
            }
//            .filter { $0.count == 2 && $0[0] >= $0[1] } // Validate balls_hit >= balls_in_hole
            .eraseToAnyPublisher()
    }
    
    func fetchErrorCodesPublisher() -> AnyPublisher<[Int], Error> {
        let endpoint = "error_codes"
        guard let url = URL(string: "\(baseURL)\(endpoint)") else {
            return Fail(error: URLError(.badURL)).eraseToAnyPublisher()
        }
        
        return URLSession.shared.dataTaskPublisher(for: url)
            .map(\.data)
            .tryMap { data in
                data.compactMap { byte -> Int? in
                    return Int(byte)
                }
            }
            .eraseToAnyPublisher()
    }
    
    func sendCourseStatePublisher(mode: Mode, motorPositions: [String], max: Bool = false) -> AnyPublisher<Bool, Error> {
        let endpoint = "course_state"

        guard let modeAscii = mode.asciiCharacter else {
            return Fail(error: URLError(.badURL)).eraseToAnyPublisher()
        }

        let numRows = Constants.numRowMotors
        let numCols = Constants.numColsMotors
        
        guard motorPositions.count == Constants.numMotors else {
            return Fail(error: URLError(.badServerResponse)).eraseToAnyPublisher()
        }

        // Reverse the order of rows while keeping columns intact
        var mirroredMotorPositions = (0..<numRows).reversed().flatMap { rowIndex in
            Array(motorPositions[(rowIndex * numCols)..<((rowIndex + 1) * numCols)])
        }

        // normalize the first and second row to ease into transition
        if !max {
            for rowIndex in 0..<numRows {
                let rowStart = rowIndex * numCols
                let rowEnd = rowStart + numCols
                
                let divisor: Int
                switch rowIndex {
                case 0:  // first row
                    divisor = 3
                case 1:  // second row
                    divisor = 2
                default: // third row onward – no division change
                    divisor = 1
                }
                
                // Apply division to this row
                for i in rowStart..<rowEnd {
                    if let originalVal = Int(mirroredMotorPositions[i]) {
                        let newVal = originalVal / divisor
                        mirroredMotorPositions[i] = String(newVal)
                    }
                }
            }
        }
        
        let motorAsciiArray = mirroredMotorPositions.compactMap { position -> Character? in
            guard let ascii = UnicodeScalar(90 - (Int(position) ?? 0)), ascii.isASCII else { return nil }
            return Character(ascii)
        }

        guard motorAsciiArray.count == mirroredMotorPositions.count else {
            return Fail(error: URLError(.dataLengthExceedsMaximum)).eraseToAnyPublisher()
        }

        let payload = String(modeAscii) + motorAsciiArray.map { String($0) }.joined()

        return sendPOSTRequestPublisher(endpoint: endpoint, body: payload)
    }
    
    func fetchDebugMessagePublisher() -> AnyPublisher<String, Error> {
        let endpoint = "debug_msg"

        return sendGETRequestPublisher(endpoint: endpoint)
            .tryMap { data in
                guard let message = String(data: data, encoding: .utf8) else {
                    throw URLError(.cannotDecodeContentData)
                }
                return message
            }
            .eraseToAnyPublisher()
    }

    func resetStatsPublisher() -> AnyPublisher<Bool, Error> {
        let endpoint = "reset_stats"
        return sendPOSTRequestPublisher(endpoint: endpoint, body: "")
    }

    func ballReturnPublisher() -> AnyPublisher<Bool, Error> {
        let endpoint = "clear_sequence"
        return sendPOSTRequestPublisher(endpoint: endpoint, body: "")

    }
    func dispenseBallsPublisher(numberBalls: Int) -> AnyPublisher<Bool, Error> {
        let endpoint = "dispense_ball"

        guard (0...10).contains(numberBalls),
              let asciiRepresentation = UnicodeScalar(numberBalls) else {
            return Fail(error: URLError(.badURL)).eraseToAnyPublisher()
        }

        let payload = String(asciiRepresentation)
        return sendPOSTRequestPublisher(endpoint: endpoint, body: payload)
    }

    func sendSettingsPublisher(ballDispensingMode: BallDispensingMode) -> AnyPublisher<Bool, Error> {
        let endpoint = "settings"

        guard let asciiValue = ballDispensingMode.asciiCharacter else {
            return Fail(error: URLError(.cannotDecodeContentData)).eraseToAnyPublisher()
        }

        let payload = String(asciiValue)
        return sendPOSTRequestPublisher(endpoint: endpoint, body: payload)
    }

    private func sendGETRequestPublisher(endpoint: String) -> AnyPublisher<Data, Error> {
        guard let url = URL(string: "\(baseURL)\(endpoint)") else {
            return Fail(error: URLError(.badURL)).eraseToAnyPublisher()
        }

        return URLSession.shared.dataTaskPublisher(for: url)
            .map(\.data)
            .mapError { $0 }
            .eraseToAnyPublisher()
    }

    private func sendPOSTRequestPublisher(endpoint: String, body: String) -> AnyPublisher<Bool, Error> {
        guard let url = URL(string: "\(baseURL)\(endpoint)") else {
            return Fail(error: URLError(.badURL)).eraseToAnyPublisher()
        }

        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        request.httpBody = body.data(using: .utf8)

        return URLSession.shared.dataTaskPublisher(for: request)
            .tryMap { response in
                guard let httpResponse = response.response as? HTTPURLResponse,
                      httpResponse.statusCode == 200 else {
                    throw URLError(.badServerResponse)
                }
                return true
            }
            .mapError { $0 }
            .eraseToAnyPublisher()
    }
}

extension APIManager {
    func updateIPAddress(_ newIPAddress: String) {
         guard !newIPAddress.isEmpty else {
             print("Error: IP address cannot be empty.")
             return
         }
         esp8266IP = newIPAddress
         print("Updated ESP8266 IP to: \(esp8266IP)")
     }

     /// Sends default states to firmware
    func sendDefaultStates() {
        sendSettingsPublisher(ballDispensingMode: .manual)
            .sink(receiveCompletion: { _ in }, receiveValue: { success in
                if success {
                    print("Default ball dispensing mode sent successfully.")
                }
            })
            .store(in: &cancellables)
        
        sendCourseStatePublisher(mode: .staticMode, motorPositions: ["0", "0", "0", "0", "0", "0", "0", "0", "0"])
            .sink(receiveCompletion: { _ in }, receiveValue: { success in
                if success {
                    print("Default course state sent successfully.")
                }
            })
            .store(in: &cancellables)
    }
    
    func ping(completion: @escaping (Bool, String?) -> Void) {
        sendGETRequestPublisher(endpoint: "ping")
            .receive(on: DispatchQueue.main)
            .sink(receiveCompletion: { completionStatus in
                switch completionStatus {
                case .finished:
                    break
                case .failure(let error):
                    completion(false, "Connection failed: \(error.localizedDescription)")
                }
            }, receiveValue: { _ in
                completion(true, nil) // Connection succeeded
            })
            .store(in: &cancellables)
    }
}
