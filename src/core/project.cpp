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

	QString pas = getPath(BackPath::tiles);
	if (!QDir(pas).exists())
	{
		QDir().mkdir(pas);
	}
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

	openReader();
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

	if (pbCallback.cbIncrValue)
		pbCallback.cbIncrValue(end - start+1);

	barStream.close();
	boundStream.close();

}


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
	openReader();
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

		int sd = 0;
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
//			item->relen();
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

            if (checkBar3d || checkSyrcl)
			{
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
//                spotZones->addBoundy(bb.bb,displayFactor, 0);
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

			Beaf::exportDataAsPng("D:/out.png", bimg);

			if (exportImg)
				painter->drawRect(bb.bb.x*xfactor,bb.bb.y*yfactor, bb.bb.wid()*xfactor, bb.bb.hei()*yfactor);

			auto *retf = creator.createBarcode(&bimg, constr);
			auto *baritem = retf->exractItem(0);
//			baritem->removePorog(3);
//            baritem->relen();
			baritem->normalize();

			delete retf;

			float maxRet = 0;
			for (auto *bas : etalons)
			{
				float rew = baritem->compireFull(bas, CompireStrategy::CommonToLen);
				maxRet = MAX(maxRet, rew);
			}
			delete baritem;
			qDebug() <<"max:" <<  maxRet;
			if (maxRet < POROG)
			{
				spotZones->addBoundy(bb.bb,displayFactor, 2);
				img.release();
				continue;
			}
		}

		if (checkSyrcl)
		{
			if (!imgsrch.checkCircle(img, searchSetts.height.start, eps))
			{
				spotZones->addBoundy(bb.bb,displayFactor, 3);
				img.release();
				continue;
			}
		}
		//			l = 0;

		// xScale -- восколько у нас уменьшина карта. Сейчас у нас рельные пиксельные размеры
		// И чтобы их корректно отобразить, надо поделить всё на процент уменьшения.
		// 100 и 100 станут 10 и10 и нормальн отобразятся на уменьшенной в 10 раз карте

		img.release();
		spotZones->addBoundy(bb.bb, displayFactor, 1);
		text->addText(bb.bb);
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

	this->modelPath = "map.obj";
	this->heimapPath = path;

	closeReader();
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
	this->displayFactor			= json["step"].toInt();
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

