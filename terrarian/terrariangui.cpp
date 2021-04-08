#include "terrariangui.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

MaterialGui::MaterialGui()
{

}


void Terrain::initShaders()
{
	//		initShader(program, ":/shaders/vshader.glsl", ":/shaders/fshader.glsl");
	initShader(heimapShader, ":/shaders/HeightFactor.vert", ":/shaders/HeightFactor.frag");
	initShader(objectShader, ":/shaders/simpleColor.vert", ":/shaders/simpleColor.frag");
	initShader(textureShader, ":/shaders/simpleColor.vert", ":/shaders/simpleColor.frag");
}

void Terrain::addTexture(QString path)
{
	// Load cube.png image
	//":/shaders/cube.png"
	qDebug() << path;
	QImage temp = QImage(path.remove(0, 8)).mirrored();
//	qDebug() << temp.width();
	QOpenGLTexture *texture = new QOpenGLTexture(temp);

	//		texture->setMinificationFilter(QOpenGLTexture::Nearest);
	texture->setMagnificationFilter(QOpenGLTexture::Linear);

	// Wrap texture coordinates by repeating
	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
	texture->setWrapMode(QOpenGLTexture::ClampToBorder);

	textures.append(texture);
}

void Terrain::initArrays()
{
	// Transfer vertex data to VBO 0
	arrayBuf.bind();
	arrayBuf.allocate((const void *) vetexes.data(), vetexes.size() * sizeof(vertex));

	// Transfer index data to VBO 1
	indexBuf.bind();
	indexBuf.allocate((const void *) faces.data(), faces.size() * sizeof(face));
}

void Terrain::drawFull(QMatrix4x4& view)
{
	// Tell OpenGL which VBOs to use
	arrayBuf.bind();
	indexBuf.bind();

	// Offset for position
	QOpenGLShaderProgram* curshader;
	quintptr offset = 0;
	switch (this->displayMode)
	{
	case DisplayMode::Heimap:
		curshader = &this->heimapShader;
		break;
	case DisplayMode::object:
		curshader = &this->objectShader;
		break;
	case DisplayMode::texture:
		curshader = &this->textureShader;
		textures[textNum]->bind();
		break;
	default:
		break;
	}

	int modelLoc = curshader->uniformLocation("mvp_matrix");
	curshader->setUniformValue(modelLoc, view);
	// Tell OpenGL programmable pipeline how to locate vertex position data
	int vertexLocation = curshader->attributeLocation("a_position");
	curshader->enableAttributeArray(vertexLocation);
	curshader->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(vertex));

	// Offset for texture coordinate
	offset += sizeof(QVector3D);

	// Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
	int texcoordLocation = curshader->attributeLocation("a_texcoord");
	curshader->enableAttributeArray(texcoordLocation);
	curshader->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(vertex));

	// Draw cube geometry using indices from VBO 1
	glDrawElements(GL_TRIANGLE_STRIP, faces.size(), GL_UNSIGNED_INT, nullptr);
}


void Terrain::readfile(const char *filename)
{
	std::string s;
	std::ifstream fin(filename);
	if (!fin)
		return;
	while (fin >> s)
	{
		switch (*s.c_str())
		{
		case 'v': {
			vertex v;
			fin >> v.x >> v.y >> v.z;
			this->vetexes.push_back(v);
			fin >> s;
			// vt
			assert(s[0] == 'v' && s[1] == 't');
			fin >> v.texX >> v.texY;

		}
		break;
		case 'f': {
			face f;
			fin >> f.v1 >> f.v2 >> f.v3;
			faces.push_back(f);
		}
		break;
		}
	}

	initArrays();
	initShaders();
}

void Terrain::draw()
{
//	glBegin(GL_TRIANGLES);
//	for(int i=0;i<faces.size();i++)
//	{
//		vertex v1= vetexes[faces[i].v1-1];
//		vertex v2=vetexes[faces[i].v2-1];
//		vertex v3=vetexes[faces[i].v3-1];

//		glVertex3f(v1.x,v1.y,v1.z);
//		glVertex3f(v2.x,v2.y,v2.z);
//		glVertex3f(v3.x,v3.y,v3.z);
//	}
//	glEnd();
}
