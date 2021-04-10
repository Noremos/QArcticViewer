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

	QOpenGLBuffer arrBuf, modelsBuf;
	QOpenGLBuffer indexBuf;

	QOpenGLShaderProgram mshader;
	QOpenGLExtraFunctions *f;

	void addBoundy(boundy *bb)
	{
		QMatrix4x4 matr;
		matr.setToIdentity();
		matr.translate(bb->x, bb->z, bb->y);
		matr.scale(bb->wid(), bb->hei());
		boundydata.append(matr);
	}
	SpotZones();

	void updateBuffer();
	void initGL();
	void renderGL(QMatrix4x4 view, QMatrix4x4 projection);
	void initSpotModel();
};

#endif // SPOTZONES_H
