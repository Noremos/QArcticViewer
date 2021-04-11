#include "project.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

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

	openReader();

	QString bpath = getPath(BackPath::barlist);

	qDebug() << bpath;
	QFile out(bpath);
	if (out.exists())out.remove();
	if (!out.open(QFile::WriteOnly | QFile::Truncate))return;
	QTextStream boundSteam(&out);

	QString sw;
	reader->open(heimapPath.toStdWString().c_str());
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
		spotZones->addBoundy(bb->bb);
		text->addText(bb->bb);
		//			model->boundydata.append(bb);
		k++;
	}
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
	openReader();

	reader->open(path.toStdWString().c_str());
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
