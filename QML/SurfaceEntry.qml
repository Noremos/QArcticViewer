import Qt3D.Core 2.15
import Qt3D.Render 2.15

import Qt3D.Input 2.15
import Qt3D.Extras 2.15

Entity {
    id: root
    property BasicCamera mainCamera: BasicCamera {
        id: mainCamera
    }

    function setSource(filePath) {
        if (mesh.source === filePath)
            return true

        mesh.source = filePath
        return true
    }
    function setTextureSource(filePath) {
        if (material.texuteSource === filePath)
            return true

        material.texuteSource = filePath
        return true
    }
    function setMaterial(isTexu) {
        if (isTexu) {
            components = [transform, mesh, material]
        } else {
            components = [transform, mesh, phongMat]
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
    }
    components: [transform, mesh, phongMat]
    //    LevelOfDetail {
    //        id: lod
    //        camera: mainCamera.baseCamer
    //        thresholds: [10000000000]
    //        thresholdType: LevelOfDetail.DistanceToCameraThreshold
    //    }
    Transform {
        id: transform
        translation: Qt.vector3d(root.x, root.y, root.z)
        rotation: fromEulerAngles(theta, phi, 0)
        scale: root.scale
    }
    SpotLight {
        id: light
        //            localDirection: Qt.vector3d(1.0, -1.0, 0.0)
        //            color: Qt.rgba(0.2, 0.2, 0.2, 1.0)
    }
    //    SphereMesh {
    //        id: mesh
    //        radius: 3
    //    }
    //    TextureImage {
    //        id: texture
    //    }
    //    TextureLoader {
    //                       source: "assets/cubemaps/default/default_irradiance.dds"
    //                       wrapMode {
    //                           x: WrapMode.ClampToEdge
    //                           y: WrapMode.ClampToEdge
    //                       }
    //                       generateMipMaps: false
    //                   }
    Mesh {
        id: mesh
        source: "file:///2.obj" // "assets/obj/trefoil.obj"
    }
}
