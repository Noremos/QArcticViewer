#ifndef PROJECT_PARAMS_H
#define PROJECT_PARAMS_H

#include "imagesearcher.h"

#include <QJsonObject>
#include <QString>

class ProjectParametrs : public QObject
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
	ProjectParametrs();


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
        emit meterialtypeChanged(materialType);
    }


	QString modelPath;
	QString heimapPath;
	QString texturePath, texture2Path;
	int step;
	float imgMaxVal;
	float imgMinVal;
	int materialType;
public:
	SeachingSettings searchSetts;

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
