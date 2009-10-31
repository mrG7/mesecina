/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Bundle_list_model.h 102 2007-05-18 02:29:07Z miklosb $
 */

#ifndef BUNDLE_LIST_MODEL_H
#define BUNDLE_LIST_MODEL_H

#include <gui/layer_management/Bundle.h>

#include <QtCore/QAbstractListModel>
#include <vector>


class Bundle_list_model : public QAbstractListModel
{
	Q_OBJECT

public:
	Bundle_list_model(QObject *parent = 0);
	~Bundle_list_model();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value,
		int role = Qt::EditRole);

	Qt::ItemFlags flags(const QModelIndex &index) const;
	Qt::DropActions supportedDropActions() const;
	QStringList mimeTypes() const;
	QMimeData *mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData(const QMimeData *data,
		Qt::DropAction action, int row, int column, const QModelIndex &parent);

	void clear_bundles();
	void add_bundle(const Bundle& n);
	bool remove_bundle(int row, const QModelIndex & parent = QModelIndex());
	Bundle get_bundle(int row);
	void modify_bundle(int i, QByteArray state);
	int find_bundle(QString name);

	QByteArray saveState(int i = -1);
	bool restoreState(const QByteArray &state, bool add = false);

signals:
	void bundle_list_changed();

private:
	static const qint32 token = 0xB3;
	static const qint32 version = 0x01;
	std::vector<Bundle> bundles;
};

#endif //BUNDLE_LIST_MODEL_H