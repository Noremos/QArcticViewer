#ifndef BACKEND_H
#define BACKEND_H

#include "obj3d.h"

#include <QObject>
#include <QMetaObject>
#include <QQuickItem>

//#include <opencv2/opencv.hpp>
namespace cv
{
class Mat
{};
}

class Backend: public QObject
{
	Q_OBJECT

	ImageReader* reader = nullptr;

public:
	explicit Backend(QObject *parent = nullptr);

	QObject *root = nullptr;
	Q_INVOKABLE void test(QString path);
	cv::Mat imgread(QString path);
	Q_INVOKABLE void loadImage(QString path);
signals:

};

#endif // BACKEND_H
