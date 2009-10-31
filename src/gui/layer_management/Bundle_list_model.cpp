/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Bundle_list_model.cpp 351 2008-04-18 18:25:24Z miklosb $
 */

#include <QtCore/QMimeData>
#include <QtGui/QColor>
#include <QtGui/QMessageBox>

#include <gui/layer_management/Bundle_list_model.h>
#include <constants.h>


Bundle_list_model::Bundle_list_model(QObject *parent) : QAbstractListModel(parent) {
	bundles.clear();
}

Bundle_list_model::~Bundle_list_model() {
}

int Bundle_list_model::rowCount(const QModelIndex &) const {
	return (int)bundles.size();
}

QVariant Bundle_list_model::data(const QModelIndex &index, int role) const {
     if (!index.isValid())
         return QVariant();

     if (index.row() >= (int)bundles.size())
         return QVariant();

	 Bundle b(bundles[index.row()]);
	 switch (role) {
		 case Qt::DisplayRole:
			 return b.get_name();
			 break;
		 case Qt::EditRole:
			 return b.get_name();
			 break;
		 default:
			 return QVariant();
	 }
}

Qt::ItemFlags Bundle_list_model::flags(const QModelIndex &index) const {
     Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

     if (index.isValid())
         return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
     else
         return Qt::ItemIsDropEnabled | defaultFlags;
}

bool Bundle_list_model::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && role == Qt::EditRole) {
		if (value.toString() != "") {
			if (find_bundle(value.toString())!=-1)
				/// \todo Make the message box pop up from the mainwindow, as parent for the renaming of the bundle error message
				QMessageBox::critical(0, tr("%1 - Renaming bundle").arg(APPLICATION_NAME),
                              QString("Bundle has not been renamed!\nBundle <%1> already exists, choose another name!").arg(value.toString()));
			else {
				bundles[index.row()].rename(value.toString());
				emit dataChanged(index, index);
				emit bundle_list_changed();
			}
		}
		return true;
	}
	return false;
}

void Bundle_list_model::add_bundle(const Bundle& b) {
	for (unsigned i=0; i < bundles.size(); i++) {
		if (bundles[i].get_name()==b.get_name()) return;
	}
	beginInsertRows(QModelIndex(), rowCount(), rowCount()+1);
	bundles.push_back(b);
	endInsertRows();
	emit bundle_list_changed();
}

void Bundle_list_model::clear_bundles() {
	beginRemoveRows(QModelIndex(), 0, (int)bundles.size());
	bundles.clear();
    endRemoveRows();
	emit bundle_list_changed();
}

void Bundle_list_model::modify_bundle(int i, QByteArray state) {
	bundles[i].restoreState(state);
	emit bundle_list_changed();
}

int Bundle_list_model::find_bundle(QString name) {
	int idx = 0;
	std::vector<Bundle>::iterator b_it, b_end = bundles.end();
	for (b_it = bundles.begin(); b_it != b_end; b_it++, idx++) {
		if (b_it->get_name()==name) return idx;
	}
	return -1;

}

bool Bundle_list_model::remove_bundle(int row, const QModelIndex & ) {
	beginRemoveRows(QModelIndex(), row, row);
	std::vector<Bundle>::iterator n_it = bundles.begin();
	for (int i=0; i != row; n_it++, i++);
	bundles.erase(n_it);
    endRemoveRows();
	emit bundle_list_changed();
    return true;
}
Bundle Bundle_list_model::get_bundle(int row) {
	return bundles[row];
}

Qt::DropActions Bundle_list_model::supportedDropActions() const {
	return Qt::MoveAction;
}

QStringList Bundle_list_model::mimeTypes() const {
	QStringList types;
	types << "application/mesecina.bundle";
	return types;
}

QMimeData* Bundle_list_model::mimeData(const QModelIndexList &indexes) const {
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	foreach (QModelIndex index, indexes) {
		if (index.isValid()) {
			int idx = index.row();
			stream << idx;
		}
	}

	mimeData->setData("application/mesecina.bundle", encodedData);
	return mimeData;
}

bool Bundle_list_model::dropMimeData(const QMimeData *data, Qt::DropAction action,
	int row, int column, const QModelIndex &parent) {

	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat("application/mesecina.bundle"))
		return false;

	if (column > 0)
		return false;

	int beginRow;

	if (row != -1)
		beginRow = row;
	else if (parent.isValid())
		beginRow = parent.row();
	else
		beginRow = rowCount(QModelIndex());

	QByteArray encodedData = data->data("application/mesecina.bundle");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);

	int idx;
	stream >> idx;
	if (idx<beginRow && row == -1) beginRow++;
	int i;
	std::vector<Bundle>::iterator n_it = bundles.begin();
	for (i=0; i != beginRow; n_it++, i++);
	bundles.insert(n_it, bundles[idx]);

	if (i<idx) idx++;
	n_it = bundles.begin();
	for (i=0; i != idx; n_it++, i++);
	bundles.erase(n_it);

	emit bundle_list_changed();
	return true;
}


QByteArray Bundle_list_model::saveState(int i) {
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);

	stream << qint32(token);
	stream << qint32(version);


	if (i==-1) {
		std::vector<Bundle>::iterator b_it, b_end = bundles.end();
		for (b_it = bundles.begin(); b_it != b_end; b_it++) {
			stream << b_it->get_name();
			stream << b_it->saveState();
		}
	} else {
		Bundle b = bundles[i];
		stream << b.get_name();
		stream << b.saveState();
	}

	return data;
}

bool Bundle_list_model::restoreState(const QByteArray &state, bool add) {
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);

	qint32 token;
    qint32 v;
    stream >> token;
    stream >> v;
    if (token != token || v != version)
        return false;

	QString bundle_name;
	QByteArray bundle_state;
	if (!add) bundles.clear();
	while (! stream.atEnd()) {
		stream >> bundle_name;
		stream >> bundle_state;
		Bundle b(bundle_name);
		b.restoreState(bundle_state);
		add_bundle(b);
	}

    return true;
}