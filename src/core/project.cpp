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

#ifdef ENABLE_MARKERS
	widget->userMarkers->save();
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
	return u_imgMinVal;
}

float Project::getImgMaxVal() const
{
	return u_imgMaxVal;
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

		if (pbCallback.stopAction)
			break;

		qDebug() << ind << "/" << imgsrch.getMaxTiles() << ": " << size;
	}

//	QString ds = Project::proj->getPath(BackPath::root);
	imgsrch.savemat();

	if (pbCallback.cbIncrValue)
		pbCallback.cbIncrValue(end - start+1);

	barStream.close();
	boundStream.close();

}

void checSpotZone(const QVector<InstanceData> &target, SpotZones* spotZones, int corrVal, bool skipFirst, int& correct, int& totalSize)
{
	foreach (const InstanceData &bbfound, spotZones->boundydata)
	{
		if (bbfound.val != corrVal && corrVal != INT_MAX)
			continue;

		float x_found = bbfound.getX();
		float y_found = bbfound.getZ();
		float x_size = bbfound.getWid() * 2;
		float y_size = bbfound.getLen() * 2;

		for (int i =skipFirst?1:0, targetSize = target.size(); i < targetSize; ++i)
		{
			float x_target = target[i].getX();
			float y_target = target[i].getZ();

			if (abs(y_found - y_target) < y_size && abs(x_found - x_target) < x_size)
			{
				++correct;
				break;
			}
		}

		++totalSize;
	}
}

