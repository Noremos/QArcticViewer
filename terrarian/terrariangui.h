#ifndef TERRARIANGUI_H
#define TERRARIANGUI_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "src/core/project.h"
//#include "cubegl.h"

struct MaterialInfo
{
	QString Name;
	QVector3D Ambient;
	QVector3D Diffuse;
	QVector3D Specular;
	float Shininess;
};

struct LightInfo
{
	QVector4D Position;
	QVector3D Intensity;
};

struct Mesh
{
	QString name;
	unsigned int indexCount;
	unsigned int indexOffset;
	QSharedPointer<MaterialInfo> material;
};

struct Node
{
	QString name;
	QMatrix4x4 transformation;
	QVector<QSharedPointer<Mesh>> meshes;
	QVector<Node> nodes;
};


enum class DisplayMode {
	Heimap,
	object,
	texture

};
struct ObjBuffers
{
	QOpenGLBuffer arrayBuf;
	QOpenGLBuffer indexBuf;
};

class Terrain :protected QOpenGLFunctions
{
	// OpenGL data
	size_t faceSize, vertSize;
	QOpenGLBuffer arrayBuf;
	QOpenGLBuffer indexBuf;
//	QVector<ObjBuffers*> buffers;
	QOpenGLShaderProgram heimapShader, objectShader, textureShader;



	// Display data
	DisplayMode displayMode;
	int textNum;
	float minH;
	float maxH;
	QVector<QOpenGLTexture *> textures;

public:
	Terrain();
	~Terrain();
	void initGL();
	void displayHeimap(float minH, float maxH)
	{
		displayMode = DisplayMode::Heimap;
		this->minH = minH;
		this->maxH = maxH;
	}

	Object3d obj;
	void displayTexture(int textNum)
	{
		this->textNum = textNum;
		displayMode = DisplayMode::texture;
	}

	void displayObject() { displayMode = DisplayMode::object; }

	void addTexture(QString path);

	QOpenGLVertexArrayObject vao;


	void clearTextures()
	{
		vao.bind();
		for (int var = 0; var < textures.size(); ++var)
		{
			if (textures[var] != nullptr)
			{
				textures[var]->destroy();
				delete textures[var];
				textures[var] = nullptr;
			}
		}
		vao.release();
		vao.destroy();
		//		textures.clear();
//		textures.append(nullptr);
//		textures.append(nullptr);
	}

	int factor = 10;

	Project *proj;
private:
	QOpenGLExtraFunctions *f;

	static void initShader(QOpenGLShaderProgram& prog, QString vert, QString frag)
	{
		// Compile vertex shader
		if (!prog.addShaderFromSourceFile(QOpenGLShader::Vertex, vert))
			return;

		// Compile fragment shader
		if (!prog.addShaderFromSourceFile(QOpenGLShader::Fragment, frag))
			return;

		// Link shader pipeline
		if (!prog.link())
			return;

		// Bind shader pipeline for use
//		if (!prog.bind())
//			return;
	}

	void initShaders();

public:
	vertex getValue(size_t offset);
	void initArrays();
	void drawFull(QMatrix4x4 &view, QMatrix4x4 &projection);
	void setTexture(int num, QString path);
};
#include <fstream>
#include <string>

#endif // TERRARIANGUI_H
