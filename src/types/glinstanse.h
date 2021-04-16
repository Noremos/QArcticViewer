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
        initializeOpenGLFunctions();
//        initGL();
    }
    virtual void initGL() = 0;
    virtual void renderGL() = 0;
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

#endif // !GLINSTANSE_H
