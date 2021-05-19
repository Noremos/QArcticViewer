#include <QVector2D>
#include <src/core/project.h>
#include "spotzones.h"


void SpotZones::addBoundy(boundy &bb, int displayFactor, glColor type)
{
	bb.divStep(displayFactor);
	QMatrix4x4 matr;
	matr.setToIdentity();

	matr.translate(bb.x + bb.wid()/2, bb.z - bb.zei(), bb.y + bb.hei()/2);
	matr.scale(bb.wid()/2, bb.zei()/2, bb.hei()/2);

	boundydata.append(InstanceData(matr, (int)type));
}

SpotZones::SpotZones() :
	  arrBuf(QOpenGLBuffer::Type::VertexBuffer),
	  modelsBuf(QOpenGLBuffer::Type::VertexBuffer),
	  indexBuf(QOpenGLBuffer::Type::IndexBuffer)
//	  ,goodBuff(QOpenGLBuffer::Type::VertexBuffer)
{
	proj = Project::getProject();
	proj->spotZones = this;
}

void SpotZones::updateBuffer()
{
	vao.bind();
	int loc, offloc;
	mshader.bind();
	initSpotModel();

//	goodBuff.destroy();
//	goodBuff.create();
//	goodBuff.bind();
//	goodBuff.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
//	goodBuff.allocate(this->goodOrBad.data(), goodOrBad.size() * sizeof(int));



	modelsBuf.destroy();
	modelsBuf.create();
	modelsBuf.bind();
	modelsBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
	modelsBuf.allocate(this->boundydata.data(), boundydata.size() * sizeof(InstanceData));

	qDebug() << boundydata.size();
	boundySize = boundydata.size();

	GLsizei vec4size = sizeof(QVector4D);
	GLsizei matr4Size = sizeof(InstanceData);

	qDebug() << "Matr:" << matr4Size;

	loc = mshader.attributeLocation("instanceMatrix");
//	mshader.setUniformValueArray(loc, boundydata.data(), boundydata.size());
	//
	//							 loc,size, type,   norm       ,stride, off
	//	f->glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FAILSE, matr4Size, 0);
	//										off,tupleSize, stride
	mshader.setAttributeBuffer(loc, GL_FLOAT, 0,			4, matr4Size);
	mshader.enableAttributeArray(loc);
	++loc;
	mshader.setAttributeBuffer(loc, GL_FLOAT, vec4size,		4, matr4Size);
	mshader.enableAttributeArray(loc);
	++loc;
	mshader.setAttributeBuffer(loc, GL_FLOAT, vec4size * 2,	4, matr4Size);
	mshader.enableAttributeArray(loc);
	++loc;
	mshader.setAttributeBuffer(loc, GL_FLOAT, vec4size * 3,	4, matr4Size);
	mshader.enableAttributeArray(loc);

	offloc = mshader.attributeLocation("aColor");
	mshader.setAttributeBuffer(offloc, GL_FLOAT, vec4size * 4,1, matr4Size);
	mshader.enableAttributeArray(offloc);

	mshader.setUniformValue("opacity", 0.4f);
	mshader.setUniformValue("localMinHei", -1.f);

	/*
	int vertexLocation = program.attributeLocation("a_position");
	program.setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
	program.enableAttributeArray(vertexLocation);

	// Offset for texture coordinate
	offset += sizeof(QVector3D);

	//	 Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
	int texcoordLocation = program.attributeLocation("a_texcoord");
	program.setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));
	*/
	f->glVertexAttribDivisor(loc - 3, 1);
	f->glVertexAttribDivisor(loc - 2, 1);
	f->glVertexAttribDivisor(loc - 1, 1);
	f->glVertexAttribDivisor(loc    , 1);
	f->glVertexAttribDivisor(offloc , 1);


	vao.release();

//	goodBuff.release();
	modelsBuf.release();
	arrBuf.release();
	indexBuf.release();
	mshader.release();

	boundydata.clear();
}

