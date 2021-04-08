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
	proj.saveProject(projectPath+ "proj.qwr");
}

void Backend::loadSettings()
{
	proj.loadProject(projectPath+ "proj.qwr");
	qDebug() << proj.searchSetts.height.start;
	qDebug() << proj.searchSetts.heightMin();
	QMetaObject::invokeMethod(root->findChild<QObject*>("sideMenu"), "setBoxSetts");
	proj.notifySettings();
}

// void check()
// {
// 	  if (sw.length()>10000)
//             {
//                 boundSteam << sw;
//                 sw.clear();
//             }


//             auto* tem = ret->at(io);
//             tem->getStr(linesTemp);
//             linesTemp+=nl;

//             if (linesTemp.length()>10000)
//             {
//                 barsStream << linesTemp;
//                 linesTemp.clear();
//             }
//             delete tem;
// }

void Backend::processHiemap(int start, int end)
{
	if (block)
		return;

	reader = new TiffReader();


	QString bpath = projectPath + "bds.lst";
	qDebug() << bpath;
	QFile out(bpath);
	if (out.exists())out.remove();
	if (!out.open(QFile::WriteOnly | QFile::Truncate))return;
	QTextStream boundSteam(&out);

    QString sw;
	reader->open(proj.heimapPath.toStdWString().c_str());
	ImageSearcher imgsrch(dynamic_cast<TiffReader *>(reader));

    start = min(start, imgsrch.getMaxTiles()-1);
    end = min(end, imgsrch.getMaxTiles()-1);
    qDebug() << start << end;
    //1000/18
    for (int ind = start; ind <= end; ++ind)
	{
		vector<boundy> objects;
        imgsrch.findZones(objects, ind, 1);

		sw = "t " + QString::number(ind) + nl;
        for (size_t io = 0, totobjs = objects.size(); io < totobjs; ++io)
		{
			sw += objects[io].getStr() + nl;
		}

		qDebug() << ind << "/" << imgsrch.getMaxTiles() << ": " << objects.size();

		out.close();
        sw.clear();
	}
}

QEntity *Backend::getMarkerZone()
{
	QQuickItem *item = root->findChild<QQuickItem *>("surf");
	QEntity* item2 = item->findChild<QEntity *>("markerZones");
	return item2;
}

using namespace Qt3DExtras;

bool Backend::checkBounty(boundy& bb)
{
	float coof;
    uint dmin, dmax;
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

    coof = float(dmax) / float(dmin);

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

	return true;
}


void Backend::findByParams()
{
	if (block)return;

	float xScale = 10;

	InstanseModel *model = spotZone->findChild<InstanseModel *>("buffer");
	model->clearAll();

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

//	QImage image("D:/Learning/BAR/Moscow/50_60_1_2_2m_v3.0-20201116T184630Z-001/test.png");
	QPixmap image("D:/Learning/BAR/Moscow/50_60_1_2_2m_v3.0-20201116T184630Z-001/test.png");

	float xfactor = image.width() / 2500.f;
	float yfactor = image.height() / 2500.f;

	qDebug() << image.width();
	QPainter painter(&image);
	QPen pen;
	pen.setWidth(3);
	pen.setColor(Qt::red);
	painter.setPen(pen);

//	painter.drawRect(0,0,50,50);


	reader = new TiffReader();
	reader->open(proj.heimapPath.toStdWString().c_str());
	ImageSearcher imgsrch(dynamic_cast<TiffReader *>(reader));

	QTextStream stream(&out);
	QString line;
	int k = 0, l = 0;
	while (stream.readLineInto(&line))
	{
		// tile
		if (line.startsWith("t"))
		{
//            qDebug() << l;
			l = 0;
			continue;
		}

		// data
		QStringList list = line.split(" ");
		InstInfo* bb = new InstInfo(
			list[0].toInt(), list[1].toInt(), list[2].toFloat(),
			list[3].toInt(), list[4].toInt(), list[5].toFloat());

		if (!checkBounty(bb->bb))
			continue;
		++l;

		bb->setFactor(xScale);
		//sd
		if (!imgsrch.checkCircle(bb->bb))
			continue;


		painter.drawRect(bb->bb.x*xfactor,bb->bb.y*yfactor, bb->bb.wid()*xfactor, bb->bb.hei()*yfactor);

        model->boundydata.append(bb);
		k++;
	}

	painter.save();
	painter.end();
	image.save("D:\\rest.jpg");
	//
	model->updateAll();
//	spotZone->setProperty("buffer", QVariant::fromValue(dataList));
	saveSettings();

	reader->close();
	delete reader;
}

void Backend::test(QString path)
{
	if (block)return;
}

QString Backend::loadImage(QString path, int step, int type)
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
    object.setMode((ProcessMode) type);
	object.setStep(step);
    object.write(projectPath+ "2.obj", 0, 0);

	this->proj.imgMinVal = reader->min;
	this->proj.imgMaxVal = reader->max;
	this->proj.modelPath = projectPath+ "2.obj";
	this->proj.heimapPath = path;
	this->proj.step = step;

    proj.notifySettings();
	this->saveSettings();

	return this->proj.modelPath;
}
