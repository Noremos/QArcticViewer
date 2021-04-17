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
#include <QPainter>
#include <QPainterPath>
#include <cmath>

MainWidget::MainWidget(QWidget */*parent*/)
{
	cameraStartPos = QVector3D(-25, 551, -159);
	
	sky = new SkyBoxGUI();//58.25 -34.25 -24.4309 573.158 -205.05
	camera = new CameraGui(cameraStartPos, QVector3D(0,1,0), 56, -32.25);
	terra = new Terrain();
	zones = new SpotZones();
	text = new Text2d();

	useTimer = false;
}

MainWidget::~MainWidget()
{
	// Make sure the context is current when deleting the texture
	// and the buffers.
	makeCurrent();
	delete sky;
	delete terra;
	delete camera;
	delete zones;
	delete text;
//    delete texture;
//    delete geometries;
	doneCurrent();
}

//! [1]

void MessageCallback( GLenum /*source*/,
					 GLenum type,
					 GLuint /*id*/,
					 GLenum severity,
					 GLsizei /*length*/,
					 const GLchar* message,
					 const void* /*userParam*/ )
{
	fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
			type, severity, message );
}



void MainWidget::initializeGL()
{
	memset(keys, 0, 1024);

	timeStart = std::chrono::steady_clock::now();
	lastFrame = std::chrono::steady_clock::now();

	initializeOpenGLFunctions();
	f = QOpenGLContext::currentContext()->extraFunctions();
	//	QOpenGLContext *ctx = QOpenGLContext::currentContext();


	glEnable(GL_DEPTH_TEST);
	// Enable back face culling
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_FRONT_AND_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.3, 0.3, 0, 1);

	// During init, enable debug output
//	glEnable( GL_DEBUG_OUTPUT );
//	f->glDebugMessageCallback( MessageCallback, 0 );


	geometries = new CubeGui();

	sky->initializeGL();

	camera->setEnableTraking(false);
	camera->invertX = true;
	camera->invertY = true;
//	camera->invertX = false;
//	camera->invertY = false;

	terra->initGL();

	zones->initGL();
	text->initGL();

	emit startTimer();
//		terra->readfile("D:\2.obj");
//	terra->readfile("D:\\2_.OBJ");
//	drawTerra = true;

	// terra->addTexture("file:///D:/2.png");
	// terra->addTexture("file:///D:/Learning/BAR/Moscow/50_60_1_2_2m_v3.0-20201116T184630Z-001/test.png");
	// terra->addTexture("D:/Learning/BAR/Moscow/50_60_1_2_2m_v3.0-20201116T184630Z-001/50_60_1_2_2m_v3.0_reg_dem_browse.tif");
	// terra->displayTexture(0);


}

//! [0]
void MainWidget::mousePressEvent(QMouseEvent *e)
{
	camera->setEnableTraking(true);
//	sky->mousePressEvent(e);

	// Save mouse press position

	mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
	camera->setEnableTraking(false);
	// #######################################################
	// #######################################################
	// #######################################################

	QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;
	QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
	qreal acc = diff.length() / 100.0;
	rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();
	angularSpeed += acc;
}
//! [0]

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
//	sky->mouseMoveEvent(event);
    camera->ProcessMouseMovement(event->localPos().x(), event->localPos().y(), deltaTime);

//	if (!useTimer && camera->isEnableTraking())
//		update();
}

void MainWidget::wheelEvent(QWheelEvent *event)
{
//	sky->wheelEvent(event);
	camera->ProcessMouseScroll(event->delta(), deltaTime);
//	if (!useTimer)
//		update();
}


void MainWidget::keyPressEvent(QKeyEvent *event)
{
	shitfp = event->modifiers() & Qt::ShiftModifier;
	auto key = event->key();
	if (key >= 0 && key < 1024)
	{
//		bool jg = keys[key];
		keys[key] = true;

//		if (!jg)
//		{
//			++pressed;
//		}

		if (key==Qt::Key::Key_R)
		{
			camera->Position = cameraStartPos;
			camera->Up = QVector3D(0,1,0);
			camera->Yaw = 56;
			camera->Pitch = -32.25;
		}
	}
//	if (!useTimer)
//		update();
}
//! [4]
void MainWidget::keyReleaseEvent(QKeyEvent *event)
{
	shitfp = event->modifiers() & Qt::ShiftModifier;

	auto key = event->key();
	if (key >= 0 && key < 1024)
	{
//		bool jg = keys[key];

		keys[key] = false;

//		if (jg)
//		{
//			--pressed;
//		}

	}

//	qDebug() << pressed;

//	if (!useTimer)
//		update();
}

