import QtQuick 2.1 as QQ2
import Qt3D.Core 2.15
import Qt3D.Render 2.15
import Qt3D.Input 2.15
import Qt3D.Extras 2.15
import QtQuick.Scene3D 2.0

import QtQml 2.15

Scene3D {
    id: scene3d
    focus: true
    aspects: ["input", "logic"]
    cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
    property double surfaceFactor: smesh.factor

    property alias spotZones: spotZones
    property alias markerZones: markerZones

    Component.onCompleted: backend.setStopItem(spotZones)
    function printHits(desc, hits) {
        console.log(desc, hits.length)
        for (var i = 0; i < hits.length; i++) {
            if ("surface" !== hits[i].entity.objectName)
                continue

            console.log("  ", hits[i].worldIntersection.x,
                        hits[i].worldIntersection.y,
                        hits[i].worldIntersection.z)
        }
    }

    Entity {
        id: sceneRoot
        objectName: "entity"

        components: [
            RenderSettings {
                id: render
                activeFrameGraph: RenderStateSet {
                    renderStates: [
                        CullFace {
                            mode: CullFace.NoCulling
                        }
                    ]
                    ForwardRenderer {
                        camera: smesh.mainCamera.baseCamer
                        clearColor: "transparent"
                    }
                }

                pickingSettings.pickMethod: PickingSettings.BoundingVolumePicking
                pickingSettings.faceOrientationPickingMode: PickingSettings.FrontAndBackFace
            },
            InputSettings {
                id: inputs
            },
            ScreenRayCaster {
                id: screenRayCaster
            },
            MouseHandler {
                id: mouseHandler
                sourceDevice: MouseDevice {}

                //                onReleased: {

                //                    //                    console.log(Qt.point(mouse.x, mouse.y))
                //                    screenRayCaster.trigger(Qt.point(mouse.x, mouse.y))
                //                    printHits("Screen hits", screenRayCaster.hits)
                //                }
            },
            ObjectPicker {
                onPressed: {
                    console.log(pick.entity.objectName)
                }
            }
        ]

        SpotItems {
            id: spotZones
            factor: scene3d.surfaceFactor
            objectName: "spotZones"
            mainCamera: smesh.mainCamera.baseCamer
            //            enabled: false
            //            SpotsEffect {

            //                //                id: test
            //                position: Qt.vector3d(10, 20, 30)
            //            }
        }
        SpotItems {
            id: markerZones
            objectName: "markerZones"
        }

        SurfaceEntry {
            id: smesh
            objectName: "surface"
            surffactor: surfaceFactor
        }
    }

    function setTextureSource(filePath) {
        let needUpdateSRC = smesh.setTextureSource(filePath)
        //        if (needUpdateSRC)
        //            scene3d.update()
    }
    function setTexturetype() {
        smesh.setMaterial()
    }
    function setFindingMode(useText) {
        spotZones.enabled = useText
    }

    function ude() {
        spotZones.enabled = true
        console.log(spotZones.childNodes[5].components)
        scene3d.update()
    }
}