void Project::checkCorrect(const QVector<InstanceData> &target, bool skipFirst)
{
	int wrong = 0;
	int correct = 0;
	int notFound = 0;
	int totalSize = 0;

	if (this->markersShowState == MarkersShowState::none)
	{
		return;
	}

	int corrVal = (int)getBCColor(this->markersShowState); //MarkersShowState::all
	if (this->markersShowState == MarkersShowState::all)
	{
		corrVal = INT_MAX;
	}

	checSpotZone(target, spotZones, corrVal, skipFirst, correct, totalSize);

	checSpotZone(target, badZones, corrVal, skipFirst, correct, totalSize);


	notFound = target.size() - correct;

	// Всего выделено  - сколько выделено правильно
	wrong = totalSize - correct;
	//	QString filename = "D:\\re.txt";
	//	QFile fileout(filename);
	//	if (fileout.open(QFile::ReadWrite | QFile::Text)){
	//		QTextStream out(&fileout);
	//		foreach (const auto &vec, realshape)
	//		{
	//			out << vec.x() << " " << vec.y() << Qt::endl;
	//		}

	//		fileout.close();
	//	}

	status = QString("Cor|wrg|nt fnd;\n%1 %2 %3\nTtl (Fnd/trgt): %4 %5").arg(correct).arg(wrong).arg(notFound).arg(correct + wrong).arg(correct + notFound);
	qDebug() << "Cor |  wrng | nt fnd;";
	qDebug() << correct << wrong << notFound;
	qDebug() <<"Ttl (Fnd/trgt):" <<totalSize << target.size();
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
	bool exportImg = true;

	bool checkBoundy = true;
    bool checkBar3d = false;
	bool checkSyrcl = false;

	bool addAnother = true;

    checkBoundy = useBoundyChec;
    checkBar3d = useBarcoed;
    checkSyrcl = useCycle;

    float POROG = minBarShooj;

    // qDebug() << image.width();

	//!###############################EXPORT##########################
	QPixmap image;
	std::unique_ptr<QPainter> painter;
	float xfactor = u_displayFactor;
	float yfactor = u_displayFactor;

	QPen penGreen;
	penGreen.setWidth(3);
	penGreen.setColor(Qt::green);

	QPen penPurpure;
	penPurpure.setWidth(3);
	penPurpure.setColor(Qt::red);

	QPen penBlue;
	penBlue.setWidth(3);
	penBlue.setColor(Qt::blue);

	if (exportImg)
	{
		QString texture = getPath(BackPath::texture1);
		image.load(texture);
		painter.reset(new QPainter(&image));

		xfactor = (float)image.width() / reader->widght();
		yfactor = (float)image.height() / reader->height();
		qDebug() << "X Factor:" << xfactor;
		qDebug() << "Y Factor:" << yfactor;
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
		pbCallback.cbSetMax(imgsrch.getMaxTiles() - 1);

	spotZones->boundySize = 0;
	spotZones->boundydata.clear();
	badZones->boundydata.clear();
	badZones->boundySize = 0;
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
//			qDebug() << l;

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
//				tile.release(); //TODO is it correct?
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

		if (!checkBounty(bb.bb))
		{
			if (addAnother)
			{
				badZones->addBoundy(bb.bb, u_displayFactor, getBCColor(MarkersShowState::boundyNotPassed));
			}
			continue;
		}

//		if (checkBoundy)
//		{
			int offX=0, offY=0;
			imgsrch.getOffset(tileindex, offX, offY);
			if (!checkHolm(bb.bb, tile, offX, offY))
			{
//				if (addAnother)
				spotZones->addBoundy(bb.bb, u_displayFactor, getBCColor(MarkersShowState::holmNotPassed));
				continue;
			}
//		}


		Img rectImg;

        if (checkBar3d || checkSyrcl)
		{
            int startInTileX;
            int startInTileY;
            imgsrch.getFileOffset(tileindex, bb.bb, startInTileX, startInTileY);
			tile.getRect(startInTileX,startInTileY, (int)bb.bb.wid(), (int)bb.bb.hei(), rectImg);
//			Beaf::exportDataAsPngInner("D:\\re.png", img.wid, img.hei, img.data);
//			Beaf::exportHeightAsPng("D:\\rw.png", Beaf::getFromRawData(img.wid, img.hei, img.data), true);
		}

		if (checkBar3d)
		{
			bc::BarImg<float> bimg(rectImg.wid, rectImg.hei, 1, reinterpret_cast<uchar *>(rectImg.data), false, false);

//			Beaf::exportDataAsPng("D:/out.png", bimg);

//			if (exportImg)
//				painter->drawRect(bb.bb.x*xfactor,bb.bb.y*yfactor, bb.bb.wid()*xfactor, bb.bb.hei()*yfactor);

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

//			qDebug() <<"max:" <<  maxRet;
			if (maxRet < POROG)
			{
				spotZones->addBoundy(bb.bb, u_displayFactor, getBCColor(MarkersShowState::barcodeNotPassed));
				rectImg.release();
				if (exportImg)
				{
					painter->setPen(penPurpure);
					painter->drawRect(bb.bb.x*xfactor,bb.bb.y*yfactor, bb.bb.wid()*xfactor, bb.bb.hei()*yfactor);
				}
				continue;
			}
		}

		if (checkSyrcl)
		{
			if (!imgsrch.checkCircle(rectImg, searchSetts.height.start, eps))
			{
				spotZones->addBoundy(bb.bb,u_displayFactor, getBCColor(MarkersShowState::circleNotPassed));
				if (exportImg)
				{
					painter->setPen(penBlue);
					painter->drawRect(bb.bb.x*xfactor,bb.bb.y*yfactor, bb.bb.wid()*xfactor, bb.bb.hei()*yfactor);
				}
				rectImg.release();
				continue;
			}
		}
		//			l = 0;

		// xScale -- восколько у нас уменьшина карта. Сейчас у нас рельные пиксельные размеры
		// И чтобы их корректно отобразить, надо поделить всё на процент уменьшения.
		// 100 и 100 станут 10 и10 и нормальн отобразятся на уменьшенной в 10 раз карте
		if (exportImg)
		{
			painter->setPen(penGreen);
			painter->drawRect(bb.bb.x * xfactor, bb.bb.y * yfactor, bb.bb.wid() * xfactor, bb.bb.hei() * yfactor);
		}

		rectImg.release();
		spotZones->addBoundy(bb.bb, u_displayFactor, getBCColor(MarkersShowState::found));
//		text->addText(bb.bb);
		//			model->boundydata.append(bb);
		k++;
	}

	tile.release();

	if (pbCallback.cbIncrValue)
		pbCallback.cbIncrValue(imgsrch.getMaxTiles()-1);

//    closeReader();
	if (exportImg)
	{
		painter->save();
		painter->end();
		image.save(getPath(BackPath::root) + "result.jpg");
	}

	if (checkBar3d)
	{
		for (int i = 0; i < etalons.size(); ++i)
		{
			delete etalons[i];
		}
	}

	qDebug() << "Found: " << k << " from " << l;


//	checkCorrect(k, widget->markers->boundydata);
// #ifdef ENABLE_MARKERS
// 	checkCorrect(k, widget->importedMakrers->boundydata, true);
// #endif

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

	this->u_modelPath = "map.obj";
	this->u_heimapPath = path;

	openReader();
	if (!reader->ready)
		return ;
	//	int hei = 500;
	Obj3d object(reader);
	object.setMode((ProcessMode) type);
	object.setStep(step);
	object.write(pbCallback, getPath(BackPath::object), start, end);

	this->u_imgMinVal = reader->min;
	this->u_imgMaxVal = reader->max;
	this->u_displayFactor = step;

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
	modelWid = reader->widght() / u_displayFactor;
	modelHei = reader->height() / u_displayFactor;

	notifySettings();
	return true;
}
static const char* jsn_modelPath = "modelPath";
static const char* jsn_heimapPath = "heimapPath";
static const char* jsn_texturePath = "texturePath";
static const char* jsn_texture2Path = "texture2Path";
static const char* jsn_displayFacto = "step";
static const char* jsn_imgMinVal	 = "imgMinVal";
static const char* jsn_imgMaxVal	 = "imgMaxVal";
static const char* jsn_materialType = "materialType";
static const char* jsn_geojsonPath  = "geojsonPath";

