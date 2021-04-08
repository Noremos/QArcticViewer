#ifndef TERRARIANGUI_H
#define TERRARIANGUI_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include <QVector2D>
#include <QVector3D>

#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "cubegl.h"

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

class MaterialGui
{
public:
	MaterialGui();
};


enum class DisplayMode {
	Heimap,
	object,
	texture

};
class Terrain
{
	// OpenGL data
	QOpenGLBuffer arrayBuf;
	QOpenGLBuffer indexBuf;
	QOpenGLShaderProgram heimapShader, objectShader, textureShader;


	// Display data
	DisplayMode displayMode;
	int textNum;
	float minH;
	float maxH;
	QVector<QOpenGLTexture *> textures;



	struct vertex
	{
		float x, y, z, texX, texY;
	};
	struct face
	{
		unsigned int v1, v2, v3;
	};
	QVector<vertex> vetexes;
	QVector<face> faces;
	QVector<QVector2D> textureCoords;

public:

	void displayHeimap(float minH, float maxH)
	{
		displayMode = DisplayMode::Heimap;
		this->minH = minH;
		this->maxH = maxH;
	}

	void displayTexture(int textNum)
	{
		this->textNum = textNum;
		displayMode = DisplayMode::texture;
	}

	void displayObject() { displayMode = DisplayMode::object; }

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
		if (!prog.bind())
			return;
	}

	void initShaders();

	void addTexture(QString path);

	void initArrays();
	void readfile(const char* filename);
	void draw();
	void drawFull(QMatrix4x4 &view);
};
#include <fstream>
#include <string>

#endif // TERRARIANGUI_H
