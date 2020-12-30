#ifndef BACKEND_H
#define BACKEND_H

#include "instansemodel.h"
#include "obj3d.h"
#include "project.h"

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
	QString projectPath = "D:\\";
	explicit Backend(QObject *parent = nullptr);
	bool block = true;
//	cv::Mat imgread(QString path);

	~Backend();
	ProjectParametrs proj;

	Qt3DCore::QEntity* spotZone;
	Q_INVOKABLE void findByParams();
	Q_INVOKABLE void test(QString path);
    Q_INVOKABLE QString loadImage(QString path, int step, int type, int startRow, int lastRow);

	Qt3DCore::QEntity *getMarkerZone();
	Qt3DCore::QEntity *getSpotZone();

	Q_INVOKABLE void setStopItem(Qt3DCore::QEntity *area);
	Q_INVOKABLE void setFactorSpinBox(QQuickItem *spinBox);

	Q_INVOKABLE int getHei();
	Q_INVOKABLE void setSearchingsettings(/*float coof, int minD, int maxD, float minHei, float maxHei, float bottomLineProc*/);
	Q_INVOKABLE void saveSettings();

    Q_PROPERTY(bool block READ getBlock)

    void loadSettings();
	bool getBlock()
	{
		return block;
	}
    Q_INVOKABLE void processHiemap(int start, int end);

	//	Q_PROPERTY(ProjectParametrs *projParams READ getProjSetts)
//	ProjectParametrs *getProjSetts() { return &proj; }
	bool checkBounty(boundy &bb);
signals:

};

#endif // BACKEND_H