void SpotZones::renderGL(QMatrix4x4 view, QMatrix4x4 projection)
{
	// draw meteorites
	mshader.bind();

	//projection * view * model * vec4(position, 1.0f);
	mshader.setUniformValue("projection", projection);
	mshader.setUniformValue("view", view);
	mshader.setUniformValue("factor", proj->heiFactor);
	mshader.setUniformValue("minHei", proj->getImgMinVal()/ proj->displayFactor);

	vao.bind();
	f->glDrawArraysInstanced(GL_TRIANGLES, 0, 24, boundySize);
	vao.release();
	mshader.release();

}

void SpotZones::initSpotModel()
{
	const float MaxTop = 40.0f;
	// const float MaxTop = 1.0f;
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f, // Треугольник 1 : начало
		-1.0f,-1.0f, 1.0f,
		-1.0f, MaxTop, 1.0f, // Треугольник 1 : конец
		1.0f, MaxTop,-1.0f, // Треугольник 2 : начало
		-1.0f,-1.0f,-1.0f,
		-1.0f, MaxTop,-1.0f, // Треугольник 2 : конец
//		1.0f,-1.0f, 1.0f,//
//		-1.0f,-1.0f,-1.0f,
//		1.0f,-1.0f,-1.0f,//
		1.0f, MaxTop,-1.0f,//
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,//
		-1.0f,-1.0f,-1.0f,//
		-1.0f, MaxTop, 1.0f,
		-1.0f, MaxTop,-1.0f,//
//		1.0f,-1.0f, 1.0f,
//		-1.0f,-1.0f, 1.0f,
//		-1.0f,-1.0f,-1.0f,
		-1.0f, MaxTop, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, MaxTop, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, MaxTop,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, MaxTop, 1.0f,
		1.0f,-1.0f, 1.0f,
//		1.0f, 1.0f, 1.0f,
//		1.0f, 1.0f,-1.0f,
//		-1.0f, 1.0f,-1.0f,
//		1.0f, 1.0f, 1.0f,
//		-1.0f, 1.0f,-1.0f,
//		-1.0f, 1.0f, 1.0f,
		1.0f, MaxTop, 1.0f,
		-1.0f, MaxTop, 1.0f,
		1.0f,-1.0f, 1.0f
	};
//	vao.bind();

	// Generate 2 VBOs


	arrBuf.destroy();
	arrBuf.create();
	// Transfer vertex data to VBO 0
	arrBuf.bind();
	arrBuf.allocate(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));

	// Transfer index data to VBO 1
//	indexBuf.destroy();
//	indexBuf.create();
//	indexBuf.bind();
//	indexBuf.allocate(cube_elements, 36);

//	mshader.bind();
	int vertexLocation = mshader.attributeLocation("aPos");
	mshader.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));
	mshader.enableAttributeArray(vertexLocation);


//	vao.release();
//	arrBuf.release();
//	indexBuf.release();
//	mshader.release();
}

void SpotZones::initGL()
{
	initializeOpenGLFunctions();
	f = QOpenGLContext::currentContext()->extraFunctions();

	glinstanse::initShader(mshader, ":/shaders/spotZone.vert", ":/shaders/spotZone.frag");
	modelsBuf.create();
//	goodBuff.create();
	vao.create();
	arrBuf.create();
	indexBuf.create();
//	int index = 0;
//	float offset = 0.1f;
//	for(int y = -10; y < 10; y += 2)
//	{
//		for(int x = -10; x < 10; x += 2)
//		{
//			QVector2D translation;
//			translation.setX((float)x / 10.0f + offset);
//			translation.setY((float)y / 10.0f + offset);
//			translations[index++] = translation;
//		}
//	}
//	shader.use();
//	for(unsigned int i = 0; i < 100; i++)
//	{
//		stringstream ss;
//		string index;
//		ss << i;
//		index = ss.str();
//		shader.setVec2(("offsets[" + index + "]").c_str(), translations[i]);
//	}
}
