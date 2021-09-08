
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

	markers = new StaticMarkers();
#ifdef ENABLE_MARKERS
	userMarkers = new UserMarkers();
#endif

	useTimer = false;
	Project::getProject()->widget = this;
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
	delete markers;
#ifdef ENABLE_MARKERS
	delete userMarkers;
#endif
	delete cubeRot;

//    delete texture;
//    delete geometries;
	doneCurrent();
}

//! [1]


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


	cubeRot = new CubeGui();

	sky->initializeGL();

	camera->setEnableTraking(false);
	camera->invertX = true;
	camera->invertY = true;
//	camera->invertX = false;
//	camera->invertY = false;

	terra->initGL();

	zones->initGL();
	text->initGL();

	markers->initGL();
#ifdef ENABLE_MARKERS
	userMarkers->initGL();
#endif

//	line.initGL();

	emit startTimer();
//		terra->readfile("D:\2.obj");
//	terra->readfile("D:\\2_.OBJ");
//	drawTerra = true;

	// terra->addTexture("file:///D:/2.png");
	// terra->addTexture("file:///D:/Learning/BAR/Moscow/50_60_1_2_2m_v3.0-20201116T184630Z-001/test.png");
	// terra->addTexture("D:/Learning/BAR/Moscow/50_60_1_2_2m_v3.0-20201116T184630Z-001/50_60_1_2_2m_v3.0_reg_dem_browse.tif");
	// terra->displayTexture(0);


}

QVector3D MainWidget::CreateRay(QMatrix4x4& projection, QMatrix4x4& view,  float mouseX, float mouseY)
{
	// these positions must be in range [-1, 1] (!!!), not [0, width] and [0, height]
	// mouseX, mouseY
	mouseX = mouseX / (this->width()  * 0.5f) - 1.0f;
	mouseY = -mouseY / (this->height() * 0.5f) - 1.0f;

	QMatrix4x4 invVP = (projection * view).inverted();
	QVector4D screenPos(mouseX, mouseY, 1.0f, 1.0f);
	QVector4D worldPos = invVP * screenPos;

	QVector3D dir = worldPos.toVector3D().normalized();
	return dir;
}

//more occuroncy
QVector3D MainWidget::mouseCast(QMatrix4x4& projection, QMatrix4x4& view,  int mouse_x, int mouse_y)
{
	float x = (2.0f * mouse_x) /  this->width() - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) /  this->height();

//	QVector4D r = projection * QVector4D(x, y, 0, 1);
//	QVector3D rdir = view.transposed() * r.toVector3D();

//	return rdir;
//	float z = 1.0f;
	QVector4D ray_clip(x, y, -1.0, 1.0);
	QVector4D ray_eye = projection.inverted() * ray_clip;
	ray_eye = QVector4D(ray_eye.x(), ray_eye.y(), -1.0, 0.0);

	QVector4D worldPos = (view.inverted() * ray_eye);
	// don't forget to normalise the vector at some point
	QVector3D ray_wor = worldPos.toVector3D().normalized();
//	ray_wor.setX(ray_wor.x() * 0.8);
//	ray_wor.setY(ray_wor.y() * 0.8);
//	ray_wor.setZ(ray_wor.z() * 0.8);

	return ray_wor;
}



QVector4D MainWidget::getVal(int x, int z) { return QVector4D(x, terra->getValue(x, z), z, 1.0);}

