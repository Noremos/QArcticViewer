#ifndef MARKERS_H
#define MARKERS_H

#include <QOpenGLFunctions>

#include "../types/types.h"
#include "../types/glinstanse.h"


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

#include <src/core/object3d.h>

class Project;

class Markers : public glinstanse
{
public:
	Markers();
public:
	Project *proj;
	Object3d obj;
	uint faceSize = 0;
	QVector<InstanceData> boundydata;

	QOpenGLVertexArrayObject vao;
	int factor = 10;
	QOpenGLBuffer arrBuf, modelsBuf, goodBuff;
	QOpenGLBuffer indexBuf;

	QOpenGLShaderProgram mshader;
	QOpenGLExtraFunctions *f;

	size_t boundySize = 0;
	// type: 0=bad;1=good; 2= more good, 3=more bad
	void addBoundy(QVector3D &bb, int displayFactor, int type = 1);

	void updateBuffer();

	// glinstanse interface
public:
	void initGL() override;
	void renderGL(QMatrix4x4& view, QMatrix4x4& projection) override;
	void initModel();
};

#endif // MARKERS_H
