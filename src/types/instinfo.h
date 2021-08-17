#ifndef INSTINFO_H
#define INSTINFO_H

#include "types.h"

#include <QVector3D>

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
	/*explicit*/ InstInfo(uint _x, uint _y, float _z, uint _endX, uint _endY, float _endZ) : /*QObject(NULL),*/ bb(_x, _y, _z, _endX, _endY, _endZ)
	{
		factor = 0;
	}

//	Q_PROPERTY(QVector3D trans READ getTrans)
//	Q_PROPERTY(QVector3D textTrans READ getTextTrans)
//	Q_PROPERTY(QVector3D scale READ getSacle)
//	Q_PROPERTY(QString ttext READ getText)
	inline void setFactor(float fact)
	{
		bb.divStep(fact);
		factor = fact;
	}

	QVector3D getTrans() const { return QVector3D(bb.x + bb.wid() / 2, bb.endZ + bb.zei()/2 + 1, bb.y + bb.hei() / 2); }
	QVector3D getTextTrans() const { return QVector3D(bb.x, bb.endZ, bb.y); }
	QVector3D getSacle() const { return QVector3D(bb.wid(), 1, bb.hei()); }
	inline QString getText() const
	{
		QString nam;
		//			nam.append("Dimentions:\n");
		nam.append(" L: ");
        nam.append(QString::number(factor * bb.hei() * resol));
		nam.append(" W: ");
        nam.append(QString::number(factor * bb.wid() * resol));
		nam.append(" H: ");
		nam.append(QString::number(bb.zei()));

//		qDebug() << nam;
		return nam;
	}
};
#endif // !INSTINFO_H