QVector3D MainWidget::getMouseCast(const QVector2D &mousePos)
{
	QMatrix4x4 view = camera->GetViewMatrix();
	QMatrix4x4 projection;
	projection.setToIdentity();
	projection.perspective(fov, aspect, zNear, zFar);

	QVector3D ray;
	//	ray = CreateRay(projection, view, mousePressPosition.x(), mousePressPosition.y());
	//	qDebug() << ray;
	ray = mouseCast(projection, view, mousePos.x(), mousePos.y());


	QVector4D mins[4];
	int locWid = Project::getProject()->modelWid - 1;
	int locHei = Project::getProject()->modelHei - 1;

	mins[0] = getVal(0, 0);
	mins[1] = getVal(0, locHei);
	mins[2] = getVal(locWid, 0);
	mins[3] = getVal(locWid, locHei);

	QVector3D rayStartPositon = camera->Position;
	//(projection * view * model * camera->Position.toVector4D()).toVector3D();

	float maxDist = 0;
	for (auto &a : mins)
	{
		QVector3D gl_Position = a.toVector3D();
		//(projection * view * model * a).toVector3D();

		float dist = gl_Position.distanceToPoint(rayStartPositon);
		maxDist = MAX(dist, maxDist);
	}

	int off = maxDist;
	int divis = off / 2;

	QVector3D rayEndPosition;
	float hei = 0;
	while (divis >= 1)
	{
		rayEndPosition = rayStartPositon + ray * off;
		hei = terra->getValue(rayEndPosition.x(), rayEndPosition.z());

		if (hei == -9999 || rayEndPosition.y() < hei)
			off -= divis;
		else
			off += divis;
		divis /= 2;
	}
//	ray.setX(ray.x() * 0.85);
//	ray.setZ(ray.z() * 0.85);
	rayEndPosition = rayStartPositon + ray * off;
//	rayEndPosition.setX(rayEndPosition.x() * 0.8);
//	rayEndPosition.setZ(rayEndPosition.z() * 0.8);
//	qDebug() << ray << rayEndPosition;
	rayEndPosition.setY(hei);
	return rayEndPosition;
}

void MainWidget::mousePressEvent(QMouseEvent *e)
{
	camera->setEnableTraking(true);

	// Save mouse press position
	mousePressPosition = QVector2D(e->localPos());
//	QVector2D vec(mousePressPosition.x() * width(), mousePressPosition.y() * height());

//	if (e->button() != Qt::MouseButton::RightButton)
//		return;

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

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
//	sky->mouseMoveEvent(event);
	mouseCurrentPosition = QVector2D(event->localPos());
	camera->ProcessMouseMovement(mouseCurrentPosition.x(), mouseCurrentPosition.y(), deltaTime);
//	qDebug() << vec;
#ifdef ENABLE_MARKERS

	if (userMarkers->enable && Project::getProject()->showMarkers)
	{
		//		vec.setX(event->localPos().x() * width());
		//		vec.setY(event->localPos().y() * height());
//		QVector2D vecs(event->localPos().x() * width(), event->localPos().y() * height());

		userMarkers->move(0, getMouseCast(mouseCurrentPosition));
	}
#endif

//	QMatrix4x4 view = camera->GetViewMatrix();
//	QMatrix4x4 projection;
//	projection.setToIdentity();
//	projection.perspective(fov, aspect, zNear, zFar);

//	QVector3D rayStartPositon = camera->Position;

//	QVector3D ray = mouseCast(projection, view, event->localPos().x(), event->localPos().y());
//	ray = QVector3D(ray.x(), ray.z(), ray.y());
//	auto rayEndPosition = rayStartPositon + ray * 100;
//	line.setLine(rayStartPositon, rayEndPosition);
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
	ctrl = event->modifiers() & Qt::ControlModifier;
	auto key = event->nativeVirtualKey();
	if (key < 1024)
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
			memset(keys, 0, 1024);

//			line.setLine(QVector3D(0, 0, 0), QVector3D(100, 100, 100));



			// camera->Position = QVector3D(981.52, 29.6308, 271.393);
			// camera->Up = QVector3D(0.468725, 0.881798, 0.0522446);
			// camera->Yaw = 6.36;
			// camera->Pitch = -28.14;
//			camera->Position = QVector3D(1024.25, 29.6308, 233.794);
//			camera->Up = QVector3D(0.468725, 0.881798, 0.0522446);
//			camera->Yaw = 6.36;
//			camera->Pitch = -28.14;
			camera->updateCameraVectors();
		}


		// if (ctrl)
		// {
		// 	qDebug() << camera->Position;
		// 	qDebug() << camera->Up;
		// 	qDebug() << camera->Yaw;
		// 	qDebug() << camera->Pitch;
		// 	memset(keys, 0, 1024);
		// }
	}
//	if (!useTimer)
//		update();
}

