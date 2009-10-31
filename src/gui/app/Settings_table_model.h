/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Settings_table_model.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef SETTINGS_TABLE_MODEL_H
#define SETTINGS_TABLE_MODEL_H


#include <QtCore/QAbstractTableModel>
#include <vector>
#include <gui/app/static/Application_settings.h>

class Settings_table_model : public QAbstractTableModel
{
	Q_OBJECT

public:
	Settings_table_model(QObject *parent = 0);
	~Settings_table_model();

	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
//	int non_filtered_count() const;
	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
//	void clear();

//	void set_variable(Application_variable v, bool apply_filter = true);
	void process_filter();
	static void load_current_settings(const QString& file_name);
	void save_current_settings(const QString& file_name);


public slots:
	void filter_apply(const QString& text);

signals:
	void application_settings_changed(const QString& settings_name);


private:
	std::vector<QString> filtered_tooltips;
	std::vector<QString> filtered_names;
	std::vector<Application_setting_type> filtered_types;

	QString filter_text;
};

#endif //SETTINGS_TABLE_MODEL_H