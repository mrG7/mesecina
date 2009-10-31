/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_voronoi_layer_2.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef MESECINA_V_TRIANGULATION_LAYER_2_H
#define MESECINA_V_TRIANGULATION_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>
#include <geometries/union_of_balls_2/Union_of_balls_2.h>

template <class Union_of_balls_2>
class V_triangulation_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Union_of_balls_2::V_triangulation_2				 Triangulation_2;
	typedef typename Triangulation_2::Finite_edges_iterator		   Finite_edges_iterator;
	typedef typename Triangulation_2::Finite_faces_iterator		   Finite_faces_iterator;
	typedef typename Triangulation_2::Face_handle							 Face_handle;
	typedef typename Union_of_balls_2::Line_2 Line_2;
	typedef typename Union_of_balls_2::Ray_2   Ray_2;
	typedef typename Union_of_balls_2::Segment_2 Segment_2;

	V_triangulation_layer_2(const QString& name, Union_of_balls_2* m, const QString ttip, bool voronoi, bool classified = false, V_classification vc = V_UNCLASSIFIED, bool vertex = false) : GL_draw_layer_2(name, ttip), parent(m) {
		do_voronoi = voronoi;
		do_classified = classified;
		do_vertex = vertex;
		classification = vc;

	}

	virtual void draw_commands() {
		Triangulation_2* t = parent->get_v_triangulation();
		if (!do_classified) {
			if (do_voronoi) {
				*this << NO_BOUNDING;
				for (Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
					Face_handle f = e_it->first;
					//int id = e_it->second;
					CGAL::Object o = t->dual(e_it);
					Line_2  l;
					Ray_2   r;
					Segment_2 s;
					if (CGAL::assign(s,o)) *this << s;
					if (CGAL::assign(r,o)) *this << r;
					if (CGAL::assign(l,o)) *this << l;
				}
				*this << DO_BOUNDING;
			} else {
				*this << *t;
			}
		} else {
			t = parent->get_classified_v_triangulation();
			*this << NO_BOUNDING;
			if (do_vertex) {
				t = parent->get_flooded_v_triangulation();
				for (Finite_faces_iterator f_it = t->finite_faces_begin(), f_end = t->finite_faces_end(); f_it != f_end; ++f_it) {
					if (f_it->get_classification() == classification) {
						*this << t->dual(f_it);
					}
				}
			} else {
				for (Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
					Face_handle f = e_it->first;
					int id = e_it->second;
					if (f->get_classification(id) == classification) {
						CGAL::Object o = t->dual(e_it);
						Line_2  l;
						Ray_2   r;
						Segment_2 s;
						if (CGAL::assign(s,o)) *this << s;
						if (CGAL::assign(r,o)) *this << r;
						if (CGAL::assign(l,o)) *this << l;
					}
				}
			}
			*this << DO_BOUNDING;
		}

	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
		case LINE_WIDTH_EDITABLE:
			return !do_vertex;
			break;
		case POINT_SIZE_EDITABLE:
			return do_vertex;
		default:
			return false;
			break;
		}
	}

private:
	Union_of_balls_2 *parent;
	bool do_voronoi, do_classified,	do_vertex;
	V_classification classification;

};


#endif //MESECINA_V_TRIANGULATION_LAYER_2_H