/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info at http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Ball_triangulation_layer_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_BALL_TRIANGULATION_LAYER_2_H
#define MESECINA_BALL_TRIANGULATION_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>
#include <geometries/union_of_balls_2/Union_of_balls_2.h>

template <class Union_of_balls_2>
class Ball_triangulation_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Union_of_balls_2::Ball_triangulation_2						 Ball_triangulation_2;
	typedef typename Ball_triangulation_2::Finite_edges_iterator	Finite_edges_iterator;
	typedef typename Union_of_balls_2::Line_2  Line_2;
	typedef typename Union_of_balls_2::Ray_2  Ray_2;
	typedef typename Union_of_balls_2::Segment_2 Segment_2;

	Ball_triangulation_layer_2(const QString& name, Union_of_balls_2* m, const QString& tt = "", bool dual = false) : GL_draw_layer_2(name, tt), 
			parent(m), do_dual(dual) {}

	virtual void draw_commands() {
		Ball_triangulation_2* t = parent->get_ball_triangulation();
		if (do_dual) {
			*this << NO_BOUNDING;
			for (Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
				CGAL::Object o = t->dual(e_it);
				Line_2  l;
				Ray_2   r;
				Segment_2 s;
				if (CGAL::assign(s,o)) *this << s;
				if (CGAL::assign(r,o)) *this << r;
				if (CGAL::assign(l,o)) *this << l;
			}
			*this << DO_BOUNDING;
		} else
		*this << *t;
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
	Union_of_balls_2 *parent;
	bool do_dual;
};


#endif //MESECINA_BALL_TRIANGULATION_LAYER_2_H