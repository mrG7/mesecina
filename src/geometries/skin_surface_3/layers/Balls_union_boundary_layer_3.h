/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Balls_union_boundary_layer_3.h 374 2008-07-11 20:46:09Z miklosb $
 */

#ifndef MESECINA_BALLS_UNION_BOUNDARY_LAYER_3_H
#define MESECINA_BALLS_UNION_BOUNDARY_LAYER_3_H

#include <gui/gl_widget_3/GL_draw_layer_3.h>

template <class Skin_surface_3>
class Balls_union_boundary_layer_3 : public GL_draw_layer_3 {
public:
	typedef typename Skin_surface_3::Polyhedron						 Polyhedron;

	Balls_union_boundary_layer_3(const QString& name, Skin_surface_3* m, const QString& ttip, bool wireframe) : GL_draw_layer_3(name, ttip), parent(m) {
		do_only_wireframe = wireframe;
	}

	virtual void draw_commands() {
		Polyhedron* p = parent->get_skin_surface();
		if (do_only_wireframe) *this << RENDER_NO_FILL;
		else *this << RENDER_FILL;
		*this << *p;
		if (!do_only_wireframe) *this << RENDER_NO_FILL;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case LINE_WIDTH_EDITABLE:
			return do_only_wireframe;
		case POINT_SIZE_EDITABLE:
		default:
			return false;
			break;
		}
	}

private:
	Skin_surface_3 *parent;
	bool do_only_wireframe;
};


#endif //MESECINA_BALLS_UNION_BOUNDARY_LAYER_3_H