static const char* jsns_searchingSettings  = "searchingSettings";
static const char* jsns_coof  = "coof";
static const char* jsns_diametrMin  = "diametrMin";
static const char* jsns_diametrMax  = "diametrMax";
static const char* jsns_heightMin  = "heightMin";
static const char* jsns_heightMax  = "heightMax";
static const char* jsns_bottom  = "bottom";


void Project::read(const QJsonObject &json)
{
	this->u_modelPath = json[jsn_modelPath].toString();
	this->u_heimapPath = json[jsn_heimapPath].toString();
	this->u_texturePath = json[jsn_texturePath].toString();
	this->u_texture2Path = json[jsn_texture2Path].toString();
	this->u_displayFactor = json[jsn_displayFacto].toInt();
	this->u_imgMinVal = json[jsn_imgMinVal].toDouble();
	this->u_imgMaxVal = json[jsn_imgMaxVal].toDouble();
	this->u_materialType = json[jsn_materialType].toInt();
	this->u_geojsonPath = json[jsn_geojsonPath].toString("");

	QJsonObject setts = json[jsns_searchingSettings].toObject();
	searchSetts.coof = setts[jsns_coof].toDouble();
	searchSetts.setDiametrMin(setts[jsns_diametrMin].toInt());
	searchSetts.setDiametrMax(setts[jsns_diametrMax].toInt());
	searchSetts.setHeightMin(setts[jsns_heightMin].toDouble());
	searchSetts.setHeightMax(setts[jsns_heightMax].toDouble());
	searchSetts.bottomProc = setts[jsns_bottom].toInt();
}

void Project::write(QJsonObject &json) const
{
	json[jsn_modelPath]		= this->u_modelPath;
	json[jsn_heimapPath]	= this->u_heimapPath;
	json[jsn_texturePath] = this->u_texturePath;
	json[jsn_texture2Path] = this->u_texture2Path;
	json[jsn_displayFacto]		= this->u_displayFactor;
	json[jsn_imgMaxVal]	= this->u_imgMaxVal;
	json[jsn_imgMinVal]	= this->u_imgMinVal;
	json[jsn_materialType] = this->u_materialType;
	json[jsn_geojsonPath] = this->u_geojsonPath;

	QJsonObject setts;
	setts[jsns_coof] = searchSetts.coof;
	setts[jsns_diametrMin] = searchSetts.diamert.start;
	setts[jsns_diametrMax] = searchSetts.diamert.end;
	setts[jsns_heightMin] = searchSetts.height.start;
	setts[jsns_heightMax] = searchSetts.height.end;
	setts[jsns_bottom] = searchSetts.bottomProc;
	json[jsns_searchingSettings] = setts;


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

void Project::readGeojson()
{
	QFile jsonFile(getPath(BackPath::geojson));
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

	widget->importedMakrers->release();
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

		int x = normal(coord.x(), u_displayFactor);
		int z = normal(coord.z(), u_displayFactor);
		coord.setY(widget->terra->getValue(x, z));

		widget->importedMakrers->addBoundy(coord, u_displayFactor);
	}
	jsonFile.close();

	widget->importedMakrers->updateBuffer();
}


#include "side-src/fast_float/fast_float.h"


void Project::readMyGeo(bool reinitY)
{
	QFile inputFile(getPath(BackPath::geojson));
	if (!inputFile.open(QIODevice::ReadOnly))
	{
		return;
	}

	QTextStream in(&inputFile);
	while (!in.atEnd())
	{
		QString line = in.readLine();
		auto splo = line.split(' ');
		if (splo.size()!=3)
			continue; // skip t K

		QVector3D coord(splo[0].toFloat(), splo[1].toFloat(), splo[2].toFloat());

		if (reinitY)
		{
			coord.setY(widget->terra->getValue(coord.x(), coord.z()));
		}

		widget->markers->addBoundy(coord, 1);
	}
	inputFile.close();

	widget->markers->updateBuffer();
}


void Project::readMarkers()
{
	QFile listFile(Project::getPath(BackPath::markers));
	if (listFile.open(QIODevice::ReadOnly))
	{
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
	}

	if (widget->userMarkers->boundydata.size() == 0)
		widget->userMarkers->addBoundy(1, 1, 1);
}


