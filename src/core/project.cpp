#include "project.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QPixmap>
#include "side-src/Barcode/PrjBarlib/include/barcodeCreator.h"

#include "src/render/spotzones.h"
#include "src/render/text2d.h"

#include "mainwidget.h"

using std::min;
using std::vector;

Project *Project::proj = nullptr;

Project::Project()
{
	projectPath = "D:\\Programs\\Barcode\\_bar\\_p2\\";
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
	saveFile.close();

	QString pas = getPath(BackPath::tiles);
	if (!QDir(pas).exists())
	{
		QDir().mkdir(pas);
	}

#ifdef ENABLE_MAKERS
	widget->userMarkers->save();
	widget->userMarkers->openFile();
#endif

	return true;
}

bool Project::checkBounty(boundy &bb)
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

	coof = float(dmax) / float(dmin);

	// sootn
	if (coof > searchSetts.coof)
		return false;


	// diametr// dmin in pixels. Cast in  to meters
	if (dmin * resol < searchSetts.diamert.start || dmax * resol > searchSetts.diamert.end)
		return false;

	if (bb.zei() < searchSetts.height.start)
		return false;

	if (bb.zei() > searchSetts.height.end)
		return false;

	if (dmax * 1.5 < bb.zei())
		return false;

	return true;
}

// #include <opencv2/opencv.hpp>
// #include <opencv2/core/mat.hpp>

