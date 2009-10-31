/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Bundle_list_widget.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef BUNDLE_LIST_WIDGET_H
#define BUNDLE_LIST_WIDGET_H

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>

#include <QtGui/QVBoxLayout>
#include <QtGui/QFrame>

#include <gui/layer_management/Layer_list_view.h>

class Bundle_list_widget : public QFrame {

public:
	Bundle_list_widget(QListView* list_view, QWidget *parent = 0);
	//~Layer_list_widget();

	void setup_ui();

	QVBoxLayout *widget_layout;
	QLabel *title_label;
	QListView *bundle_list;
};

#endif //BUNDLE_LIST_WIDGET_H