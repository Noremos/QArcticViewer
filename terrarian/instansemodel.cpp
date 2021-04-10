#include "instansemodel.h"

QHash<int, QByteArray> InstanseModel::roleNames() const
{
	QHash<int, QByteArray> rez;

	rez[BBYtypes::mtrans] = "mtrans";
	rez[BBYtypes::mscale] = "mscale";
	rez[BBYtypes::mtextTrans] = "mtextTrans";
	rez[mttext] = "mttext";
	return rez;
}

void InstanseModel::clearAll()
{
	if (boundydata.size()==0)
		return;
	beginRemoveRows(QModelIndex(), 0, boundydata.size()-1);
	for (int i = 0; i < boundydata.size(); ++i)
	{
		delete boundydata[i];
	}
	boundydata.clear();
	endRemoveRows();
}

void InstanseModel::updateAll()
{
	beginInsertRows(QModelIndex(), 0, boundydata.size()-1);
	endInsertRows();
    qDebug() << "updated" << boundydata.size();
}

InstanseModel::InstanseModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

QVariant InstanseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// FIXME: Implement me!
	return QVariant();
}

int InstanseModel::rowCount(const QModelIndex &parent) const
{
	// For list models only the root node (an invalid parent) should return the list's size. For all
	// other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
//	if (parent.isValid())
		return boundydata.size();
//	return 0;
	// FIXME: Implement me!
}

QVariant InstanseModel::data(const QModelIndex &index, int role) const
{
//	qDebug() << role << index.isValid();
	if (!index.isValid())
		return QVariant();

	switch ((BBYtypes)role) {
	case InstanseModel::mttext:
		return boundydata[index.row()]->getText();
	case InstanseModel::mtrans:
		return boundydata[index.row()]->getTrans();
	case InstanseModel::mtextTrans:
		return boundydata[index.row()]->getTextTrans();
	case InstanseModel::mscale:
		return boundydata[index.row()]->getSacle();

	}
	// FIXME: Implement me!
	return QVariant();
//	::fromValue(*boundydata[index.row()]);
}

InstanseModel::~InstanseModel()
{
	clearAll();
}
