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
	proj.searchSetts.bottomProc = 0.1f;
	proj.searchSetts.coof = 1.7f;
	proj.searchSetts.diamert = TRange<int>(10, 400);
	proj.searchSetts.height = TRange<float>(2, 6);
	qDebug() << proj.searchSetts.diamert.end;

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
//	if (block)return;

	spotZone = area;
}

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
	proj.saveProject("D:\\proj.qwr");
}

void Backend::loadSettings()
{
	proj.loadProject("D:\\proj.qwr");
	qDebug() << proj.searchSetts.height.start;
	qDebug() << proj.searchSetts.heightMin();
	QMetaObject::invokeMethod(root->findChild<QObject*>("sideMenu"), "setBoxSetts");
	proj.notifySettings();
}

void Backend::processHiemap()
{
	if (block)
		return;

	reader = new TiffReader();
	QDir dir(projectPath);
	QString bpath = dir.path() + "bds.lst";
	qDebug() << bpath;
	QFile out(bpath);
	if (out.exists())
		out.remove();

	if (!out.open(QFile::WriteOnly | QFile::Truncate))
		return;

	QTextStream stream(&out);

	QString sw;
	reader->open(proj.heimapPath.toStdWString().c_str());
	ImageSearcher imgsrch(dynamic_cast<TiffReader *>(reader));

	for (int ind = 0; ind < imgsrch.getMaxTiles(); ++ind)
	{
		vector<boundy> objects;
		imgsrch.findZones(objects, ind, 1);

		sw = "t " + QString::number(ind) + nl;
		for (int io = 0, totobjs = objects.size(); io < totobjs; ++io)
		{
			sw += objects[io].getStr() + nl;
		}
		stream << sw;
		qDebug() << ind << "/" << imgsrch.getMaxTiles() << ": " << objects.size();
	}

	out.close();
	sw.clear();
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

void createTextEnty(QEntity* parent, boundy& bb, int wid, int hei, float zei, QMaterial* textMaterial)
{
	QFont font(QFontDatabase().families()[0], 32, -1, false);

	Qt3DCore::QEntity *textEnty = new Qt3DCore::QEntity(parent);
	QExtrudedTextMesh *text = new QExtrudedTextMesh(textEnty);
	Qt3DCore::QTransform *testtrans = new Qt3DCore::QTransform(textEnty);

	testtrans->setTranslation(QVector3D(bb.x, bb.endZ, bb.y));
	testtrans->setRotationX(-90);
	testtrans->setRotationY(180);
	//			testtrans->setScale(0.5);
	testtrans->setScale3D(QVector3D(0.5,0.5,0.1));

	QString nam;
	//			nam.append("Dimentions:\n");
	nam.append(" L: "); nam.append(QString::number(hei));
	nam.append(" W: "); nam.append(QString::number(wid));
	nam.append(" H: ");	nam.append(QString::number(zei));

	text->setText(nam);
	//			text->setFont(font);

	textEnty->addComponent(text);
	textEnty->addComponent(testtrans);
	textEnty->addComponent(textMaterial);
}


bool Backend::checkBounty(boundy& bb)
{
	float coof;
	int dmin, dmax;
	if (bb.wid() > bb.hei())
	{
		dmin = bb.hei();
		dmax = bb.wid();
	}
	else
	{
		dmin = bb.wid();
		dmax = bb.hei();
	}

	coof = float(dmax) / dmin;

	// sootn
	if (coof > proj.searchSetts.coof)
		return false;


	// diametr
	if (dmin * resol < proj.searchSetts.diamert.start || dmax * resol > proj.searchSetts.diamert.end)
		return false;

	if (bb.zei() < proj.searchSetts.height.start)
		return false;

	if (bb.zei() > proj.searchSetts.height.end)
		return false;

	qDebug() << proj.searchSetts.diamert.start << proj.searchSetts.diamert.end;
	qDebug() << proj.searchSetts.height.start << proj.searchSetts.height.end;
	return true;
}

void Backend::findByParams()
{
	if (block)return;

	float xScale = 10;
//	float yScale = 10;

	qDebug() << proj.heimapPath;
	reader = new TiffReader();

	reader->open(proj.heimapPath.toStdWString().c_str());
	ImageSearcher imgsrch(dynamic_cast<TiffReader *>(reader));

	QMesh *mesh = spotZone->findChild<QMesh *>("mesh");
	QMaterial *mater = spotZone->findChild<QMaterial *>("material");


	auto *textMaterial = spotZone->findChild<QPhongMaterial *>("phong");
	textMaterial->setDiffuse(QColor(0, 255, 255));

	QString bpath = projectPath + "bds.lst";
	QFile out(bpath);

	if (!out.exists())
	{
		qDebug() << "FILE NOT EXISTS";
		return;
	}
	if (!out.open(QFile::ReadOnly | QFile::Text))
	{
		qDebug() << "FILE NOT OPENED";
		return;
	}
	QTextStream stream(&out);
	QString line;
	Qt3DCore::QEntity *tileentry;
	int ind, k = 0, l = 0;
	while (stream.readLineInto(&line))
	{
		// tile
		if (line.startsWith("t"))
		{
			ind = line.split(" ")[1].toInt();
			tileentry = spotZone->findChild<Qt3DCore::QEntity *>("tile" + QString::number(ind));
			if (tileentry == nullptr)
			{
				tileentry = new Qt3DCore::QEntity(spotZone);
				tileentry->setObjectName("tile" + QString::number(ind));
				tileentry->setEnabled(true);
			}
			else
			{
				for (auto &v : tileentry->children())
				{
					v->setParent(nullptr);
					v->deleteLater();
				}
			}
			qDebug() << tileentry->childNodes().size() << l;
			//			qDebug() << tileentry->children().size();

			l = 0;
			continue;
		}

		// data
		QStringList list = line.split(" ");
		boundy bb(list[0].toInt(), list[1].toInt(), list[2].toFloat(), list[3].toInt(), list[4].toInt(), list[5].toFloat());

		if (!checkBounty(bb))
			continue;
		++l;

		int wid = bb.wid();
		int hei = bb.hei();
		float zei = bb.zei();
		qDebug() << bb.wid() << bb.hei() << bb.zei();


		Qt3DCore::QEntity *entry = new Qt3DCore::QEntity(tileentry);
		entry->setObjectName("MEW " + QString::number(ind)+ " "+ QString::number(k));
		entry->setEnabled(true);

		Qt3DCore::QTransform *trans = new Qt3DCore::QTransform(entry);


		bb.divStep(xScale);

		trans->setScale3D(QVector3D(bb.wid(), 1, bb.hei()));
		trans->setTranslation(QVector3D(bb.x + bb.wid() / 2, bb.endZ, bb.y + bb.hei() / 2));

		entry->addComponent(mesh);
		entry->addComponent(trans);
		entry->addComponent(mater);

		createTextEnty(tileentry, bb, wid, hei, zei, textMaterial);

		k++;
	} // for ind
	saveSettings();
}

void Backend::test(QString path)
{
	if (block)return;
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

QString Backend::loadImage(QString path, int step, int type, int startRow, int lastRow)
{
	if (block)return "";

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

	this->proj.imgMinVal = reader->min;
	this->proj.imgMaxVal = reader->max;
	this->proj.modelPath = "D:\\2.obj";
	this->proj.heimapPath = path;
	this->proj.step = step;

	this->saveSettings();

	return "D:/2.obj";
}
