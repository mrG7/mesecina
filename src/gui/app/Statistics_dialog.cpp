/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Statistics_dialog.cpp 98 2007-05-01 23:11:20Z miklosb $
 */

#include <gui/app/Statistics_dialog.h>
#include <QtGui/QVBoxLayout>


Statistics_dialog::Statistics_dialog(QWidget* parent, QString name): QDialog(parent) {
	setModal(false);
	setWindowTitle("Statistics and benchmarks");

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(2);
    layout->setMargin(0);
	
	splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Vertical);
	splitter->setHandleWidth(7);

	statistics_widget = new Statistics_widget(splitter, "Statistics");

	time_widget = new Statistics_widget(splitter, "Times");

	splitter->setStretchFactor(splitter->indexOf(statistics_widget), 0);
	splitter->setStretchFactor(splitter->indexOf(time_widget), 1);

	layout->addWidget(splitter);
	setLayout(layout);
}

void Statistics_dialog::closeEvent(QCloseEvent* e) {
	emit closing(false);
	QDialog::closeEvent(e);
}


void Statistics_dialog::keyPressEvent(QKeyEvent *e) {
	switch (e->key()) {
		case Qt::Key_W:
			if (e->modifiers() == Qt::ControlModifier)
				close();
			break;
	}
}

