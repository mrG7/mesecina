/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Record_toolbar.h 98 2007-05-01 23:11:20Z miklosb $
 */

#ifndef RECORD_TOOLBAR_H
#define RECORD_TOOLBAR_H

#include <QtGui/QToolBar>
#include <QtGui/QPushButton>


class Record_toolbar : public QToolBar {
	Q_OBJECT

public:
	Record_toolbar(QWidget * parent = 0);

	QPushButton* record_button;
private slots:
	void set_button_color(bool checked);
};

#endif //RECORD_TOOLBAR_H