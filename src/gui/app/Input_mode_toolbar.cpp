/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Record_toolbar.cpp 98 2007-05-01 23:11:20Z miklosb $
 */

#include <gui/app/Input_mode_toolbar.h>
#include <QtGui/QColorGroup>
//#include

Input_mode_toolbar::Input_mode_toolbar(QWidget* parent): QToolBar(parent) {
	this->setObjectName("Input toolbar");
	setMovable(false);

	point_button = new QPushButton("Point",this);
	point_button->setCheckable(true);
	point_button->setMaximumWidth(45);
	circle_button = new QPushButton("Circle",this);
	circle_button->setCheckable(true);
	circle_button->setMaximumWidth(45);
	//record_button->setFlat(true);
	addWidget(point_button);
	addWidget(circle_button);

	connect(point_button, SIGNAL(toggled(bool)), this, SLOT(point_button_pushed(bool)));
	connect(circle_button, SIGNAL(toggled(bool)), this, SLOT(circle_button_pushed(bool)));

	input_mode = POINT_INPUT_MODE;
	point_button->setChecked(true);
	emit input_mode_selected(input_mode);
}

void Input_mode_toolbar::point_button_pushed(bool checked) {
	if (checked) {
		if (circle_button->isChecked()) circle_button->setChecked(false);
		input_mode = POINT_INPUT_MODE;
		emit input_mode_selected(input_mode);
	}
}

void Input_mode_toolbar::circle_button_pushed(bool checked) {
	if (checked) {
		if (point_button->isChecked()) point_button->setChecked(false);
		input_mode = CIRCLE_INPUT_MODE;
		emit input_mode_selected(input_mode);
	}
}

