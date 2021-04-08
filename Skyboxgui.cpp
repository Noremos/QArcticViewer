#include "Skyboxgui.h"

#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QVector3D>
SkyBoxGUI::SkyBoxGUI(QWidget *parent) : mTexture(QOpenGLTexture::TargetCubeMap), mVertexBuf(QOpenGLBuffer::VertexBuffer), mSpeed(0.0f)
{
	mFrontImagePath = ":/skybox/front.jpg";
	mBackImagePath = ":/skybox/back.jpg";
	mTopImagePath = ":/skybox/top.jpg";
	mBottomImagePath = ":/skybox/bottom.jpg";
	mLeftImagePath = ":/skybox/left.jpg";
	mRightImagePath = ":/skybox/right.jpg";
	mLookAt.eye = {+0.0f, +0.0f, +0.0f};
	mLookAt.center = {+0.0f, +0.0f, -1.0f};
	mLookAt.up = {+0.0f, +1.0f, +0.0f};
}

void SkyBoxGUI::loadImages()
{
	const QImage posx = QImage(mRightImagePath).convertToFormat(QImage::Format_RGBA8888);
	const QImage negx = QImage(mLeftImagePath).convertToFormat(QImage::Format_RGBA8888);

	const QImage posy = QImage(mTopImagePath).convertToFormat(QImage::Format_RGBA8888);
	const QImage negy = QImage(mBottomImagePath).convertToFormat(QImage::Format_RGBA8888);

	const QImage posz = QImage(mFrontImagePath).convertToFormat(QImage::Format_RGBA8888);
	const QImage negz = QImage(mBackImagePath).convertToFormat(QImage::Format_RGBA8888);

	mTexture.create();
	mTexture.setSize(posx.width(), posx.height(), posx.depth());
	mTexture.setFormat(QOpenGLTexture::RGBA8_UNorm);
	mTexture.allocateStorage();

	mTexture.setData(0, 0, QOpenGLTexture::CubeMapPositiveX, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, posx.constBits(), Q_NULLPTR);

	mTexture.setData(0, 0, QOpenGLTexture::CubeMapPositiveY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, posy.constBits(), Q_NULLPTR);

	mTexture.setData(0, 0, QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, posz.constBits(), Q_NULLPTR);

	mTexture.setData(0, 0, QOpenGLTexture::CubeMapNegativeX, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, negx.constBits(), Q_NULLPTR);

	mTexture.setData(0, 0, QOpenGLTexture::CubeMapNegativeY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, negy.constBits(), Q_NULLPTR);

	mTexture.setData(0, 0, QOpenGLTexture::CubeMapNegativeZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, negz.constBits(), Q_NULLPTR);

	mTexture.setWrapMode(QOpenGLTexture::ClampToEdge);
	mTexture.setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	mTexture.setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
}


void SkyBoxGUI::initializeGL()
{
	initializeOpenGLFunctions();

	mProgram.addShaderFromSourceCode(QOpenGLShader::Vertex,
									 R"(
				attribute vec3 aPosition;
				varying vec3 vTexCoord;


				uniform mat4 projection;
				uniform mat4 view;

				void main()
				{
					vec4 pos = projection * view * vec4(aPosition, 1.0);
					gl_Position = pos.xyww;
					vTexCoord = aPosition;
				}
				)");

	mProgram.addShaderFromSourceCode(QOpenGLShader::Fragment,
									 R"(
				uniform samplerCube uTexture;
				varying vec3 vTexCoord;

				void main()
				{
					gl_FragColor = textureCube(uTexture, vTexCoord);
				}
				)");

	mProgram.link();
	mProgram.bind();

	loadImages();

	QVector3D vertices[] = {{-1.0f, 1.0f, -1.0f},  {-1.0f, -1.0f, -1.0f}, {+1.0f, -1.0f, -1.0f}, {+1.0f, -1.0f, -1.0f}, {+1.0f, +1.0f, -1.0f}, {-1.0f, +1.0f, -1.0f},

							{-1.0f, -1.0f, +1.0f}, {-1.0f, -1.0f, -1.0f}, {-1.0f, +1.0f, -1.0f}, {-1.0f, +1.0f, -1.0f}, {-1.0f, +1.0f, +1.0f}, {-1.0f, -1.0f, +1.0f},

							{+1.0f, -1.0f, -1.0f}, {+1.0f, -1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, -1.0f}, {+1.0f, -1.0f, -1.0f},

							{-1.0f, -1.0f, +1.0f}, {-1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f}, {+1.0f, -1.0f, +1.0f}, {-1.0f, -1.0f, +1.0f},

							{-1.0f, +1.0f, -1.0f}, {+1.0f, +1.0f, -1.0f}, {+1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f}, {-1.0f, +1.0f, +1.0f}, {-1.0f, +1.0f, -1.0f},

							{-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, +1.0f}, {+1.0f, -1.0f, -1.0f}, {+1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, +1.0f}, {+1.0f, -1.0f, +1.0f}};

	mVertexBuf.create();
	mVertexBuf.bind();
	mVertexBuf.allocate(vertices, 36 * sizeof(QVector3D));

	mProgram.enableAttributeArray("aPosition");
	mProgram.setAttributeBuffer("aPosition", GL_FLOAT, 0, 3, sizeof(QVector3D));

	mProgram.setUniformValue("uTexture", 0);
	//!!!!!!!!!!!!!!!!!!!
	//	mProgram.disableAttributeArray("aPosition");
	//		mProgram.release();
	//	mVertexBuf.release();
}



