import Qt3D.Core 2.15
import Qt3D.Render 2.15

import Qt3D.Input 2.15
import Qt3D.Extras 2.15

Entity {
    id: root

    property double surffactor: 10.0
    property BasicCamera mainCamera: BasicCamera {
        id: mainCamera
    }
    property var stdComps: [transform, mesh, light, heimat] //phongMat heimat
    property var phongComps: [transform, mesh, light, phongMat] //phongMat heimat
    property var textureComps: [transform, mesh, light, material]
    property var texture2Comps: [transform, mesh, light, material2]

    function setMaterial() {
        console.log("settign path " +projectParams.texturePath)
        if (projectParams.materialType == 0) {
            components = stdComps
        }
        if (projectParams.materialType == 1) {
            components = phongComps
        }
        if (projectParams.materialType == 2) {
            components = textureComps
        }
        if (projectParams.materialType == 3) {
            components = texture2Comps
        }
    }

    property real x: 0.0
    property real y: 0.0
    property real z: 0.0
    property real scale: 1.0
    property real theta: 0.0
    property real phi: 0.0

    PhongMaterial {
        id: phongMat
        ambient: Qt.rgba(0.5, 0.5, 0.5, 1.0)
    }

    SurfaceMaterial {
        id: material
        factor: surffactor
        texuteSource: projectParams.texturePath
    }
    SurfaceMaterial {
        id: material2
        factor: surffactor
        texuteSource: projectParams.texture2Path
    }
    HeightMaterial {
        id: heimat
        factor: surffactor
    }

    components: stdComps

    Transform {
        id: transform
        translation: Qt.vector3d(0, 0, 0)
        //        rotation: fromEulerAngles(theta, phi, 0)
        //        scale: root.scale
    }

    SpotLight {
        id: light
        localDirection: Qt.vector3d(1.0, 0.0, 1.0)
        color: Qt.rgba(1.0, 1.0, 1.0, 1.0)
        intensity: 0.9
    }

    Mesh {
        id: mesh
        objectName: "terrain"
        source: (projectParams ? "file:///" + projectParams.modelPath : "")
        //        "file:///2.obj" // "assets/obj/trefoil.obj"
    }
}
