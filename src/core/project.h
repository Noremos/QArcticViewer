#ifndef PROJECT_PARAMS_H
#define PROJECT_PARAMS_H

#include "imagesearcher.h"

#include <QDir>
#include <QJsonObject>
#include <QString>

#include "src/types/instinfo.h"
#include "src/core/obj3d.h"

#include <src/render/markers.h>

class SpotZones;
class Text2d;
class MainWidget;

enum class BackPath
{
	texture1,
	texture2,
	object,
	project,
	barlist,
	roilist,
	heimap,
	tiles,
	root
};


class Project : public QObject
{
public:
	Q_OBJECT
	Q_PROPERTY(QString modelPath MEMBER modelPath NOTIFY modelChanged)
	Q_PROPERTY(QString heimapPath MEMBER heimapPath NOTIFY heimapChanged)
	Q_PROPERTY(QString texturePath MEMBER texturePath NOTIFY textureChanged)
	Q_PROPERTY(QString texture2Path MEMBER texture2Path NOTIFY texture2Changed)
	Q_PROPERTY(int displayFactor MEMBER displayFactor)
    Q_PROPERTY(float imgMinVal MEMBER imgMinVal NOTIFY imgMinValChanged)
    Q_PROPERTY(float imgMaxVal MEMBER imgMaxVal NOTIFY imgMaxValChanged)
	Q_PROPERTY(int materialType MEMBER materialType NOTIFY meterialtypeChanged)
//	Q_PROPERTY(SeachingSettings* searchSetts READ getSerchSetts)
//	Q_PROPERTY(SeachingSettings searchSetts MEMBER searchSetts)
//	SeachingSettings* getSerchSetts(){return &searchSetts;}

	Project();
	~Project()
	{
		closeReader();
		silense();
	}
public:

	bool block = false;

	bool loadProject(QString path);
	bool saveProject();
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
	int displayFactor=10;
	float imgMaxVal;
	float imgMinVal;
	int materialType=0;

	TiffReader *reader = nullptr;

	void closeReader()
	{
		if (reader)
		{
			reader->close();
			delete reader;
			reader = nullptr;
		}
	}

	void openReader()
	{
		if (!reader)
			reader = new TiffReader();

		if (!reader->ready)
			reader->open(getPath(BackPath::heimap).toStdWString().c_str());
	}

	SpotZones *spotZones;
	Text2d *text;

	static Project *proj;
public:
	SeachingSettings searchSetts;
	//= "D:\\Programs\\Barcode\\_bar\\";

	void setProjectPath(QString &path)
	{
		projectPath = QFileInfo(path).absoluteDir().absolutePath();
		QChar last = projectPath[projectPath.length() - 1];
		if (last != '\\' || last != '/')
			projectPath += '/';
	}

	static Project *getProject()
	{
		if (proj == nullptr)
			proj = new Project();

		return proj;
	}
	static void dropProject()
	{
		if (proj != nullptr)
		{
			delete proj;
			proj = nullptr;
		}
	}

	MainWidget *widget;
private:

	QString projectPath;
	bool checkBounty(boundy& bb)
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

		return true;
	}

public:
	bool isTileCached(int ind)
	{
		QString path = getTilePath(ind);
		return QFile::exists(path);
	}
	QString getTilePath(int ind)
	{
		return getPath(BackPath::tiles) + QString::number(ind);
	}

	QString getPath(BackPath pathI)
	{
		switch (pathI)
		{
		case BackPath::tiles:
			return projectPath + "tiles/";
		case BackPath::project:
			return projectPath + "proj.qwr";
		case BackPath::barlist:
			return projectPath + "bds.json";
		case BackPath::roilist:
			return projectPath + "bds.lst";
		case BackPath::texture1:
			return texturePath;
		case BackPath::texture2:
			return texture2Path;
		case BackPath::object:
			return projectPath + modelPath;
		case BackPath::heimap:
			return heimapPath;
		case BackPath::root:
			return projectPath;
		default:
			return "";
		}
	}
	
	void findROIsOnHiemap(const PrjgBarCallback &pbCallback, int start, int end);

	void filterROIs(const PrjgBarCallback &pbCallback, bool useBoundyChec, bool useBarcoed, float minBarShooj, bool useCycle, float eps, int start, int maxTile);


	void loadImage(const PrjgBarCallback &pbCallback, QString path, int step, int type, int start =0, int end=0);
	float getImgMaxVal() const;

	float getImgMinVal() const;


	void mouseCast(int x, int y);
	void readGeoshape();
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
