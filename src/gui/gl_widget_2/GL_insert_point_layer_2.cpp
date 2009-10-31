/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_insert_point_layer_2.cpp 125 2007-07-02 11:11:29Z miklosb $
 */


#include <constants.h>
#include <gui/app/static/Application_settings.h>


#include <gui/gl_widget_2/GL_insert_point_layer_2.h>
#include <gui/gl_widget_2/GL_widget_2.h>

#include <iostream>


GL_insert_point_layer_2::GL_insert_point_layer_2() : state(NONE){
	QSettings settings;
	if (!settings.contains("insert-point-min-squared-distance")) 
		settings.setValue("insert-point-min-squared-distance", MIN_SQUARED_DISTANCE_SPRAY);
	Application_settings::add_setting("insert-point-min-squared-distance", AS_INTEGER);
}

GL_insert_point_layer_2::~GL_insert_point_layer_2() {}

void GL_insert_point_layer_2::mousePressEvent(QMouseEvent *e) {
	// insert - without left button
	if ((is_control(e)/* || is_alt(e)*/) && e->button() == Qt::LeftButton) {
		double x, y;
		x = widget->x_real(e->x());
		y = widget->y_real(e->y());
		if (is_alt(e)) {
			points_buffer.push_back(QPointF(x,y));
			emit add_point_to_selection(QPointF(x,y));
		}
		else
			emit point_created(x,y);
	}
	// spray - with middle button
	if ((is_control(e)/* || is_alt(e)*/) && e->button() == Qt::MidButton) {
		state = SPRAY;
		QSettings settings;
		min_squared_distance = settings.value("insert-point-min-squared-distance").toInt();
//		std::cout << "insert-point-min-squared-distance: " << min_squared_distance << std::endl;
		last_spray_x = last_spray_y = -1;
		mouseMoveEvent(e); // to insert the first point
	}
}

void GL_insert_point_layer_2::mouseMoveEvent(QMouseEvent *e) {
	if (state==SPRAY) {
		int screen_x = e->x();
		int screen_y = e->y();
		if (
			(last_spray_x == -1 && last_spray_y == -1) ||
			((screen_x-last_spray_x)*(screen_x-last_spray_x) + (screen_y-last_spray_y)*(screen_y-last_spray_y) > min_squared_distance)
			) {

				double x, y;
				x = widget->x_real(e->x());
				y = widget->y_real(e->y());
				points_buffer.push_back(QPointF(x,y));
				last_spray_x = e->x(); last_spray_y = e->y();
				emit add_point_to_selection(QPointF(x,y));
		}
	}
}

void GL_insert_point_layer_2::mouseReleaseEvent(QMouseEvent *e) {
	if (is_control(e) && e->button() == Qt::MidButton) {
		state = NONE;
		emit points_created(&points_buffer);
		emit clear_points_from_selection();
		points_buffer.clear();
	}
	if (is_control(e) && e->button() == Qt::MidButton) {
		state = NONE;
	}
}

void GL_insert_point_layer_2::keyPressEvent(QKeyEvent *e) {
	switch (e->key()) {
		case Qt::Key_Control:
//			widget->cursor()->setShape(Qt::CrossCursor);
			QCursor c = widget->get_default_cursor();
			c.setShape(Qt::CrossCursor);
			widget->setCursor(c);
			break;
	}
}

void GL_insert_point_layer_2::keyReleaseEvent(QKeyEvent *e) {
	if (is_control(e) && !points_buffer.empty()) {
		emit points_created(&points_buffer);
		emit clear_points_from_selection();
		points_buffer.clear();
		
	}
	if (is_control(e)) widget->setCursor(QCursor( QPixmap( (const char**)hand_xpm)));
	//handle releasing alt, so that we send out points and delete selection;
}
