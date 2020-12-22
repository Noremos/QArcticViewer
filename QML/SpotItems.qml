import Qt3D.Core 2.15
import Qt3D.Render 2.15

import Qt3D.Input 2.15
import Qt3D.Extras 2.15

Entity {
    id: root
    property double factor: factor

    Mesh {
        id: mesh
        objectName: "mesh"
        source: "qrc:/3dspot.obj"
    }

    PhongMaterial {
        id: phong
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
        components: [phong, trans2, mesh]
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
}
