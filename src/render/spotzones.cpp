#include <QVector2D>
#include <src/core/project.h>
#include "spotzones.h"

SpotZones::SpotZones() :
	  arrBuf(QOpenGLBuffer::Type::VertexBuffer),
	  modelsBuf(QOpenGLBuffer::Type::VertexBuffer),
	  indexBuf(QOpenGLBuffer::Type::IndexBuffer)
{
	Project::getProject()->spotZones = this;
}

void SpotZones::updateBuffer()
{
	vao.bind();

	mshader.bind();
	initSpotModel();


	modelsBuf.destroy();
	modelsBuf.create();
	modelsBuf.bind();
	modelsBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
	modelsBuf.allocate(this->boundydata.data(), boundydata.size() * sizeof(QMatrix4x4));
	qDebug() << boundydata.size();

	GLsizei vec4size = sizeof(QVector4D);
	GLsizei matr4Size = sizeof(QMatrix4x4);

	int loc = mshader.attributeLocation("instanceMatrix");
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

	f->glVertexAttribDivisor(loc-3, 1);
	f->glVertexAttribDivisor(loc-2, 1);
	f->glVertexAttribDivisor(loc-1, 1);
	f->glVertexAttribDivisor(loc  , 1);


	vao.release();

	modelsBuf.release();
	arrBuf.release();
	indexBuf.release();
	mshader.release();

//	boundydata.clear();
}

void SpotZones::renderGL(QMatrix4x4 view, QMatrix4x4 projection)
{

	// draw meteorites
	mshader.bind();

	//projection * view * model * vec4(position, 1.0f);
	mshader.setUniformValue("projection", projection);
	mshader.setUniformValue("view", view);
	mshader.setUniformValue("factor", factor);

	vao.bind();
	f->glDrawArraysInstanced(GL_TRIANGLES, 0, 36, boundydata.size());
	vao.release();
	mshader.release();

}

void SpotZones::
	initSpotModel()
{
	static const GLfloat g_vertex_buffer_data[108] = {
		-1.0f,-1.0f,-1.0f, // Треугольник 1 : начало
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // Треугольник 1 : конец
		1.0f, 1.0f,-1.0f, // Треугольник 2 : начало
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // Треугольник 2 : конец
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
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
