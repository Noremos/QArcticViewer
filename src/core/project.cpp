#include "project.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

Project::Project()
{

}

bool Project::saveProject(QString path)
{
	QFile saveFile(path);

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

bool Project::loadProject(QString path)
{
	projectPath = QDir(path);
	QFile loadFile(path);

	if (!loadFile.open(QIODevice::ReadOnly))
	{
		qWarning("Couldn't open save file.");
		return false;
	}

	QByteArray saveData = loadFile.readAll();

	QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

	read(loadDoc.object());
	
		proj.loadProject(getPath(BackPath::project));
	qDebug() << proj.searchSetts.height.start;
	qDebug() << proj.searchSetts.heightMin();
	proj.notifySettings();
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
