/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Dual_edges_layer_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_BALLS_DUAL_LAYER_2_H
#define MESECINA_BALLS_DUAL_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>
#include <geometries/union_of_balls_2/Union_of_balls_2.h>

template <class Union_of_balls_2>
class Balls_dual_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Union_of_balls_2::Ball_triangulation_2  			 Triangulation_2;
	typedef typename Triangulation_2::Finite_edges_iterator		   Finite_edges_iterator;
	typedef typename Triangulation_2::Finite_faces_iterator		   Finite_faces_iterator;
	typedef typename Triangulation_2::Face_handle							 Face_handle;
	typedef typename Union_of_balls_2::Segment_2 Segment_2;
	typedef typename Union_of_balls_2::Triangle_2 Triangle_2;

	Balls_dual_layer_2(const QString& name, Union_of_balls_2* m, const QString ttip, bool faces, Union_classification c = U_INTERIOR) : 
		GL_draw_layer_2(name, ttip), parent(m), classification(c), do_faces(faces) {
	}

	virtual void draw_commands() {
		Triangulation_2* t = parent->get_dual_ball_triangulation();
		if (do_faces) {
			for (Finite_faces_iterator f_it = t->finite_faces_begin(), f_end = t->finite_faces_end(); f_it != f_end; ++f_it) {
				if (f_it->get_classification() == U_INTERIOR)
					*this << Triangle_2(f_it->vertex(0)->point(), f_it->vertex(1)->point(), f_it->vertex(2)->point());
			}
		} else {
			for (Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
				Face_handle f = e_it->first;
				int id = e_it->second; 
				if (f->get_classification(id) == classification) {
					*this << Segment_2(f->vertex((id+1)%3)->point(), f->vertex((id+2)%3)->point());
				}
			}
		}

	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case LINE_WIDTH_EDITABLE:
			return !do_faces;
			break;
		case POINT_SIZE_EDITABLE:
		default:
			return false;
			break;
		}
	}


private:
	Union_of_balls_2 *parent;
	Union_classification classification;
	bool do_faces;
};


#endif //MESECINA_BALLS_DUAL_LAYER_2_H