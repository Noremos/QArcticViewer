import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.12

Material {
    id: root
    property double factor: 10.0
    parameters: [
        Parameter {
            name: "factor"
            value: factor
        },
        Parameter {
            name: "minHei"
            value: projectParams?projectParams.imgMinVal/10.0:0
        },
        Parameter {
            name: "maxHei"
            value: projectParams?projectParams.imgMaxVal/10.0:0
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