void MainWidget::keyReleaseEvent(QKeyEvent *event)
{
	shitfp = event->modifiers() & Qt::ShiftModifier;
	ctrl = event->modifiers() & Qt::ControlModifier;

	auto key = event->nativeVirtualKey();
	if (key < 1024)
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
	//	const QList<QOpenGLDebugMessage> messages = logger->loggedMessages();
	//	for (const QOpenGLDebugMessage &message : messages)
	//		qDebug() << message;

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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


//	geometries->model.setToIdentity();
//	geometries->model.translate(0.0, 0.0, 0.0);
//	geometries->model.rotate(timediff(currentFrame, timeStart) * 20.0f, QVector3D(0.0f, 1.0f, 0.0f));
//	geometries->drawCubeGeometry(view, projection);

	if (drawTerra)
	{
//		if (polyLine)
//			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		terra->drawFull(view, projection);
		markers->renderGL(view, projection);

#ifdef ENABLE_MARKERS
		if (Project::getProject()->showMarkers)
			userMarkers->renderGL(view, projection);
#endif
//		if (polyLine)
//			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//		line.renderGL(view, projection);
	}

	if (drawZones)
	{
		zones->renderGL(view, projection);
		text->renderGL(view, projection);
	}

	///skybox
	glDepthFunc(GL_LEQUAL);
	sky->paintGL(skyboxview, projection); //projection * matrix
	glDepthFunc(GL_LESS);

	printErrors();
}


void MainWidget::Do_Movement()
{
	// Camera controls
	float factor = 1.f;
	if (shitfp)
		factor = 2.f;
	if (ctrl)
		factor = 0.3;

	if(keys[Qt::Key::Key_W])
		camera->ProcessKeyboard(FORWARD, deltaTime, factor);
	if(keys[Qt::Key::Key_S])
		camera->ProcessKeyboard(BACKWARD, deltaTime, factor);
	if(keys[Qt::Key::Key_A])
		camera->ProcessKeyboard(LEFT, deltaTime, factor);
	if(keys[Qt::Key::Key_D])
		camera->ProcessKeyboard(RIGHT, deltaTime, factor);

	if (keys[Qt::Key::Key_E])
	{
		camera->Yaw += 50 * deltaTime;
		camera->updateCameraVectors();
	}
	if (keys[Qt::Key::Key_Q])
	{
		camera->Yaw -= 50 * deltaTime;
		camera->updateCameraVectors();
	}

	if (keys[Qt::Key::Key_P])
	{
		polyLine = !polyLine;
	}


#ifdef ENABLE_MARKERS
	if (Project::getProject()->showMarkers)
	{
		//calls only ones
		if (keys[Qt::Key::Key_Space])
		{
			if (!pressed[Qt::Key::Key_Space])
			{
				userMarkers->addBoundy(getMouseCast(mouseCurrentPosition));
				pressed[Qt::Key::Key_Space] = true;
				qDebug() << "Marker added";
			}
		}
		else
			pressed[Qt::Key::Key_Space] = false;


		if (keys[Qt::Key::Key_M] || keys[Qt::Key::Key_Y])
		{
			if (!pressed[Qt::Key::Key_M])
			{
				for (int i = 1; i < userMarkers->boundydata.size(); ++i)
				{
					float x = userMarkers->boundydata[i].getX();
					float z = userMarkers->boundydata[i].getZ();

					QVector3D pos = getMouseCast(mouseCurrentPosition);
					if (abs(pos.x() - x) <= 5 && abs(pos.z() - z) <= 5)
					{
						userMarkers->boundydata.remove(i);
						break;
					}
				}
				pressed[Qt::Key::Key_M] = true;
				qDebug() << "Marker added";
			}
		}
		else
			pressed[Qt::Key::Key_M] = false;
	}
#endif

	if(keys[38])
		camera->ProcessKeyboard(FORWARD, deltaTime, factor);
	if(keys[40])
		camera->ProcessKeyboard(BACKWARD, deltaTime, factor);
	if(keys[37])
		camera->ProcessKeyboard(LEFT, deltaTime, factor);
	if(keys[39])
		camera->ProcessKeyboard(RIGHT, deltaTime, factor);
}
