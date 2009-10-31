/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Popup_slider_frame.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef POPUP_SLIDER_FRAME_H
#define POPUP_SLIDER_FRAME_H

#include <QtGui/QWidgetAction>
#include <QtGui/QLineEdit>
#include <QtGui/QSlider>
#include <QtGui/QHBoxLayout>
#include <QtGui/QDoubleValidator>

class Popup_slider_frame : public QWidgetAction {
	Q_OBJECT

public:
	Popup_slider_frame(QWidget* parent);
	// destructor might be needed because we are are allocating memory in constructor
	
	QSlider *slider;
	QHBoxLayout *layout;
	QLineEdit *line_edit;

	void set_range(float min, float max);

	float value_to_float();
	void set_float_value(float val);

public slots:
	void slider_value_changed(int value);
	void value_entered();

private:
	float min, max;
	QDoubleValidator* validator;
	
};

#endif //POPUP_SLIDER_FRAME_H