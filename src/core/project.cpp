#include "project.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QPixmap>
#include "side-src/Barcode/PrjBarlib/include/barcodeCreator.h"

#include "src/render/spotzones.h"
#include "src/render/text2d.h"

using std::min;
using std::vector;

Project *Project::proj = nullptr;

Project::Project()
{
	projectPath = "D:\\Programs\\Barcode\\_bar\\";
}

bool Project::saveProject()
{
	QFile saveFile(getPath(BackPath::project));

	if (!saveFile.open(QIODevice::WriteOnly))
	{
		qWarning("Couldn't open save file.");
		return false;
	}

	QJsonObject gameObject;
	write(gameObject);
	QJsonDocument saveDoc(gameObject);
	saveFile.write(saveDoc.toJson());
	return true;
}

float Project::getImgMinVal() const
{
	return imgMinVal;
}

float Project::getImgMaxVal() const
{
	return imgMaxVal;
}

void Project::processHiemap(int start, int end)
{
	if (block)
		return;


	QString bpath = getPath(BackPath::barlist);

	qDebug() << bpath;
	QFile out(bpath);
	if (out.exists())out.remove();
	if (!out.open(QFile::WriteOnly | QFile::Truncate))return;
	QTextStream boundSteam(&out);

	QString sw;
	openReader();
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

	closeReader();
}


#include <fstream>

