#ifndef SKYBOXGUI_H
#define SKYBOXGUI_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QWheelEvent>

#include <QVector2D>
#include <QVector3D>

#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class SkyBoxGUI : public QObject, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	explicit SkyBoxGUI(const QString& frontImagePath,
								 const QString& backImagePath,
								 const QString& topImagePath,
								 const QString& bottomImagePath,
								 const QString& leftImagePath,
								 const QString& rightImagePath);

	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL(QMatrix4x4 view);

	void mouseReleaseEvent();
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent *event);

	void mousePressEvent(QMouseEvent*);
protected:
	void timerEvent(QTimerEvent *) override;

private:
	void loadImages();

	QOpenGLShaderProgram mProgram;
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
