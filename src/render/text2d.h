#ifndef TEXT2D_H
#define TEXT2D_H




#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>

#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "../types/types.h"
#include "../types/glinstanse.h"


//struct charVAO
//{
//	int _char;
//	QVector<QMatrix4x4> boundydata;
//	QOpenGLBuffer arrBuf, modelsBuf;
//	QOpenGLShaderProgram mshader;
//	void addPosition(boundy &&bb)
//	{
//		QMatrix4x4 matr;
//		matr.setToIdentity();
//		matr.translate(bb.x + bb.wid() / 2, bb.endZ + bb.zei() / 2 + 1, bb.y + bb.hei() / 2);
//		//text bb.x, bb.endZ, bb.y
//		matr.scale(bb.wid(), 1, bb.hei());

//		boundydata.append(matr);
//	}
//	QOpenGLTexture *texture = new QOpenGLTexture(QImage(":/text.bmp").mirrored());

//	charVAO(QImage& img)
//	{

//		//    // Set nearest filtering mode for texture minification
//		texture->setMinificationFilter(QOpenGLTexture::Nearest);

//		//    // Set bilinear filtering mode for texture magnification
//		texture->setMagnificationFilter(QOpenGLTexture::Linear);
//		//    // Wrap texture coordinates by repeating
//		//	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
//		texture->setWrapMode(QOpenGLTexture::Repeat);


//		int xSize= texture->width() / 8;
//		int ySize= texture->height() / 8;
//	}
//};

class Text2d :protected QOpenGLFunctions
{
public:
	QVector<MatrixData> boundydata;
	QOpenGLTexture *texture;
	QOpenGLVertexArrayObject vao;
	int factor = 10;
	QOpenGLBuffer arrBuf, modelsBuf;

	QOpenGLShaderProgram mshader;
	QOpenGLExtraFunctions *f;

	size_t boundySize = 0;
	int xSize;
	int ySize;
	Text2d();

	void updateBuffer();
	void initGL();
	void renderGL(QMatrix4x4 view, QMatrix4x4 projection);
	void initSpotModel();

	void addText(boundy &bb);
};


















//#include "D:\Install\Qt\5.15.2\Src\qtbase\src\3rdparty\freetype\include\ft2build.h"
//#include FT_FREETYPE_H
//#include <QDebug>


//void initFonts()
//{
//	FT_Library ft;
//	if (FT_Init_FreeType(&ft))
//		qDebug() << "ERROR::FREETYPE: Could not init FreeType Library";
//	FT_Face face;
//	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
//		qDebug() << "ERROR::FREETYPE: Failed to load font";

//	FT_Set_Pixel_Sizes(face, 0, 48);

//	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
//		qDebug() << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

//	face->glyph->bitmap;
//}

//#include <QString>
////#include <
//class CharCoords
//{
//	int x, y, width, height;
//};

//int getWidth(QString text) {
//	int totalwidth = 0;
//	for (int i = 0; i < text.length(); i++) {
//		CharCoords coo = m_charMap.get(text.charAt(i));
//		totalwidth += coo.width;
//	}
//	return totalwidth;
//}

//int getHeight()
//{
//	return m_fontMetrics.getHeight();
//}

//static void drawString(Matrix4f PVMmat, String text, Color color, HAlign halign, VAlign valign)
//{
//	Vector2f offset = new Vector2f();

//	// Font alignment
//	if(halign == HAlign.Center){
//		offset.x = -(int) (0.5f * getWidth(text));
//	}else if(halign == HAlign.Right){
//		offset.x = -getWidth(text);
//	}
//	if(valign == VAlign.Middle){
//		offset.y = -(int) (0.5f * getHeight());
//	}else if(valign == VAlign.Top){
//		offset.y = -getHeight();
//	}

//	m_shader.bind();
//	m_shader.setAttributeBuffer("vertex", m_vertexBuffer, 2);
//	m_shader.setUniformMatrix("PVMmat", PVMmat);
//	m_shader.setUniformVector("color", color);
//	m_shader.setUniformScalar("texSize", (float)m_textureSize);
//	m_shader.setTexture("tex", m_fontTexture, GL11.GL_TEXTURE_2D);
//	GL15.glBindBuffer(GL15.GL_ELEMENT_ARRAY_BUFFER, m_model.getIndexBuffer());
//	for(int i = 0; i < text.length(); ++i) {
//		CharCoords coo = m_charMap.get(text.charAt(i));
//		m_shader.setUniformVector("charCoords", new Vector4f(coo.x, coo.y, coo.width, coo.height));
//		m_shader.setUniformVector("offset", offset);
//		GL11.glDrawElements(GL11.GL_TRIANGLES, m_indexCount, GL11.GL_UNSIGNED_INT, 0);
//		offset.x += coo.width;
//	}
//	GL15.glBindBuffer(GL15.GL_ELEMENT_ARRAY_BUFFER, 0);
//	m_shader.unbind();
//}




#endif // TEXT2D_H

