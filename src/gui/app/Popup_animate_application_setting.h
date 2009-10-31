/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Popup_color_map_frame.h 258 2007-12-05 14:40:50Z miklosb $
 */

#ifndef POPUP_ANIMATE_APPLICATION_SETTING_H
#define POPUP_ANIMATE_APPLICATION_SETTING_H

#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>

#include <gui/app/static/Application_settings.h>

class Popup_animate_application_setting : public QFrame {
	Q_OBJECT

public:
	Popup_animate_application_setting(QWidget* parent);
	
	void set_application_variable(QString& application_variable_name, Application_setting_type type);

private slots:
	void animate_up();
	void animate_down();
	void from_changed(const QString & text);
	void until_changed(const QString & text);
	void steps_changed(const QString & text);
	void slider_value_changed(int value );



signals:
	void application_variable_changed(const QString& app_variable_name);

private:
	void resize_slider();

	QGridLayout* gridLayout;
	QLabel* val_label;
	QLabel* title_label;

	QLineEdit* from; 
	QLineEdit* until;
	QLineEdit* steps;
	QSlider* slider;

	QPushButton* forward_button, *back_button;
	QString application_variable_name;
	Application_setting_type type;	
};

#endif //POPUP_ANIMATE_APPLICATION_SETTING_H