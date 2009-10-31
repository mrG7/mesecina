/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Record_toolbar.h 98 2007-05-01 23:11:20Z miklosb $
 */

#ifndef MESECINA_INPUT_MODE_TOOLBAR_H
#define MESECINA_INPUT_MODE_TOOLBAR_H

#include <QtGui/QToolBar>
#include <QtGui/QPushButton>

typedef enum Input_mode { POINT_INPUT_MODE, CIRCLE_INPUT_MODE };

class Input_mode_toolbar : public QToolBar {
	Q_OBJECT

public:
	Input_mode_toolbar(QWidget * parent = 0);

	QPushButton* point_button;
	QPushButton* circle_button;

	Input_mode input_mode;

private slots:
	void point_button_pushed(bool checked);
	void circle_button_pushed(bool checked);
signals:
	void input_mode_selected(Input_mode input_mode);
};

#endif //MESECINA_INPUT_MODE_TOOLBAR_H