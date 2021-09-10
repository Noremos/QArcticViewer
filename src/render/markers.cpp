#include "markers.h"


#include <QVector2D>
#include <src/core/project.h>

StaticMarkers::StaticMarkers()
{
	proj = Project::getProject();
}

void StaticMarkers::initGL()
{
	initializeOpenGLFunctions();
	f = QOpenGLContext::currentContext()->extraFunctions();

//	initModel();
}


void StaticMarkers::addBoundy(QVector3D& bb, int displayFactor)
{
	bb.setX(bb.x() / displayFactor);
//	bb.setY(bb.y() / displayFactor);
	bb.setZ(bb.z() / displayFactor);
	QMatrix4x4 matr;
	matr.setToIdentity();

	matr.translate(bb.x(), bb.y(), bb.z());
	matr.scale(1,10,1);


	boundydata.append(InstanceData(matr, (int)glColor::Oragne));
}


void StaticMarkers::updateBuffer()
{
	obj.readFile(":/resources/objects/makr.obj");

	vao.destroy();
	indexBuf.destroy();
	arrBuf.destroy();
	modelsBuf.destroy();

	mshader.create();
	glinstanse::initShader(mshader, ":/shaders/spotZone.vert", ":/shaders/spotZone.frag");

	modelsBuf.create();
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

	arrBuf.bind();
	arrBuf.allocate((const void *)vect.data(), vect.size() * sizeof(lvertex));
	arrBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

//	arrBuf.bind();
//	arrBuf.allocate((const void *)obj.lvetexes.data(), obj.lvetexes.size() * sizeof(lvertex));
//	arrBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

	// Transfer index data to VBO 1
//	indexBuf.bind();
//	indexBuf.allocate((const void *)obj.faces.data(), obj.faces.size() * sizeof(face));
//	indexBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

	// Position
	int vertexLocation = mshader.attributeLocation("aPos");
	mshader.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(lvertex));//sizeof(vertex)
	mshader.enableAttributeArray(vertexLocation);

	//	vao.release();

	//	mshader.release();

	faceSize = obj.faces.size() * 3;
	obj.faces.clear();
	obj.lvetexes.clear();

	int loc, offloc;

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

	mshader.setUniformValue("opacity", 0.8f);
	mshader.setUniformValue("localMinHei", -1.f);

	f->glVertexAttribDivisor(loc - 3, 1);
	f->glVertexAttribDivisor(loc - 2, 1);
	f->glVertexAttribDivisor(loc - 1, 1);
	f->glVertexAttribDivisor(loc, 1);
	f->glVertexAttribDivisor(offloc, 1);


	vao.release();
	arrBuf.release();
	indexBuf.release();
	modelsBuf.release();
	mshader.release();

	// mast be at end
	boundySize = boundydata.size();
//	boundydata.clear();
}

void StaticMarkers::renderGL(QMatrix4x4 &view, QMatrix4x4 &projection)
{
	if (boundySize==0)
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

//	indexBuf.bind();
//	modelsBuf.bind();
//	arrBuf.bind();
//	printErrors();
	//	f->glDrawArraysInstanced(GL_TRIANGLES, 0, faceSize, boundySize); //spot
	//glDrawElements(GL_TRIANGLES, faceSize, GL_UNSIGNED_INT, nullptr);//terra

//	f->glDrawElementsInstanced(GL_TRIANGLES, faceSize, GL_UNSIGNED_INT, 0, boundySize);
	f->glDrawArraysInstanced(GL_TRIANGLES, 0, faceSize, boundySize);

	vao.release();
//	indexBuf.release();
//	modelsBuf.release();
//	arrBuf.release();
	mshader.release();
}

void StaticMarkers::initModel()
{

}

