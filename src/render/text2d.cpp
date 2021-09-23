#include <QChar>
#include <QChar>
#include <QVector2D>
#include <src/core/project.h>
#include "text2d.h"

void Text2d::addText(boundy &bb)
{
	return;
	float factor = 10;
	QString text = "";
	text.append(" L: ");
	text.append(QString::number(factor * bb.hei() * resol));
	text.append(" W: ");
	text.append(QString::number(factor * bb.wid() * resol));
	text.append(" H: ");
	text.append(QString::number(factor * bb.zei()));

	for (int i = 0; i < text.length(); ++i)
	{
		QMatrix4x4 matr;
		matr.setToIdentity();
		matr.translate(bb.x + bb.wid() / 2, bb.endZ + bb.zei()/2 + 1,bb.y + bb.hei() / 2);
		//text bb.x, bb.endZ, bb.y
		matr.scale(bb.wid(), 1, bb.hei());

//		matr.translate(bb.x + bb.wid() / 2 + i * xSize, bb.endZ + bb.zei() / 2 + 1, bb.y + bb.hei() / 2);
//		matr.scale(bb.wid(), 1, bb.hei());
		//text bb.x, bb.endZ, bb.y
		int b = text[i].toUpper().toLatin1();

		//skip  not user letters
		b -= 32;
		if (b>=64)
			continue;

		matr(3, 3) = (float)b;
		boundydata.append(MatrixData(matr));
	}
}

Text2d::Text2d() : arrBuf(QOpenGLBuffer::Type::VertexBuffer), modelsBuf(QOpenGLBuffer::Type::VertexBuffer)
{
	Project::getProject()->text = this;
}

void Text2d::updateBuffer()
{
	return;

	vao.bind();

	mshader.bind();
	initSpotModel();


	modelsBuf.destroy();
	modelsBuf.create();
	modelsBuf.bind();
	modelsBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
	modelsBuf.allocate(this->boundydata.data(), boundydata.size() * sizeof(QMatrix4x4));
	qDebug() << boundydata.size();
	boundySize = boundydata.size();

	GLsizei vec4size = sizeof(QVector4D);
	GLsizei matr4Size = sizeof(QMatrix4x4);

	int loc = mshader.attributeLocation("instanceMatrix");
	//	mshader.setUniformValueArray(loc, boundydata.data(), boundydata.size());
	//
	//							 loc,size, type,   norm       ,stride, off
	//	f->glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FAILSE, matr4Size, 0);
	//										off,tupleSize, stride
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

	f->glVertexAttribDivisor(loc - 3, 1);
	f->glVertexAttribDivisor(loc - 2, 1);
	f->glVertexAttribDivisor(loc - 1, 1);
	f->glVertexAttribDivisor(loc, 1);


	vao.release();

	modelsBuf.release();
	arrBuf.release();
	mshader.release();

	boundydata.clear();
}

void Text2d::renderGL(QMatrix4x4 view, QMatrix4x4 projection)
{
	return;

	// draw meteorites
	mshader.bind();
	texture->bind();
	//projection * view * model * vec4(position, 1.0f);
	mshader.setUniformValue("projection", projection);
	mshader.setUniformValue("view", view);

	vao.bind();
	f->glDrawArraysInstanced(GL_TRIANGLES, 0, 6, boundySize);
	vao.release();
	texture->release();
	mshader.release();
}

void Text2d::initSpotModel()
{
	GLfloat vertices[] = {
		// Первый треугольник
		0.5f,  0.5f, 0.0f,  1.0f, 1.0f,// Верхний правый угол
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f,// Нижний правый угол
		-0.5f,  0.5f, 0.0f,  0.0f, 0.0f,// Верхний левый угол
		// Второй треугольник
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // Нижний правый угол
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f,// Нижний левый угол
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f   // Верхний левый угол
	};

	//	vao.bind();

	// Generate 2 VBOs


	arrBuf.destroy();
	arrBuf.create();
	// Transfer vertex data to VBO 0
	arrBuf.bind();
	arrBuf.allocate(vertices, sizeof(vertices));

	// Transfer index data to VBO 1
	//	indexBuf.destroy();
	//	indexBuf.create();
	//	indexBuf.bind();
	//	indexBuf.allocate(cube_elements, 36);

	//	mshader.bind();
	int vertexLocation = mshader.attributeLocation("a_position");
	mshader.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D) + sizeof(QVector2D));
	mshader.enableAttributeArray(vertexLocation);


	//	 Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
	int texcoordLocation = mshader.attributeLocation("a_texcoord");
	mshader.setAttributeBuffer(texcoordLocation, GL_FLOAT, 3, 2, sizeof(QVector3D) + sizeof(QVector2D));
	mshader.enableAttributeArray(texcoordLocation);
	mshader.setUniformValue("texture0", 0);

	//	vao.release();
	//	arrBuf.release();
	//	indexBuf.release();
	//	mshader.release();
}

void Text2d::initGL()
{
	initializeOpenGLFunctions();	
	f = QOpenGLContext::currentContext()->extraFunctions();

	return;

	glinstanse::initShader(mshader, ":/shaders/text.vert", ":/shaders/text.frag");



	texture = new QOpenGLTexture(QImage(":/text.bmp").mirrored());

	//    // Set nearest filtering mode for texture minification
	texture->setMinificationFilter(QOpenGLTexture::Nearest);

	//    // Set bilinear filtering mode for texture magnification
	texture->setMagnificationFilter(QOpenGLTexture::Linear);
	//    // Wrap texture coordinates by repeating
	//	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
	texture->setWrapMode(QOpenGLTexture::Repeat);


	xSize= texture->width() / 8;
	ySize= texture->height() / 8;


	modelsBuf.create();
	vao.create();
	arrBuf.create();
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
