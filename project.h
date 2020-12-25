#ifndef PROJECT_H
#define PROJECT_H

#include <QJsonObject>
#include <QString>

class Project
{

public:
	Project();

	bool loadProject(QString path);
	bool saveProject(QString path);
private:
	void write(QJsonObject &json) const;
	void read(const QJsonObject &json);


	QString modelPath;
	QString heimapPath;
	QString texturePath;
	int step;
	float imgMaxVal;
	float imgMinVal;
	int materialType;
};

#endif // PROJECT_H
