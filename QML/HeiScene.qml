

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
    id: root

    /*  View3D {
        id: view

        anchors.fill: parent
        // Scene to view
        Node {
            id: scene
            DirectionalLight {
                ambientColor: Qt.rgba(1.0, 1.0, 1.0, 1.0)
            }
            //                Light {

            //                    id: directionalLight
            //                }
            PerspectiveCamera {
                id: cameraPerspectiveTwo
                z: 600
            }

            //                Camera {
            //                    id: camera
            //                    // It's important that your camera is not inside
            //                    // your model so move it back along the z axis
            //                    // T2he Camera is implicitly facing up the z axis,
            //                    // so we should be looking towards (0, 0, 0)
            //                    z: -600
            //                }
            Model {
                source: "file:///D:\\1.obj" //"#Cube"
                y: -104
                scale: Qt.vector3d(3, 3, 0.1)
                eulerRotation.x: -90
                materials: [
                    DefaultMaterial {
                        diffuseColor: Qt.rgba(0.8, 0.8, 0.8, 1.0)
                    }
                ]
            }
        }
    }
*/ }
