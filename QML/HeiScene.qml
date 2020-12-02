

/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtDataVisualization 1.0
import "."
import My 1.0

import QtQuick3D 1.15

Item {
    id: mainview

    property real fontSize: 12
    //    property alias heightMapFile: heiMap.heightMapFile
    function run(filePath) {
        mapModel.loadImage(filePath)
        surfaceLayers.update()
    }

    Item {
        id: surfaceView
        width: mainview.width - buttonLayout.width
        height: mainview.height
        anchors.right: mainview.right

        //! [0]
        ColorGradient {
            id: layerOneGradient
            ColorGradientStop {
                position: 0.0
                color: "black"
            }
            ColorGradientStop {
                position: 0.31
                color: "tan"
            }
            ColorGradientStop {
                position: 0.32
                color: "green"
            }
            ColorGradientStop {
                position: 0.40
                color: "darkslategray"
            }
            ColorGradientStop {
                position: 1.0
                color: "white"
            }
        }

        ColorGradient {
            id: layerTwoGradient
            ColorGradientStop {
                position: 0.315
                color: "blue"
            }
            ColorGradientStop {
                position: 0.33
                color: "white"
            }
        }

        ColorGradient {
            id: layerThreeGradient
            ColorGradientStop {
                position: 0.0
                color: "red"
            }
            ColorGradientStop {
                position: 0.15
                color: "black"
            }
        }
        View3D {
            id: view

            anchors.fill: parent
            // Scene to view
            Node {
                id: scene

                Light {

                    id: directionalLight
                }

                Camera {
                    id: camera
                    // It's important that your camera is not inside
                    // your model so move it back along the z axis
                    // The Camera is implicitly facing up the z axis,
                    // so we should be looking towards (0, 0, 0)
                    z: -600
                }

                Model {
                    id: cubeModel
                    // #Cube is one of the "built-in" primitive meshes
                    // Other Options are:
                    // #Cone, #Sphere, #Cylinder, #Rectangle
                    source: "#Cube"

                    // When using a Model, it is not enough to have a
                    // mesh source (ie "#Cube")
                    // You also need to define what material to shade
                    // the mesh with. A Model can be built up of
                    // multiple sub-meshes, so each mesh needs its own
                    // material. Materials are defined in an array,
                    // and order reflects which mesh to shade

                    // All of the default primitive meshes contain one
                    // sub-mesh, so you only need 1 material.
                    materials: [

                        DefaultMaterial {

                            // We are using the DefaultMaterial which
                            // dynamically generates a shader based on what
                            // properties are set. This means you don't
                            // need to write any shader code yourself.
                            // In this case we just want the cube to have
                            // a red diffuse color.
                            id: cubeMaterial
                            diffuseColor: "red"
                        }
                    ]
                }
            }
        }

        //! [0]
        //        Surface3D {
        //            id: surfaceLayers
        //            width: surfaceView.width
        //            height: surfaceView.height
        //            theme: Theme3D {
        //                type: Theme3D.ThemeEbony
        //                font.pointSize: 35
        //                colorStyle: Theme3D.ColorStyleRangeGradient
        //            }
        //            shadowQuality: AbstractGraph3D.ShadowQualityNone
        //            selectionMode: AbstractGraph3D.SelectionRow | AbstractGraph3D.SelectionSlice
        //            scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeft
        //            //            axisY.min: 20
        //            //            axisY.max: 200
        //            axisX.segmentCount: 5
        //            axisX.subSegmentCount: 2
        //            axisX.labelFormat: "%i"
        //            axisZ.segmentCount: 5
        //            axisZ.subSegmentCount: 2
        //            axisZ.labelFormat: "%i"
        //            axisY.segmentCount: 5
        //            axisY.subSegmentCount: 2
        //            axisY.labelFormat: "%f"

        //            //! [1]
        //            //! [2]
        //            Surface3DSeries {
        //                id: surfaceSeries
        //                flatShadingEnabled: false
        //                drawMode: Surface3DSeries.DrawSurface

        //                ItemModelSurfaceDataProxy {
        //                    itemModel: HeiMapModel {
        //                        id: mapModel
        //                    }
        //                    rowRole: "row" //"longitude"
        //                    columnRole: "column" //"latitude"
        //                    yPosRole: "value" //"height"
        //                }
        //                //                onDrawModeChanged: checkState()
        //            }
        //            //            Surface3DSeries {
        //            //                id: layerOneSeries
        //            //                baseGradient: layerOneGradient

        //            //                //! [2]
        //            //                //                dataProxy: heiMap
        //            //                HeightMapSurfaceDataProxy {
        //            //                    id: heiMap
        //            //                    //D:\Programs\QT\QArcticViewer\sources\QArcticViewer
        //            //                    heightMapFile: ":/heightmaps/layer_1.png"

        //            //                    //"file:///C:\Users\Sam\Downloads\SETSM_WV01_20141119_102001003621A900_1020010036AA3A00_seg2_2m_v1.0"
        //            //                    //":/heightmaps/layer_1.png"
        //            //                }
        //            //                flatShadingEnabled: false
        //            //                drawMode: Surface3DSeries.DrawSurface
        //            //                //! [4]
        //            //                visible: layerOneToggle.checked // bind to checkbox state
        //            //                //! [4]
        //            //            }
        //            //! [1]
        //        }
    }

    ColumnLayout {
        id: buttonLayout
        anchors.top: parent.top
        anchors.left: parent.left
        spacing: 0

        //! [3]
        GroupBox {
            flat: true
            Layout.fillWidth: true
            Column {
                spacing: 10

                Label {
                    font.pointSize: fontSize
                    font.bold: true
                    text: "Layer Selection"
                }

                CheckBox {
                    id: layerOneToggle
                    checked: true
                    style: CheckBoxStyle {
                        label: Label {
                            font.pointSize: fontSize
                            text: "Show Ground Layer"
                        }
                    }
                }
            }
        }
        //! [3]

        //! [5]
        GroupBox {
            flat: true
            Layout.fillWidth: true
            Column {
                spacing: 10

                Label {
                    font.pointSize: fontSize
                    font.bold: true
                    text: "Layer Style"
                }

                CheckBox {
                    id: layerOneGrid
                    style: CheckBoxStyle {
                        label: Label {
                            font.pointSize: fontSize
                            text: "Show Ground as Grid"
                        }
                    }
                    onCheckedChanged: {
                        if (checked)
                            layerOneSeries.drawMode = Surface3DSeries.DrawWireframe
                        else
                            layerOneSeries.drawMode = Surface3DSeries.DrawSurface
                    }
                }
            }
        }
        NewButton {
            id: testButton
            fontSize: fontSize
            Layout.fillWidth: true
            Layout.minimumHeight: 40
            text: "test"

            onClicked: {
                backend.test()
            }
        }

        TextField {
            id: renderLabel
            font.pointSize: fontSize
            Layout.fillWidth: true
            Layout.minimumHeight: 40
            enabled: false
            horizontalAlignment: TextInput.AlignHCenter
            text: "Indirect, " + surfaceLayers.msaaSamples + "xMSAA"
        }
    }
}
