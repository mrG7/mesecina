/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_points_layer_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_V_POINTS_LAYER_2_H
#define MESECINA_V_POINTS_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>
#include <geometries/union_of_balls_2/Union_of_balls_2.h>

#include <list>

template <class Union_of_balls_2>
class V_points_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Union_of_balls_2::V_triangulation_2				 Triangulation_2;
	typedef typename Triangulation_2::Finite_vertices_iterator	Finite_vertices_iterator;
	typedef typename Triangulation_2::Vertex_handle						   Vertex_handle;
	typedef typename Union_of_balls_2::Point_2										 Point_2;
	typedef typename Union_of_balls_2::Ball_triangulation_2				 Ball_triangulation_2;
	typedef typename Ball_triangulation_2::Face_handle				 Ball_face_handle;
	typedef typename Ball_triangulation_2::Edge									Polar_edge;
	typedef typename std::list<Polar_edge>				Polar_edge_list;
	typedef typename Polar_edge_list::iterator Polar_edge_list_iterator;
	typedef typename Union_of_balls_2::Triangle_2 Triangle_2;
	typedef typename Union_of_balls_2::Segment_2 Segment_2;


	V_points_layer_2(const QString& name, Union_of_balls_2* m, const QString& ttip, bool source_edge, bool source_triangle) : 
	GL_draw_layer_2(name, ttip), parent(m) {
		do_source_edge = source_edge;
		do_source_triangle = source_triangle;
	}

	virtual void draw_commands() {
		Triangulation_2* t = parent->get_v_triangulation();
		if ( !do_source_edge && !do_source_triangle) {
			Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
			for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++) {
				*this << v_it->point();
			}
		}
		else {
			for (Finite_vertices_iterator v_it = t->finite_vertices_begin(), v_end = t->finite_vertices_end();
				v_it != v_end; ++v_it) {
					Polar_edge_list es = v_it->get_origin_edges();
					Polar_edge_list_iterator e_end = es.end();
					for (Polar_edge_list_iterator e_it = es.begin(); e_it != e_end; e_it++) {
 						Ball_face_handle f = e_it->first;
						int id = e_it->second; 
						if (do_source_triangle) *this << Triangle_2(f->vertex((id+1)%3)->point(),
							f->vertex((id+2)%3)->point(), v_it->point());
						else *this << Segment_2(midpoint(f->vertex((id+1)%3)->point(),
							f->vertex((id+2)%3)->point()), v_it->point());
					}
			}
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
		case POINT_SIZE_EDITABLE:
			return !do_source_edge && !do_source_triangle;
			break;
		case LINE_WIDTH_EDITABLE:
			return do_source_edge;
		default:
			return false;
			break;
		}
	}

private:
	Union_of_balls_2* parent;
	bool do_source_edge, do_source_triangle;

};

#endif //MESECINA_V_POINTS_LAYER_2_H
