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

#include "mainwidget.h"

#include <QMouseEvent>

#include <QOpenGLDebugLogger>
#include <cmath>

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
	makeCurrent();
//    delete texture;
//    delete geometries;
	doneCurrent();
}

//! [1]
void MainWidget::timerEvent(QTimerEvent * ev)
{
	sky->timerEvent(ev);
    // Decrease angular speed (friction)
	angularSpeed *= 0.99;

	// Stop rotation when speed goes below threshold
	if (angularSpeed < 0.01) {
		angularSpeed = 0.0;
	} else {
		// Update rotation
		rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

		// Request an update
		update();
	}
}
//! [1]

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

//    glClearColor(0, 0, 0, 1);

	initShaders();
	initTextures();

	// Enable depth buffer
//	glEnable(GL_DEPTH_TEST);

	// Enable back face culling
	glEnable(GL_CULL_FACE);

	geometries = new GeometryEngine();


	sky = new SkyBoxGUI();
	sky->initializeGL();

	camera = new CameraGui();
	memset(keys, 0, 1024);


//	QOpenGLContext *ctx = QOpenGLContext::currentContext();
//	logger = new QOpenGLDebugLogger(this);
//	ctx->hasExtension(QByteArrayLiteral("GL_KHR_debug"));

//	logger->initialize(); // initializes in the current context, i.e. ctx

//	logger->startLogging();


    // Use QBasicTimer because its faster than QTimer
	timer.start(12, this);
}

//! [3]
void MainWidget::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
		return;;

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
		return;;

    // Link shader pipeline
    if (!program.link())
		return;;

    // Bind shader pipeline for use
}
//! [3]

//! [4]
void MainWidget::initTextures()
{
//    // Load cube.png image
	texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());

//    // Set nearest filtering mode for texture minification
	texture->setMinificationFilter(QOpenGLTexture::Nearest);

//    // Set bilinear filtering mode for texture magnification
	texture->setMagnificationFilter(QOpenGLTexture::Linear);

//    // Wrap texture coordinates by repeating
//	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
	texture->setWrapMode(QOpenGLTexture::Repeat);
}


//! [0]
void MainWidget::mousePressEvent(QMouseEvent *e)
{
//	sky->mousePressEvent(e);

	// Save mouse press position

	mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
//	sky->mouseReleaseEvent(e);

	//Mouse release position - mouse press position
	QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

	// Rotation axis is perpendicular to the mouse position difference
	// vector
	QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

	// Accelerate angular speed relative to the length of the mouse sweep
	qreal acc = diff.length() / 100.0;

	// Calculate new rotation axis as weighted sum
	rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

	// Increase angular speed
	angularSpeed += acc;
}
//! [0]

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
//	sky->mouseMoveEvent(event);
	camera->ProcessMouseMovement(event->localPos().x(), event->localPos().y());
}

void MainWidget::wheelEvent(QWheelEvent *event)
{
//	sky->wheelEvent(event);
	camera->ProcessMouseScroll(event->y());
}


void MainWidget::keyPressEvent(QKeyEvent *event)
{
	auto key = event->key();
	if (key >= 0 && key < 1024)
	{
		keys[key] = true;
	}
}
//! [4]
void MainWidget::keyReleaseEvent(QKeyEvent *event)
{
	auto key = event->key();
	if (key >= 0 && key < 1024)
	{
		keys[key] = false;
	}
}

//! [5]
void MainWidget::resizeGL(int w, int h)
{
	sky->resizeGL(w, h);

    // Calculate aspect ratio
	aspect = qreal(w) / qreal(h ? h : 1);

	// Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
//	const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

	// Reset projection
	projection.setToIdentity();

	// Set perspective projection
//	projection.perspective(fov, aspect, zNear, zFar);

}
//! [5]

void MainWidget::paintGL()
{
	const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;


	GLfloat currentFrame = time(0);
	deltaTime = difftime(currentFrame, lastFrame);
	lastFrame = currentFrame;


	Do_Movement();
	QMatrix4x4 view = camera->GetViewMatrix();
	QMatrix4x4 projection;
	projection.setToIdentity();
	projection.perspective(fov, aspect, zNear, zFar);
//	 sky->mPerspective.verticalAngle, mPerspective.aspectRatio, mPerspective.nearPlane, mPerspective.farPlane
//	 Set perspective projection


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sky->paintGL(view, projection); //projection * matrix

//	sky->update();
	// makeCurrent();
	// doneCurrent();


	// Clear color and depth buffer
	QMatrix4x4 matrix;
	matrix.translate(0.0, 0.0, -5.0);
	matrix.rotate(rotation);

	texture->bind();
	program.bind();
	program.setUniformValue("mvp_matrix", projection * matrix);
	program.setUniformValue("texture", 0);
	geometries->drawCubeGeometry(&program);
	texture->release();

//	update();




//	const QList<QOpenGLDebugMessage> messages = logger->loggedMessages();
//	for (const QOpenGLDebugMessage &message : messages)
//		qDebug() << message;
}


void MainWidget::Do_Movement()
{
	// Camera controls
	if(keys[Qt::Key::Key_W])
		camera->ProcessKeyboard(FORWARD, deltaTime);
	if(keys[Qt::Key::Key_S])
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	if(keys[Qt::Key::Key_A])
		camera->ProcessKeyboard(LEFT, deltaTime);
	if(keys[Qt::Key::Key_D])
		camera->ProcessKeyboard(RIGHT, deltaTime);
}
