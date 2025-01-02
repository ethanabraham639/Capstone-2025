////
////  APIManager.swift
////  PuttPilot
////
////  Created by Ria Narang on 2024-12-27.
////

import Foundation
import Combine

enum Mode: String {
    case staticMode = "1"
    case wave = "2"
    case tsunami = "3"
    case ballReturn = "4"

    var asciiCharacter: Character? {
        guard let asciiValue = rawValue.first?.asciiValue else { return nil }
        return Character(UnicodeScalar(asciiValue))
    }
}

enum BallDispensingMode: String {
    case automatic = "1"
    case manual = "0"

    var asciiCharacter: Character? {
        guard let asciiValue = rawValue.first?.asciiValue else { return nil }
        return Character(UnicodeScalar(asciiValue))
    }
}

class APIManager {
    static let shared = APIManager()
    
    private var esp8266IP: String = "10.0.0.31" // Default value
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
                    guard let intValue = Int(String(UnicodeScalar(byte))) else { return nil }
                    return intValue
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
                    guard let intValue = Int(String(UnicodeScalar(byte))) else { return nil }
                    return intValue
                }
            }
            .eraseToAnyPublisher()
    }
    
    func sendCourseStatePublisher(mode: Mode, motorPositions: [String]) -> AnyPublisher<Bool, Error> {
        let endpoint = "course_state"

        guard let modeAscii = mode.asciiCharacter else {
            return Fail(error: URLError(.badURL)).eraseToAnyPublisher()
        }

        let motorAsciiArray = motorPositions.compactMap { position -> Character? in
            guard let ascii = UnicodeScalar(Int(position) ?? -1), ascii.isASCII else { return nil }
            return Character(ascii)
        }

        guard motorAsciiArray.count == motorPositions.count else {
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


//
//import Foundation
//
//// TODO: move to seperate file
//enum Mode: String {
//    case staticMode = "1"  // ASCII representation for "static"
//    case wave = "2"        // ASCII representation for "wave"
//    case tsunami = "3"     // ASCII representation for "tsunami"
//    case ballReturn = "4"  // ASCII representation for "ball return"
//
//    /// Converts the mode's raw value into its ASCII `Character` representation
//    var asciiCharacter: Character? {
//        guard let asciiValue = rawValue.first?.asciiValue else { return nil }
//        return Character(UnicodeScalar(asciiValue))
//    }
//}
//
//
//class APIManager {
//    // Would it make sense to set up the Combine publishers here for polled data sources
//    // Adding error messages ?
//
//    static let shared = APIManager()
//
//    // could make an input field for IP so we dont have to rebuild everytime
//    private let esp8266IP = "10.0.0.31" // Need to replace with ESP IP address
//    private let baseURL: URL
//
//    init() {
//        guard let url = URL(string: "http://\(esp8266IP)/") else {
//            fatalError("Invalid ESP8266 IP Address")
//        }
//
//        self.baseURL = url
//    }
//
//    /// The position of every motor in the course based on the user input. And the course mode (static, ball return, wave, …)
//    func sendCourseState(mode: Mode, motorPositions: [String], completion: @escaping (Bool) -> Void) {
//        // TODO: can add a check to ensure the motorPositions array is of the correct size
//        let endpoint = "course_state"
//
//        guard let modeAscii = mode.asciiCharacter else {
//            print("Error: Failed to convert mode to ASCII.")
//            completion(false)
//            return
//        }
//
//        // Convert motor positions to ASCII
//        let motorAsciiArray = motorPositions.compactMap { position -> Character? in
//            guard let ascii = UnicodeScalar(Int(position) ?? -1), ascii.isASCII else { return nil }
//            return Character(ascii)
//        }
//
//        guard motorAsciiArray.count == motorPositions.count else {
//            print("Error: Failed to convert one or more motor positions to ASCII.")
//            completion(false)
//            return
//        }
//
//        let payload = String(modeAscii) + motorAsciiArray.map { String($0) }.joined()
//
//        sendPOSTRequest(endpoint: endpoint, body: payload, completion: completion)
//    }
//
//    /// Fetches the current error codes from the firmware
//    /// Returns: An array of integers representing error codes
//    func fetchErrorCodes(completion: @escaping ([Int]?) -> Void) {
//        let endpoint = "error_codes"
//        sendGETRequest(endpoint: endpoint) { data in
//            guard let data = data else {
//                completion(nil)
//                return
//            }
//
//            let errorCodes = data.compactMap { byte -> Int? in
//                if let intValue = Int(String(UnicodeScalar(byte))) {
//                    return intValue
//                }
//                return nil
//            }
//
//            if !errorCodes.isEmpty {
//                completion(errorCodes)
//            } else {
//                print("No error codes received or invalid response")
//                completion(nil)
//            }
//        }
//    }
//
//
//
//    // TODO: DEBUG_MSG
//    func fetchDebugMessage(completion: @escaping (String?) -> Void) {
//        let endpoint = "debug_msg"
//        sendGETRequest(endpoint: endpoint) { data in
//            guard let data = data else {
//                completion(nil)
//                return
//            }
//            // TODO: Update to also print motor positions, and course error codes, as well as the debug message
//            completion(data.base64EncodedString())
//        }
//    }
//
//    func resetStats(completion: @escaping (Bool) -> Void) {
//        let endpoint = "reset_stats"
//        sendPOSTRequest(endpoint: endpoint, body: "", completion: completion)
//    }
//
//    /// Fetches the current stats from the firmware
//    /// Returns: [balls_hit, balls_in_hole]
//    func fetchStats(completion: @escaping ([Int]?) -> Void) {
//        let endpoint = "stats"
//        sendGETRequest(endpoint: endpoint) { data in
//            guard let data = data else {
//                completion(nil)
//                return
//            }
//
//            let stats = data.compactMap { byte -> Int? in
//                if let intValue = Int(String(UnicodeScalar(byte))) {
//                    return intValue
//                }
//                return nil
//            }
//
//            if stats.count == 2, stats[0] >= stats[1] { // Validate balls_hit >= balls_in_hole
//                completion(stats)
//            } else {
//                print("Invalid stats received: \(stats)")
//                completion(nil)
//            }
//        }
//    }
//
//    func dispenseBalls(numberBalls: Int, completion: @escaping (Bool) -> Void) {
//        let endpoint = "dispense_ball"
//
//        guard (0...10).contains(numberBalls) else {
//            print("Error: Number of balls must be between 0 and 10.")
//            completion(false)
//            return
//        }
//
//        guard let asciiRepresentation = UnicodeScalar(numberBalls) else {
//            print("Error: Failed to convert number to ASCII.")
//            completion(false)
//            return
//        }
//
//        let payload = String(asciiRepresentation)
//        sendPOSTRequest(endpoint: endpoint, body: payload, completion: completion)
//    }
//
//
//    /// Tells firmware different settings
//    /// ballDispensing: 0 = manual, 1 = autoDispense
//    func sendSettings(ballDispensingMode: Character, completion: @escaping (Bool) -> Void) {
//        let endpoint = "settings"
//
//        guard let asciiValue = ballDispensingMode.asciiValue else {
//            print("Error: Failed to convert ballDispensingMode to ASCII.")
//            completion(false)
//            return
//        }
//
//        let payload: String = String(bytes: [asciiValue], encoding: .utf8) ?? ""
//        sendPOSTRequest(endpoint: endpoint, body: payload, completion: completion)
//    }
//
//
//
//    // MARK: Private
//
//    private func sendGETRequest(endpoint: String, completion: @escaping (Data?) -> Void) {
//        guard let url = URL(string: "\(baseURL)\(endpoint)") else {
//            print("Invalid URL for GET: \(endpoint)")
//            completion(nil)
//            return
//        }
//
//        let request = URLRequest(url: url)
//        URLSession.shared.dataTask(with: request) { data, response, error in
//            if let error = error {
//                print("GET request failed: \(error)")
//                completion(nil)
//                return
//            }
//
//            if let httpResponse = response as? HTTPURLResponse, httpResponse.statusCode == 200 {
//                completion(data)
//            } else {
//                completion(nil)
//            }
//        }.resume()
//    }
//
//    private func sendPOSTRequest(endpoint: String, body: String, completion: @escaping (Bool) -> Void) {
//        guard let url = URL(string: "\(baseURL)\(endpoint)") else {
//            print("Invalid URL for POST: \(endpoint)")
//            completion(false)
//            return
//        }
//
//        var request = URLRequest(url: url)
//        request.httpMethod = "POST"
//        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
//        request.httpBody = try? JSONSerialization.data(withJSONObject: body, options: [])
//
//        URLSession.shared.dataTask(with: request) { _, response, error in
//            if let error = error {
//                print("POST request failed: \(error)")
//                completion(false)
//                return
//            }
//
//            if let httpResponse = response as? HTTPURLResponse, httpResponse.statusCode == 200 {
//                completion(true)
//            } else {
//                completion(false)
//            }
//        }.resume()
//    }
//}
//
//
//
