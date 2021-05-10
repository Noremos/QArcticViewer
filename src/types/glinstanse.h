#ifndef GLINSTANSE_H
#define GLINSTANSE_H

#include <QVector2D>
#include <QVector3D>

#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class glinstanse :protected QOpenGLFunctions
{
private:
public:
    glinstanse(/* args */)
    {
//        initGL();
    }
    virtual void initGL() = 0;
	virtual void renderGL(QMatrix4x4& view, QMatrix4x4& projection) = 0;

    virtual ~glinstanse()
    {}
    
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
};

struct InstanceData
{
	QVector4D v0;
	QVector4D v1;
	QVector4D v2;
	QVector4D v3;
	//	QMatrix4x4 model;
	float val;
	//	int zer1;
	//	int zer2;
	InstanceData(QMatrix4x4 matr, int val)
	{
		//		model = matr;
		v0 = matr.column(0);
		v1 = matr.column(1);
		v2 = matr.column(2);
		v3 = matr.column(3);
		this->val = val;
	}
};
#endif // !GLINSTANSE_H
