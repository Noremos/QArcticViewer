#include "line.h"



void Line::setLine(QVector3D start, QVector3D end)
{
	startPoint = start;
	endPoint = end;

	vertices = {
		start.x(), start.y(), start.z(),
		end.x(), end.y(), end.z(),
	};

	VAO.bind();
	VBO.bind();
	VBO.allocate(vertices.data(), sizeof(vertices));

	int offloc = shaderProgram.attributeLocation("a_position");
	shaderProgram.setAttributeBuffer(offloc, GL_FLOAT, 0, 3 * sizeof(float));
	shaderProgram.enableAttributeArray(offloc);

	VAO.release();
	VBO.release();
}

void Line::setColor(QVector3D color)
{
	lineColor = color;
}

void Line::renderGL(QMatrix4x4 &view, QMatrix4x4 &projection)
{
	VAO.bind();
	shaderProgram.bind();

	QMatrix4x4 model;
	model.setToIdentity();
	model.translate(0, 0, 0);
	model.scale(1, 1, 1);

	shaderProgram.setUniformValue("MVP", projection * view * model);
	shaderProgram.setUniformValue("color", lineColor);

	glDrawArrays(GL_LINES, 0, 2);
	VAO.release();

	shaderProgram.release();
}

Line::~Line()
{
	VAO.destroy();
	VBO.destroy();
}

void Line::initGL()
{
	initializeOpenGLFunctions();

	const char *vertexShaderSource = "#version 330 core\n"
									 "layout (location = 0) in vec3 a_position;\n"
									 "uniform mat4 MVP;\n"
									 "void main(void)\n"
									 "{\n"
									 "   gl_Position = MVP * vec4(a_position, 1.0);\n"
									 "}\0";
	const char *fragmentShaderSource = "#version 330 core\n"
									   "out vec4 FragColor;\n"
									   "uniform vec3 color;\n"
									   "void main(void)\n"
									   "{\n"
									   "   FragColor = vec4(color, 1.0f);\n"
									   "}\n\0";

	// vertex shader
	initShaderSourceCode(shaderProgram, vertexShaderSource, fragmentShaderSource);

	VAO.create();
	VBO.create();
	VBO.setUsagePattern(QOpenGLBuffer::DynamicDraw);

	lineColor = QVector3D(1, 0, 1);
	setLine({0, 0, 0}, {100, 100, 100});
}


