//
//  Item.swift
//  PuttPilot
//
//  Created by Ria Narang on 2024-12-27.
//

import Foundation
import SwiftData

@Model
final class Item {
    var timestamp: Date
    
    init(timestamp: Date) {
        self.timestamp = timestamp
    }
}
