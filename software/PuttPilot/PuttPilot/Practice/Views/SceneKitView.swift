//
//  SceneKitView.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-02-19.
//

import SwiftUI
import SceneKit

struct SceneKitView: UIViewRepresentable {
    @Binding var animationState: [[Double]]
    var numRows: Int
    var numCols: Int

    func makeUIView(context: Context) -> SCNView {
        let scnView = SCNView()
        scnView.scene = createScene()
        scnView.allowsCameraControl = true // Enable basic camera controls
        scnView.backgroundColor = UIColor.clear
        return scnView
    }
    
    func updateUIView(_ scnView: SCNView, context: Context) {
        guard let rootNode = scnView.scene?.rootNode else { return }
        
        // Remove existing surface node
        if let existingNode = rootNode.childNode(withName: "greenSurface", recursively: false) {
            existingNode.removeFromParentNode()
        }

        // Create new surface node
        let surfaceNode = SCNNode(geometry: createSurfaceGeometry())
        surfaceNode.name = "greenSurface"
        rootNode.addChildNode(surfaceNode)
    }


    
    // Build the SceneKit scene with a camera, lighting, and the green surface node
    func createScene() -> SCNScene {
        let scene = SCNScene()
        
        // Create and add the green surface node
        let surfaceNode = SCNNode(geometry: createSurfaceGeometry())
        surfaceNode.name = "greenSurface"
        scene.rootNode.addChildNode(surfaceNode)
        
        // Add a camera
        let cameraNode = SCNNode()
        cameraNode.camera = SCNCamera()
        cameraNode.position = SCNVector3(x: 0, y: 20, z: 40)
        cameraNode.eulerAngles = SCNVector3(-3.14 / 6, 0, 0)  // Tilt down 30 degrees
        scene.rootNode.addChildNode(cameraNode)
        
        // Add a main light
        let lightNode = SCNNode()
        lightNode.light = SCNLight()
        lightNode.light?.type = .omni
        lightNode.position = SCNVector3(x: 0, y: 50, z: 50)
        scene.rootNode.addChildNode(lightNode)
        
        // Add ambient light
        let ambientLight = SCNNode()
        ambientLight.light = SCNLight()
        ambientLight.light?.type = .ambient
        ambientLight.light?.color = UIColor.darkGray
        scene.rootNode.addChildNode(ambientLight)
        
        return scene
    }
    
    func createSurfaceGeometry() -> SCNGeometry {
        let width: CGFloat = 20
        let length: CGFloat = 60

        // Use a resolution factor (e.g., 5) to increase mesh detail.
        let resolutionFactor = 10

        // First, get the fine grid using bilinear interpolation.
        let fineGrid = smoothGrid(from: animationState, resolutionFactor: resolutionFactor)
        let fineRows = fineGrid.count
        let fineCols = fineGrid[0].count
        
        let dx = width / CGFloat(fineCols - 1)
        let dz = length / CGFloat(fineRows - 1)

        var vertices: [SCNVector3] = []
        for row in 0..<fineRows {
            for col in 0..<fineCols {
                let x = -width / 2 + CGFloat(col) * dx
                let z = -length / 2 + CGFloat(row) * dz
                // Scale the fine grid value to your desired height.
                let y = CGFloat(fineGrid[row][col]) / 90.0 * 5.0
                vertices.append(SCNVector3(x, y, z))
            }
        }

        var indices: [Int32] = []
        for row in 0..<(fineRows - 1) {
            for col in 0..<(fineCols - 1) {
                let topLeft = Int32(row * fineCols + col)
                let topRight = topLeft + 1
                let bottomLeft = topLeft + Int32(fineCols)
                let bottomRight = bottomLeft + 1
                
                indices.append(contentsOf: [topLeft, bottomLeft, topRight])
                indices.append(contentsOf: [topRight, bottomLeft, bottomRight])
            }
        }

        let vertexSource = SCNGeometrySource(vertices: vertices)
        let indexData = Data(bytes: indices, count: indices.count * MemoryLayout<Int32>.size)
        let element = SCNGeometryElement(data: indexData,
                                         primitiveType: .triangles,
                                         primitiveCount: indices.count / 3,
                                         bytesPerIndex: MemoryLayout<Int32>.size)
        let geometry = SCNGeometry(sources: [vertexSource], elements: [element])
        
        let material = SCNMaterial()
        material.diffuse.contents = UIImage(named: "GrassTexture") ?? UIColor.green
        geometry.materials = [material]

        return geometry
    }
    
    /// Generates a fine grid from a coarse grid by bilinear interpolation.
    /// - Parameters:
    ///   - coarse: The original grid of values (e.g. motor positions).
    ///   - resolutionFactor: How many subdivisions per coarse cell.
    /// - Returns: A fine grid with smoothed values.
    func smoothGrid(from coarse: [[Double]], resolutionFactor: Int) -> [[Double]] {
        let nRows = coarse.count
        let nCols = coarse[0].count
        let fineRows = (nRows - 1) * resolutionFactor + 1
        let fineCols = (nCols - 1) * resolutionFactor + 1
        
        var fineGrid = Array(
            repeating: Array(repeating: 0.0, count: fineCols),
            count: fineRows
        )
        
        for i in 0..<fineRows {
            // Compute a normalized coordinate (0...1) in the coarse grid vertically.
            let t = Double(i) / Double(fineRows - 1)
            let coarseRowFloat = t * Double(nRows - 1)
            let rowIndex = Int(coarseRowFloat)
            let rowFrac = coarseRowFloat - Double(rowIndex)
            
            for j in 0..<fineCols {
                // Compute a normalized coordinate (0...1) in the coarse grid horizontally.
                let u = Double(j) / Double(fineCols - 1)
                let coarseColFloat = u * Double(nCols - 1)
                let colIndex = Int(coarseColFloat)
                let colFrac = coarseColFloat - Double(colIndex)
                
                // Get the four neighboring coarse values.
                let v00 = coarse[rowIndex][colIndex]
                let v10 = (rowIndex + 1 < nRows) ? coarse[rowIndex + 1][colIndex] : v00
                let v01 = (colIndex + 1 < nCols) ? coarse[rowIndex][colIndex + 1] : v00
                let v11 = (rowIndex + 1 < nRows && colIndex + 1 < nCols) ? coarse[rowIndex + 1][colIndex + 1] : v00
                
                // Perform bilinear interpolation.
                let v0 = v00 * (1 - colFrac) + v01 * colFrac
                let v1 = v10 * (1 - colFrac) + v11 * colFrac
                let v = v0 * (1 - rowFrac) + v1 * rowFrac
                
                fineGrid[i][j] = v
            }
        }
        
        return fineGrid
    }

}
