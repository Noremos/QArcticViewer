#ifndef SKYBOXGUI_H
#define SKYBOXGUI_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QWheelEvent>

#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <QOpenGLExtraFunctions>

class SkyBoxGUI : protected QOpenGLFunctions
{
public:
	SkyBoxGUI(QWidget *parent = Q_NULLPTR);

	void initializeGL() ;
	void resizeGL(int w, int h) ;
	void paintGL(QMatrix4x4 view, QMatrix4x4 projection);

	void mousePressEvent(QMouseEvent *e) ;
	void mouseReleaseEvent(QMouseEvent *) ;
	void mouseMoveEvent(QMouseEvent *event) ;
	void wheelEvent(QWheelEvent *event) ;

	void timerEvent(QTimerEvent *) ;

private:
	QOpenGLExtraFunctions *f;
	void loadImages();

	QOpenGLShaderProgram mProgram;
	QOpenGLVertexArrayObject vao;
	QOpenGLTexture mTexture;
	QOpenGLBuffer mVertexBuf;
	QBasicTimer mTimer;

	struct
	{
		float verticalAngle;
		float aspectRatio;
		float nearPlane;
		float farPlane;
	} mPerspective;

	struct
	{
		QVector3D eye;
		QVector3D center;
		QVector3D up;
	} mLookAt;

	QMatrix4x4 mModelMat, mViewMat, mProjectionMat;

	QVector3D mRotationAxis;
	QQuaternion mRotation;

	QVector2D mMousePressPosition;
	float mSpeed;

	QString mFrontImagePath;
	QString mBackImagePath;
	QString mTopImagePath;
	QString mBottomImagePath;
	QString mLeftImagePath;
	QString mRightImagePath;
};

#endif // SKYBOXGUI_H
