/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_navigate_layer_2.cpp 533 2009-02-14 23:24:47Z miklosb $
 */


#include <iostream>

#include <constants.h>
#include <gui/app/static/Application_settings.h>

#include <gui/gl_widget_2/GL_navigate_layer_2.h>
#include <gui/gl_widget_2/GL_widget_2.h>




GL_navigate_layer_2::GL_navigate_layer_2() :s(NONE), first_x(0), first_y(0), rubberBand_origin(QPoint()), rubber_band(0) {
	QSettings settings;
	if (!settings.contains("navigation-smooth-zoom-steps")) 
		settings.setValue("navigation-smooth-zoom-steps", SMOOTH_ZOOM_STEPS);
	Application_settings::add_setting("navigation-smooth-zoom-steps", AS_INTEGER);
}

GL_navigate_layer_2::~GL_navigate_layer_2() { delete rubber_band;}

void GL_navigate_layer_2::mousePressEvent(QMouseEvent *e) {
	if (is_pure(e) || is_alt(e)) {
		switch (e->button()) {
			case Qt::LeftButton:
				if (s != PAN) {
					widget->setCursor(QCursor( QPixmap( (const char**)holddown_xpm)));
					first_x = e->x();
					first_y = e->y();
					s = PAN;
				}
				break;
			case Qt::RightButton:
				if (s != ZOOM && s != ZOOM_RECT) {
					widget->setCursor(QCursor( QPixmap( (const char**)zoomin_xpm)));
					first_x = e->x();
					first_y = e->y();
					s = ZOOM;
				}	
				break;
			case Qt::MidButton:
				if (s != ZOOM_RECT) {
					widget->setCursor(QCursor( QPixmap( (const char**)zoomin_rect_xpm)));
					s = ZOOM_RECT;
					rubberBand_origin = e->pos();
					if (!rubber_band) {
						rubber_band = new QRubberBand(QRubberBand::Rectangle, widget);
						windows_style = new QWindowsStyle();
						rubber_band->setStyle(windows_style);
					}
					rubber_band->setGeometry(QRect(rubberBand_origin, QSize()));
					rubber_band->show();
					first_x = e->x();
					first_y = e->y();
				}	
				break;
		}
	}
}

void GL_navigate_layer_2::mouseMoveEvent(QMouseEvent *e) {
	switch (s) {
		case PAN:
			double xcoord, ycoord, xfirstcoord, yfirstcoord, distx, disty;
			xcoord = widget->x_real(e->x());
			ycoord = widget->y_real(e->y());
			xfirstcoord = widget->x_real(first_x);
			yfirstcoord = widget->y_real(first_y);

			distx = xfirstcoord - xcoord;
			disty = yfirstcoord - ycoord;
			widget->move_center(distx, disty);
			first_x = e->x();
			first_y = e->y();
			break;
		case ZOOM:
			if (first_y>e->y()) widget->setCursor(QCursor( QPixmap( (const char**)zoomin_xpm)));
			else widget->setCursor(QCursor( QPixmap( (const char**)zoomout_xpm)));
			widget->zoom(1.0 + (first_y-e->y())/100.0);
			first_y = e->y();
			break;
		case ZOOM_RECT:
			rubber_band->setGeometry(QRect(rubberBand_origin, e->pos()).normalized());
			break;
	}
}

void GL_navigate_layer_2::mouseReleaseEvent(QMouseEvent *e) {
	switch (s) {
		case PAN:
			s = NONE;
			break;
		case ZOOM:
			s = NONE;
			break;
		case ZOOM_RECT:
			rubber_band->hide();
			delete rubber_band;
			rubber_band = 0;
			widget->repaintGL();

			if((e->x() != first_x) && (e->y() != first_y)) {
				double x, y, xfirst2, yfirst2;
				x = widget->x_real(e->x());
				y = widget->y_real(e->y());
				xfirst2 = widget->x_real(first_x);
				yfirst2 = widget->y_real(first_y);

				double	xmin, xmax, ymin, ymax;
				if(x < xfirst2) {xmin = x; xmax = xfirst2;}
				else {xmin = xfirst2; xmax = x;};
				if(y < yfirst2) {ymin = y; ymax = yfirst2;}
				else {ymin = yfirst2; ymax = y;};

				if (is_alt(e)) widget->set_window(xmin, xmax, ymin, ymax);
				else smooth_zoom(xmin, xmax, ymin, ymax);
			} else if (widget->has_boundingbox()) {
				// jump to bounding box
				double x_min = widget->get_bounding_xmin();
				double x_max = widget->get_bounding_xmax();
				double y_min = widget->get_bounding_ymin();
				double y_max = widget->get_bounding_ymax();
				if (x_min != x_max && y_min != y_max)	{
					double margin_percent= BOUNDING_BOX_MARGIN_PERCENT;
					double x_margin = (x_max - x_min) * margin_percent;
					double y_margin = (y_max - y_min) * margin_percent;
					x_min -= x_margin;	x_max += x_margin;
					y_min -= y_margin;	y_max += y_margin;
					if (is_alt(e)) widget->set_window(x_min, x_max, y_min, y_max);
					else smooth_zoom(x_min, x_max, y_min, y_max);
				}
			}
			s = NONE;
			break;
	}
	if (is_pure(e) || is_alt(e) || is_shift(e)) widget->setCursor(QCursor( QPixmap( (const char**)hand_xpm)));
	else {
			QCursor c = widget->get_default_cursor();
			c.setShape(Qt::CrossCursor);
			widget->setCursor(c);
	}
}

