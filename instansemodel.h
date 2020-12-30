#ifndef INSTANSEMODEL_H
#define INSTANSEMODEL_H

#include "imagesearcher.h"

#include <QVector3D>

#include <QAbstractListModel>

struct InstInfo /*: public QObject*/
{
//	Q_OBJECT
	float factor;

public:
	boundy bb;
//	InstInfo() : /*QObject(NULL),*/ bb(0, 0, 0, 0) {}
//	explicit InstInfo(InstInfo &info) :/* QObject(NULL),*/ bb(0, 0, 0, 0)
//	{
//		bb = info.bb;
//		factor = info.factor;
//	}
	/*explicit*/ InstInfo(uint _x, uint _y, float _z, uint _endX, uint _endY, float _endZ) : /*QObject(NULL),*/ bb(_x, _y, _z, _endX, _endY, _endZ) {}

//	Q_PROPERTY(QVector3D trans READ getTrans)
//	Q_PROPERTY(QVector3D textTrans READ getTextTrans)
//	Q_PROPERTY(QVector3D scale READ getSacle)
//	Q_PROPERTY(QString ttext READ getText)
	inline void setFactor(float fact)
	{
		bb.divStep(fact);
		factor = fact;
	}

	QVector3D getTrans() const { return QVector3D(bb.x + bb.wid() / 2, bb.endZ, bb.y + bb.hei() / 2); }
	QVector3D getTextTrans() const { return QVector3D(bb.x, bb.endZ, bb.y); }
	QVector3D getSacle() const { return QVector3D(bb.wid(), 1, bb.hei()); }
	inline QString getText() const
	{
		QString nam;
		//			nam.append("Dimentions:\n");
		nam.append(" L: ");
		nam.append(QString::number(factor*bb.hei()));
		nam.append(" W: ");
		nam.append(QString::number(factor*bb.wid()));
		nam.append(" H: ");
		nam.append(QString::number(bb.zei()));

		qDebug() << nam;
		return nam;
	}
};


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
