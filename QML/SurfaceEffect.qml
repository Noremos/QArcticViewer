import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    id: root

    //    AlphaCoverage { id: alphaCoverage }
    //    DepthTest {depthFunction: DepthTest.Less }
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
            //parameters:
            renderPasses: [
                RenderPass {
                    shaderProgram: ShaderProgram {
                        vertexShaderCode: loadSource(
                                              Qt.resolvedUrl(
                                                  "../shaders/simpleColor.vert"))
                        fragmentShaderCode: loadSource(
                                                Qt.resolvedUrl(
                                                    "../shaders/simpleColor.frag"))
                        //                        renderStates: [alphaCoverage ]
                    }
                }
            ]
        }
    ]
}
