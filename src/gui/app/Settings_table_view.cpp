/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Bundle_list_view.cpp 120 2007-06-22 01:50:19Z miklosb $
 */

#include <gui/app/Settings_table_view.h>

#include <QtGui/QMouseEvent>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <iostream>

#include <gui/app/static/Application_settings.h>
#include <gui/app/Popup_animate_application_setting.h>



Settings_table_view::Settings_table_view(QWidget *parent) : QTableView(parent) {
	animate_popup = new Popup_animate_application_setting(this);
}

Settings_table_view::~Settings_table_view() {
	delete animate_popup;
}

void Settings_table_view::context_menu(QMouseEvent * event, const QModelIndex &index) {
	if (!index.isValid() || index.row()==0) return;
	int ind = index.data(Qt::UserRole).toInt();
	if (Application_settings::setting_types[ind] != AS_DOUBLE && Application_settings::setting_types[ind] != AS_INTEGER) {
		std::cout << "One can animate currently only integer or double variables" << std::endl;
		return;
	}

	animate_popup->move(event->globalPos());
	animate_popup->set_application_variable(Application_settings::setting_names[ind], Application_settings::setting_types[ind]);
	animate_popup->show();
}

void Settings_table_view::mouseReleaseEvent(QMouseEvent * event) {
	QTableView::mouseReleaseEvent(event);

	QPoint pos = event->pos();
	QPersistentModelIndex index = indexAt(pos);
	if (event->button() & Qt::RightButton)
		context_menu(event, index);
}