void Project::findByParams()
{
	if (block)return;

	float xScale = 10;

	//		InstanseModel *model = nullptr;
	//spotZone->findChild<InstanseModel *>("buffer");
	//		model->clearAll();

	QString bpath = getPath(BackPath::barlist);
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

	//!###############################SETS##########################
	bool checkSyrcl = false;
	bool exportImg = false;
	bool checkBoundy = true;
	bool checkBar3d = true;



	// qDebug() << image.width();

	//!###############################EXPORT##########################
	QPixmap image;
	QPainter *painter;
	float xfactor = image.width() / 2500.f;
	float yfactor = image.height() / 2500.f;
	if (exportImg)
	{
		image.load("D:/Learning/BAR/Moscow/50_60_1_2_2m_v3.0-20201116T184630Z-001/test.png");
		painter = new QPainter(&image);
		QPen pen;
		pen.setWidth(3);
		pen.setColor(Qt::red);
		painter->setPen(pen);
	}

	ImageSearcher *imgsrch = nullptr;
	//!###############################КРУГ############################

	if (checkSyrcl)
	{
		openReader();
		imgsrch = new ImageSearcher(dynamic_cast<TiffReader *>(reader));
	}

	//!###############################BARCODE############################
	float POROG = 0.3;
	QVector<Baritem<float> *> etalons;
	int maxetalcount = 5;
	bc::BarcodeCreator<float> creator;
	BarConstructor<float> constr;
	if (checkBar3d)
	{
		if (imgsrch == nullptr)
		{
			openReader();
			imgsrch = new ImageSearcher(dynamic_cast<TiffReader *>(reader));
		}
		constr.addStructure(ProcType::f0t255, ColorType::gray, ComponentType::Component);
		constr.returnType = ReturnType::barcode3d;
		constr.createBinayMasks = false;
		constr.createGraph = false;

		QDir directory("D:/Programs/Python/barcode/experements/geo/imgs_one");
		QStringList images = directory.entryList(QStringList() << "*.bf", QDir::Files);
		int sd = 0;
		foreach (QString filename, images)
		{
			QFile file(directory.path() + "/" + filename);
			file.open(QIODevice::ReadOnly);

			QByteArray arr = file.read(file.size());
			char shape[2]{arr.at(0), arr.at(1)};
			uchar *imgdata = (uchar *) (arr.data() + 2);

			BarImg<float> fileimg(60, 60, 1, imgdata, false, false);

			auto *ret = creator.createBarcode(&fileimg, constr);
			auto *item = ret->exractItem(0);
			item->removePorog(3);
			etalons.append(item);
			delete ret;
			file.close();
			if (++sd >= maxetalcount)
				break;
			//do whatever you need to do
		}
	}


	QTextStream stream(&out);
	QString line;
	int k = 0, l = 0;

	while (stream.readLineInto(&line))
	{
		++l;
//		if (l!=685584)
//			continue;
		// tile
		if (line.startsWith("t"))
		{
			//            qDebug() << l;
//			l = 0;
			continue;
		}

		// data
		QStringList list = line.split(" ");
		InstInfo* bb = new InstInfo(
			list[0].toInt(), list[1].toInt(), list[2].toFloat(),
			list[3].toInt(), list[4].toInt(), list[5].toFloat());

		if (checkBoundy)
		{
			if (!checkBounty(bb->bb))
				continue;
		}



		if (checkSyrcl)
		{
			if (!imgsrch->checkCircle(bb->bb))
				continue;
		}

		if (checkBar3d)
		{
			Img *img = imgsrch->getRect(bb->bb);
			if (img == nullptr)
				continue;
			bc::BarImg<float> bimg(img->wid, img->hei, 1, reinterpret_cast<uchar *>(img->data), false, false);

//			qDebug() << bimg.wid() << bimg.hei() << bimg.get(0, 0) << bimg.get(1, 0);
//			qDebug() << "Line num:" << l;S



			// EXPOT AS RAW
			std::ofstream fout;
//			fout.open("D:/Programs/QT/ArctivViewer/ArcticViewer/temp/out.fd", std::ios::binary | std::ios::out | std::ios::trunc);
//			char wid[] = {(char) bimg.wid(), (char) bimg.hei()};
//			fout.write(wid, 2);
//			fout.write((char *) bimg.getData(), bimg.wid() * bimg.hei() * 4);
//			fout.close();

			//			QFile file("D:/Programs/QT/ArctivViewer/ArcticViewer/temp/out.fd");
			//			file.open(QIODevice::WriteOnly);
			//			QDataStream out(&file); // we will serialize the data into the file
			//			out << (uchar) bimg.wid(); // serialize a string
			//			out << (uchar) bimg.hei(); // serialize an integer
			//			out.writeBytes((char *) bimg.getData(), bimg.wid() * bimg.hei() * 4);
			//			file.close();

			//SAVE PNG
//			QImage imggray(bimg.wid(), bimg.hei(), QImage::Format::Format_Grayscale8);

//			float max = bimg.max();
//			float min = bimg.min();
//			float maxmin = max - min;
//			for (int y = 0; y < bimg.hei(); ++y)
//			{
//				for (int x = 0; x < bimg.wid(); ++x)
//				{
//					int grayVal = 255 * ((bimg.get(x, y)-min) / maxmin);
//					grayVal = MAX(0, MIN(255, grayVal));
//					imggray.setPixelColor(x, y, QColor(grayVal));
//				}
//			}
//			imggray.save("D:/Programs/QT/ArctivViewer/ArcticViewer/temp/out.png");

			if (exportImg)
				painter->drawRect(bb->bb.x*xfactor,bb->bb.y*yfactor, bb->bb.wid()*xfactor, bb->bb.hei()*yfactor);

			auto *retf = creator.createBarcode(&bimg, constr);
			auto *baritem = retf->exractItem(0);
			baritem->removePorog(3);

			delete retf;
			delete img;

			float maxRet = 0;
			for (auto *bas : etalons)
			{
				float rew = baritem->compireFull(bas, CompireStrategy::CommonToLen);
				maxRet = MAX(maxRet, rew);
			}
			delete baritem;
			qDebug() <<"max:" <<  maxRet;
			if (maxRet < POROG)
				continue;
		}
		
		// xScale -- восколько у нас уменьшина карта. Сейчас у нас рельные пиксельные размеры
		// И чтобы их корректно отобразить, надо поделить всё на процент уменьшения.
		// 100 и 100 станут 10 и10 и нормальн отобразятся на уменьшенной в 10 раз карте
		bb->setFactor(xScale);
		spotZones->addBoundy(bb->bb);
		text->addText(bb->bb);
		//			model->boundydata.append(bb);
		k++;
	}


	if (checkSyrcl)
	{
		delete imgsrch;
		closeReader();
	}
	if (exportImg)
	{
		delete painter;
	}

	if (checkBar3d)
	{
		if (imgsrch != nullptr)
		{
			delete imgsrch;
			closeReader();
		}
		for (int i = 0; i < etalons.size(); ++i)
		{
			delete etalons[i];
		}
	}

//	painter.save();
//	painter.end();
//	image.save("D:\\rest.jpg");

	spotZones->updateBuffer();
	text->updateBuffer();

	saveProject();


}

