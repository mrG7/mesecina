/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Interior_voronoi_balls_layer_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_POLAR_BALLS_LAYER_2_H
#define MESECINA_POLAR_BALLS_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <list> 
#include <algorithm>

template <class Power_crust_2>
class Polar_balls_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Power_crust_2::Polar_triangulation_2						 Polar_triangulation_2;
	typedef typename Polar_triangulation_2::Finite_vertices_iterator			   Finite_vertices_iterator;
	typedef typename Power_crust_2::Point_2										 Point_2;

	typedef typename Power_crust_2::Circle_2										 Circle_2;

	Polar_balls_layer_2(const QString& name, Power_crust_2* m, const QString& ttip, bool only_center = false,
								   bool fill = false, bool not_all = false, bool interior = false) : GL_draw_layer_2(name, ttip), parent(m) {
		this->fill = fill;
		this->not_all = not_all;
		this->interior = interior;
		this->only_center = only_center;
	}

	virtual void draw_commands() {
		if (!only_center && fill) *this << RENDER_FILL;

		if (!not_all) { // show all voronoi balls
			*this << NO_BOUNDING;
			Polar_triangulation_2* t = parent->get_polar_triangulation();
			Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
			for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++) {
				if (only_center)
					*this << v_it->point().point();
				else
					*this << Circle_2(v_it->point().point(),v_it->point().weight());
			}
			*this << DO_BOUNDING;
		} else if (interior) { // show interior
			Polar_triangulation_2* t = parent->get_classified_polar_triangulation();
			Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
			for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++) {
				if (v_it->get_classification() == P_INTERIOR) {
					if (only_center)
						*this << v_it->point().point();
					else
						*this << Circle_2(v_it->point().point(),v_it->point().weight());

				}
			}
		} else { // show exterior
			*this << NO_BOUNDING;
			Polar_triangulation_2* t = parent->get_classified_polar_triangulation();
			Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
			for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++) {
				if (v_it->get_classification() != P_INTERIOR) {
					if (only_center)
						*this << v_it->point().point();
					else
						*this << Circle_2(v_it->point().point(),v_it->point().weight());

				}
			}
			*this << DO_BOUNDING;
		}
		
		if (!only_center && fill) *this << RENDER_NO_FILL;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case POINT_SIZE_EDITABLE:
			return only_center;
		case LINE_WIDTH_EDITABLE:
			return !fill; break;
		default:
			return false;
			break;
		}
	}
private:
	Power_crust_2 *parent;
	bool fill;
	bool not_all;
	bool interior;
	bool only_center;

};


#endif //MESECINA_POLAR_BALLS_LAYER_2_H