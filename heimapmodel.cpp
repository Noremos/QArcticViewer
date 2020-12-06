#include "heimapmodel.h"
#include "tiffreader.h"

HeiMapModel::HeiMapModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

QHash<int, QByteArray> HeiMapModel::roleNames() const
{
	return { {Qt::DisplayRole, "display"}, {Qt::UserRole+1, "row"}, {Qt::UserRole+2, "column"}, {Qt::UserRole+3, "value"} };
}

QVariant HeiMapModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
{
	return QVariant();
	// FIXME: Implement me!
}

void HeiMapModel::loadImage(QString path)
{
	if (reader)
	{
		delete reader;
		reader = nullptr;
	}

	int type = 0;
	switch (type)
	{
	default:
		reader = new TiffReader();
		break;
	}
	reader->open(path.toStdWString().c_str());

	beginInsertRows(QModelIndex(), 0, reader->height() * reader->widght());
	sdata = new float *[reader->height()];
	for (int i = 0; i < reader->height(); ++i)
	{
		sdata[i] = reinterpret_cast<float*>(reader->getRowData(i));
	}
	endInsertRows();
}
//const int L= 1000;
int HeiMapModel::rowCount(const QModelIndex &parent) const
{
	// For list models only the root node (an invalid parent) should return the list's size. For all
	// other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
	if (parent.isValid() || reader==nullptr)
		return 0;

	return reader->height() * reader->widght();
//	return L*L;
	// FIXME: Implement me!
}

QVariant HeiMapModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()|| reader==nullptr)
		return QVariant();

	int row = getRow(index);
	int col = getColumn(index);
	switch (role)
	{
	case Qt::UserRole + 1: //row
		return row;

	case Qt::UserRole + 2: //col
		return col;

	case Qt::UserRole + 3: //value
		return sdata[row][col];
		void *data = reader->getRow(row);
		switch (reader->getType())
		{
		case ImageType::float32:
			float *fdata = reinterpret_cast<float *>(data);

			//		HeiValue val;
			//		val.row = row;
			//		val.col = col;
			//		val.value = fdata[index.column()];
			//		return val.value;
			//QVariant::fromValue(val);
//			qDebug() << "row:" << row << " col:" << col << " value:" << fdata[col];
			return fdata[col];
		}
		return QVariant();
	}

	// FIXME: Implement me!
	return QVariant();
}

QVariant HeiMapModel::getData(const QModelIndex &index) const
{
	if (!index.isValid()|| reader==nullptr)
		return QVariant ();

	void *data = reader->getRow(index.row());

	switch (reader->getType())
	{
	case ImageType::float32:
		float *fdata = reinterpret_cast<float *>(data);
//		HeiValue val;
//		val.row = row;
//		val.col = col;
//		val.value = fdata[index.column()];
		return  fdata[index.column()];
		//QVariant::fromValue(val);
		break;

	}
	return QVariant();
}

qint32 HeiMapModel::getRow(const QModelIndex &index) const
{
	return index.row() / reader->widght();
//	return L;
	//reader->widght();
}

qint32 HeiMapModel::getColumn(const QModelIndex &index) const
{
	return index.row() % reader->widght();
//	return index.row() % L;
}

//int HeiMapModel::columnCount(const QModelIndex &parent) const
//{
//	return  reader->widght();
//}
