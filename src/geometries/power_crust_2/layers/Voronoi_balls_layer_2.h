/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Interior_voronoi_balls_layer_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_VORONOI_BALLS_LAYER_2_H
#define MESECINA_VORONOI_BALLS_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <list> 
#include <algorithm>

template <class Power_crust_2>
class Voronoi_balls_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Power_crust_2::Triangulation_2						 Triangulation_2;
	typedef typename Triangulation_2::Finite_faces_iterator			   Finite_faces_iterator;
	typedef typename Power_crust_2::Point_2										 Point_2;
	typedef typename Power_crust_2::Circle_2										 Circle_2;

	Voronoi_balls_layer_2(const QString& name, Power_crust_2* m, const QString& ttip, bool only_center = false,
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
			Triangulation_2* t = parent->get_input_triangulation();
			Finite_faces_iterator f_it, f_end = t->finite_faces_end();
			for (f_it = t->finite_faces_begin(); f_it!=f_end; f_it++) {
				Point_2 center = t->dual(f_it);
				if (only_center)
					*this << center;
				else
					*this << Circle_2(center, squared_distance(center, f_it->vertex(0)->point()));
			}
			*this << DO_BOUNDING;
		} else if (interior) { // show interior
			Triangulation_2* t = parent->get_crust_classified_triangulation();
			Finite_faces_iterator f_it, f_end = t->finite_faces_end();
			for (f_it = t->finite_faces_begin(); f_it!=f_end; f_it++) {
				if (f_it->get_onestep_classification() == O_INTERIOR) {
					Point_2 center = t->dual(f_it);
					if (only_center)
						*this << center;
					else
						*this << Circle_2(center, squared_distance(center, f_it->vertex(0)->point()));

				}
			}
		} else { // show exterior
			*this << NO_BOUNDING;
			Triangulation_2* t = parent->get_crust_classified_triangulation();
			Finite_faces_iterator f_it, f_end = t->finite_faces_end();
			for (f_it = t->finite_faces_begin(); f_it!=f_end; f_it++) {
				if (f_it->get_onestep_classification() != O_INTERIOR) {
					Point_2 center = t->dual(f_it);
					if (only_center)
						*this << center;
					else
						*this << Circle_2(center, squared_distance(center, f_it->vertex(0)->point()));

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


#endif //MESECINA_VORONOI_BALLS_LAYER_2_H