/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Help_browser_dialog.cpp 103 2007-05-19 03:16:07Z miklosb $
 */

#include <gui/app/Help_browser_dialog.h>
#include <QtGui/QVBoxLayout>


Help_browser_dialog::Help_browser_dialog(QWidget* parent): QDialog(parent) {
	setModal(false);
	setWindowTitle("Manual");

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(2);
    layout->setMargin(0);
	

	text_browser = new QTextBrowser(this);

	layout->addWidget(text_browser);
	setLayout(layout);
}

void Help_browser_dialog::closeEvent(QCloseEvent* e) {
	emit closing(false);
	QDialog::closeEvent(e);
}

void Help_browser_dialog::keyPressEvent(QKeyEvent *e) {
	switch (e->key()) {
		case Qt::Key_F1:
			close();
			break;
	}
}