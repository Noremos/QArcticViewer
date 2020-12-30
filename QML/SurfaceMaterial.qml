import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.12

Material {
    property alias texuteSource: skyTextureImage.source

    id: root
    property double factor: 10.0
    parameters: [
        Parameter {
            name: "terrain"
            value: Texture2D {
                id: skyTexture
                minificationFilter: Texture.LinearMipMapLinear
                magnificationFilter: Texture.Linear
                wrapMode {
                    x: WrapMode.Repeat
                    y: WrapMode.Repeat
                }
                generateMipMaps: true
                maximumAnisotropy: 16.0
                TextureImage {
                    id: skyTextureImage
                }
            }
        },
        Parameter {
            name: "factor"
            value: factor
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
                            vertexShaderCode: loadSource(
                                                  Qt.resolvedUrl(
                                                      "../shaders/simpleColor.vert"))
                            fragmentShaderCode: loadSource(
                                                    Qt.resolvedUrl(
                                                        "../shaders/simpleColor.frag"))
                        }
                    }
                ]
            }
        ]
    }
}
