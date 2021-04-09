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



	struct vertex
	{
		vertex(float _x=0, float _y=0, float _z=0) : x(_x),y(_y),z(_z),texX(_x),texY(_y)
		{}

		float x, y, z;
		float texX, texY;
	};
	struct face
	{
		unsigned int v1, v2, v3;
		face(unsigned int _v1=0, int _v2=0, int _v3=0) : v1(_v1),v2(_v2),v3(_v3)
		{

		}
	};
	QVector<vertex> vetexes;
	QVector<face> faces;
	QVector<QVector2D> textureCoords;

public:
	Terrain();
	void initGL();
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

	void addTexture(QString path);

	QOpenGLVertexArrayObject vao;

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



	void initArrays();
	void draw();
public:
	void readfile(const char* filename);
	void drawFull(QMatrix4x4 &view, QMatrix4x4 &projection);
};
#include <fstream>
#include <string>

#endif // TERRARIANGUI_H
