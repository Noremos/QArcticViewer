import QtQuick 2.1 as QQ2
import Qt3D.Core 2.15
import Qt3D.Render 2.15
import Qt3D.Input 2.15
import Qt3D.Extras 2.15
import QtQuick.Scene3D 2.0

Scene3D {
    id: scene3d
    focus: true
    aspects: ["input", "logic"]
    cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

    Entity {
        id: sceneRoot

        components: [
            RenderSettings {
                activeFrameGraph: ForwardRenderer {
                    camera: smesh.mainCamera.baseCamer
                    clearColor: "transparent"
                }
            },
            InputSettings {}
        ]

        SurfaceEntry {
            id: smesh
        }
    }

    function setModelSource(filePath) {
        let needUpdateSRC = smesh.setSource(filePath)
        if (needUpdateSRC)
            scene3d.update()
    }

    function setTextureSource(filePath) {
        let needUpdateSRC = smesh.setTextureSource(filePath)
        if (needUpdateSRC)
            scene3d.update()
    }

    //    Entity {
    //        id: root

    //        // Render from the mainCamera
    //        components: [
    //            RenderSettings {
    //                activeFrameGraph: ForwardRenderer {
    //                    id: renderer
    //                    camera: mainCamera
    //                }
    //            },
    //            // Event Source will be set by the Qt3DQuickWindow
    //            InputSettings {}
    //        ]

    //        Camera {
    //            id: camera
    //            projectionType: CameraLens.PerspectiveProjection
    //            fieldOfView: 45
    //            nearPlane: 0.1
    //            farPlane: 1000.0
    //            position: Qt.vector3d(0.0, 0.0, 40.0)
    //            upVector: Qt.vector3d(0.0, 1.0, 0.0)
    //            viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    //        }

    //        Entity {
    //            id: mainEntry
    //            components: [mainCamera, mesh]
    //        }

    //        SphereMesh {
    //            id: mesh
    //            radius: 3
    //        }

    //        BasicCamera {
    //            id: mainCamera
    //            position: Qt.vector3d(0.0, 0.0, 15.0)
    //            viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    //        }

    //        //        FirstPersonCameraController {
    //        //            camera: mainCamera
    //        //        }
    //        SurfaceMaterial {
    //            id: wireframeMaterial
    //            effect: SurfaceEffect {}
    //            ambient: Qt.rgba(0.2, 0.0, 0.0, 1.0)
    //            diffuse: Qt.rgba(0.8, 0.0, 0.0, 1.0)

    //        }

    //        SurfaceMesh {
    //            id: smesh
    //            material: wireframeMaterial
    //            x: 0
    //            y: 0
    //            z: 0
    //        }
    //    }
}
