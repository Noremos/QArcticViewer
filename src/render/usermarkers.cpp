#include "usermarkers.h"


#include <QVector2D>
#include <src/core/project.h>

DynamicMarkers::DynamicMarkers()
{
	proj = Project::getProject();
}

DynamicMarkers::~DynamicMarkers()
{
//	buffer.close();
}

void DynamicMarkers::initGL()
{
	initializeOpenGLFunctions();
	f = QOpenGLContext::currentContext()->extraFunctions();
	initArrays();


	//	initModel();
}


void DynamicMarkers::addBoundy(float x, float y, float z)
{
	QMatrix4x4 matr;
	matr.setToIdentity();

	matr.translate(x, y, z);
	matr.scale(1,10,1);

	boundydata.append(MatrixData(matr));
//	buffer.writeLine(QString("%1 %2 %3").arg(x).arg(y).arg(z));
}

void DynamicMarkers::addBoundy(const QVector3D &vec)
{
	addBoundy(vec.x(), vec.y(), vec.z());
}

void DynamicMarkers::move(size_t i, const QVector3D &pos)
{
	if (abs(pos.y()) == 9999)
		return;
	QMatrix4x4 matr;
	matr.setToIdentity();

	matr.translate(pos);
	matr.scale(1,10,1);

	boundydata[i] = matr;
}


void DynamicMarkers::initArrays()
{
	obj.readFile(":/resources/objects/makr.obj");

	vao.destroy();
	indexBuf.destroy();
	arrBuf.destroy();

	mshader.create();
	glinstanse::initShader(mshader, ":/shaders/marker.vert", ":/shaders/marker.frag");

	vao.create();
	arrBuf.create();
	indexBuf.create();
	vao.bind();
	mshader.bind();

	QVector<lvertex> vect;
	for (int i = 0; i < obj.faces.size(); ++i)
	{
		vect.append(obj.lvetexes[obj.faces[i].v1]);
		vect.append(obj.lvetexes[obj.faces[i].v2]);
		vect.append(obj.lvetexes[obj.faces[i].v3]);
	}
	faceSize = vect.size();

	arrBuf.bind();
	arrBuf.allocate((const void *)vect.data(), vect.size() * sizeof(lvertex));
	arrBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

	// Position
	int vertexLocation = mshader.attributeLocation("aPos");
	mshader.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(lvertex));//sizeof(vertex)
	mshader.enableAttributeArray(vertexLocation);


	vao.release();
	arrBuf.release();
	indexBuf.release();
	mshader.release();

}

void DynamicMarkers::renderGL(QMatrix4x4 &view, QMatrix4x4 &projection)
{
	if (boundydata.size()==0)
		return;
	// draw meteorites
	mshader.bind();

	//projection * view * model * vec4(position, 1.0f);
	mshader.setUniformValue("projection", projection);
	mshader.setUniformValue("view", view);
	mshader.setUniformValue("factor", proj->u_heiFactor);
	mshader.setUniformValue("minHei", proj->getImgMinVal() / proj->u_displayFactor);
	//	mshader.setUniformValue("opacity", 1.0f);

	vao.bind();

	for (size_t i = 0, total = boundydata.size(); i < total; ++i)
	{
		mshader.setUniformValue("model", boundydata[i].toMatrix());
		f->glDrawArrays(GL_TRIANGLES, 0, faceSize);
	}
	vao.release();
	mshader.release();
}

void DynamicMarkers::save()
{
	buffer.openFileStream(Project::getProject()->getPath(BackPath::markers), 1000);

	for (size_t i = 0, total = boundydata.size(); i < total; ++i)
	{
		buffer.writeLine(QString("%1 %2 %3").arg(boundydata[i].getX()).arg(boundydata[i].getY()).arg(boundydata[i].getZ()));
	}
	buffer.dowrite();
}

void DynamicMarkers::openFile()
{
	enable = true;
}

