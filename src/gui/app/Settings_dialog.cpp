/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Settings_dialog.cpp 123 2007-07-01 21:43:01Z miklosb $
 */

#include <gui/app/Settings_dialog.h>
#include <QtGui/QVBoxLayout>


Settings_dialog::Settings_dialog(QWidget* parent, QString name): QDialog(parent) {
	setModal(false);
	setWindowTitle("Application settings");

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(2);
    layout->setMargin(0);
	
	settings_list_widget = new Settings_list_widget(this, "Statistics");

	layout->addWidget(settings_list_widget);
	setLayout(layout);
}

void Settings_dialog::closeEvent(QCloseEvent* e) {
	emit closing(false);
	QDialog::closeEvent(e);
}


void Settings_dialog::keyPressEvent(QKeyEvent *e) {
	switch (e->key()) {
		case Qt::Key_E:
			if (e->modifiers() == Qt::ControlModifier)
				close();
			break;
	}
}