void GL_navigate_layer_2::wheelEvent(QWheelEvent *) {
	//std::cout << "wheel" << std::endl;
}



void GL_navigate_layer_2::smooth_zoom(double x_min, double x_max, double y_min, double y_max) {
	widget->smooth_zoom(x_min,x_max,y_min,y_max);

//	double o_x_min = widget->x_min();
//	double o_x_max = widget->x_max();
//	double o_y_min = widget->y_min();
//	double o_y_max = widget->y_max();
//	if (o_x_min == x_min && o_x_max == x_max && o_y_min == y_min && o_y_max == y_max) return;
//
//	QSettings settings;
//	int min_squared_distance = settings.value("navigation-smooth-zoom-steps").toInt();
////	std::cout << "navigation-smooth-zoom-steps: " << min_squared_distance << std::endl;
//
//	// these parameters should be moved outside, ini file or user interface...
//	double uniform_ratio = 1.00;
//	double total_steps = min_squared_distance;//SMOOTH_ZOOM_STEPS;
//	
//	double uniform_steps = (int) total_steps * uniform_ratio;
//	double accel_steps = (int) total_steps - uniform_steps;
//
//	double c_x_min = o_x_min; double x_min_step = uniform_ratio * (x_min - o_x_min)/uniform_steps;
//	double c_x_max = o_x_max; double x_max_step = uniform_ratio * (x_max - o_x_max)/uniform_steps;
//	double c_y_min = o_y_min; double y_min_step = uniform_ratio * (y_min - o_y_min)/uniform_steps;
//	double c_y_max = o_y_max; double y_max_step = uniform_ratio * (y_max - o_y_max)/uniform_steps;
//	for (int i=0; i<uniform_steps; i++) {
//		c_x_min += x_min_step;
//		c_x_max += x_max_step;
//		c_y_min += y_min_step;
//		c_y_max += y_max_step;
//		widget->set_window(c_x_min, c_x_max, c_y_min, c_y_max);
////		msg("ren");
//	}
//
//	if (accel_steps==0) return;
//	double acc_x_min = ((1-uniform_ratio) * (x_min - o_x_min) - x_min_step * accel_steps) / accel_steps / accel_steps;
//	double acc_x_max = ((1-uniform_ratio) * (x_max - o_x_max) - x_max_step * accel_steps) / accel_steps / accel_steps;
//	double acc_y_min = ((1-uniform_ratio) * (y_min - o_y_min) - y_min_step * accel_steps) / accel_steps / accel_steps;
//	double acc_y_max = ((1-uniform_ratio) * (y_max - o_y_max) - y_max_step * accel_steps) / accel_steps / accel_steps;
//	double a_x_min = c_x_min;
//	double a_x_max = c_x_max;
//	double a_y_min = c_y_min;
//	double a_y_max = c_y_max;
//	for (int i=1; i<accel_steps+1; i++) {
//		a_x_min = c_x_min + x_min_step*i + acc_x_min * i*i;
//		a_x_max = c_x_max + x_max_step*i + acc_x_max * i*i;
//		a_y_min = c_y_min + y_min_step*i + acc_y_min * i*i;
//		a_y_max = c_y_max + y_max_step*i + acc_y_max * i*i;
//		widget->set_window(a_x_min, a_x_max, a_y_min, a_y_max);
//	}
////	widget->set_window(x_min, x_max, y_min, y_max);
}