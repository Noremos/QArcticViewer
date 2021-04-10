#include <QVector2D>
#include "spotzones.h"

SpotZones::SpotZones() :
	  arrBuf(QOpenGLBuffer::Type::VertexBuffer),
	  modelsBuf(QOpenGLBuffer::Type::VertexBuffer),
	  indexBuf(QOpenGLBuffer::Type::IndexBuffer)
{
	initializeOpenGLFunctions();
	f = QOpenGLContext::currentContext()->extraFunctions();

	initGL();
}

void SpotZones::updateBuffer()
{
	vao.bind();

	modelsBuf.bind();
	modelsBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
	modelsBuf.allocate(this->boundydata.data(), boundydata.size() * sizeof(QMatrix4x4));


	GLsizei vec4size = sizeof(QVector4D);
	GLsizei matr4Size = sizeof(QMatrix4x4);

	int loc = mshader.attributeLocation("instanceMatrix");
	mshader.setAttributeBuffer(loc, GL_FLOAT, 0, matr4Size, 0);
	mshader.enableAttributeArray(loc);
	//	++loc;
	//	mshader.setAttributeBuffer(loc, GL_FLOAT, 0, matr4Size, vec4size);
	//	mshader.enableAttributeArray(loc);
	//	++loc;
	//	mshader.setAttributeBuffer(loc, GL_FLOAT, 0, matr4Size, vec4size*2);
	//	mshader.enableAttributeArray(loc);
	//	++loc;
	//	mshader.setAttributeBuffer(loc, GL_FLOAT, 0, matr4Size, vec4size*3);
	//	mshader.enableAttributeArray(loc);


	modelsBuf.release();

	vao.release();

}

void SpotZones::renderGL(QMatrix4x4 view, QMatrix4x4 projection)
{

	// draw meteorites
	mshader.bind();

	//projection * view * model * vec4(position, 1.0f);
	mshader.setUniformValue("projection", projection);
	mshader.setUniformValue("view", view);
	mshader.setUniformValue("factor", 1.0f);

	vao.bind();
	f->glDrawElementsInstanced(GL_TRIANGLES, boundydata.size(), GL_UNSIGNED_INT, 0, 34);
	vao.release();
	mshader.release();

}

void SpotZones::initSpotModel()
{
	QVector3D vertices[] = {
		// Vertex data for face 0
		QVector3D(-1.0f, -1.0f,  1.0f), // v0
		QVector3D( 1.0f, -1.0f,  1.0f), // v1
		QVector3D(-1.0f,  1.0f,  1.0f), // v2
		QVector3D( 1.0f,  1.0f,  1.0f), // v3

		// Vertex data for face 1
		QVector3D( 1.0f, -1.0f,  1.0f), // v4
		QVector3D( 1.0f, -1.0f, -1.0f), // v5
		QVector3D( 1.0f,  1.0f,  1.0f), // v6
		QVector3D( 1.0f,  1.0f, -1.0f), // v7

		// Vertex data for face 2
		QVector3D( 1.0f, -1.0f, -1.0f), // v8
		QVector3D(-1.0f, -1.0f, -1.0f), // v9
		QVector3D( 1.0f,  1.0f, -1.0f), // v10
		QVector3D(-1.0f,  1.0f, -1.0f), // v11

		// Vertex data for face 3
		QVector3D(-1.0f, -1.0f, -1.0f), // v12
		QVector3D(-1.0f, -1.0f,  1.0f), // v13
		QVector3D(-1.0f,  1.0f, -1.0f), // v14
		QVector3D(-1.0f,  1.0f,  1.0f), // v15

		// Vertex data for face 4
		QVector3D(-1.0f, -1.0f, -1.0f), // v16
		QVector3D( 1.0f, -1.0f, -1.0f), // v17
		QVector3D(-1.0f, -1.0f,  1.0f), // v18
		QVector3D( 1.0f, -1.0f,  1.0f), // v19

		// Vertex data for face 5
		QVector3D(-1.0f,  1.0f,  1.0f), // v20
		QVector3D( 1.0f,  1.0f,  1.0f), // v21
		QVector3D(-1.0f,  1.0f, -1.0f), // v22
		QVector3D( 1.0f,  1.0f, -1.0f), // v23
	};

	// Indices for drawing cube faces using triangle strips.
	// Triangle strips can be connected by duplicating indices
	// between the strips. If connecting strips have opposite
	// vertex order then last index of the first strip and first
	// index of the second strip needs to be duplicated. If
	// connecting strips have same vertex order then only last
	// index of the first strip needs to be duplicated.
	GLushort indices[] = {
		0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
		4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
		8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
		12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
		16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
		20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
	};

	vao.bind();

	// Generate 2 VBOs
	arrBuf.create();
	indexBuf.create();

	// Transfer vertex data to VBO 0
	arrBuf.bind();
	arrBuf.allocate(vertices, 24 * sizeof(QVector3D));

	// Transfer index data to VBO 1
	indexBuf.bind();
	indexBuf.allocate(indices, 34 * sizeof(GLushort));

	mshader.bind();
	int vertexLocation = mshader.attributeLocation("aPos");
	mshader.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));
	mshader.enableAttributeArray(vertexLocation);

	vao.release();
	arrBuf.release();
	indexBuf.release();
	mshader.release();
}

void SpotZones::initGL()
{

	glinstanse::initShader(mshader, ":/shaders/spotZone.vert", ":/shaders/spotZone.frag");
	modelsBuf.create();
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
