/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Statistics_table_model.h 98 2007-05-01 23:11:20Z miklosb $
 */

#ifndef STATISTICS_TABLE_MODEL_H
#define STATISTICS_TABLE_MODEL_H


#include <QtCore/QAbstractTableModel>
#include <vector>
#include <gui/app/Application_variable.h>

class Statistics_table_model : public QAbstractTableModel
{
	Q_OBJECT

public:
	Statistics_table_model(QObject *parent = 0);
	~Statistics_table_model();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
//	int non_filtered_count() const;
	QVariant data(const QModelIndex &index, int role) const;
//	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	void clear();

	void set_variable(Application_variable v, bool apply_filter = true);
	void process_filter();

public slots:
	void filter_apply(const QString& text);

private:
	std::vector<Application_variable> variables;
	std::vector<Application_variable> filtered_variables;

	QString filter_text;
};

#endif //STATISTICS_TABLE_MODEL_H