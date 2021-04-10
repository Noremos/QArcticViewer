#ifndef SPOTZONES_H
#define SPOTZONES_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>

#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "../types/types.h"
#include "../types/glinstanse.h"

class SpotZones :protected QOpenGLFunctions
{
public:
	QVector<QMatrix4x4> boundydata;

	QOpenGLVertexArrayObject vao;
	int factor = 10;
	QOpenGLBuffer arrBuf, modelsBuf;
	QOpenGLBuffer indexBuf;

	QOpenGLShaderProgram mshader;
	QOpenGLExtraFunctions *f;

	size_t boundySize = 0;
	void addBoundy(boundy &bb)
	{
		QMatrix4x4 matr;
		matr.setToIdentity();
		matr.translate(bb.x + bb.wid() / 2, bb.endZ + bb.zei()/2 + 1,bb.y + bb.hei() / 2);
		//text bb.x, bb.endZ, bb.y
		matr.scale(bb.wid(), 1, bb.hei());
		boundydata.append(matr);
	}
	SpotZones();

	void updateBuffer();
	void initGL();
	void renderGL(QMatrix4x4 view, QMatrix4x4 projection);
	void initSpotModel();


	void printErrors()
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
			// Process/log the error.
		}
	}

};

#endif // SPOTZONES_H
