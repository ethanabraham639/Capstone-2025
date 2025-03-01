//
//  SceneKitView.swift
//  PuttPilot
//
//  Created by Ria Narang on 2025-02-19.
//

import SwiftUI
import SceneKit

struct SceneKitView: UIViewRepresentable {
    @Binding var gridInputs: [[Double]]
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
        // Update the green surface geometry with new actuator heights
        if let surfaceNode = scnView.scene?.rootNode.childNode(withName: "greenSurface", recursively: false) {
            surfaceNode.geometry = createSurfaceGeometry()
        }
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
    
    // Build the surface geometry using gridInputs
    func createSurfaceGeometry() -> SCNGeometry {
        // Define the overall width and length of the putting green.
        let width: CGFloat = 20  // adjust to your course dimensions
        let length: CGFloat = 60 // adjust to your course dimensions
        
        // Compute the spacing between grid points.
        let dx = width / CGFloat(numCols - 1)
        let dz = length / CGFloat(numRows - 1)
        
        var vertices: [SCNVector3] = []
        for row in 0..<numRows {
            for col in 0..<numCols {
                // Calculate the x and z positions so that the green is centered.
                let x = -width / 2 + CGFloat(col) * dx
                let z = -length / 2 + CGFloat(row) * dz
                // Map the actuator value (0-90) to a height (y). Here we assume a maximum height of 5 units.
                let y = CGFloat(gridInputs[row][col]) / 90.0 * 5.0
                vertices.append(SCNVector3(x, y, z))
            }
        }
        
        // Create indices to form triangles for the grid mesh.
        var indices: [Int32] = []
        for row in 0..<(numRows - 1) {
            for col in 0..<(numCols - 1) {
                let topLeft = Int32(row * numCols + col)
                let topRight = topLeft + 1
                let bottomLeft = topLeft + Int32(numCols)
                let bottomRight = bottomLeft + 1
                
                // Triangle 1
                indices.append(contentsOf: [topLeft, bottomLeft, topRight])
                // Triangle 2
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
        
        // Set a green material with an optional texture.
        let material = SCNMaterial()
        // Use a grass texture if you have one, otherwise default to green.
        material.diffuse.contents = UIImage(named: "GrassTexture") ?? UIColor.green
        material.emission.contents = [UIColor.black]
        geometry.materials = [material]
        
        return geometry
    }
}
