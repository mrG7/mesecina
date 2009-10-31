/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Visualization_layer.cpp 88 2007-04-04 21:38:55Z miklosb $
 */

#include <gui/gl_widget/Visualization_layer.h>
#include <QtGui/QCursor>

Visualization_layer::Visualization_layer() : widget(0), active(false), default_bounding_box(true) {
	bounding_xmin = bounding_xmax = bounding_ymin = bounding_ymax = 0;
}

Visualization_layer::~Visualization_layer() {}

//#ifdef MESECINA_3D
//void Visualization_layer::attach(GL_widget_3* w) {
//	widget = w;
//}
//#else
//void Visualization_layer::attach(GL_widget_2* w) {
//	widget = w;
//}
//#endif

void Visualization_layer::activate() { set_active(true); }
void Visualization_layer::deactivate() { set_active(false); }
void Visualization_layer::set_active(bool a) { 
	active = a;
}
bool Visualization_layer::is_active() { 
	return active;
}
void Visualization_layer::selection_at(double, double, double ) {
}

void Visualization_layer::update_bounding_box(const QRectF& r) {
	if (default_bounding_box || r.x() < bounding_xmin) bounding_xmin = r.x();
	if (default_bounding_box || r.x()+r.width() > bounding_xmax) bounding_xmax = r.x()+r.width();
	if (default_bounding_box || r.y() < bounding_ymin) bounding_ymin = r.y();
	if (default_bounding_box || r.y()+r.height() > bounding_ymax) bounding_ymax = r.y()+r.height();
	default_bounding_box = false;
}

QRectF Visualization_layer::get_bounding_box(bool *is_valid) {
	*is_valid = !default_bounding_box;
	return QRectF(bounding_xmin, bounding_ymin, bounding_xmax- bounding_xmin, bounding_ymax-bounding_ymin);
}

#ifdef MESECINA_3D
Box3D Visualization_layer::get_3d_bounding_box(bool *is_valid) {
	*is_valid = !default_bounding_box;
	return Box3D(bounding_xmin, bounding_ymin, bounding_zmin, bounding_xmax, bounding_ymax, bounding_zmax);
}
#endif

