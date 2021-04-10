#ifndef PROJECT_PARAMS_H
#define PROJECT_PARAMS_H

#include "imagesearcher.h"

#include <QDir>
#include <QJsonObject>
#include <QString>

enum class BackPath
{
	texture1,
	texture2,
	object,
	project,
	barlist
};

class Project : public QObject
{
public:
	Q_OBJECT
	Q_PROPERTY(QString modelPath MEMBER modelPath NOTIFY modelChanged)
	Q_PROPERTY(QString heimapPath MEMBER heimapPath NOTIFY heimapChanged)
	Q_PROPERTY(QString texturePath MEMBER texturePath NOTIFY textureChanged)
	Q_PROPERTY(QString texture2Path MEMBER texture2Path NOTIFY texture2Changed)
	Q_PROPERTY(int step MEMBER step)
    Q_PROPERTY(float imgMinVal MEMBER imgMinVal NOTIFY imgMinValChanged)
    Q_PROPERTY(float imgMaxVal MEMBER imgMaxVal NOTIFY imgMaxValChanged)
	Q_PROPERTY(int materialType MEMBER materialType NOTIFY meterialtypeChanged)
//	Q_PROPERTY(SeachingSettings* searchSetts READ getSerchSetts)
//	Q_PROPERTY(SeachingSettings searchSetts MEMBER searchSetts)
//	SeachingSettings* getSerchSetts(){return &searchSetts;}

public:
	Project();


	bool loadProject(QString path);
	bool saveProject(QString path);
	void notifySettings()
	{
		emit heimapChanged(heimapPath);
		emit textureChanged(texturePath);
		emit texture2Changed(texture2Path);
        emit modelChanged(modelPath);
        emit imgMinValChanged(imgMinVal);
        emit imgMaxValChanged(imgMaxVal);
//        emit meterialtypeChanged(materialType);
    }


	QString modelPath;
	QString heimapPath;
	QString texturePath, texture2Path;
    int step=10;
	float imgMaxVal;
	float imgMinVal;
    int materialType=0;
public:
	SeachingSettings searchSetts;
	QDir projectPath;
	//= "D:\\Programs\\Barcode\\_bar\\";

	QString getPath(BackPath pathI)
	{
		switch (pathI) {
		case BackPath::project:
			return projectPath.path() + "proj.qwr";
			break;
		case BackPath::barlist:
			return projectPath.path() + "bds.lst";
			break;
		case BackPath::texture1:
			return texturePath;
		case BackPath::texture2:
			return texture2Path;
		case BackPath::object:
			return projectPath.path() + "2.obj";
		default:
			break;
		}

	}
	
	void processHiemap(int start, int end)
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

		InstanseModel *model = nullptr;
		//spotZone->findChild<InstanseModel *>("buffer");
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
			model->boundydata.append(bb);
			k++;
		}
		model->updateAll();
	//	spotZone->setProperty("buffer", QVariant::fromValue(dataList));
		saveSettings();
	}


	loadImage(QString path, int step, int type)
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
signals:
	void meterialtypeChanged(int);
	void heimapChanged(QString);
	void textureChanged(QString);
	void texture2Changed(QString);
	void modelChanged(QString);
    void imgMinValChanged(float);
    void imgMaxValChanged(float);
private:
	void write(QJsonObject &json) const;
	void read(const QJsonObject &json);
};

#endif
