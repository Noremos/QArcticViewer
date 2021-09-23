#include "terrariangui.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QFile>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <fstream>

Terrain::Terrain() : indexBuf(QOpenGLBuffer::IndexBuffer), displayMode(DisplayMode::Heimap), textNum(0)
{
	textures.append(nullptr);
	textures.append(nullptr);
	this->proj = Project::getProject();
}

Terrain::~Terrain()
{
	clearTextures();
}

void Terrain::initGL()
{
	initializeOpenGLFunctions();
	f = QOpenGLContext::currentContext()->extraFunctions();
	initShaders();
}

void Terrain::initShaders()
{
	glinstanse::initShader(heimapShader, ":/shaders/HeightFactor.vert", ":/shaders/HeightFactor.frag");
	glinstanse::initShader(objectShader, ":/shaders/simpleColor.vert", ":/shaders/terraColor.frag");
	glinstanse::initShader(textureShader, ":/shaders/simpleColor.vert", ":/shaders/simpleColor.frag");
	//	initShader(textureShader, ":/vshader.glsl", ":/fshader.glsl");
}


float Terrain::getValue(int x, int z)
{
	if (x < 0 || z < 0 || x >= proj->modelWid || z >= proj->modelHei)
		return -9999;

	return getValue(x * proj->modelHei + z).y;
}

vertex Terrain::getValue(size_t offset)
{
	vertex vert[1];
	arrayBuf.bind();
	arrayBuf.read(offset * sizeof(vertex), vert, sizeof(vertex));
	arrayBuf.release();
	return vert[0];
}

void Terrain::setTexture(int num, QString path)
{
	// Load cube.png image
	//":/shaders/cube.png"
	qDebug() << path;
	if (path.length() == 0)
		return;
	if (path.startsWith("file:///"))
		path = path.remove(0, 8);

	QOpenGLTexture *texture = new QOpenGLTexture(QImage(path).mirrored());

	texture->setMinificationFilter(QOpenGLTexture::Nearest);
	texture->setMagnificationFilter(QOpenGLTexture::Linear);

	// Wrap texture coordinates by repeating
	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
	texture->setWrapMode(QOpenGLTexture::WrapMode::Repeat);

	if (textures[num] != nullptr)
		delete textures[num];
	textures[num] = texture;
}

void Terrain::addTexture(QString path)
{
	// Load cube.png image
	//":/shaders/cube.png"
	qDebug() << path;
	if (path.startsWith("file:///"))
		path = path.remove(0, 8);

	QOpenGLTexture *texture = new QOpenGLTexture(QImage(path).mirrored());

	texture->setMinificationFilter(QOpenGLTexture::Nearest);
	texture->setMagnificationFilter(QOpenGLTexture::Linear);

	// Wrap texture coordinates by repeating
	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
	texture->setWrapMode(QOpenGLTexture::WrapMode::Repeat);

	textures.append(texture);
}

void Terrain::initArrays()
{
	arrayBuf.create();
	indexBuf.create();
	vao.create();
	vao.bind();

	textureShader.bind();
	// Transfer vertex data to VBO 0
	qDebug() << "Vert tatol size:" << obj.vetexes.size() * sizeof(vertex);
	arrayBuf.bind();
	arrayBuf.allocate((const void *)obj.vetexes.data(), obj.vetexes.size() * sizeof(vertex));
	arrayBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

	// Transfer index data to VBO 1
	indexBuf.bind();
	indexBuf.allocate((const void *)obj.faces.data(), obj.faces.size() * sizeof(face));
	indexBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

	int offset = 0;
	int vertexLocation = textureShader.attributeLocation("a_position");
	textureShader.setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(vertex));
	textureShader.enableAttributeArray(vertexLocation);

	// Offset for texture coordinate
	offset += sizeof(QVector3D);

	int texcoordLocation = textureShader.attributeLocation("a_texcoord");
	textureShader.setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(vertex));
	textureShader.enableAttributeArray(texcoordLocation);

	vao.release();
	textureShader.release();

	arrayBuf.release();
	indexBuf.release();
	faceSize = obj.faces.size() * 3;
	vertSize = obj.vetexes.size();
	obj.faces.clear();
	obj.vetexes.clear();
}

void Terrain::renderGL(QMatrix4x4 &view, QMatrix4x4 &projection)
{
	// Tell OpenGL which VBOs to use

	// Offset for position
	QOpenGLShaderProgram *curshader;
	switch (this->displayMode)
	{
	case DisplayMode::Heimap:
		curshader = &this->heimapShader;
		curshader->bind();
		curshader->setUniformValue("maxHei", proj->getImgMaxVal() / proj->u_displayFactor);
		break;

	case DisplayMode::object:
		curshader = &this->objectShader;
		curshader->bind();
		break;

	case DisplayMode::texture:
		curshader = &this->textureShader;
		if (textNum < textures.size() && textures[textNum]!=nullptr)
			textures[textNum]->bind();

		curshader->bind();
		//			int idt = textures[textNum]->textureId() - 1;
		curshader->setUniformValue("texture0", 0);
		break;
	default:
		return;
	}

	QMatrix4x4 model;
	model.setToIdentity();
	model.scale(1, 1, 1);
	model.translate(0, 0, 0);

	curshader->setUniformValue("factor", proj->u_heiFactor);
	curshader->setUniformValue("minHei", proj->getImgMinVal() / proj->u_displayFactor);

	curshader->setUniformValue("projection", projection);
	curshader->setUniformValue("view", view);
	curshader->setUniformValue("model", model);

	// Draw cube geometry using indices from VBO 1
	vao.bind();
	glDrawElements(GL_TRIANGLES, faceSize, GL_UNSIGNED_INT, nullptr);
	vao.release();

	curshader->release();

	switch (this->displayMode)
	{
	case DisplayMode::texture:
		if (textNum < textures.size() && textures[textNum]!=nullptr)
			textures[textNum]->release();
		break;
	default:
		break;
	}
}
