#ifndef USER_MARKERS_H
#define USER_MARKERS_H

#include <QOpenGLFunctions>

#include "../types/types.h"
#include "../types/glinstanse.h"


#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QVector3D>

#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <src/core/imagesearcher.h>
#include <src/core/object3d.h>
#include <src/core/subclasses.h>

class Project;

class DynamicMarkers : public glinstanse
{
public:
	DynamicMarkers();
	~DynamicMarkers();

	FileBuffer buffer;
public:
	Project *proj;
	Object3d obj;
	uint faceSize = 0;
	QVector<MatrixData> boundydata;

	QOpenGLVertexArrayObject vao;
	FileBuffer buff;
	QOpenGLBuffer arrBuf;
	QOpenGLBuffer indexBuf;

	QOpenGLShaderProgram mshader;
	QOpenGLExtraFunctions *f;

	size_t boundySize = 0;
	// type: 0=bad;1=good; 2= more good, 3=more bad
	void addBoundy(float x, float y, float z);

	void initArrays();

	bool show = false;
	// glinstanse interface
public:
	void initGL() override;
	void renderGL(QMatrix4x4& view, QMatrix4x4& projection) override;
	void initModel();

	void addBoundy(const QVector3D& vec);

	void move(size_t i, const QVector3D &pos);
};

class UserMarkers : public DynamicMarkers
{
public:
	void save();
	bool enable = false;
};

#endif // USER_MARKERS_H
