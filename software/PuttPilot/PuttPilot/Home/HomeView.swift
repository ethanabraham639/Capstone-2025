//
//  HomeView.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-01-18.
//

import Foundation
import SwiftUI

struct HomeView: View {
    
    // TODO: Add background image and button styling (Styles.swift)
    var body: some View {
        VStack(alignment: /*@START_MENU_TOKEN@*/.center/*@END_MENU_TOKEN@*/, content: {
            Spacer()
            
            NavigationLink("Practice", destination: PracticePageView())
                .buttonStyle(.borderedProminent)
                .padding(20)
            
            NavigationLink("Games", destination: StartGameView())
                .buttonStyle(.borderedProminent)
                .padding(20)
            
            Spacer()
        })
    }

}
