#ifndef HEIMAPMODEL_H
#define HEIMAPMODEL_H

#include <QAbstractListModel>
#include <qqml.h>

#include <tiffreader.h>

struct HeiValue
{
	Q_GADGET
public:
	qint32 col = -1;
	qint32 row = -1;
	QVariant value;
	Q_PROPERTY(qint32 col MEMBER col)
	Q_PROPERTY(qint32 row MEMBER row)
	Q_PROPERTY(QVariant value MEMBER value)
};

class HeiMapModel : public QAbstractListModel
{
	Q_OBJECT
	QML_ELEMENT

	ImageReader* reader = nullptr;
public:
	explicit HeiMapModel(QObject *parent = nullptr);

	QHash<int, QByteArray> roleNames() const override;
	float **sdata;
	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	Q_INVOKABLE void loadImage(QString path);
	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
//	int columnCount(const QModelIndex &parent) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	Q_INVOKABLE QVariant getData(const QModelIndex &index) const;
	Q_INVOKABLE qint32 getRow(const QModelIndex &index) const;
	Q_INVOKABLE qint32 getColumn(const QModelIndex &index) const;


	~HeiMapModel()
	{
		for (int i = 0; i < reader->height(); ++i)
		{
			delete[] sdata[i];
		}
		delete[] sdata;
		if (reader)	delete reader;
	}
};

#endif // HEIMAPMODEL_H
