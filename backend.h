#ifndef BACKEND_H
#define BACKEND_H

#include "obj3d.h"

#include <QObject>
#include <QMetaObject>
#include <QQuickItem>

#include <QEntity>
#include <QTransform>
#include <Qt3DCore>

//#include <opencv2/opencv.hpp>
//namespace cv
//{
//class Mat
//{};
//}

class Backend: public QObject
{
	Q_OBJECT

	ImageReader* reader = nullptr;

public:
	QObject *root = nullptr;

	explicit Backend(QObject *parent = nullptr);

//	cv::Mat imgread(QString path);

	~Backend();


	Qt3DCore::QEntity* spotZone;
	Q_INVOKABLE void findZones(int step, int str);
	Q_INVOKABLE void test(QString path);
	Q_INVOKABLE QString loadImage(QString path, int step, int type);

	Qt3DCore::QEntity *getMarkerZone();
	Qt3DCore::QEntity *getSpotZone();

	Q_INVOKABLE void setStopItem(Qt3DCore::QEntity *area);

	Q_INVOKABLE int getHei();

signals:

};

#endif // BACKEND_H
