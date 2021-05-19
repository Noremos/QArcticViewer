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

class Project;

enum class glColor {
	Green = 0, // vec3(0.0, 1.0, 0.0);
	Purpure, // vec3(0.54, 0.0, 0.5);
	Blue, // vec3(0.0, 0.3, 0.7);
	Red, //vec3(1.0, 0.0, 0.0);
	Oragne //vec3(1.0, 0.5, 0.0);
};


class SpotZones :protected QOpenGLFunctions
{
public:
	QVector<InstanceData> boundydata;

	QOpenGLVertexArrayObject vao;
//	int factor = 10;
	QOpenGLBuffer arrBuf, modelsBuf, goodBuff;
	QOpenGLBuffer indexBuf;

	QOpenGLShaderProgram mshader;
	QOpenGLExtraFunctions *f;

	size_t boundySize = 0;
	// type: 0=bad;1=good; 2= more good, 3=more bad
	void addBoundy(boundy &bb, int displayFactor, glColor type = glColor::Green);
	SpotZones();

	void updateBuffer();
	void initGL();
	void renderGL(QMatrix4x4 view, QMatrix4x4 projection);
	void initSpotModel();

	float minVal = 9999;

	Project * proj;
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
