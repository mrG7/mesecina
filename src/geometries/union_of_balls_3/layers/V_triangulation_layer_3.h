/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_triangulation_layer_3.h 373 2008-07-11 02:20:13Z miklosb $
 */

#ifndef MESECINA_V_TRIANGULATION_LAYER_3_H
#define MESECINA_V_TRIANGULATION_LAYER_3_H

#include <gui/gl_widget_3/GL_draw_layer_3.h>
#include <gui/app/static/Application_settings.h>

template <class Union_of_balls_3>
class V_triangulation_layer_3 : public GL_draw_layer_3 {

public:
	typedef typename Union_of_balls_3::V_triangulation_3						 V_triangulation_3;
	typedef typename V_triangulation_3::Finite_vertices_iterator				Finite_vertices_iterator;
	typedef typename V_triangulation_3::Finite_cells_iterator					Finite_cells_iterator;
	typedef typename V_triangulation_3::Finite_edges_iterator					Finite_edges_iterator;
	typedef typename V_triangulation_3::Cell_circulator					Cell_circulator;
	typedef typename V_triangulation_3::Polar_facet								Polar_facet;
	typedef typename V_triangulation_3::Polar_facet_set							Polar_facet_set;
	typedef typename V_triangulation_3::Cell_handle							Cell_handle;
	typedef typename Union_of_balls_3::Triangle_3								Triangle_3;
	typedef typename Union_of_balls_3::Segment_3								Segment_3;
	typedef typename Union_of_balls_3::Point_3									Point_3;
	typedef typename Union_of_balls_3::Vector_3									Vector_3;

	typedef typename V_triangulation_3::Edge									V_edge;
	typedef std::vector<V_edge> Edge_vector;
	typedef typename Edge_vector::iterator			Edge_vector_iterator;
	typedef std::back_insert_iterator<Edge_vector>	Insert_iterator;

	typedef typename V_triangulation_3::Facet					V_facet;
	typedef std::vector<V_facet> Facet_vector;
	typedef typename Facet_vector::iterator			Facet_vector_iterator;
	typedef std::back_insert_iterator<Facet_vector>	Facet_insert_iterator;

	V_triangulation_layer_3(const QString& name, Union_of_balls_3* m, const QString& ttip, bool vertex, bool origin, bool edges = true, bool dual = false) : GL_draw_layer_3(name, ttip), parent(m) {
		do_vertex = vertex;
		do_origin = origin;
		do_edges = edges;
		do_dual = dual;
		if (do_vertex || do_origin) Application_settings::add_int_setting("v-vertex-id", 0);
	}