void SkyBoxGUI::paintGL(QMatrix4x4 view, QMatrix4x4 projection)
{
	mProgram.bind();
	mVertexBuf.bind();
	mTexture.bind();

//	view.lookAt(mLookAt.eye, mLookAt.center, mLookAt.up);

//	projection.setToIdentity();
//	projection.perspective(mPerspective.verticalAngle, mPerspective.aspectRatio, mPerspective.nearPlane, mPerspective.farPlane);


	mProgram.enableAttributeArray("aPosition");
	mProgram.setAttributeBuffer("aPosition", GL_FLOAT, 0, 3, sizeof(QVector3D));

	//projection * view * model * vec4(position, 1.0f);
	mProgram.setUniformValue("uTexture", 0);
	mProgram.setUniformValue("projection", projection);
	mProgram.setUniformValue("view", view);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	mProgram.disableAttributeArray("aPosition");
	mVertexBuf.release();
	mTexture.release();
	mProgram.release();
}

void SkyBoxGUI::resizeGL(int w, int h)
{
	mPerspective.verticalAngle = 60.0;
	mPerspective.nearPlane = 0.0;
	mPerspective.farPlane = 1.0;
	mPerspective.aspectRatio =
			static_cast<float>(w) / static_cast<float>(h ? h : 1.0f);
}

void SkyBoxGUI::mouseMoveEvent(QMouseEvent *event)
{
//	mMousePressPosition = QVector2D(event->localPos());
////	if(event->buttons() & Qt::LeftButton)
//	{
//		auto diff = QVector2D(event->localPos()) - mMousePressPosition;
//		auto n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
//		mSpeed = diff.length() / 100.0f;
//		if(mSpeed > 1.0f) mSpeed = 1.0f; // speed threshold
//		mRotationAxis = (mRotationAxis + n * mSpeed).normalized();
//	}
}

void SkyBoxGUI::mousePressEvent(QMouseEvent *event)
{
//	mTimer.start(10, this);
}

void SkyBoxGUI::mouseReleaseEvent(QMouseEvent*)
{
//	mTimer.stop();
}

void SkyBoxGUI::timerEvent(QTimerEvent *)
{
//	mRotation = QQuaternion::fromAxisAndAngle(mRotationAxis, mSpeed) * mRotation;

//	QMatrix4x4 mat;
//	mat.setToIdentity();
//	mat.rotate(mRotation);

//	mLookAt.center = {+0.0f, +0.0f, -1.0f};
//	mLookAt.center = mLookAt.center * mat;

//	update();
}

void SkyBoxGUI::wheelEvent(QWheelEvent *event)
{
	float delta = event->delta() > 0 ? -5.0f : +5.0f;
//	mPerspective.verticalAngle += delta;
//	if(mPerspective.verticalAngle < 10.0f)
//		mPerspective.verticalAngle = 10.0f;
//	else if(mPerspective.verticalAngle > 120.0f)
//		mPerspective.verticalAngle = 120.0f;

//	update();
}