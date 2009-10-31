/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Popup_scalar_controls_frame.cpp 402 2008-12-16 19:54:01Z miklosb $
 */

#include <gui/app/Popup_animate_application_setting.h>
#include <iostream>
#include <QtCore/QSettings>

Popup_animate_application_setting::Popup_animate_application_setting(QWidget* parent) : QFrame(parent,  Qt::Popup) {
	setFrameShape(QFrame::StyledPanel);

	application_variable_name = QString();

	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(2);
	gridLayout->setMargin(4);

	title_label = new QLabel("Title");
	title_label->setAlignment(Qt::AlignCenter);

	from = new QLineEdit(); from->setAlignment(Qt::AlignRight); from->setMaximumWidth(60);
	until = new QLineEdit(); until->setAlignment(Qt::AlignRight); until->setMaximumWidth(60);
	steps = new QLineEdit(); steps->setAlignment(Qt::AlignRight); steps->setMaximumWidth(60);
	connect(from, SIGNAL(textChanged( const QString &)), this, SLOT(from_changed( const QString &)));
	connect(until, SIGNAL(textChanged( const QString &)), this, SLOT(until_changed( const QString &)));
	connect(steps, SIGNAL(textChanged( const QString &)), this, SLOT(steps_changed( const QString &)));

	QLabel* from_label = new QLabel("from");
	QLabel* until_label = new QLabel(" until");
	QLabel* steps_label = new QLabel(" steps");
	
	slider = new QSlider(Qt::Horizontal);
	connect(slider, SIGNAL(valueChanged ( int )), this, SLOT(slider_value_changed( int )));


	forward_button = new QPushButton("Forward");
	back_button = new QPushButton("Back");
	connect(forward_button, SIGNAL(clicked()), this, SLOT(animate_up()));
	connect(back_button, SIGNAL(clicked()), this, SLOT(animate_down()));

	val_label = new QLabel();
	val_label->setAlignment(Qt::AlignCenter);

	gridLayout->addWidget(title_label, 0, 0, 1, 7);

	gridLayout->addWidget(from_label, 1, 0, Qt::AlignRight);
	gridLayout->addWidget(from, 1, 1);
	gridLayout->addWidget(until_label, 1, 3, Qt::AlignRight);
	gridLayout->addWidget(until, 1, 4);
	gridLayout->addWidget(steps_label, 1, 5, Qt::AlignRight);
	gridLayout->addWidget(steps, 1, 6);

	gridLayout->addWidget(slider, 2, 0, 1, 7);


	gridLayout->addWidget(back_button, 3, 0, 1, 4);
	gridLayout->addWidget(val_label, 3, 4);
	gridLayout->addWidget(forward_button, 3, 5, 1, 3);

	setLayout(gridLayout);
}

void Popup_animate_application_setting::set_application_variable(QString& app_variable_name, Application_setting_type t) {
	application_variable_name = app_variable_name;
	type = t;
	title_label->setText(tr("Animate %1").arg(application_variable_name));
	QSettings settings;

	//load last values for from until steps
	QVariant val; bool ok; int i; double d;
	switch (type) {
		case AS_INTEGER:
			val = settings.value(tr("default/from--%1").arg(application_variable_name), 0);
			i = val.toInt(&ok);
			if (ok) from->setText(val.toString());

			val = settings.value(tr("default/until--%1").arg(application_variable_name), 10);
			i = val.toInt(&ok);
			if (ok) until->setText(val.toString());

			val = settings.value(tr("default/steps--%1").arg(application_variable_name), 1);
			i = val.toInt(&ok);
			if (ok) steps->setText(val.toString());
			break;
		case AS_DOUBLE:
			val = settings.value(tr("default/from--%1").arg(application_variable_name), 0);
			d = val.toDouble(&ok);
			if (ok) from->setText(val.toString());

			val = settings.value(tr("default/until--%1").arg(application_variable_name), 1);
			d = val.toDouble(&ok);
			if (ok) until->setText(val.toString());

			val = settings.value(tr("default/steps--%1").arg(application_variable_name), 0.1);
			d = val.toDouble(&ok);
			if (ok) steps->setText(val.toString());
			break;
		default:
			break;
	}
}


void Popup_animate_application_setting::from_changed(const QString & text) {
	QSettings settings;
	int i; double d; bool ok;
	switch (type) {
		case AS_INTEGER:
			i = text.toInt(&ok);
			if (ok) settings.setValue(tr("default/from--%1").arg(application_variable_name), text);
			break;
		case AS_DOUBLE:
			d = text.toDouble(&ok);
			if (ok) settings.setValue(tr("default/from--%1").arg(application_variable_name), text);
			break;
		default:
			break;
	}
	resize_slider();
}

void Popup_animate_application_setting::until_changed(const QString & text) {
	QSettings settings;
	int i; double d; bool ok;
	switch (type) {
		case AS_INTEGER:
			i = text.toInt(&ok);
			if (ok) settings.setValue(tr("default/until--%1").arg(application_variable_name), text);
			break;
		case AS_DOUBLE:
			d = text.toDouble(&ok);
			if (ok) settings.setValue(tr("default/until--%1").arg(application_variable_name), text);
			break;
		default:
			break;
	}
	resize_slider();
}

void Popup_animate_application_setting::steps_changed(const QString & text) {
	QSettings settings;
	int i; double d; bool ok;
	switch (type) {
		case AS_INTEGER:
			i = text.toInt(&ok);
			if (ok) settings.setValue(tr("default/steps--%1").arg(application_variable_name), text);
			break;
		case AS_DOUBLE:
			d = text.toDouble(&ok);
			if (ok) settings.setValue(tr("default/steps--%1").arg(application_variable_name), text);
			break;
		default:
			break;
	}
	resize_slider();
}

void Popup_animate_application_setting::slider_value_changed(int value ) {
	bool ok1, ok2, ok3; ok1 = false; double df,du,ds;
	df = from->text().toDouble(&ok1);
	du = until->text().toDouble(&ok2);
	ds = steps->text().toDouble(&ok3);
	if (ok1&&ok2&&ok3) {
		double dl = df + (du-df)*value/slider->maximum();
		QSettings settings;
		settings.setValue(application_variable_name,dl);
		val_label->setText(tr("%1").arg(dl));
		emit application_variable_changed(application_variable_name);
	}
}


void Popup_animate_application_setting::resize_slider() {
	bool ok1, ok2, ok3; ok1 = false; double df,du,ds;
	df = from->text().toDouble(&ok1);
	du = until->text().toDouble(&ok2);
	ds = steps->text().toDouble(&ok3);
	if (ok1&&ok2&&ok3) {
		slider->setMinimum(0);
		slider->setMaximum((int)((du-df)/ds));
	}
}

void Popup_animate_application_setting::animate_up() {
	for (int i=0; i<=slider->maximum(); i++) {
		slider->setValue(i);
		repaint();
	}
}

void Popup_animate_application_setting::animate_down() {
	for (int i=slider->maximum(); i>=0; i--) {
		slider->setValue(i);
		repaint();
	}
}
