/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Record_toolbar.cpp 98 2007-05-01 23:11:20Z miklosb $
 */

#include <gui/app/Record_toolbar.h>
#include <QtGui/QColorGroup>
//#include

Record_toolbar::Record_toolbar(QWidget* parent): QToolBar(parent) {
	this->setObjectName("Recording toolbar");
	setMovable(false);

	record_button = new QPushButton("Record",this);
	record_button->setCheckable(true);
	//record_button->setFlat(true);
	addWidget(record_button);

	connect(record_button, SIGNAL(toggled(bool)), this, SLOT(set_button_color(bool)));
}

void Record_toolbar::set_button_color(bool checked) {
	QPalette pal = record_button->palette();
	if (checked) {
		pal.setColor(QPalette::ButtonText, QColor(255,0,0));
		record_button->setPalette(pal);
		record_button->setText("Recording...");
	} else {
		pal.setColor(QPalette::ButtonText, QColor(0,0,0));
		record_button->setPalette(pal);
		record_button->setText("Record");
	}
}
