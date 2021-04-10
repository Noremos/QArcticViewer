#include "backend.h"

#include <QImage>

Backend::Backend(QObject *parent) : QObject(parent)
{
	//	reader = new TiffReader();
	//	reader->open(L"D:\\Учеба\\БАР\\Москва\\50_59_1_2_2m_v3.0\\50_59_1_2_2m_v3.0_reg_dem.tif");
	//	reader->getRowData(20020);
	qDebug() << proj.searchSetts.diamert.end;
}

//using namespace Qt3DRender;

//QEntity *Backend::getSpotZone()
//{
////	QQuickItem *item = root->findChild<QQuickItem *>("surf");
////	item->
////	QEntity* item2 = item->findChild<QEntity *>("spotZones");
//	return spotZone;
//}

//void Backend::setStopItem(QEntity *area)
//{
////	if (block)return;

//	spotZone = area;
//}

void Backend::setFactorSpinBox(QQuickItem *spinBox)
{
	if (block)return;

}

int Backend::getHei()
{
	if (block)return 0;

	return reader->height();
}

void Backend::setSearchingsettings(/*float coof, int minD, int maxD, float minHei, float maxHei, float bottomLineProc*/)
{
	if (block)return;

//	proj.searchSetts.coof = coof;
//	proj.searchSetts.diamert = {minD, maxD};
//	proj.searchSetts.height = {minHei, maxHei};
//	proj.searchSetts.bottomProc = bottomLineProc;
	qDebug() << proj.searchSetts.coof;
	qDebug() << proj.searchSetts.diamert.start;
	qDebug() << proj.searchSetts.diamert.end;
	qDebug() << proj.searchSetts.height.start;
	qDebug() << proj.searchSetts.height.end;
	qDebug() << proj.searchSetts.bottomProc;
}

void Backend::saveSettings()
{
	proj.saveProject(projectPath+ "proj.qwr");
}

void Backend::loadSettings()
{
	proj.loadProject(getPath(BackPath::project));
	qDebug() << proj.searchSetts.height.start;
	qDebug() << proj.searchSetts.heightMin();
	QMetaObject::invokeMethod(root->findChild<QObject*>("sideMenu"), "setBoxSetts");
	proj.notifySettings();
//	m_openrender->init();
}

//QEntity *Backend::getMarkerZone()
//{
//	QQuickItem *item = root->findChild<QQuickItem *>("surf");
//	QEntity* item2 = item->findChild<QEntity *>("markerZones");
//	return item2;
//}

void Backend::test(QString path)
{
	if (block)return;
}

QString Backend::loadImage(QString path, int step, int type)
{

}
