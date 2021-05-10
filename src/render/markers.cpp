#include "markers.h"


#include <QVector2D>
#include <src/core/project.h>

Markers::Markers()
{
	proj = Project::getProject();
}

void Markers::initGL()
{
	initializeOpenGLFunctions();
	f = QOpenGLContext::currentContext()->extraFunctions();

	glinstanse::initShader(mshader, ":/shaders/spotZone.vert", ":/shaders/spotZone.frag");
	modelsBuf.create();
	vao.create();
	arrBuf.create();
	indexBuf.create();
	initModel();
}


void Markers::addBoundy(QVector3D& bb, int displayFactor, int type)
{
	bb.setX(bb.x() / displayFactor);
	bb.setY(bb.y() / displayFactor);
	QMatrix4x4 matr;
	matr.setToIdentity();

	matr.translate(bb.x(), bb.z(), bb.y());
//	matr.scale(bb.wid() / 2, bb.zei() / 2, bb.hei() / 2);

	boundydata.append(InstanceData(matr, type));
}


void Markers::updateBuffer()
{
	vao.bind();
	int loc, offloc;
	mshader.bind();

	modelsBuf.destroy();
	modelsBuf.create();
	modelsBuf.bind();
	modelsBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
	modelsBuf.allocate(this->boundydata.data(), boundydata.size() * sizeof(InstanceData));

	qDebug() << boundydata.size();

	GLsizei vec4size = sizeof(QVector4D);
	GLsizei matr4Size = sizeof(InstanceData);

	qDebug() << "Matr:" << matr4Size;

	loc = mshader.attributeLocation("instanceMatrix");

	mshader.setAttributeBuffer(loc, GL_FLOAT, 0, 4, matr4Size);
	mshader.enableAttributeArray(loc);
	++loc;
	mshader.setAttributeBuffer(loc, GL_FLOAT, vec4size, 4, matr4Size);
	mshader.enableAttributeArray(loc);
	++loc;
	mshader.setAttributeBuffer(loc, GL_FLOAT, vec4size * 2, 4, matr4Size);
	mshader.enableAttributeArray(loc);
	++loc;
	mshader.setAttributeBuffer(loc, GL_FLOAT, vec4size * 3, 4, matr4Size);
	mshader.enableAttributeArray(loc);

	offloc = mshader.attributeLocation("aColor");
	mshader.setAttributeBuffer(offloc, GL_FLOAT, vec4size * 4, 1, matr4Size);
	mshader.enableAttributeArray(offloc);

	mshader.setUniformValue("opacity", 1.0f);

	f->glVertexAttribDivisor(loc - 3, 1);
	f->glVertexAttribDivisor(loc - 2, 1);
	f->glVertexAttribDivisor(loc - 1, 1);
	f->glVertexAttribDivisor(loc, 1);
	f->glVertexAttribDivisor(offloc, 1);


	vao.release();

	//	goodBuff.release();
	modelsBuf.release();
	arrBuf.release();
	indexBuf.release();
	mshader.release();


	// mast be at end
	boundySize = boundydata.size();
	boundydata.clear();
}

void Markers::renderGL(QMatrix4x4 &view, QMatrix4x4 &projection)
{
	if (boundySize==0)
		return;
	// draw meteorites
	mshader.bind();

	//projection * view * model * vec4(position, 1.0f);
	mshader.setUniformValue("projection", projection);
	mshader.setUniformValue("view", view);
	mshader.setUniformValue("factor", factor);
	mshader.setUniformValue("minHei", proj->getImgMinVal() / proj->displayFactor);
	mshader.setUniformValue("opacity", 1.0f);

	vao.bind();
	f->glDrawElementsInstanced(GL_TRIANGLES, faceSize, GL_UNSIGNED_INT, 0, boundySize);
	vao.release();
	mshader.release();
}

void Markers::initModel()
{
	arrBuf.bind();
	obj.readFile(":/resources/objects/makr.obj");


	vao.bind();

	arrBuf.allocate((const void *)obj.vetexes.data(), obj.vetexes.size() * sizeof(vertex));
	arrBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

	// Transfer index data to VBO 1
	indexBuf.bind();
	indexBuf.allocate((const void *)obj.faces.data(), obj.faces.size() * sizeof(face));
	indexBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

	// Position
	int vertexLocation = mshader.attributeLocation("aPos");
	mshader.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));
	mshader.enableAttributeArray(vertexLocation);

	vao.release();

	arrBuf.release();
	indexBuf.release();
	faceSize = obj.faces.size() * 3;
	obj.faces.clear();
	obj.vetexes.clear();
}

