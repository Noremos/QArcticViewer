#include "backend.h"

#include <QImage>
#include <imagesearcher.h>
#include <Qt3DRender/QMesh>
#include <QEffect>
#include <QMaterial>
#include <QParameter>
#include <QTechnique>
#include <QGraphicsApiFilter>
#include <QPhongMaterial>

using namespace cv;
using namespace Qt3DCore;
Backend::Backend(QObject *parent) : QObject(parent)
{
//	reader = new TiffReader();
//	reader->open(L"D:\\Учеба\\БАР\\Москва\\50_59_1_2_2m_v3.0\\50_59_1_2_2m_v3.0_reg_dem.tif");
	//	reader->getRowData(20020);
}

Backend::~Backend()
{
	if (reader!= nullptr)
	{
		reader->close();
		delete reader;
	}
}
using namespace Qt3DRender;

QEntity *Backend::getSpotZone()
{
//	QQuickItem *item = root->findChild<QQuickItem *>("surf");
//	item->
//	QEntity* item2 = item->findChild<QEntity *>("spotZones");
	return spotZone;
}

void Backend::setStopItem(QEntity *area)
{
	spotZone = area;

}

int Backend::getHei()
{
	return reader->height();
}

QEntity *Backend::getMarkerZone()
{
	QQuickItem *item = root->findChild<QQuickItem *>("surf");
	QEntity* item2 = item->findChild<QEntity *>("markerZones");
	return item2;
}

QMaterial *createSpotMaterial(QEntity *prnt)
{
	QMaterial *mater = new QMaterial(prnt);
	QTechnique *tech = new QTechnique(prnt);
	QEffect *eff = new QEffect(mater);
	QRenderPass *pass = new QRenderPass(eff);
	QShaderProgram *shad = new QShaderProgram(pass);

	QFilterKey *key = new QFilterKey(eff);
	key->setName("renderingStyle");
	key->setValue("forward");

	shad->setVertexShaderCode(QShaderProgram::loadSource(QUrl("qrc:/shaders/spotZone.vert")));
	qDebug() << shad->log();
	qDebug() << shad->status();

	shad->setFragmentShaderCode(QShaderProgram::loadSource(QUrl("qrc:/shaders/spotZone.frag")));
	qDebug() << shad->log();
	qDebug() << shad->status();
	pass->setShaderProgram(shad);

	tech->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
	tech->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);
	tech->graphicsApiFilter()->setMajorVersion(3);
	tech->graphicsApiFilter()->setMinorVersion(1);
	tech->addFilterKey(key);
	tech->addRenderPass(pass);

//	mater->parameters().append(new QParameter("color", QVector4D(1.0, 1.0, 0, 1.0)));
	mater->setEffect(eff);

	eff->addTechnique(tech);
	eff->addParameter(new QParameter("color", QVector4D(1.0, 1.0, 0, 1.0)));
	return mater;
}

void Backend::findZones(int step)
{
//	QMesh *mesh1 = spotZone->findChild<QMesh *>("mesh");
//	QMaterial *mater1 = spotZone->findChild<QMaterial *>("material");

//	Qt3DCore::QEntity *entry = new Qt3DCore::QEntity(spotZone);
//	entry->setObjectName("MEW t");
//	entry->setEnabled(true);
//	Qt3DCore::QTransform *trans1 = new Qt3DCore::QTransform(entry);
//	trans1->setObjectName("local trans");
//	trans1->setTranslation(QVector3D(-50, 0, 0));
//	trans1->setScale3D(QVector3D(1000, 1000, 1000));
//	entry->addComponent(mesh1);
//	entry->addComponent(trans1);
//	entry->addComponent(mater1);
//	return;
float xScale = 10;
float yScale = 10;

	reader = new TiffReader();
	QString st = "D:/Учеба/БАР/Москва/50_59_1_2_2m_v3.0/50_59_1_2_2m_v3.0_reg_dem.tif";
	reader->open(st.toStdWString().c_str());
	ImageSearcher imgsrch(dynamic_cast<TiffReader *>(reader));

	vector<boundy> objects;
	imgsrch.findZones(objects);

//	QEntity *stopZones = getSpotZone();

	QMesh *mesh = spotZone->findChild<QMesh *>("mesh");
		// new QMesh(stopZones);
//	mesh->setSource(QUrl("qrc:/spot.obj"));
//	mesh->setMeshName("spot");
	QMaterial *mater = spotZone->findChild<QMaterial*>("material");

	//new Qt3DExtras::QPhongMaterial(stopZones);
//	createSpotMaterial(stopZones);
//	mater->
	step = 100;
	for (int i = 0, total = objects.size(); i < total; ++i)
	{
		boundy &bb = objects[i];

		Qt3DCore::QEntity *entry = new Qt3DCore::QEntity(spotZone);
		entry->setObjectName("MEW " + QString::number(i));
		entry->setEnabled(true);
		Qt3DCore::QTransform *trans = new Qt3DCore::QTransform(entry);
		trans->setObjectName("size trans");

		float mider = xScale;
		float x = bb.x / mider;
		float y = bb.y / mider;
		float wid = bb.wid() / mider;
		float hei = bb.hei() / mider;

		trans->setScale3D(QVector3D(wid, 10, hei));
		trans->setTranslation(QVector3D(x + wid/2, bb.localMax/ mider, y + hei/2));
//		trans->setTranslation(QVector3D(-50, 0, 0));
//		trans->setScale3D(QVector3D(1000, 1000, 1000));
		//		trans->setTranslation(QVector3D(bb.x, bb.y, 0));
		//		trans->setScale3D(QVector3D(bb.wid(), bb.hei(), 1000));
		entry->addComponent(mesh);
		entry->addComponent(trans);
		entry->addComponent(mater);
	}
}

void Backend::test(QString path)
{
}

//cv::Mat Backend::imgread(QString path)
//{
//	QFile file(path);
//	if (!file.open(QIODevice::ReadOnly))
//		return cv::Mat();

//	QByteArray blob = file.readAll();
//	std::vector<unsigned char> data(blob.begin(), blob.end());
////	return imdecode(cv::Mat(data), 1);
//	return cv::Mat();
//}

QString Backend::loadImage(QString path, int step, int type)
{
	if (reader)
	{
		reader->close();
		delete reader;
		reader = nullptr;
	}

	int imgtype = 0;
	switch (imgtype)
	{
	default:
		reader = new TiffReader();
		break;
	}

	reader->open(path.toStdWString().c_str());
	if (!reader->ready)
		return "";
//	int hei = 500;
	Obj3d object(reader);
	object.setMode((::ProcessMode) type);
	object.setStep(step);
	object.write("D:\\2.obj");

	return "D:/2.obj";
}
