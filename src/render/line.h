#ifndef LINE_H
#define LINE_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <src/types/glinstanse.h>


class Line : glinstanse
{
	QOpenGLShaderProgram shaderProgram;
	QOpenGLBuffer VBO;
	QOpenGLVertexArrayObject VAO;

	std::vector<float> vertices;
	QVector3D startPoint;
	QVector3D endPoint;

	QVector3D lineColor;
public:
	Line()	:  VBO(QOpenGLBuffer::Type::VertexBuffer)
	{}

	void setColor(QVector3D color);

	~Line();

	// glinstanse interface
public:
	void initGL() override;
	void renderGL(QMatrix4x4 &view, QMatrix4x4 &projection) override;
	void setLine(QVector3D start, QVector3D end);
};

#endif // LINE_H