//! [5]
void MainWidget::resizeGL(int w, int h)
{
//	sky->resizeGL(w, h);

    // Calculate aspect ratio
	aspect = qreal(w) / qreal(h ? h : 1);

//	if (!useTimer)
//		update();

	// Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
//	const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

	// Reset projection
//	projection.setToIdentity();

	// Set perspective projection
//	projection.perspective(fov, aspect, zNear, zFar);x

}
//! [5]

void MainWidget::printErrors()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		switch (err)
		{
		case GL_INVALID_ENUM:
			qDebug() << "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			qDebug() << "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			qDebug() << "GL_INVALID_OPERATION";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			qDebug() << "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			qDebug() << "GL_OUT_OF_MEMORY";
			break;
		case GL_STACK_UNDERFLOW:
			qDebug() << "GL_STACK_UNDERFLOW";
			break;
		case GL_STACK_OVERFLOW:
			qDebug() << "GL_STACK_OVERFLOW";
			break;
		default:
			break;
		}
		qDebug() << err;
		// Process/log the error.
	}
}
#include <QtOpenGLExtensions/QOpenGLExtensions>
double timediff(timeType &t1, timeType &t2)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t2).count() * 0.001;
}
void MainWidget::paintGL()
{
	angularSpeed *= 0.99;

	if (angularSpeed < 0.01)
	{
		angularSpeed = 0.0;
	}
	else
	{
		// Update rotation
		rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
	}

	const qreal zNear = 0.1, zFar = 10000.0, fov = 60.0;

	timeType currentFrame = std::chrono::steady_clock::now();
	deltaTime = timediff(currentFrame, lastFrame);
	lastFrame = currentFrame;
	if (fpsLabel != nullptr)
	{
		fpsLabel->setText("fps: " + QString::number(round(1.0 / deltaTime)));
	}

	Do_Movement();
	QMatrix4x4 view = camera->GetViewMatrix();
	QMatrix4x4 skyboxview(view.normalMatrix());
	QMatrix4x4 projection;
	projection.setToIdentity();
	projection.perspective(fov, aspect, zNear, zFar);
//	 sky->mPerspective.verticalAngle, mPerspective.aspectRatio, mPerspective.nearPlane, mPerspective.farPlane
//	 Set perspective projection

//	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// makeCurrent();
	// doneCurrent();


//	texture->bind();

	geometries->model.setToIdentity();
	geometries->model.translate(0.0, 0.0, 0.0);
	geometries->model.rotate(timediff(currentFrame, timeStart) * 20.0f, QVector3D(0.0f, 1.0f, 0.0f));

//	program.bind();
	geometries->drawCubeGeometry(view, projection);

	if (drawTerra)
		terra->drawFull(view, projection);


	if (drawZones)
	{
		zones->renderGL(view, projection);
		text->renderGL(view, projection);
	}

//	QPainterPath path;
////	glDisable(GL_LIGHTING);
//	QFont font("Arial", 40);
//	path.addText(QPointF(0, 0), QFont("Arial", 40), QString(tr("This is a test")));
//	QList<QPolygonF> poly = path.toSubpathPolygons();
//	for (QList<QPolygonF>::iterator i = poly.begin(); i != poly.end(); i++){
//		glBegin(GL_LINE_LOOP);
//		for (QPolygonF::iterator p = (*i).begin(); p != i->end(); p++)
//			glVertex3f(p->rx()*0.1f, -p->ry()*0.1f, 0);
//		glEnd();
//	}
//	glEnable(GL_LIGHTING);
//	QPainter painter(this);
//	painter.setPen(Qt::black);
//	painter.setFont(QFont("Arial", 56));
//	painter.drawText(0, 0, width(), height(), Qt::AlignCenter, "Hello World!");
//	painter.end();

	///skybox
	glDepthFunc(GL_LEQUAL);
	sky->paintGL(skyboxview, projection); //projection * matrix
	glDepthFunc(GL_LESS);

	printErrors();
//	doneCurrent();
//	update();




//	const QList<QOpenGLDebugMessage> messages = logger->loggedMessages();
//	for (const QOpenGLDebugMessage &message : messages)
	//		qDebug() << message;
}


void MainWidget::Do_Movement()
{
	// Camera controls
	bool isShift = shitfp;
	if(keys[Qt::Key::Key_W])
		camera->ProcessKeyboard(FORWARD, deltaTime, isShift);
	if(keys[Qt::Key::Key_S])
		camera->ProcessKeyboard(BACKWARD, deltaTime, isShift);
	if(keys[Qt::Key::Key_A])
		camera->ProcessKeyboard(LEFT, deltaTime, isShift);
	if(keys[Qt::Key::Key_D])
		camera->ProcessKeyboard(RIGHT, deltaTime, isShift);
}
