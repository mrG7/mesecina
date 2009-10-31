/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Bundle_list_widget.cpp 78 2007-03-25 03:19:20Z miklosb $
 */

#include <gui/layer_management/Bundle_list_widget.h>

Bundle_list_widget::Bundle_list_widget(QListView* list_view, QWidget *parent) : QFrame(parent), bundle_list(list_view) { 
	setup_ui();
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

void Bundle_list_widget::setup_ui() {
	widget_layout = new QVBoxLayout(this);
	widget_layout->setSpacing(2);
    widget_layout->setMargin(0);

	title_label = new QLabel(this);
	title_label->setText("B   U   N   D   L   E   S");
	title_label->setAlignment(Qt::AlignCenter);
	widget_layout->addWidget(title_label);

	widget_layout->addWidget(bundle_list);
}