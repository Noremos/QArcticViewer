#ifndef INSTANSEMODEL_H
#define INSTANSEMODEL_H

#include "imagesearcher.h"

#include <QVector3D>

#include <QAbstractListModel>

#include "../types/instinfo.h"

struct lowline
{
    lowline()
    {
        start = 0;
        len = 0;
        bar3d = nullptr;
    }
    bool isEmpty()
    {
        return bar3d==nullptr;
    }
    lowline(barline<float>* org)
    {
        this->start = org->start;
        this->len = org->len;
        this->bar3d = org->bar3d;
        org->bar3d = nullptr;
    }
    float start;
    float len;
    bc::barcounter<float>* bar3d;
    float end()
    {
        return start+len;
    }
    void getStr(QString & str)
    {
        if (isEmpty())
            return;

       str+= QString::number(start) + " " +  QString::number(len);
       qDebug() << bar3d->size();
       for(auto& l:(*bar3d))
       {
           str+= " " + QString::number(l.value)+ " " + QString::number(l.count);
       }
    }


};
typedef vector<lowline*> veclines;

//struct InstInfo : public boundy, QObject
//{
//	Q_OBJECT
//	QString text;

//};
//Q_DECLARE_METATYPE(InstInfo)

class InstanseModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum BBYtypes { mtrans = Qt::UserRole + 1, mscale, mtextTrans, mttext };
	Q_ENUM(BBYtypes)

	QList<InstInfo *> boundydata;

	void clearAll();
	void updateAll();

	explicit InstanseModel(QObject *parent = nullptr);
	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	virtual ~InstanseModel();

private:

	// QAbstractItemModel interface
public:
	QHash<int, QByteArray> roleNames() const override;
};

#endif // INSTANSEMODEL_H
