/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Polar_edges_layer_3.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_POLAR_TRIANGULATION_LAYER_3_H
#define MESECINA_POLAR_TRIANGULATION_LAYER_3_H

#include <geometries/power_crust_3/Power_crust_3.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>

template <class Power_crust_3>
class Polar_triangulation_layer_3 : public GL_draw_layer_3 {
public:
	typedef typename Power_crust_3::Polar_triangulation_3						 Triangulation_3;
	Polar_triangulation_layer_3(const QString& name, Power_crust_3* m, const QString& tt) : GL_draw_layer_3(name,tt), parent(m) {
	}

	virtual void draw_commands() {
		Triangulation_3* t = parent->get_polar_triangulation();
		*this << NO_BOUNDING;
		*this << *t;
		*this << DO_BOUNDING;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case LINE_WIDTH_EDITABLE:
			return true;
			break;
		case POINT_SIZE_EDITABLE:
		default:
			return false;
			break;
		}
	}

private:
	Power_crust_3 *parent;
};


#endif //MESECINA_POLAR_TRIANGULATION_LAYER_3_H