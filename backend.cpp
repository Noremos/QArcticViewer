#include "backend.h"

#include <QImage>
#include <Qt3DRender/QMesh>
#include <QEffect>
#include <QMaterial>
#include <QParameter>
#include <QTechnique>
#include <QGraphicsApiFilter>
#include <QPhongMaterial>
#include <QExtrudedTextMesh>


#include <Qt3DExtras/Qt3DExtras>

using namespace cv;
using namespace Qt3DCore;
Backend::Backend(QObject *parent) : QObject(parent)
{
	//	reader = new TiffReader();
	//	reader->open(L"D:\\Учеба\\БАР\\Москва\\50_59_1_2_2m_v3.0\\50_59_1_2_2m_v3.0_reg_dem.tif");
	//	reader->getRowData(20020);
	setts.bottomProc = 0.1f;
	setts.coof = 1.7f;
	setts.diamert = TRange<int>(10, 300);
	setts.height = TRange<float>(2, 6);
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

void Backend::setFactorSpinBox(QQuickItem *spinBox)
{

}

int Backend::getHei()
{
	return reader->height();
}

void Backend::setSearchingsettings(float coof, int minD, int maxD, float minHei, float maxHei, float bottomLineProc)
{
	setts.coof = coof;
	setts.diamert = {minD, maxD};
	setts.height = {minHei, maxHei};
	setts.bottomProc = bottomLineProc;
	qDebug() << coof;
	qDebug() << minD;
	qDebug() << maxD;
	qDebug() << minHei;
	qDebug() << maxHei;
	qDebug() << bottomLineProc;
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
using namespace Qt3DExtras;

void Backend::findZones(QString path, int start, int len)
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

	qDebug() << path;
	reader = new TiffReader();
//	QString st = "D:/Учеба/БАР/Москва/50_59_1_2_2m_v3.0/50_59_1_2_2m_v3.0_reg_dem.tif";
	QString st = "D:/Учеба/БАР/bugr.tif";
	reader->open(path.toStdWString().c_str());
	ImageSearcher imgsrch(dynamic_cast<TiffReader *>(reader));
	imgsrch.settings = setts;

	QMesh *mesh = spotZone->findChild<QMesh *>("mesh");
	QMaterial *mater = spotZone->findChild<QMaterial *>("material");


	auto *textMaterial = spotZone->findChild<QPhongMaterial *>("phong");
	textMaterial->setDiffuse(QColor(0, 255, 255));

	for (int ind = start; ind < start+ len; ++ind)
	{

	vector<boundy> objects;
	imgsrch.findZones(objects, ind, 1);


	Qt3DCore::QEntity *tileentry = spotZone->findChild<Qt3DCore::QEntity *>("tile" + QString::number(ind));
	if (tileentry == nullptr)
	{
		tileentry = new Qt3DCore::QEntity(spotZone);
		tileentry->setObjectName("tile" + QString::number(ind));
		tileentry->setEnabled(true);
	}
	else
	{
		for (auto& v : tileentry->children())
		{
			v->setParent(nullptr);
			v->deleteLater();
		}
	}

	qDebug() << tileentry->childNodes().size();
	qDebug() << tileentry->children().size();

	for (int i = 0, total = objects.size(); i < total; ++i)
	{
		boundy &bb = objects[i];
		Qt3DCore::QEntity *entry = new Qt3DCore::QEntity(tileentry);

		entry->setObjectName("MEW " + QString::number(ind)+ " "+ QString::number(i));
		entry->setEnabled(true);
		Qt3DCore::QTransform *trans = new Qt3DCore::QTransform(entry);
//		trans->setObjectName("size trans");

		float mider = xScale;
		bb.divStep(mider);

		trans->setScale3D(QVector3D(bb.wid(), 1, bb.hei()));
		trans->setTranslation(QVector3D(bb.x + bb.wid() / 2, bb.z, bb.y + bb.hei() / 2));

		entry->addComponent(mesh);
		entry->addComponent(trans);
		entry->addComponent(mater);

		{
			QFont font(QFontDatabase().families()[0], 32, -1, false);

			Qt3DCore::QEntity *textEnty = new Qt3DCore::QEntity(tileentry);
			QExtrudedTextMesh *text = new QExtrudedTextMesh(textEnty);
			Qt3DCore::QTransform *testtrans = new Qt3DCore::QTransform(textEnty);

			testtrans->setTranslation(QVector3D(bb.x, bb.endZ, bb.y));
			testtrans->setRotationX(-90);
			testtrans->setRotationY(180);
//			testtrans->setScale(0.5);
			testtrans->setScale3D(QVector3D(0.5,0.5,0.1));

			QString nam;
//			nam.append("Dimentions:\n");
			nam.append(" L: "); nam.append(QString::number(bb.sizeHei));
			nam.append(" W: "); nam.append(QString::number(bb.sizeWid));
			nam.append(" H: ");	nam.append(QString::number(bb.sizeTop));

			text->setText(nam);
//			text->setFont(font);

			textEnty->addComponent(text);
			textEnty->addComponent(testtrans);
			textEnty->addComponent(textMaterial);
		}


	} // for ind
	objects.clear();

	}
}

void Backend::test(QString path)
{}

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

QString Backend::loadImage(QString path, int step, int type, int startRow, int lastRow)
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
	object.write("D:\\2.obj", startRow, lastRow);

	return "D:/2.obj";
}
