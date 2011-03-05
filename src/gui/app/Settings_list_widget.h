/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Settings_list_widget.h 586 2009-04-03 20:01:21Z miklosb $
 */

#ifndef SETTINGS_LIST_WIDGET_H
#define SETTINGS_LIST_WIDGET_H

#include <QtGui/QFrame>
#include <QtGui/QCloseEvent>
#include <QtGui/QLineEdit>
#include <QtGui/QTableView>

#include <gui/app/Settings_table_model.h>
#include <gui/app/Settings_table_view.h>
#include <gui/app/Application_variable.h>


class Settings_list_widget : public QFrame {
	Q_OBJECT

public:
	Settings_list_widget(QWidget * parent = 0, QString name = "Statistics");
	void process_filter();
	void load_settings_and_emit_change(const QString& file_name);

signals:
	void closing(bool);

public slots:
	void delete_filter_text(bool);
	void filter_changed(const QString&);
	void settings_changed(const QString& settings_name);

	void load_button(bool);
	void save_button(bool);

signals:
	void application_settings_changed(const QString& settings_name);

public:
	virtual void resizeEvent ( QResizeEvent * e);
	void clear();
	QLineEdit* filter_box;

private:

	Settings_table_model* table_model;
	Settings_table_view *table_view;

};

#endif //SETTINGS_LIST_WIDGET_H