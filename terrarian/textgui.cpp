#include "textgui.h"

textGui::textGui()
{

}
//void GLBox::renderText(D3DVECTOR &textPosWorld, QString text)
//{
//	int width = this->width();
//	int height = this->height();

//	GLdouble model[4][4], proj[4][4];
//	GLint view[4];
//	glGetDoublev(GL_MODELVIEW_MATRIX, &model[0][0]);
//	glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
//	glGetIntegerv(GL_VIEWPORT, &view[0]);
//	GLdouble textPosX = 0, textPosY = 0, textPosZ = 0;

//	project(textPosWorld.x, textPosWorld.y, textPosWorld.z,
//				&model[0][0], &proj[0][0], &view[0],
//			&textPosX, &textPosY, &textPosZ);

//	textPosY = height - textPosY; // y is inverted

//	QPainter painter(this);
//	painter.setPen(Qt::yellow);
//	painter.setFont(QFont("Helvetica", 8));
//	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
//	painter.rotate(0.01);
//	painter.drawText(textPosX, textPosY, text); // z = pointT4.z + distOverOp / 4
//	painter.end();
//}


//inline GLint GLBox::project(GLdouble objx, GLdouble objy, GLdouble objz,
//							const GLdouble model[16], const GLdouble proj[16],
//							const GLint viewport[4],
//							GLdouble * winx, GLdouble * winy, GLdouble * winz)
//{
//	GLdouble in[4], out[4];

//	in[0] = objx;
//	in[1] = objy;
//	in[2] = objz;
//	in[3] = 1.0;
//	transformPoint(out, model, in);
//	transformPoint(in, proj, out);

//	if (in[3] == 0.0)
//		return GL_FALSE;

//	in[0] /= in[3];
//	in[1] /= in[3];
//	in[2] /= in[3];

//	*winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
//	*winy = viewport[1] + (1 + in[1]) * viewport[3] / 2;

//	*winz = (1 + in[2]) / 2;
//	return GL_TRUE;
//}


//inline void GLBox::transformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4])
//{
//#define M(row,col)  m[col*4+row]
//	out[0] =
//		M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
//	out[1] =
//		M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
//	out[2] =
//		M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
//	out[3] =
//		M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
//#undef M
//}
