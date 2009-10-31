/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Delaunay_faces_layer_3.h 98 2007-05-01 23:11:20Z miklosb $
 */

#ifndef MESECINA_DELAUNAY_LAYER_3_H
#define MESECINA_DELAUNAY_LAYER_3_H

#include <geometries/power_crust_3/Power_crust_3.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>

template <class Power_crust_3>
class Delaunay_layer_3 : public GL_draw_layer_3 {
public:
	typedef typename Power_crust_3::Triangulation_3						 Triangulation_3;
	typedef typename Triangulation_3::Finite_facets_iterator	Finite_facets_iterator;
	typedef typename Triangulation_3::Finite_cells_iterator	Finite_cells_iterator;
	typedef typename Triangulation_3::Finite_vertices_iterator	Finite_vertices_iterator;
	typedef typename Triangulation_3::Cell_handle						   Cell_handle;
	typedef typename Power_crust_3::Triangle_3										 Triangle_3;
	typedef typename Power_crust_3::Segment_3										 Segment_3;

	
	Delaunay_layer_3(const QString& name, Power_crust_3* m, const QString& tt, bool vertex, bool edge, bool face, bool dual) : GL_draw_layer_3(name,tt), parent(m) {
		do_vertex = vertex;
		do_edge = edge;
		do_face = face;
		do_dual = dual;
	}

	virtual void draw_commands() {
		Triangulation_3* t = parent->get_input_triangulation();
		if (do_dual) *this << NO_BOUNDING;
		if (do_vertex) {
			*this << POINTS_START;
			if (do_dual) {
				Finite_cells_iterator c_it, c_end = t->finite_cells_end();
				for (c_it = t->finite_cells_begin (); c_it!=c_end; c_it++) {
					*this << t->dual(c_it);
				}
			} else {
				Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
				for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++) {
					*this << CGAL::to_double(v_it->point().x());
					*this << v_it->point();
				}
			}
			*this << POINTS_END;
		} else if (do_edge) {
			if (do_dual) {
				Finite_facets_iterator f_it, f_end = t->finite_facets_end();
				*this << SEGMENTS_START;
				for (f_it = t->finite_facets_begin(); f_it!=f_end; f_it++) {
					Cell_handle c = f_it->first;
					int id = f_it->second;
					Cell_handle c_n = c->neighbor(id);
					if (!t->is_infinite(c) && !t->is_infinite(c_n))
						*this << Segment_3( t->dual(c), t->dual(c_n));
				}
				*this << SEGMENTS_END;
			} else *this << *t;
		} else if (do_face) {
			std::cout << PROGRESS_STATUS << "Extracting Delaunay faces" << std::endl;
			Finite_facets_iterator f_it, f_end = t->finite_facets_end(); int i = 0;
			*this << TRIANGLES_START;
			for (f_it = t->finite_facets_begin(); f_it!=f_end; f_it++, i++) {
				Cell_handle c = f_it->first;
				int id = f_it->second;
				*this << Triangle_3(
					c->vertex((id+1)%4)->point(),
					c->vertex((id+2)%4)->point(),
					c->vertex((id+3)%4)->point());
			}
			*this << TRIANGLES_END;
			std::cout << PROGRESS_DONE << std::endl;
		}
		if (do_dual) *this << NO_BOUNDING;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case POINT_SIZE_EDITABLE:
			return do_vertex;
			break;
		case LINE_WIDTH_EDITABLE:
			return do_edge;
			break;
		default:
			return false;
			break;
		}
	}

private:
	Power_crust_3 *parent;
	bool do_vertex, do_edge, do_face, do_dual;
};


#endif //MESECINA_DELAUNAY_LAYER_3_H