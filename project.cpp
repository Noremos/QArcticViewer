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
	QFile loadFile(path);

	if (!loadFile.open(QIODevice::ReadOnly))
	{
		qWarning("Couldn't open save file.");
		return false;
	}

	QByteArray saveData = loadFile.readAll();

	QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

	read(loadDoc.object());
	return true;
}

void Project::read(const QJsonObject &json)
{
	this->modelPath		= json["modelPath"].toString();
	this->heimapPath	= json["heimapPath"].toString();
	this->texturePath	= json["texturePath"].toString();
	this->step			= json["step"].toInt();
	this->imgMaxVal		= json["imgMaxVal"].toDouble();
	this->imgMinVal		= json["imgMaxVal"].toDouble();
	this->materialType	= json["imgMaxVal"].toInt();
}

void Project::write(QJsonObject &json) const
{
	json["model"]		= this->modelPath;
	json["heimapPath"]	= this->heimapPath;
	json["texturePath"] = this->texturePath;
	json["step"]		= this->step;
	json["imgMaxVal"]	= this->imgMaxVal;
	json["imgMaxVal"]	= this->imgMinVal;
	json["imgMaxVal"]	= this->materialType;
}
