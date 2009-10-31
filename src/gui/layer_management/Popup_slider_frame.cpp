/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Popup_slider_frame.cpp 78 2007-03-25 03:19:20Z miklosb $
 */

#include <gui/layer_management/Popup_slider_frame.h>
#define SLIDER_LENGTH 120


Popup_slider_frame::Popup_slider_frame(QWidget* parent) : QWidgetAction(parent) {
	QWidget* default_widget = new QWidget();

	slider = new QSlider(Qt::Horizontal,default_widget);
	slider->setMinimum(1);
	slider->setMaximum(200);
	slider->setMinimumWidth(SLIDER_LENGTH);
	line_edit = new QLineEdit(default_widget);
	line_edit->setText(tr("1.00"));
	QSize line_size = line_edit->maximumSize();
	line_size.setWidth(40);
	line_edit->setMaximumSize(line_size);
	validator = new QDoubleValidator(line_edit);
	line_edit->setValidator(validator);
	line_edit->setAlignment(Qt::AlignRight);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(slider_value_changed(int)));
	connect(line_edit, SIGNAL(returnPressed()), this, SLOT(value_entered()));


	layout = new QHBoxLayout(default_widget);
	layout->setSpacing(0);
    layout->setMargin(2);
	layout->addWidget(slider);
	layout->addWidget(line_edit);

	min = 1;
	max = 10;
	validator->setBottom(min);
	validator->setTop(max);

	setDefaultWidget(default_widget);
}

void Popup_slider_frame::set_range(float min, float max) {
	this->min = min;
	this->max = max;
	validator->setBottom(min);
	validator->setTop(max);
}

float Popup_slider_frame::value_to_float() {
	return min + (float)(slider->value()-slider->minimum()) / (slider->maximum() - slider->minimum()) * (max - min);
}

void Popup_slider_frame::set_float_value(float val) {
	slider->setValue((int)(slider->minimum() + (val-min)/(max-min) * (slider->maximum()-slider->minimum())));
}

void Popup_slider_frame::slider_value_changed(int) {
	line_edit->setText(tr("%1").arg(value_to_float(), 4, 'f', 2));
}

void Popup_slider_frame::value_entered() {
	double val = line_edit->text().toDouble();
	int value_on_slider = (int)(slider->minimum() + (val-min)/(max-min) * (slider->maximum()-slider->minimum()));
	bool increased = false;
	while (slider->value() < value_on_slider) {
		slider->setValue(slider->value()+1);
		slider->repaint();
		increased = true;
	}
	if (increased) return;
	while (slider->value() > value_on_slider+1) {
		slider->setValue(slider->value()-1);
		slider->repaint();
	}

}