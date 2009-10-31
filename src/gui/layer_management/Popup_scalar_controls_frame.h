/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Popup_scalar_controls_frame.h 255 2007-12-03 23:01:56Z miklosb $
 */

#ifndef POPUP_SCALAR_CONTROLS_FRAME_H
#define POPUP_SCALAR_CONTROLS_FRAME_H

#include <QtGui/QWidgetAction>
#include <QtGui/QLineEdit>
#include <QtGui/QSlider>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDoubleValidator>

class Popup_scalar_controls_frame : public QWidgetAction {
	Q_OBJECT

public:
	Popup_scalar_controls_frame(QWidget* parent);
	// destructor might be needed because we are are allocating memory in constructor
	
	QVBoxLayout *layout;
	QHBoxLayout *min_layout, *max_layout, *color_layout;
	QSlider *min_slider, *max_slider;
	QLineEdit *min_line_edit, *max_line_edit;
	QAction *solid_color;

	void set_range(float min, float max);

	float min_value_to_float();
	void set_min_float_value(float val);

	float max_value_to_float();
	void set_max_float_value(float val);

	void set_color_map(QString name);

public slots:
	void min_slider_value_changed(int value);
	void max_slider_value_changed(int value);
	void min_value_entered();
	void max_value_entered();

private:
	float min, max;
	QDoubleValidator *min_validator, *max_validator;
	
};

#endif //POPUP_SCALAR_CONTROLS_FRAME_H