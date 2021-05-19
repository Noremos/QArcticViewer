/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "cubeg.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLDebugLogger>
#include <QLabel>

#include "Skyboxgui.h"

#include "terrarian/cameragui.h"
#include "terrarian/terrariangui.h"

#include <chrono>
#include <QOpenGLExtraFunctions>

#include <src/render/line.h>
#include <src/render/spotzones.h>
#include <src/render/text2d.h>
#include <src/render/usermarkers.h>

class CubeGL;
typedef std::chrono::time_point<std::chrono::steady_clock> timeType;

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

	const qreal zNear = 0.1, zFar = 10000.0, fov = 60.0;


public:
	bool useTimer = false;

	MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

	QLabel *fpsLabel;
	void Do_Movement();

	Terrain *terra = nullptr;
	SpotZones* zones;
	StaticMarkers *markers;
	UserMarkers *userMarkers;
	Line line;

	bool drawTerra = false;
	bool drawZones = false;

	void printErrors();


	void readGeoshape();
protected:
	QVector3D getMouseCast(const QVector2D &mousePos);

	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;

	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

    void initShaders();
	void initTextures();

private:
	bool shitfp;
	bool ctrl;
	bool keys[1024];
	int pressed = 0;
	qreal aspect;
	QVector3D cameraStartPos;


	double deltaTime = 0;
	timeType lastFrame;
	timeType timeStart;

	CameraGui *camera;
	SkyBoxGUI *sky;
	Text2d *text;

	CubeGui *cubeRot = nullptr;


	QOpenGLDebugLogger *logger;

    QMatrix4x4 projection;

    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed = 0;
	QQuaternion rotation;

	QOpenGLExtraFunctions *f;
	// QPaintDevice interface
	// QWidget interface
	QVector4D getVal(int x, int z);
protected:

	QVector3D CreateRay(QMatrix4x4 &projection, QMatrix4x4 &view, float mouseX, float mouseY);
	QVector3D mouseCast(QMatrix4x4 &projection, QMatrix4x4 &view, int mouse_x, int mouse_y);
signals:
	void startTimer();

};

#endif // MAINWIDGET_H
