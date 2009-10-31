/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Poles_layer_3.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_POLES_LAYER_3_H
#define MESECINA_POLES_LAYER_3_H

#include <geometries/power_crust_3/Power_crust_3.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>


template <class Power_crust_3>
class Poles_layer_3 : public GL_draw_layer_3 {
public:
	typedef typename Power_crust_3::Triangulation_3						 Triangulation_3;
	typedef typename Triangulation_3::Finite_cells_iterator			Finite_cells_iterator;
	typedef typename Triangulation_3::Finite_vertices_iterator			Finite_vertices_iterator;
	typedef typename Triangulation_3::Cell_handle						   Cell_handle;
	typedef typename Power_crust_3::Segment_3										 Segment_3;
	Poles_layer_3(const QString& name, Power_crust_3* m, const QString& tt, Voronoi_classification c, bool origin) : GL_draw_layer_3(name,tt), parent(m), classification(c) {
		do_origin = origin;

	}

	virtual void draw_commands() {
		Triangulation_3* t = parent->get_pole_classified_triangulation();

		*this << NO_BOUNDING;
		if (do_origin) {
			*this << SEGMENTS_START;  int i = 0;
			Finite_vertices_iterator v_it, v_end = t->finite_vertices_end ();
			for (v_it = t->finite_vertices_begin (); v_it!=v_end; v_it++, i++) {
				Cell_handle c;
				if (classification == V_PLUS) c = v_it->get_pole_plus();
				else c = v_it->get_pole_minus();
				if (c!=0 && !t->is_infinite(c)) {
					*this << Segment_3(c->get_circumcenter(), v_it->point());
				}
				else if (c==0) {
//					*this << v_it->point();
					std::cout << "no pole found at origin layer at vertex " << i << std::endl;
				}
			}
			*this << SEGMENTS_END;
		} else {
			*this << POINTS_START;
			Finite_cells_iterator f_it, f_end = t->finite_cells_end();
			for (f_it = t->finite_cells_begin (); f_it!=f_end; f_it++) {
				if (f_it->get_classification() == classification) {
					*this << f_it->get_circumcenter();
				}
			}
			*this << POINTS_END;
		}
		*this << DO_BOUNDING;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case POINT_SIZE_EDITABLE:
			return !do_origin;
			break;
		case LINE_WIDTH_EDITABLE:
			return do_origin;
			break;
		default:
			return false;
			break;
		}
	}

private:
	Power_crust_3 *parent;
	Voronoi_classification classification;
	bool do_origin;
};


#endif //MESECINA_POLES_LAYER_3_H