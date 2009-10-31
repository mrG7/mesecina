/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Popup_scalar_controls_frame.cpp 531 2009-02-05 22:02:06Z miklosb $
 */

#include <gui/layer_management/Popup_scalar_controls_frame.h>
#define SLIDER_LENGTH 120

Popup_scalar_controls_frame::Popup_scalar_controls_frame(QWidget* parent) : QWidgetAction(parent) {
	QWidget* default_widget = new QWidget();
	layout = new QVBoxLayout(default_widget);
	layout->setSpacing(0);
    layout->setMargin(2);

	min_slider = new QSlider(Qt::Horizontal,default_widget);
	min_slider->setMinimum(1);
	min_slider->setMaximum(200);
	min_slider->setMinimumWidth(SLIDER_LENGTH);
	min_line_edit = new QLineEdit(default_widget);
	min_line_edit->setText(tr("1.00"));
	QSize min_line_size = min_line_edit->maximumSize();
	min_line_size.setWidth(60);
	min_line_edit->setMaximumSize(min_line_size);
	min_validator = new QDoubleValidator(min_line_edit);
	min_line_edit->setValidator(min_validator);
	min_line_edit->setAlignment(Qt::AlignRight);
	connect(min_slider, SIGNAL(valueChanged(int)), this, SLOT(min_slider_value_changed(int)));
	connect(min_line_edit, SIGNAL(returnPressed()), this, SLOT(min_value_entered()));

	min_layout = new QHBoxLayout();
	min_layout->setSpacing(0);
    min_layout->setMargin(2);
	min_layout->addWidget(min_slider);
	min_layout->addWidget(min_line_edit);
	layout->addLayout(min_layout);


	max_slider = new QSlider(Qt::Horizontal,default_widget);
	max_slider->setMinimum(1);
	max_slider->setMaximum(200);
	max_line_edit = new QLineEdit(default_widget);
	max_line_edit->setText(tr("1.00"));
	QSize max_line_size = max_line_edit->maximumSize();
	max_line_size.setWidth(60);
	max_line_edit->setMaximumSize(max_line_size);
	max_validator = new QDoubleValidator(max_line_edit);
	max_line_edit->setValidator(max_validator);
	max_line_edit->setAlignment(Qt::AlignRight);
	connect(max_slider, SIGNAL(valueChanged(int)), this, SLOT(max_slider_value_changed(int)));
	connect(max_line_edit, SIGNAL(returnPressed()), this, SLOT(max_value_entered()));

	max_layout = new QHBoxLayout();
	max_layout->setSpacing(0);
    max_layout->setMargin(2);
	max_layout->addWidget(max_slider);
	max_layout->addWidget(max_line_edit);
	layout->addLayout(max_layout);

	min = 1;
	max = 10;
	min_validator->setBottom(min);
	min_validator->setTop(max);
	max_validator->setBottom(min);
	max_validator->setTop(max);

	setDefaultWidget(default_widget);
}

void Popup_scalar_controls_frame::set_range(float min, float max) {
	this->min = min;
	this->max = max;
	min_validator->setBottom(min);
	min_validator->setTop(max);
	max_validator->setBottom(min);
	max_validator->setTop(max);
}

float Popup_scalar_controls_frame::min_value_to_float() {
	return min_line_edit->text().toDouble();
//	return min + (float)(min_slider->value()-min_slider->minimum()) / (min_slider->maximum() - min_slider->minimum()) * (max - min);
}

float Popup_scalar_controls_frame::max_value_to_float() {
	return max_line_edit->text().toDouble();
//	return min + (float)(max_slider->value()-max_slider->minimum()) / (max_slider->maximum() - max_slider->minimum()) * (max - min);
}

void Popup_scalar_controls_frame::set_min_float_value(float val) {
	min_slider->setValue((int)(min_slider->minimum() + (val-min)/(max-min) * (min_slider->maximum()-min_slider->minimum())));
	min_line_edit->setText(tr("%1").arg(val));
}

void Popup_scalar_controls_frame::set_max_float_value(float val) {
	max_slider->setValue((int)(max_slider->minimum() + (val-min)/(max-min) * (max_slider->maximum()-max_slider->minimum())));
	max_line_edit->setText(tr("%1").arg(val));
}

void Popup_scalar_controls_frame::min_slider_value_changed(int) {
	if (min_slider->value() > max_slider->value()) 
		max_slider->setValue(min_slider->value());
	min_line_edit->setText(tr("%1").arg(min + (float)(min_slider->value()-min_slider->minimum()) / (min_slider->maximum() - min_slider->minimum()) * (max - min), 4, 'f', 2));
}

void Popup_scalar_controls_frame::max_slider_value_changed(int) {
	if (max_slider->value() < min_slider->value()) 
		min_slider->setValue(max_slider->value());
	max_line_edit->setText(tr("%1").arg(min + (float)(max_slider->value()-max_slider->minimum()) / (max_slider->maximum() - max_slider->minimum()) * (max - min), 4, 'f', 2));
}

void Popup_scalar_controls_frame::set_color_map(QString name) {
	//set the color_map
}


void Popup_scalar_controls_frame::min_value_entered() {
	double val = min_line_edit->text().toDouble();
	int value_on_slider = (int)(min_slider->minimum() + (val-min)/(max-min) * (min_slider->maximum()-min_slider->minimum()));
	bool increased = false;
	while (min_slider->value() < value_on_slider) {
		min_slider->setValue(min_slider->value()+1);
		min_slider->repaint();
		increased = true;
	}
	if (!increased)	while (min_slider->value() > value_on_slider+1) {
		min_slider->setValue(min_slider->value()-1);
		min_slider->repaint();
	}
	min_line_edit->setText(tr("%1").arg(val));
	if (max_value_to_float()<val) {
		max_line_edit->setText(tr("%1").arg(val));
	}
}

void Popup_scalar_controls_frame::max_value_entered() {
	double val = max_line_edit->text().toDouble();
	int value_on_slider = (int)(max_slider->minimum() + (val-min)/(max-min) * (max_slider->maximum()-max_slider->minimum()));
	bool increased = false;
	while (max_slider->value() < value_on_slider) {
		max_slider->setValue(max_slider->value()+1);
		max_slider->repaint();
		increased = true;
	}
	if (!increased)	while (max_slider->value() > value_on_slider+1) {
		max_slider->setValue(max_slider->value()-1);
		max_slider->repaint();
	}
	max_line_edit->setText(tr("%1").arg(val));
	if (min_value_to_float()>val) {
		min_line_edit->setText(tr("%1").arg(val));
	}
}