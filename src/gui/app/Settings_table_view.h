/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Bundle_list_view.h 102 2007-05-18 02:29:07Z miklosb $
 */

#ifndef MESECINA_SETTINGS_TABLE_VIEW_H
#define MESECINA_SETTINGS_TABLE_VIEW_H

#include <QtGui/QTableView>
#include <vector>

class Popup_animate_application_setting;

class Settings_table_view : public QTableView {
	Q_OBJECT

public:
	Settings_table_view(QWidget *parent = 0);
	~Settings_table_view();

protected:
	virtual void mouseReleaseEvent(QMouseEvent * event);

public:
	void context_menu(QMouseEvent * event, const QModelIndex &index);

	Popup_animate_application_setting* animate_popup;
};

#endif //MESECINA_SETTINGS_TABLE_VIEW_H