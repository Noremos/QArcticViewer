#ifndef PROJECT_PARAMS_H
#define PROJECT_PARAMS_H

#include "imagesearcher.h"

#include <QDir>
#include <QJsonObject>
#include <QString>
#include <QVector3D>

#include "src/types/instinfo.h"
#include "src/core/obj3d.h"

#include <src/render/markers.h>

class SpotZones;
class Text2d;
class MainWidget;

enum MarkersShowState { found = 0, ather, barcodeNotPassed, circleNotPassed, boundyNotPassed, holmNotPassed, allExceptRed, all, none };

static glColor getBCColor(MarkersShowState state)
{
	switch (state)
	{
	case found:
		return glColor::Green;

	case ather:
		return glColor::Red;

	case holmNotPassed:
		return glColor::Brown;

	case boundyNotPassed:
		return glColor::Red;

	case barcodeNotPassed:
		return  glColor::Purpure;

	case circleNotPassed:
		return  glColor::Blue;

	default:
		std::exception();
		break;
	}
	return glColor::null;
}

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
	root,
	markers,
	geojson
};


class Project : public QObject
{
public:
	Q_OBJECT
	Q_PROPERTY(QString u_modelPath MEMBER u_modelPath NOTIFY modelChanged)
	Q_PROPERTY(QString u_heimapPath MEMBER u_heimapPath NOTIFY heimapChanged)
	Q_PROPERTY(QString u_texturePath MEMBER u_texturePath NOTIFY textureChanged)
	Q_PROPERTY(QString u_texture2Path MEMBER u_texture2Path NOTIFY texture2Changed)
	Q_PROPERTY(int u_displayFactor MEMBER u_displayFactor)
	Q_PROPERTY(float u_imgMinVal MEMBER u_imgMinVal NOTIFY imgMinValChanged)
	Q_PROPERTY(float u_imgMaxVal MEMBER u_imgMaxVal NOTIFY imgMaxValChanged)
	Q_PROPERTY(int u_materialType MEMBER u_materialType NOTIFY meterialtypeChanged)
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
	int modelWid;
	int modelHei;
	bool loadProject(QString path);
	bool saveProject();
	void notifySettings()
	{
		emit heimapChanged(u_heimapPath);
		emit textureChanged(u_texturePath);
		emit texture2Changed(u_texture2Path);
		emit modelChanged(u_modelPath);
		emit imgMinValChanged(u_imgMinVal);
		emit imgMaxValChanged(u_imgMaxVal);
//        emit meterialtypeChanged(materialType);
    }


	QString u_modelPath;
	QString u_heimapPath;
	QString u_texturePath, u_texture2Path;
	int u_displayFactor=10;
	int u_heiFactor=10;
	float u_imgMaxVal;
	float u_imgMinVal;
	int u_materialType=0;
	QString u_geojsonPath;

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
			reader->open(getPath(BackPath::heimap).toStdString().c_str());
	}

	SpotZones *spotZones;
	SpotZones *badZones;
	Text2d *text;

	static Project *proj;

	QString status;
public:
	MarkersShowState markersShowState = MarkersShowState::found;
	SeachingSettings searchSetts;

	//= "D:\\Programs\\Barcode\\_bar\\";

	void setProjectPath(const QString &path)
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

public:

	bool checkBounty(boundy& bb);

	static bool checkHolm(boundy& bb, Img &tile, int offX, int offY);

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
			return u_texturePath;
		case BackPath::texture2:
			return u_texture2Path;
		case BackPath::object:
			return projectPath + u_modelPath;
		case BackPath::heimap:
			return u_heimapPath;
		case BackPath::root:
			return projectPath;
		case BackPath::markers:
			return projectPath + "markers.lst";
		case BackPath::geojson:
			return u_geojsonPath;
				break;
		default:
			return "";
		}
	}
	
	void findROIsOnHiemap(const PrjgBarCallback &pbCallback, int start, int end);

	void filterROIs(const PrjgBarCallback &pbCallback, bool useBoundyChec, bool useBarcoed, float minBarShooj, bool useCycle, float eps, int start, int maxTile);


	void loadImage(const PrjgBarCallback &pbCallback, QString path, int step, int type, int start =0, int end=0);
	float getImgMaxVal() const;

	float getImgMinVal() const;


	void readGeojson();
	void readMarkers();
	void readMyGeo(bool reinitY);

	void checkCorrect(const QVector<InstanceData> &target, bool skipFirst = false);
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
