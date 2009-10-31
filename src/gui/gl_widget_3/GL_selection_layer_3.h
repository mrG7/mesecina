/* This source file is part of mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info at http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_cutting_plane_layer_3.h 93 2007-04-06 22:00:48Z miklosb $
 */

#ifndef MESECINA_SELECTION_LAYER_3_H
#define MESECINA_SELECTION_LAYER_3_H

#include <gui/gl_widget_3/GL_draw_layer_3.h>


class W_API GL_selection_layer_3 : public GL_draw_layer_3 {
	Q_OBJECT
public:

	GL_selection_layer_3(const QString& name);
	virtual void draw_commands();

	virtual bool has_property(Layer_property prop);
	virtual void application_settings_changed(const QString& settings_name);

public slots:
	void add_point_to_selection(double x, double y, double z);
	void remove_point_from_selection(double x, double y, double z);
	void clear_selection();

private:
	std::list<Point3D> points;

};
#endif //MESECINA_SELECTION_LAYER_3_H