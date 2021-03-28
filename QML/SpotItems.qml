import Qt3D.Core 2.15
import Qt3D.Render 2.15

import Qt3D.Input 2.15
import Qt3D.Extras 2.15

import QtQml.Models 2.15
import My 1.0

Entity {
    id: root
    property double factor: factor

    property alias model: buffer
    property var mainCamera
    Mesh {
        id: mesh
        objectName: "mesh"
        source: "qrc:/3dspot.obj"
    }

    PhongMaterial {
        id: textMater
        objectName: "phong"
        diffuse: Qt.rgba(0, 1.0, 1.0, 1.0)
        //        diffuse: Qt.vector3d(1.0, 1.0, 0)
        //        ambient: Qt.vector4d(1.0, 1.0, 0)
    }

    Material {
        id: mater
        objectName: "material"
        //        parameters: [
        //            Parameter {
        //                name: "hei"
        //                value: 100
        //            }
        //        ]
        effect: Effect {
            parameters: [
                Parameter {
                    name: "factor"
                    value: root.factor
                    //                    onValueChanged: console.log(root.factor)
                }
            ]
            techniques: [
                Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        majorVersion: 3
                        minorVersion: 1
                    }

                    filterKeys: [
                        FilterKey {
                            name: "renderingStyle"
                            value: "forward"
                        }
                    ]
                    renderPasses: [
                        RenderPass {
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource(
                                                      Qt.resolvedUrl(
                                                          "qrc:/shaders/spotZone.vert"))
                                fragmentShaderCode: loadSource(
                                                        Qt.resolvedUrl(
                                                            "qrc:/shaders/spotZone.frag"))
                            }
                            //                            renderStates: [alphaCoverage]
                        }
                    ]
                }
            ]
        }
    }

    property int counter: 0
    function addItem() {
        var newObject = Qt.createQmlObject(
                    'import Qt3D.Core 2.15; import Qt3D.Render 2.15;  Transform {scale3D: Qt.vector3d(100, 100, 100); translation: Qt.vector3d(-10, 10, -10);  }',
                    root, "dynamicSnippet" + counter)
        counter += 1
    }

    Entity {
        objectName: "ZeroArrow"
        //                id: test
        Transform {
            id: trans2
            scale3D: Qt.vector3d(1, 300, 1)
            translation: Qt.vector3d(0, 0, 0)
        }
        components: [textMater, trans2, mesh]
    }

    Entity {
        objectName: "test"
        //                id: test
        Transform {
            id: trans3
            scale3D: Qt.vector3d(100, 1, 100)
            translation: Qt.vector3d(-50, 10, -50)
        }
        components: [mater, trans3, mesh]
    }

    InstanseModel {
        id: buffer
        objectName: "buffer"
    }

    NodeInstantiator {
        id: spots

        model: buffer
        //        asynchronous: true
        //        model: root.count
        delegate: Entity {
            required property var mtrans
            required property var mscale
            required property var mtextTrans
            required property string mttext
            Entity {
                //                pbejctName:"MEW " + index+ " " k;
                Transform {
                    id: boxTrans
                    scale3D: mscale
                    translation: mtrans
                }
                components: [mater, boxTrans, mesh]
            }
            Entity {
                Text2DEntity {

                    //                LevelOfDetail {
                    //                    id: lod
                    //                    camera: root.mainCamera
                    //                    //                    thresholds: [500, 1000,1500]
                    //                    //                    thresholdType: LevelOfDetail.DistanceToCameraThreshold
                    //                    thresholds: [1000, 600, 300, 180]
                    //                    thresholdType: LevelOfDetail.ProjectedScreenPixelSizeThreshold
                    //                    volumeOverride: lod.createBoundingSphere(Qt.vector3d(0, 0,
                    //                                                                         0),
                    //                                                             2.0)
                    //                    //                    onCurrentIndexChanged: textMesh.enabled = lod.currentIndex == 0
                    //                }
                    id: textMesh
                    text: mttext

                    font.family: "Helvetica"
                    font.pointSize: 2
                    font.bold: true
                    color: "#ff7f50"

                    width: text.length * font.pointSize
                    height: font.pointSize * 2
                }
                Transform {
                    id: textTrans
                    //                    scale3D: Qt.vector3d(0.5, 0.5, 0.1)
                    //                    scale: Qt.vector2d(0.5, 0.5)
                    translation: mtextTrans
                    //                    rotation: fromEulerAngles(root.mainCamera.upVector)
                    rotationX: -90
                    rotationY: 180
                }
                components: [/*lod,*/ mttext, textTrans]
            }
        }
    }
}
