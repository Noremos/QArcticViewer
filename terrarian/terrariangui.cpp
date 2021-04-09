#include "terrariangui.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>


Terrain::Terrain():indexBuf(QOpenGLBuffer::IndexBuffer)
{
	initializeOpenGLFunctions();
	f = QOpenGLContext::currentContext()->extraFunctions();
}

void Terrain::initGL()
{

	initShaders();
}

void Terrain::initShaders()
{
	initShader(heimapShader, ":/shaders/HeightFactor.vert", ":/shaders/HeightFactor.frag");
	initShader(objectShader, ":/shaders/simpleColor.vert", ":/shaders/simpleColor.frag");
	initShader(textureShader, ":/shaders/simpleColor.vert", ":/shaders/simpleColor.frag");
//	initShader(textureShader, ":/vshader.glsl", ":/fshader.glsl");
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
struct VertexData
{
	QVector3D v;
	QVector2D t;
};

void Terrain::initArrays()
{
	arrayBuf.create();
	indexBuf.create();
	vao.create();
	vao.bind();

	textureShader.bind();
	// Transfer vertex data to VBO 0
	arrayBuf.bind();
	arrayBuf.allocate((const void *) vetexes.data(), vetexes.size() * sizeof(vertex));
	arrayBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

	// Transfer index data to VBO 1
	indexBuf.bind();
	indexBuf.allocate((const void *) faces.data(), faces.size() * sizeof(face));
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
	faceSize = faces.size();
	vertSize = vetexes.size();
	faces.clear();
	vetexes.clear();
}

void Terrain::drawFull(QMatrix4x4& view, QMatrix4x4& projection)
{
	// Tell OpenGL which VBOs to use


	// Offset for position
	QOpenGLShaderProgram* curshader;
	switch (this->displayMode)
	{
	case DisplayMode::Heimap:
		curshader = &this->heimapShader;
		curshader->bind();
		break;
	case DisplayMode::object:
		curshader = &this->objectShader;
		curshader->bind();
		break;
	case DisplayMode::texture:
		{
			curshader = &this->textureShader;
			textures[textNum]->bind();
			curshader->bind();
//			int idt = textures[textNum]->textureId() - 1;
			curshader->setUniformValue("texture0", 0);
			break;
		}
	default:
		break;
	}


	QMatrix4x4 model;
	model.setToIdentity();
	model.scale(0.005, 0.005, 0.005);
	model.translate(-200.0, -30.0, -200.0);
	curshader->setUniformValue("projection", projection);
	curshader->setUniformValue("view", view);
	curshader->setUniformValue("model", model);

	// Draw cube geometry using indices from VBO 1
	vao.bind();
	glDrawElements(GL_TRIANGLES, faceSize*3, GL_UNSIGNED_INT, nullptr);
	vao.release();

	curshader->release();

	switch (this->displayMode)
	{
	case DisplayMode::texture:
		textures[textNum]->release();
		break;
	default:
		break;
	}
}

int parceFace(const char *str, int len)
{
	const char *razd = str;
	int poz = 0;
	while (*razd != '/')
	{
		if (len == poz)
			return 0;
		++razd;
		++poz;
	}
	return poz;
}

void initFace(std::string &str, unsigned int &faceV)
{
	const char *cstr = str.c_str();
	int poz = parceFace(cstr, str.length());
	str[poz] = '\0';
	faceV = atoi(cstr);
}

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

void raedFast(const char* fName)
{
//	struct stat sb;
//	long cntr = 0;
//	int fd, lineLen;
//	char *data;
//	char *line;
//	// map the file
//	fd = open(fName, O_RDONLY);
//	fstat(fd, &sb);
//	//// int pageSize;
//	//// pageSize = getpagesize();
//	//// data = mmap((caddr_t)0, pageSize, PROT_READ, MAP_PRIVATE, fd, pageSize);
//	data = mmap((void*)0, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);


//	// проецировать файл в память
//	auto hMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
//	if (hMapping == INVALID_HANDLE_VALUE)
//	{
//		CloseHandle(hFile);
//		return ERR_SYS_FUNCTION;
//	}
//	// отобразить файл в память
//	image = (octet*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
//	if (image == NULL)
//	{
//		CloseHandle(hMapping), CloseHandle(hFile);
//		return ERR_SYS_FUNCTION;
//	}

//	line = data;
//	// get lines
//	while(cntr < sb.st_size) {
//		lineLen = 0;
//		line = data;
//		// find the next line
//		while(*data != '\n' && cntr < sb.st_size) {
//			data++;
//			cntr++;
//			lineLen++;
//		}
//		/***** PROCESS LINE *****/
//		// ... processLine(line, lineLen);
//	}
//	return 0;
}
#include  <sstream>
void Terrain::readfile(const char *filename)
{
	std::string s;
	std::ifstream fin(filename);
	if (!fin)
		return;
	size_t lines = 0;

	std::string temp;

	faces.clear();
	vetexes.clear();
	while (std::getline(fin, s))
	{
		switch (*s.c_str())
		{
		case 'v': {
			std::istringstream iss(s);

			vertex v;
			iss >> temp >> v.x >> v.y >> v.z;

			std::getline(fin, s);
			lines++;

			// vt
			assert(s[0] == 'v' && s[1] == 't');

			int i = 2;
			while (s[++i] != ' ')
				;

			s[i] = '\0';

			v.texX = atof(s.c_str() + 3);
			v.texY = atof(s.c_str() + i+1);

			this->vetexes.push_back(v);

			break;
		}
		case 'f': {
			std::istringstream iss(s);
			face f;
			// skip 'f'
			iss >> temp >> temp;
			initFace(temp, f.v1);
			iss >> temp;
			initFace(temp, f.v2);
			iss >> temp;
			initFace(temp, f.v3);

			faces.push_back(f);
			}
		}
		lines++;
	}
	fin.close();
	qDebug() << "Readed " << lines << " lines";
	qDebug() << "Vs: " << vetexes.size();
	qDebug() << "Fs: " << faces.size();
	qDebug() << vetexes[vetexes.size()-1].x<< vetexes[vetexes.size()-1].y<< vetexes[vetexes.size()-1].z;
	qDebug() << faces[faces.size()-1].v1<< faces[faces.size()-1].v2<< faces[faces.size()-1].v3;
//	const unsigned char *ssda = glGetString(GL_VERSION);

//	QString sds = QString::fromLocal8Bit((const char*)ssda);
//	qDebug() << sds;
	initArrays();
	return;
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
