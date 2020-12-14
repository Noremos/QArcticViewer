import Qt3D.Core 2.15
import Qt3D.Render 2.15

import Qt3D.Input 2.15
import Qt3D.Extras 2.15

Entity {
    id: root
    property alias position: trans.translation
    Mesh {
        id: mesh
        source: "qrc:/spot.obj"
    }

    Material {
        id: mater
        //        parameters: [
        //            Parameter {
        //                name: "hei"
        //                value: 100
        //            }
        //        ]
        effect: Effect {
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

    Transform {
        id: trans
        objectName: "transform"
        scale3D: Qt.vector3d(100, 100, 100)
        translation: Qt.vector3d(-10, 10, -10)
    }
    PhongMaterial {
        id: phong
    }
    components: [mesh, phong, trans]
}
