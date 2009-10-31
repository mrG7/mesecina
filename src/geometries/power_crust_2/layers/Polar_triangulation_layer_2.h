/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info at http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Delaunay_layer_2.h 132 2007-07-25 16:50:33Z miklosb $
 */

#ifndef MESECINA_POLAR_TRIANGULATION_LAYER_2_H
#define MESECINA_POLAR_TRIANGULATION_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

template <class Power_crust_2>
class Polar_triangulation_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Power_crust_2::Polar_triangulation_2						 Polar_triangulation_2;
	typedef typename Polar_triangulation_2::Face_handle							 Face_handle;
	typedef typename Polar_triangulation_2::Finite_vertices_iterator	Finite_vertices_iterator;
	typedef typename Power_crust_2::Point_2  Point_2;
	typedef typename Power_crust_2::Line_2  Line_2;
	typedef typename Power_crust_2::Ray_2  Ray_2;
	typedef typename Power_crust_2::Segment_2 Segment_2;
	typedef typename Polar_triangulation_2::Finite_edges_iterator		   Finite_edges_iterator;

	Polar_triangulation_layer_2(const QString& name, Power_crust_2* m, const QString& tt = "", bool _do_voronoi = false, bool _do_crust = false) : 
		GL_draw_layer_2(name,tt), parent(m), do_voronoi(_do_voronoi), do_crust(_do_crust) {}

	virtual void draw_commands() {
		if (do_crust) { // do only reconstruction edges
			Polar_triangulation_2* t = parent->get_classified_polar_triangulation();
			if (do_voronoi) { // do the power crust, the reconstruction

				for (Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
					Face_handle f = e_it->first;
					int id = e_it->second;
					if (f->vertex((id+1)%3)->get_classification() != f->vertex((id+2)%3)->get_classification() &&
						f->vertex((id+1)%3)->get_classification() != P_UNCLASSIFIED && 
						f->vertex((id+2)%3)->get_classification() != P_UNCLASSIFIED) {

							CGAL::Object o = t->dual(e_it);
							Line_2  l;
							Ray_2   r;
							Segment_2 s;
							if (CGAL::assign(s,o)) *this << s;
							if (CGAL::assign(r,o)) *this << r;
							if (CGAL::assign(l,o)) *this << l;
					}
				}
			} else { // do the regular egedes not dual to the power crust
				*this << NO_BOUNDING;
				for (Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
					Face_handle f = e_it->first;
					int id = e_it->second;
					if (!(f->vertex((id+1)%3)->get_classification() != f->vertex((id+2)%3)->get_classification() &&
						f->vertex((id+1)%3)->get_classification() != P_UNCLASSIFIED && 
						f->vertex((id+2)%3)->get_classification() != P_UNCLASSIFIED)) {
							Point_2 p1 = f->vertex((id+1)%3)->point();
							Point_2 p2 = f->vertex((id+2)%3)->point();
							*this << Segment_2(p1,p2);
					}
				}
				*this << DO_BOUNDING;
			}
		}
		else { // do whole delaunay triangulation/voronoi diagram
			*this << NO_BOUNDING;
			Polar_triangulation_2* t = parent->get_polar_triangulation();
			if (do_voronoi) { //power diagram
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
			} else // regular triangulation
				*this << *t;
			*this << DO_BOUNDING;
		}
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
	Power_crust_2 *parent;
	bool do_voronoi, do_crust;
};


#endif //MESECINA_POLAR_TRIANGULATION_LAYER_2_H