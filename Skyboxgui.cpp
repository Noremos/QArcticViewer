#include "Skyboxgui.h"

#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QVector3D>
SkyBoxGUI::SkyBoxGUI(QWidget */*parent*/) : mTexture(QOpenGLTexture::TargetCubeMap), mVertexBuf(QOpenGLBuffer::VertexBuffer), mSpeed(0.0f)
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

SkyBoxGUI::~SkyBoxGUI()
{
	mTexture.destroy();
	vao.destroy();
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
	f = QOpenGLContext::currentContext()->extraFunctions();

	mProgram.addShaderFromSourceCode(QOpenGLShader::Vertex,
									 R"(
				#version 330 core
				layout(location = 0) in vec3 a_positionition;

				out vec3 vTexCoord;


				uniform mat4 projection;
				uniform mat4 view;

				void main()
				{
					vec4 pos = projection * view * vec4(a_positionition, 1.0);
					gl_Position = pos.xyww;
					vTexCoord = a_positionition;
				}
				)");

	mProgram.addShaderFromSourceCode(QOpenGLShader::Fragment,
									 R"(
				#version 330 core
				out vec4 FragColor;

				uniform samplerCube uTexture;
				in vec3 vTexCoord;

				void main()
				{
					FragColor = texture(uTexture, vTexCoord);
				}
				)");

	mProgram.link();

	loadImages();

	QVector3D vertices[] = {{-1.0f, 1.0f, -1.0f},  {-1.0f, -1.0f, -1.0f}, {+1.0f, -1.0f, -1.0f}, {+1.0f, -1.0f, -1.0f}, {+1.0f, +1.0f, -1.0f}, {-1.0f, +1.0f, -1.0f},

							{-1.0f, -1.0f, +1.0f}, {-1.0f, -1.0f, -1.0f}, {-1.0f, +1.0f, -1.0f}, {-1.0f, +1.0f, -1.0f}, {-1.0f, +1.0f, +1.0f}, {-1.0f, -1.0f, +1.0f},

							{+1.0f, -1.0f, -1.0f}, {+1.0f, -1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, -1.0f}, {+1.0f, -1.0f, -1.0f},

							{-1.0f, -1.0f, +1.0f}, {-1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f}, {+1.0f, -1.0f, +1.0f}, {-1.0f, -1.0f, +1.0f},

							{-1.0f, +1.0f, -1.0f}, {+1.0f, +1.0f, -1.0f}, {+1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f}, {-1.0f, +1.0f, +1.0f}, {-1.0f, +1.0f, -1.0f},

							{-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, +1.0f}, {+1.0f, -1.0f, -1.0f}, {+1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, +1.0f}, {+1.0f, -1.0f, +1.0f}};

	vao.create();
	vao.bind();

	mVertexBuf.create();
	mVertexBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
	mVertexBuf.bind();
	mVertexBuf.allocate(vertices, 36 * sizeof(QVector3D));

	mProgram.bind();
	int loc = mProgram.attributeLocation("a_positionition");
	assert(loc >= 0);
//	mProgram.setAttributeArray(loc, GL_FLOAT, 0, 3, sizeof(QVector3D));
	mProgram.setAttributeBuffer(loc, GL_FLOAT, 0, 3, sizeof(QVector3D));
	mProgram.enableAttributeArray(loc);
//	f->glVertexAttribDivisor()

	mProgram.setUniformValue("uTexture", 0);
	//!!!!!!!!!!!!!!!!!!!
	vao.release();
	mVertexBuf.release();

	mProgram.release();
}

void SkyBoxGUI::paintGL(QMatrix4x4 view, QMatrix4x4 projection)
{
	mProgram.bind();
	mTexture.bind();
	mProgram.setUniformValue("projection", projection);
	mProgram.setUniformValue("view", view);

	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	vao.release();

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
