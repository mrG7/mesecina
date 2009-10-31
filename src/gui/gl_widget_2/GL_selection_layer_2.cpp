/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_selection_layer_2.cpp 351 2008-04-18 18:25:24Z miklosb $
 */
#include <constants.h>

#include <gui/gl_widget_2/GL_selection_layer_2.h>

GL_selection_layer_2::GL_selection_layer_2(const QString& n) : GL_draw_layer_2(n), rubber_band(0) {
	clear_all();
	color = QColor(192,192,192);
	point_size = 7;
	line_width = 2;
	status = NONE;
}

GL_selection_layer_2::~GL_selection_layer_2() {}

int GL_selection_layer_2::add_point(QPointF p) {
	points.push_back(p);
	widget->repaintGL();
	return static_cast<int>(points.size());
}

void GL_selection_layer_2::remove_point(QPointF p) { 
	points.remove(p);
	widget->repaintGL();
}

void GL_selection_layer_2::clear_points() {
	points.clear();
	widget->repaintGL();
}


int GL_selection_layer_2::add_line(QLineF l) {
	lines.push_back(l);
	return static_cast<int>(lines.size());
}

void GL_selection_layer_2::remove_line(QLineF l) {
	lines.remove(l);
	widget->repaintGL();
}

void GL_selection_layer_2::clear_lines() {
	lines.clear();
	widget->repaintGL();
}

int GL_selection_layer_2::add_circle(QPointF c, float r) {
	centers.push_back(c);
	radii.push_back(r);
	return static_cast<int>(centers.size());
}

void GL_selection_layer_2::remove_circle(QPointF , float ) {
	std::cout << LOG_WARNING << "Removing single line from selection is not implemented yet. Imeplent GL_selection_layer_2::remove_circle" << std::endl;
}

void GL_selection_layer_2::clear_circles() {
	centers.clear();
	radii.clear();
}

void GL_selection_layer_2::clear_all() {
	lines.clear();
	points.clear();
	centers.clear();
}

void GL_selection_layer_2::draw() {

	std::list<QPointF>::iterator p_it, p_end = points.end();
	std::list<QLineF>::iterator l_it, l_end = lines.end();
	std::list<QPointF>::iterator c_it, c_end = centers.end();
	std::list<float>::iterator r_it, r_end = radii.end();
	switch (widget->get_renderer_type()) {
		case GL_widget_2::OPEN_GL:
			//widget->makeCurrent();

			glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
			glLineWidth(line_width);
			glPointSize(point_size);

			for (p_it = points.begin(); p_it != p_end; p_it++)
				widget->draw_point(p_it->x(), p_it->y());

			for (l_it = lines.begin(); l_it != l_end; l_it++)
				widget->draw_segment(l_it->x1(), l_it->y1(), l_it->x2(), l_it->y2());
			for (c_it = centers.begin(), r_it = radii.begin(); c_it != c_end; c_it++, r_it++)
				widget->draw_circle(c_it->x(), c_it->y(), *r_it);

			break;
		case GL_widget_2::QT_PAINTER:
			break;
		default:
			//never should happen;
			std::cout << LOG_ERROR << "Not good renderer for GL_draw_layer" << std::endl;
	}
}

void GL_selection_layer_2::mousePressEvent(QMouseEvent *e) {
	// creating selections
	if (has_shift(e) && !has_control(e) && !has_alt(e) && e->button() == Qt::LeftButton) { //ray selection happens on shift alone
		double x = widget->x_real(e->x());
		double y = widget->y_real(e->y());
		bool ray_hit_something = false;
		widget->ray_selection(x,y,&ray_hit_something); // they will call back the add functions if there was something added;
		if (!ray_hit_something) {
			clear_all();
			widget->clear_selection();
			widget->repaintGL();
		}
	}

	if (has_shift(e) && !has_control(e) && !has_alt(e) && e->button() == Qt::MidButton) { //box selection happens on shift alone
		status = BOX_SELECTION;
		first_x = e->x();
		first_y = e->y();
		rubber_band_origin = e->pos();
		if (!rubber_band) {
			rubber_band = new QRubberBand(QRubberBand::Rectangle, widget);
			windows_style = new QWindowsStyle();
			rubber_band->setStyle(windows_style);
		}
		rubber_band->setGeometry(QRect(rubber_band_origin, QSize()));
		rubber_band->show();

	}

	// manipulation of selections
	if (has_shift(e) && has_control(e) && e->button() == Qt::LeftButton) { //shift + control is move selection
		status = MOVE;
		last_x = first_x = e->x();
		last_y = first_y = e->y();
	}	
}

void GL_selection_layer_2::mouseMoveEvent(QMouseEvent *e) {
	// manipulating selections
	if (status == MOVE) {
		double dx = widget->x_real(e->x()) - widget->x_real(last_x);
		double dy = widget->y_real(e->y()) - widget->y_real(last_y);

		std::list<QPointF>::iterator p_it, p_end = points.end();
		for (p_it = points.begin(); p_it != p_end; p_it++)
			*p_it = QPointF(p_it->x()+dx, p_it->y()+dy);

		if (!has_alt(e)) widget->selection_moved_by(dx,dy);
		last_x = e->x();
		last_y = e->y();
		
		widget->repaintGL();
	}
	if (status == BOX_SELECTION)
		rubber_band->setGeometry(QRect(rubber_band_origin, e->pos()).normalized());
}

void GL_selection_layer_2::mouseReleaseEvent(QMouseEvent *e) {
	// manipulating selections
	if (status == MOVE) {
		status = NONE;
		if (has_alt(e)) {
			double dx = widget->x_real(e->x()) - widget->x_real(first_x);
			double dy = widget->y_real(e->y()) - widget->y_real(first_y);
			widget->selection_moved_by(dx,dy);
			widget->repaintGL();
		}
	}
	if (status == BOX_SELECTION) {
		status = NONE;
		rubber_band->hide();
		if((e->x() != first_x) && (e->y() != first_y)) {
			double x, y, xfirst2, yfirst2;
			x = widget->x_real(e->x());
			y = widget->y_real(e->y());
			xfirst2 = widget->x_real(first_x);
			yfirst2 = widget->y_real(first_y);

			double	xmin, xmax, ymin, ymax;
			if(x < xfirst2) {xmin = x; xmax = xfirst2;}
			else {xmin = xfirst2; xmax = x;}
			if(y < yfirst2) {ymin = y; ymax = yfirst2;}
			else {ymin = yfirst2; ymax = y;}
			bool ray_hit_something = false;
			widget->box_selection(xmin, ymin, xmax, ymax, &ray_hit_something);
		}

	}
	
}

void GL_selection_layer_2::keyPressEvent(QKeyEvent *) {

}

void GL_selection_layer_2::keyReleaseEvent(QKeyEvent *e) {
	switch (e->key()) {
		case Qt::Key_Delete:
		case Qt::Key_Backspace:
			if (widget->selection_deleted()) {
				clear_all();
				widget->repaintGL();
				emit selection_was_deleted();
			}
			break;
		case  Qt::Key_Alt:
			if (status == MOVE) {
				status = NONE;
				double dx = widget->x_real(last_x) - widget->x_real(first_x);
				double dy = widget->y_real(last_y) - widget->y_real(first_y);
				widget->selection_moved_by(dx,dy);
				widget->repaintGL();
			}

	}
}