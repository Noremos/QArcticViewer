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
	initShader(heimapShader, ":/shaders/HeightFactor.vert", ":/shaders/HeightFactor.frag");
	initShader(objectShader, ":/shaders/simpleColor.vert", ":/shaders/simpleColor.frag");
	initShader(textureShader, ":/shaders/simpleColor.vert", ":/shaders/simpleColor.frag");
	//	initShader(textureShader, ":/vshader.glsl", ":/fshader.glsl");
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
	arrayBuf.allocate((const void *)vetexes.data(), vetexes.size() * sizeof(vertex));
	arrayBuf.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

	// Transfer index data to VBO 1
	indexBuf.bind();
	indexBuf.allocate((const void *)faces.data(), faces.size() * sizeof(face));
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
	faceSize = faces.size() * 3;
	vertSize = vetexes.size();
	faces.clear();
	vetexes.clear();
}

void Terrain::drawFull(QMatrix4x4 &view, QMatrix4x4 &projection)
{
	// Tell OpenGL which VBOs to use

	// Offset for position
	QOpenGLShaderProgram *curshader;
	switch (this->displayMode)
	{
	case DisplayMode::Heimap:
		curshader = &this->heimapShader;
		curshader->bind();
		curshader->setUniformValue("texture0", 0);
		curshader->setUniformValue("aminHei", proj->getImgMinVal());
		curshader->setUniformValue("amaxHei", proj->getImgMaxVal());


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

	curshader->setUniformValue("factor", factor);
	curshader->setUniformValue("minval", proj->getImgMinVal()/ proj->displayFactor);

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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

void raedFast(const char */*fName*/)
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
#include <sstream>
#include <charconv>
#include "side-src/fast_float/fast_float.h"

int parceIntFast(const char *num, int len)
{
	int ret = 0;
	std::from_chars(num, num + len, ret);
	return ret;

	int sum = 0;
	for (int i = 0; i < len; i++)
	{
		sum = (sum * 10) + (num[i] - '0');
	}
	return sum;
}

//static bool getLineFast(std::istream &is, std::string &t)
//{
//	t.clear();

//	std::istream::sentry se(is, true);
//	std::streambuf *sb = is.rdbuf();

//	while (true)
//	{
//		int c = sb->sbumpc();
//		if (c == EOF)
//			return false;
//		if (c == '\n' || c == '\r')
//		{
//			c = sb->sbumpc();
//			if (c == '\n')
//				sb->sbumpc();
//			else if (c == EOF)
//				return false;
//			else
//				sb->sungetc();
//			return true;
//		}
//		t += c;
//	}
//	return true;
//}


inline bool isNotEnd(int c)
{
	return (c != ' ' && c != '\n' && c != '\r');
}
int getWord(const char *base)
{
	const char *ref = base;
	int len = 0;
	while (isNotEnd(*ref))
	{
		++ref;
		++len;
	}
	return len;
}

void parceFace(const char *str, unsigned int &f)
{
	const char *razd = str;
	//	int len = 0;
	while (*razd != '/')
	{
		++razd;
		//		++len;
	}
	std::from_chars(str, razd, f);
	--f;
	//	f = parceIntFast(str, len);
}

#include <filesystem>

void Terrain::readfile(QString path)
{
	QFile fin(path);
	//	std::string s;
	//	std::fstream fin(std::file);
	if (!fin.open(QFile::OpenModeFlag::ReadOnly))
	{
		qDebug() << "File " << path << " not found!";
		return;
	}
	size_t lines = 0;

	faces.clear();
	vetexes.clear();

	std::stringstream errss;
	std::string name;

	char rawtoken[500];
	const char *token;
	while (!fin.atEnd())
	{
		int size = fin.readLine(rawtoken, 500);
		token = rawtoken;
		lines++;

		// Skip if empty line.
		if (size==0)
			continue;

		int len;

		if (token[0] == 'v')
		{
			vertex v;

			token += 2;
			len = getWord(token);
			fast_float::from_chars(token, token + len, v.x);
			token += len + 1;

			len = getWord(token);
			fast_float::from_chars(token, token + len, v.y);
			token += len + 1;

			len = getWord(token);
			fast_float::from_chars(token, token + len, v.z);

			fin.readLine(rawtoken, 500);
			token = rawtoken;

			lines++;

			// vt
			assert(token[0] == 'v' && token[1] == 't');

			token += 3;
			len = getWord(token);

			fast_float::from_chars(token, token + len, v.texX);
			int end = size - 3;
			fast_float::from_chars(token + len + 1, token + end, v.texY);


			// token += len + 1;
			// int end = curline.length() - len - 4;
			// fast_float::from_chars(token, token + end, v.texY);

			this->vetexes.push_back(v);

			continue;
		}
		if (token[0] == 'f')
		{
			face fc;

			token += 2;
			len = getWord(token);
			parceFace(token, fc.v1);

			token += len + 1;
			len = getWord(token);
			parceFace(token, fc.v2);

			token += len + 1;
//			len = getWord(token);
			parceFace(token, fc.v3);

			faces.push_back(fc);
			continue;
		}
	}
	fin.close();
	qDebug() << "Readed " << lines << " lines";
	qDebug() << "Vs: " << vetexes.size();
	qDebug() << "Fs: " << faces.size();

//	for (int i = 0; i < 50; ++i)
//	{
//		qDebug() << vetexes[i].x << vetexes[i].y << vetexes[i].z << vetexes[i].texX << vetexes[i].texY;
//		qDebug() << faces[i].v1 << faces[i].v2 << faces[i].v3;

//	}
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