	virtual void draw_commands() {
		V_triangulation_3* t = parent->get_v_triangulation();
		Point_3 p, p1, p2, p3, mid;
		if (do_vertex) {
			if (do_dual) {
				*this << NO_BOUNDING;
				// voronoi vertices
				Finite_cells_iterator c_it, c_end = t->finite_cells_end();
				for (c_it = t->finite_cells_begin (); c_it!=c_end; c_it++) {
					*this << t->dual(c_it);
				}
				*this << DO_BOUNDING;
			} else {
				// vertices of v triangulation
				Finite_vertices_iterator v_it, v_end = t->finite_vertices_end(); int i=0;
				*this << DO_SCALAR;
				for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++, i++) {
					*this << i;
					if (do_origin) {
						p = v_it->point();

						Polar_facet_set f_set = v_it->get_dual_source_facets();
						if (f_set.size()>1) std::cout << LOG_BLUE << "We have a vertex with more than one source "<< i << std::endl;
						Polar_facet_set::iterator f_it, f_end = f_set.end();
						for (f_it = f_set.begin(); f_it!=f_end; f_it++) {
							p1 = f_it->first->vertex((f_it->second+1)%4)->point();
							p2 = f_it->first->vertex((f_it->second+2)%4)->point();
							p3 = f_it->first->vertex((f_it->second+3)%4)->point();
							if (do_edges) {
								mid = CGAL::ORIGIN + ((p1 - CGAL::ORIGIN) + (p2 - CGAL::ORIGIN) + (p3 - CGAL::ORIGIN))/3;
								*this << Segment_3(p,mid);
							} else {
								*this << Triangle_3(p,p1,p2);
								*this << Triangle_3(p,p2,p3);
								*this << Triangle_3(p,p3,p1);
							}
						}
					} else {
						*this << v_it->point();
					}
				}
			}
			*this << NO_SCALAR;
		} else if (do_edges) {
			if (!do_dual) *this << *t;
			else {
				Finite_vertices_iterator v_it, v_end = t->finite_vertices_end(); int i=0;
				*this << DO_SCALAR;
				*this << NO_BOUNDING;
				for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++, i++) {
					*this << i;
					Facet_vector facets; Facet_insert_iterator ii(facets);
					t->incident_facets(v_it, ii);
					Facet_vector_iterator f_it, f_end = facets.end();
					for (f_it = facets.begin(); f_it != f_end; f_it++) {
						Cell_handle c = f_it->first;
						int id = f_it->second;
						Cell_handle n_c = c->neighbor(id);
						//int n_id = n_c->index(c);
						if (!t->is_infinite(c) && !t->is_infinite(n_c))
							*this << Segment_3( t->dual(c), t->dual(n_c));
					}
				}
				*this << DO_BOUNDING;
				*this << NO_SCALAR;
			}
		} else {
			Finite_vertices_iterator v_it, v_end = t->finite_vertices_end(); int i=0;
			*this << DO_SCALAR;
			*this << NO_BOUNDING;
			for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++, i++) {
				*this << i;
				Edge_vector edges; Insert_iterator ii(edges);
				t->incident_edges(v_it, ii);
				Edge_vector_iterator e_it, e_end = edges.end();
				for (e_it = edges.begin(); e_it != e_end; e_it++) {
					Cell_circulator c_start, c_circ = t->incident_cells(*e_it);
					c_start = c_circ;
					std::list<Point_3> polygon;
					do {
						if (!t->is_infinite(c_circ)) { 
							polygon.push_back(t->dual(c_circ));
							//						std::cout << t->dual(c_circ) << std::endl;
						} //else std::cout << " voronoi face has an infinite vertex " << std::endl;
						c_circ++;
					} while (c_circ!=c_start);
					if (polygon.size()>2) {
						Cell_handle c = e_it->first;
						Point_3 a = c->vertex(e_it->second)->point();
						Point_3 b = c->vertex(e_it->third)->point();
						double length = sqrt(CGAL::to_double(squared_distance(a,b)));
						Vector_3 normal = (b - a) / length;
						//					*this << Segment_3(a,b);
						*this << NORMAL;
						*this << CGAL::ORIGIN + normal;
						*this << polygon;
					}
				}
			}
			*this << DO_BOUNDING;
			*this << NO_SCALAR;
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case LINE_WIDTH_EDITABLE:
			return (do_origin && do_edges) || (!do_vertex && do_edges);
			break;
		case SCALAR_EDITABLE:
			return (do_origin || (do_vertex && !do_dual)) || (!do_vertex && !do_edges);
		case POINT_SIZE_EDITABLE:
			return do_vertex && !(do_origin) ;
			break;
		default:
			return false;
			break;
		}
	}

	virtual QString get_name_user_property(Layer_property l) { 
		if (l == SCALAR_EDITABLE) return "Vertex id";
		else return "";
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if ((do_vertex || do_origin || (!do_vertex && !do_edges) || (do_edges && do_dual)) && settings_name=="v-vertex-id") {
			int id = Application_settings::get_int_setting("v-vertex-id");

			scalar_min = scalar_max = id;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}
	}

private:
	Union_of_balls_3 *parent;
	bool do_vertex, do_origin, do_edges, do_dual;
};


#endif //MESECINA_V_TRIANGULATION_LAYER_3_H