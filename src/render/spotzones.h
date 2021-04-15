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

// Qmatrix's size is 68! Not is 64!
struct InstanceData
{
//	QVector4D v0;
//	QVector4D v1;
//	QVector4D v2;
//	QVector4D v3;
	QMatrix4x4 model;
//	int val;
	//	int zer1;
	//	int zer2;
	InstanceData(QMatrix4x4 matr, int val)
	{
		model = matr;
//		v0 = matr.column(0);
//		v1 = matr.column(1);
//		v2 = matr.column(2);
//		v3 = matr.column(3);
//		this->val = val;
	}
};

class SpotZones :protected QOpenGLFunctions
{
public:
	QVector<InstanceData> boundydata;

	QOpenGLVertexArrayObject vao;
	int factor = 10;
	QOpenGLBuffer arrBuf, modelsBuf, goodBuff;
	QOpenGLBuffer indexBuf;

	QOpenGLShaderProgram mshader;
	QOpenGLExtraFunctions *f;

	size_t boundySize = 0;
	void addBoundy(boundy &bb, bool good = true);
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
