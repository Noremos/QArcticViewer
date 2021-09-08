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

	static void initShaderSourceCode(QOpenGLShaderProgram& prog, QString vert, QString frag)
	{
		// Compile vertex shader
		if (!prog.addShaderFromSourceCode(QOpenGLShader::Vertex, vert))
			return;

		// Compile fragment shader
		if (!prog.addShaderFromSourceCode(QOpenGLShader::Fragment, frag))
			return;

		// Link shader pipeline
		if (!prog.link())
			return;

		// Bind shader pipeline for use
		//		if (!prog.bind())
		//			return;
	}
	void printErrors()
	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			switch (err)
			{
			case GL_INVALID_ENUM:
				qDebug() << "GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				qDebug() << "GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				qDebug() << "GL_INVALID_OPERATION";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				qDebug() << "GL_INVALID_FRAMEBUFFER_OPERATION";
				break;
			case GL_OUT_OF_MEMORY:
				qDebug() << "GL_OUT_OF_MEMORY";
				break;
			case GL_STACK_UNDERFLOW:
				qDebug() << "GL_STACK_UNDERFLOW";
				break;
			case GL_STACK_OVERFLOW:
				qDebug() << "GL_STACK_OVERFLOW";
				break;
			default:
				break;
			}
			// Process/log the error.
		}
	}

};
struct MatrixData
{
	QVector4D v0;
	QVector4D v1;
	QVector4D v2;
	QVector4D v3;
	//	int zer2;
	MatrixData(const QMatrix4x4& matr)
	{
		//		model = matr;
		v0 = matr.column(0);
		v1 = matr.column(1);
		v2 = matr.column(2);
		v3 = matr.column(3);
	}

	QMatrix4x4 toMatrix()
	{
		QMatrix4x4 matr;
		matr.setColumn(0, v0);
		matr.setColumn(1, v1);
		matr.setColumn(2, v2);
		matr.setColumn(3, v3);
		return matr;
	}

	float getX() const { return v3[0];}
	float getY() const { return v3[1];}
	float getZ() const { return v3[2];}

	float getWid() const { return v0[0];}
	float getLen() const { return v2[2];}
};

struct InstanceData : public MatrixData
{
	//	QMatrix4x4 model;
	float val;
	//	int zer1;
	//	int zer2;
	InstanceData(QMatrix4x4 matr, int val) : MatrixData(matr)
	{
		this->val = val;
	}
};
#endif // !GLINSTANSE_H
