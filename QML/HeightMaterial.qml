import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.12

Material {
    id: root
    property double factor: 10.0
    onFactorChanged: console.log("!")
    parameters: [
        Parameter {
            name: "factor"
            value: factor
            onValueChanged: console.log("!!")
        }
    ]
    effect: Effect {
        id: effec

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
                            id: shad
                            vertexShaderCode: loadSource(
                                                  Qt.resolvedUrl(
                                                      "../shaders/HeightFactor.vert"))
                            fragmentShaderCode: loadSource(
                                                    Qt.resolvedUrl(
                                                        "../shaders/HeightFactor.frag"))
                        }
                    }
                ]
            }
        ]
    }
}
