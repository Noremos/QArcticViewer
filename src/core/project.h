#ifndef PROJECT_PARAMS_H
#define PROJECT_PARAMS_H

#include "imagesearcher.h"

#include <QDir>
#include <QJsonObject>
#include <QString>

#include "src/types/instinfo.h"
#include "src/core/obj3d.h"

class SpotZones;

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

	Project();
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
    int step=10;
	float imgMaxVal;
	float imgMinVal;
	int materialType=0;

	TiffReader *reader;

	void openReader()
	{
		if (reader)
		{
			reader->close();
			delete reader;
			reader = nullptr;
		}

		reader = new TiffReader();
	}

	SpotZones *spotZones;

	static Project *proj;
public:
	SeachingSettings searchSetts;
	//= "D:\\Programs\\Barcode\\_bar\\";

	void setProjectPath(QString &path) { projectPath = QFileInfo(path).absoluteDir().absolutePath(); }

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

private:
	QString projectPath;
	bool checkBounty(boundy& bb)
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
		if (coof > searchSetts.coof)
			return false;


		// diametr
		if (dmin * resol < searchSetts.diamert.start || dmax * resol > searchSetts.diamert.end)
			return false;

		if (bb.zei() < searchSetts.height.start)
			return false;

		if (bb.zei() > searchSetts.height.end)
			return false;

		return true;
	}
public:

	QString getPath(BackPath pathI)
	{
		switch (pathI) {
		case BackPath::project:
			return projectPath + "proj.qwr";
			break;
		case BackPath::barlist:
			return projectPath + "bds.lst";
			break;
		case BackPath::texture1:
			return texturePath;
		case BackPath::texture2:
			return texture2Path;
		case BackPath::object:
			return modelPath;
		default:
			break;
		}

	}
	
	void processHiemap(int start, int end);

	void findByParams();


	void loadImage(QString path, int step, int type);
	float getImgMaxVal() const;

	float getImgMinVal() const;

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