bool Project::checkHolm(boundy &bb, Img &tile, int offX, int offY)
{
	float dwid = bb.wid();
	float dhei = bb.hei();

	float z = bb.bottom();
	float maxHei = bb.zei();

	// -1|-1  0  1
	//  0|-1  s  1
	//  1|-1  0  1
	char offs[]{-1, -1, -1, 0, -1, 1, 0, -1, 0, 1, 1, -1, 1, 0, 1, 1};

	int midX = bb.x - offX + dwid / 2;
	int midY = bb.y - offY + dhei / 2;

//	float midv = tile.getSafe(midX, midY, z);
//	int stX = midX - dwid;
//	int stY = midY - dhei;
//	int endY = midY + dhei;
//	int endX = midX + dwid;W
//	tile.genMinMax();
//	cv::Mat mat(endY - stY + 5, endX - stX + 5, CV_8UC3, cv::Scalar(0,0,0));
//	float diffset = tile.maxVal - tile.minVal;

//	for (int i = stX; i < endX; ++i)
//	{
//		for (int j = stY; j < endY; ++j)
//		{
//			float val = tile.getSafe(i, j, tile.minVal);
//			int conv = static_cast<int>(255 * ((val - tile.minVal) / diffset));
//			uchar s = (uchar) MIN(conv, 255);
//			mat.at<cv::Vec3b>(j - stY, i - stX) = cv::Vec3b(s, s, s);
//		}
//	}

//	mat.at<cv::Vec3b>(midY - stY, midX - stX) = cv::Vec3b(255, 0, 0);
//	qDebug() <<"z:" << z << "min real:" << midv;

	dwid -= 2;
	dhei -= 2;
	//			Beaf::exportDataAsPng("D:/out.png", bimg);
	for (int i = 0; i < 16; i += 2)
	{
		//Предполаагется, что бугор может быть на крае, поэтому val будет ниже
		int nx = midX + dwid * offs[i];
		int ny = midY + dhei * offs[i + 1];

		//mat.at<cv::Vec3b>(ny - stY, nx - stX) = cv::Vec3b(0, 255, 0);

		float val = tile.getSafe(nx, ny, z);
		if (z - val >= maxHei)
		{
			return false;
//			mat.at<cv::Vec3b>(ny - stY, nx - stX) = cv::Vec3b(0, 0, 255);
		}
//		qDebug() << val;
	}
//	cv::imwrite("D:\\sad.png", mat);
//	exit(0);

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

void Project::findROIsOnHiemap(const PrjgBarCallback &pbCallback, int start, int end)
{
	if (block)
		return;

	FileBuffer boundStream;
	if (!boundStream.openFileStream(getPath(BackPath::roilist)))
		return;
	FileBuffer barStream;

	if (!barStream.openFileStream(getPath(BackPath::barlist)))
		return;

	//	openReader();
	ImageSearcher imgsrch(dynamic_cast<TiffReader *>(reader));

	--start;
	--end;


	start = min(start, imgsrch.getMaxTiles()-1);
	end = min(end, imgsrch.getMaxTiles()-1);
	qDebug() << start << end;

	if (pbCallback.cbSetMax)
		pbCallback.cbSetMax(end - start+1);

	imgsrch.setFillTileRowCache();
	//1000/18
	for (int ind = start; ind <= end; ++ind)
	{
		size_t size = imgsrch.findROIs(boundStream, barStream, ind, 1, searchSetts.bottomProc, pbCallback.stopAction);

		// if (ind != end)
		// 	barStream.writeLine(",");
		// else
		// 	barStream.writeLine();

		if (pbCallback.cbIncrValue)
			pbCallback.cbIncrValue(1);

		qDebug() << ind << "/" << imgsrch.getMaxTiles() << ": " << size;
	}

//	QString ds = Project::proj->getPath(BackPath::root);
	imgsrch.savemat();

	if (pbCallback.cbIncrValue)
		pbCallback.cbIncrValue(end - start+1);

	barStream.close();
	boundStream.close();

}


#include <QJsonArray>
#include <fstream>


void Project::filterROIs(const PrjgBarCallback &pbCallback, bool useBoundyChec, bool useBarcoed, float minBarShooj,bool useCycle, float eps, int start, int endT )
{
	if (block)return;

	//		InstanseModel *model = nullptr;
	//spotZone->findChild<InstanseModel *>("buffer");
	//		model->clearAll();

	QString bpath = getPath(BackPath::roilist);
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
	bool exportImg = false;

	bool checkBoundy = true;
    bool checkBar3d = false;
    bool checkSyrcl = false;

    checkBoundy = useBoundyChec;
    checkBar3d = useBarcoed;
    checkSyrcl = useCycle;

    float POROG = minBarShooj;

    // qDebug() << image.width();

	//!###############################EXPORT##########################
	QPixmap image;
	QPainter *painter;
    float xfactor = displayFactor;
    float yfactor = displayFactor;
	if (exportImg)
	{
		image.load("D:/Learning/BAR/Moscow/50_60_1_2_2m_v3.0-20201116T184630Z-001/test.png");
		painter = new QPainter(&image);
		QPen pen;
		pen.setWidth(3);
		pen.setColor(Qt::red);
		painter->setPen(pen);
	}

//    closeReader();
//	openReader();
	ImageSearcher imgsrch(dynamic_cast<TiffReader *>(reader));
	imgsrch.setFillTileRowCache();

	//!###############################КРУГ############################


	//!###############################BARCODE############################
	QVector<Baritem<float> *> etalons;
	int maxetalcount = 30;
	bc::BarcodeCreator<float> creator;
	BarConstructor<float> constr;
	if (checkBar3d)
	{
		constr.addStructure(ProcType::f255t0, ColorType::gray, ComponentType::Component);
		constr.returnType = ReturnType::barcode2d;
		constr.createBinayMasks = false;
		constr.createGraph = false;

		for (int i = 0; i < maxetalcount; ++i)
		{
			QFile file(":/resources/imgs_orginal/imgOut" +QString::number(i)+".bf");
			if(!file.open(QIODevice::ReadOnly))
				break;

			QByteArray arr = file.read(file.size());
//			char shape[2]{arr.at(0), arr.at(1)};
			uchar *imgdata = (uchar *) (arr.data() + 2);

			BarImg<float> fileimg(60, 60, 1, imgdata, false, false);

            bc::Barcontainer<float> *ret = creator.createBarcode(&fileimg, constr);
            bc::Baritem<float> *item = ret->exractItem(0);
//			item->removePorog(3);
			item->relen();
			item->normalize();

			etalons.append(item);
			delete ret;
			file.close();
		}
	}


	QTextStream stream(&out);
	QString line;
	int k = 0, l = 0;

	if (pbCallback.cbSetMax)
		pbCallback.cbSetMax(imgsrch.getMaxTiles()-1);

    int tileindex = 0;
	Img tile;
	while (stream.readLineInto(&line))
    {
		++l;
//		if (l!=685584)
//			continue;
		// tile
		if (line.length()==0)
			continue;

		if (line.startsWith("t"))
		{
			qDebug() << l;

			if (pbCallback.cbIncrValue)
				pbCallback.cbIncrValue(1);

			if (pbCallback.stopAction)
				break;

            QStringList listw = line.split(" ");
            tileindex = listw[1].toInt();

			if (tileindex >= endT-1)
				break;

//            if (checkBar3d || checkSyrcl)
			{
				tile.release();
				tile = imgsrch.getTile(tileindex);
			}
//			l = 0;
			continue;
		}


		if (tileindex<start-1)
			continue;

		if ((l % 20 == 0) && pbCallback.stopAction)
			break;

		// data
		QStringList list = line.split(" ");
		InstInfo bb(
			list[0].toInt(), list[1].toInt(), list[2].toFloat(),
			list[3].toInt(), list[4].toInt(), list[5].toFloat());

        if ((int)bb.bb.endZ==9999 || (int)bb.bb.z==9999)
			continue;

		if (checkBoundy)
		{
			if (!checkBounty(bb.bb))
			{
//				spotZones->addBoundy(bb.bb,displayFactor, 0);
				continue;
			}

			int offX=0, offY=0;
			imgsrch.getOffset(tileindex, offX, offY);
			if (!checkHolm(bb.bb, tile, offX, offY))
			{
				spotZones->addBoundy(bb.bb,displayFactor, glColor::Brown);
				continue;
			}
		}


		Img img;

        if (checkBar3d || checkSyrcl)
		{

            //bb.x==1054 || startX * tileWid==1000 || startInTile = 54
            int startInTileX;
            int startInTileY;
            imgsrch.getFileOffset(tileindex, bb.bb, startInTileX, startInTileY);
            tile.getRect(startInTileX,startInTileY, (int)bb.bb.wid(), (int)bb.bb.hei(), img);
//			Beaf::exportDataAsPngInner("D:\\re.png", img.wid, img.hei, img.data);
//			Beaf::exportHeightAsPng("D:\\rw.png", Beaf::getFromRawData(img.wid, img.hei, img.data), true);
		}

		if (checkBar3d)
		{
			bc::BarImg<float> bimg(img.wid, img.hei, 1, reinterpret_cast<uchar *>(img.data), false, false);

//			Beaf::exportDataAsPng("D:/out.png", bimg);

			if (exportImg)
				painter->drawRect(bb.bb.x*xfactor,bb.bb.y*yfactor, bb.bb.wid()*xfactor, bb.bb.hei()*yfactor);

			auto *retf = creator.createBarcode(&bimg, constr);
			std::unique_ptr<bc::Baritem<float>> baritem(retf->exractItem(0));
			delete retf;
//			baritem->removePorog(3);
			baritem->relen();
			baritem->normalize();


			float maxRet = 0;
			for (auto *bas : etalons)
			{
				float rew = baritem->compireFull(bas, CompireStrategy::CommonToLen);
				maxRet = MAX(maxRet, rew);
			}

			qDebug() <<"max:" <<  maxRet;
			if (maxRet < POROG)
			{
				spotZones->addBoundy(bb.bb,displayFactor, glColor::Purpure);
				img.release();
				continue;
			}
		}

		if (checkSyrcl)
		{
			if (!imgsrch.checkCircle(img, searchSetts.height.start, eps))
			{
				spotZones->addBoundy(bb.bb,displayFactor, glColor::Blue);
				img.release();
				continue;
			}
		}
		//			l = 0;

		// xScale -- восколько у нас уменьшина карта. Сейчас у нас рельные пиксельные размеры
		// И чтобы их корректно отобразить, надо поделить всё на процент уменьшения.
		// 100 и 100 станут 10 и10 и нормальн отобразятся на уменьшенной в 10 раз карте

		img.release();
		spotZones->addBoundy(bb.bb, displayFactor, glColor::Green);
//		text->addText(bb.bb);
		//			model->boundydata.append(bb);
		k++;
	}

	if (pbCallback.cbIncrValue)
		pbCallback.cbIncrValue(imgsrch.getMaxTiles()-1);

//    closeReader();
	if (exportImg)
	{
		painter->save();
		painter->end();
        image.save(getPath(BackPath::project) + "rest.jpg");
		delete painter;
	}

	if (checkBar3d)
	{
		for (int i = 0; i < etalons.size(); ++i)
		{
			delete etalons[i];
		}
	}

	qDebug() << "Found: " << k << " from " << l;

	saveProject();
}

void Project::loadImage(const PrjgBarCallback &pbCallback, QString path, int step, int type, int start, int end)
{
//	int imgtype = 0;
//	switch (imgtype)
//	{
//	default:
//		reader = new TiffReader();
//		break;
//	}

	closeReader();

	this->modelPath = "map.obj";
	this->heimapPath = path;

	openReader();
	if (!reader->ready)
		return ;
	//	int hei = 500;
	Obj3d object(reader);
	object.setMode((ProcessMode) type);
	object.setStep(step);
	object.write(pbCallback, getPath(BackPath::object), start, end);

	this->imgMinVal = reader->min;
	this->imgMaxVal = reader->max;
	this->displayFactor = step;

	notifySettings();
	saveProject();
	
//	closeReader();
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
	
//	qDebug() << searchSetts.height.start;
//	qDebug() << searchSetts.heightMin();

	openReader();
	modelWid = reader->widght() / displayFactor;
	modelHei = reader->height() / displayFactor;

	notifySettings();
	return true;
}

void Project::read(const QJsonObject &json)
{
	this->modelPath		= json["modelPath"].toString();
	this->heimapPath	= json["heimapPath"].toString();
	this->texturePath	= json["texturePath"].toString();
	this->texture2Path	= json["texture2Path"].toString();
	this->displayFactor	= json["step"].toInt();
	this->imgMinVal		= json["imgMinVal"].toDouble();
	this->imgMaxVal		= json["imgMaxVal"].toDouble();
	this->materialType  = json["materialType"].toInt();

	QJsonObject setts = json["searchingSettings"].toObject();
	searchSetts.coof = setts["coof"].toDouble();
	searchSetts.setDiametrMin(setts["diametrMin"].toInt());
	searchSetts.setDiametrMax(setts["diametrMax"].toInt());
	searchSetts.setHeightMin(setts["heightMin"].toDouble());
	searchSetts.setHeightMax(setts["heightMax"].toDouble());
	searchSetts.bottomProc = setts["bottom"].toInt();
}

void Project::write(QJsonObject &json) const
{
	json["modelPath"]		= this->modelPath;
	json["heimapPath"]	= this->heimapPath;
	json["texturePath"] = this->texturePath;
	json["texture2Path"] = this->texture2Path;
	json["step"]		= this->displayFactor;
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

int normal(float val, int factor)
{
	return  static_cast<int>(val/factor);
}

void Project::readGeoshape()
{
	QFile jsonFile(":/resources/shape.geojson");
	if (!jsonFile.open(QIODevice::ReadOnly))
	{
		return;
	}

	// Считываем весь файл
	QByteArray geodata = jsonFile.readAll();
	// Создаём QJsonDocument
	QJsonDocument jsonDocument(QJsonDocument::fromJson(geodata));
	// Из которого выделяем объект в текущий рабочий QJsonObject
	QJsonObject object = jsonDocument.object();
	QJsonArray features = object["features"].toArray();

//	openReader();
	ImageSearcher imgsrch(dynamic_cast<TiffReader *>(reader));

	/*
	 * 1900000.0000000000000000,950000.0000000000000000 :
	 * 1950000.0000000000000000,1000000.0000000000000000

		satrt: 190; 100
		end:   195;  95


Tag: 34735 ; Value:  2692054518
		Tag: 34736 ; Value:  2692054662
		Tag: 34737 ; Value:  2692054718
		Tag: 42113 ; Value:  2692054512
	 * */
	// start at 1900000 1000000


	auto maxsize = reader->getMaxModelSize();
	qDebug() << reader->tiff.ModelTiepointTag.points[0].X <<
				reader->tiff.ModelTiepointTag.points[0].Y <<
				maxsize.x() << maxsize.y();

//	Size2 size = imgsrch.getTileSize();
	foreach (auto f, features)
	{
		auto arrcoors = f["geometry"].toObject()["coordinates"].toArray();

		// Send in format x, y, T
		QVector3D coord(arrcoors[0].toDouble(), arrcoors[1].toDouble(), 0);
		coord = reader->convertModelToRaster(coord);
		// Get in format x, T, y

		if (coord.x() < 0 || coord.z() < 0 || coord.x() >= reader->widght() || coord.z() >=reader->height())
			continue;

		int x = normal(coord.x(), displayFactor);
		int z = normal(coord.z(), displayFactor);
		coord.setY(widget->terra->getValue(x, z));

		widget->markers->addBoundy(coord, displayFactor);
	}

	widget->markers->updateBuffer();
}

#include "side-src/fast_float/fast_float.h"

void Project::readMarkers()
{
	QFile listFile(Project::getPath(BackPath::markers));
	if (!listFile.open(QIODevice::ReadOnly))
	{
		return;
	}

	char rawtoken[500];
	const char *token;
	while (!listFile.atEnd())
	{
		int size = listFile.readLine(rawtoken, 500);
		if (size <= 2)
			continue;

		token = rawtoken;
		float x = 0, y = 0, z = 0;

		int len = Object3d::getWord(token);
		fast_float::from_chars(token, token + len, x);
		token += len + 1;

		len = Object3d::getWord(token);
		fast_float::from_chars(token, token + len, y);
		token += len + 1;

		len = Object3d::getWord(token);
		fast_float::from_chars(token, token + len, z);

		widget->userMarkers->addBoundy(x, y, z);
	}

	listFile.close();

	widget->userMarkers->openFile();
}