void Project::loadImage(QString path, int step, int type)
{
	int imgtype = 0;
//	switch (imgtype)
//	{
//	default:
//		reader = new TiffReader();
//		break;
//	}
	heimapPath = path;
	openReader();
	if (!reader->ready)
		return ;
	//	int hei = 500;
	Obj3d object(reader);
	object.setMode((ProcessMode) type);
	object.setStep(step);
	object.write(getPath(BackPath::object), 0, 0);

	this->imgMinVal = reader->min;
	this->imgMaxVal = reader->max;
	this->modelPath = getPath(BackPath::object);
	this->heimapPath = path;
	this->step = step;

	notifySettings();
	saveProject();
	
	closeReader();
}

bool Project::loadProject(QString path)
{
	setProjectPath(path);
	QFile loadFile(path);

	if (!loadFile.open(QIODevice::ReadOnly))
	{
		qWarning("Couldn't open save file.");
		return false;
	}

	QByteArray saveData = loadFile.readAll();
	QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
	read(loadDoc.object());
	
	qDebug() << searchSetts.height.start;
	qDebug() << searchSetts.heightMin();
	notifySettings();
	return true;
}

void Project::read(const QJsonObject &json)
{
	this->modelPath		= json["modelPath"].toString();
	this->heimapPath	= json["heimapPath"].toString();
	this->texturePath	= json["texturePath"].toString();
	this->texture2Path	= json["texture2Path"].toString();
	this->step			= json["step"].toInt();
	this->imgMinVal		= json["imgMinVal"].toDouble();
	this->imgMaxVal		= json["imgMaxVal"].toDouble();
	this->materialType  = json["materialType"].toInt();

	QJsonObject setts = json["searchingSettings"].toObject();
	searchSetts.coof = setts["coof"].toDouble();
	searchSetts.setDiametrMin(setts["diametrMin"].toInt());
	searchSetts.setDiametrMax(setts["diametrMax"].toInt());
	searchSetts.setHeightMin(setts["heightMin"].toDouble());
	searchSetts.setHeightMax(setts["heightMax"].toDouble());
	searchSetts.bottomProc = setts["bottom"].toDouble();
}

void Project::write(QJsonObject &json) const
{
	json["modelPath"]		= this->modelPath;
	json["heimapPath"]	= this->heimapPath;
	json["texturePath"] = this->texturePath;
	json["texture2Path"] = this->texture2Path;
	json["step"]		= this->step;
	json["imgMaxVal"]	= this->imgMaxVal;
	json["imgMinVal"]	= this->imgMinVal;
	json["materialType"] = this->materialType;

	QJsonObject setts;
	setts["coof"] = searchSetts.coof;
	setts["diametrMin"] = searchSetts.diamert.start;
	setts["diametrMax"] = searchSetts.diamert.end;
	setts["heightMin"] = searchSetts.height.start;
	setts["heightMax"] = searchSetts.height.end;
	setts["bottom"] = searchSetts.bottomProc;
	json["searchingSettings"] = setts;


//	QJsonObject setts;
//	setts["coof"] = coof;
//		setts.coof = coof;
//	setts.diamert = {minD, maxD};
//	setts.height = {minHei, maxHei};
//	setts.bottomProc = bottomLineProc;
}
