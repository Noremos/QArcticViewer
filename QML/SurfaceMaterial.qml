import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.12

//PhongMaterial {
Material {
    property alias texuteSource: skyTextureImage.source

    //PhongMaterial {
    //    ambient: Qt.rgba(0.5, 0.5, 0.5, 1.0)
    id: root

    //    property color ambient: Qt.rgba(0.5, 0.5, 0.5, 1.0)
    parameters: [
        //        Parameter {
        //            name: "ambient"
        //            value: Qt.rgba(1.0, 1.0, 1.0, 1.0)
        //        },

        // Parameter {name: "ka"
        // value: Qt.vector3d(ambient.r, ambient.g, ambient.b)   },
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
                    source: "file:///l.png"
                    //                    source: "file:///D:/50_59_1_2_2m_v3.0_reg_dem_browse.jpg"
                }
            }
        }
    ]
    effect: SurfaceEffect {}
} //Material {//    id: root//    property color ambient: Qt.rgba(0.05, 0.05, 0.05, 1.0)//    property color diffuse: Qt.rgba(0.7, 0.7, 0.7, 1.0)//    property color specular: Qt.rgba(0.95, 0.95, 0.95, 1.0)//    property real shininess: 150.0//    parameters: [//        Parameter {//            name: "ka"//            value: Qt.vector3d(root.ambient.r, root.ambient.g, root.ambient.b)
//        },
//        Parameter {
//            name: "kd"
//            value: Qt.vector3d(root.diffuse.r, root.diffuse.g, root.diffuse.b)
//        },
//        Parameter {
//            name: "ksp"
//            value: Qt.vector3d(root.specular.r, root.specular.g,
//                               root.specular.b)
//        },
//        Parameter {
//            name: "shininess"
//            value: root.shininess
//        },
//        Parameter {
//            name: "line.width"
//            value: root.lineWidth
//        },
//        Parameter {
//            name: "line.color"
//            value: root.lineColor
//        }
//    ]
//}

