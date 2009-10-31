/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Classified_poles_layer_3.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_CLASSIFIED_POLES_LAYER_3_H
#define MESECINA_CLASSIFIED_POLES_LAYER_3_H

#include <geometries/power_crust_3/Power_crust_3.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>

template <class Power_crust_3>
class Classified_poles_layer_3 : public GL_draw_layer_3 {
public:
	typedef typename Power_crust_3::Polar_triangulation_3						 Polar_triangulation_3;
	typedef typename Polar_triangulation_3::Finite_vertices_iterator	Polar_finite_vertices_iterator;
	typedef typename Power_crust_3::Triangulation_3						 Triangulation_3;
	typedef typename Triangulation_3::Finite_vertices_iterator	Finite_vertices_iterator;

	//typedef typename Triangulation_3::Vertex_handle						   Vertex_handle;
	typedef typename Power_crust_3::Sphere_3										 Sphere_3;
	typedef typename Power_crust_3::Segment_3										 Segment_3;

	Classified_poles_layer_3(const QString& name, Power_crust_3* m, const QString& tt, Pole_classification c, bool balls = false, bool connecting_segment = false) : GL_draw_layer_3(name,tt), parent(m), classification(c) {
		do_balls = balls;
		do_connecting_segments = connecting_segment;
	}

	virtual void draw_commands() {
		Polar_triangulation_3* t = parent->get_classified_polar_triangulation();
		if (classification== P_EXTERIOR || do_connecting_segments) *this << NO_BOUNDING;

		if (do_connecting_segments) {
			Triangulation_3* it = parent->get_pole_classified_triangulation();
			Finite_vertices_iterator v_it, v_end = it->finite_vertices_end();
			*this << DO_SCALAR;
			*this << POINTS_START;
			for (v_it = it->finite_vertices_begin(); v_it!=v_end; v_it++) {
				*this << v_it->get_poles_confidence();
				*this << v_it->point();
				if (!it->is_infinite(v_it->get_pole_plus()))
					*this << v_it->get_pole_plus()->get_circumcenter();
				if (!it->is_infinite(v_it->get_pole_minus()))
					*this << v_it->get_pole_minus()->get_circumcenter();
			}
			*this << POINTS_END;

			*this << SEGMENTS_START;
			for (v_it = it->finite_vertices_begin(); v_it!=v_end; v_it++) {
				*this << v_it->get_poles_confidence();
				if (!it->is_infinite(v_it->get_pole_plus()))
					*this << Segment_3(v_it->point(), v_it->get_pole_plus()->get_circumcenter());
				if (!it->is_infinite(v_it->get_pole_minus()))
					*this << Segment_3(v_it->point(), v_it->get_pole_minus()->get_circumcenter());
			}
			*this << SEGMENTS_END;
			*this << NO_SCALAR;
		} else {
			*this << RENDER_FILL;
			Polar_finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
			if (!do_balls) *this << POINTS_START;
			for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++) {
				if (v_it->get_classification() == classification) {
					if (do_balls) {
						*this << Sphere_3(v_it->point().point(), v_it->point().weight());
					} else *this << v_it->point();
				}
			}
			if (!do_balls) *this << POINTS_END;
		}
		if (classification== P_EXTERIOR || do_connecting_segments) *this << DO_BOUNDING;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case GLUT_SPHERE:
			return do_balls && !do_connecting_segments;
			break;
		case POINT_SIZE_EDITABLE:
			return !do_balls;
			break;
		case SCALAR_EDITABLE:
		case LINE_WIDTH_EDITABLE:
			return do_connecting_segments && !do_balls;
		default:
			return false;
			break;
		}
	}

	virtual QString get_name_user_property(Layer_property l) { 
		if (l == SCALAR_EDITABLE) return "Confidence";
		else return "";
	}

private:
	Power_crust_3 *parent;
	Pole_classification classification;
	bool do_balls, do_connecting_segments;
};


#endif //MESECINA_CLASSIFIED_POLES_LAYER_3_H