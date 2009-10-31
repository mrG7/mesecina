/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_triangulation_layer_3.h 373 2008-07-11 02:20:13Z miklosb $
 */

#ifndef MESECINA_V_TOPOLOGY_FILTRATION_LAYER_3_H
#define MESECINA_V_TOPOLOGY_FILTRATION_LAYER_3_H

#include <gui/gl_widget_3/GL_draw_layer_3.h>
#include <gui/app/static/Application_settings.h>

#include <boost/foreach.hpp>

template <class Union_of_balls_3>
class V_topology_filtration_layer_3 : public GL_draw_layer_3 {

public:
	typedef typename Union_of_balls_3::V_triangulation_3						 V_triangulation_3;
	typedef typename Union_of_balls_3::Edges_with_doubles						 Edges_with_doubles;
	typedef typename V_triangulation_3::Finite_vertices_iterator				Finite_vertices_iterator;
	typedef typename V_triangulation_3::Finite_cells_iterator					Finite_cells_iterator;
	typedef typename V_triangulation_3::Finite_edges_iterator					Finite_edges_iterator;
	typedef typename V_triangulation_3::All_edges_iterator					All_edges_iterator;
	
	typedef typename V_triangulation_3::Cell_circulator					Cell_circulator;
	typedef typename V_triangulation_3::Polar_facet								Polar_facet;
	typedef typename V_triangulation_3::Polar_facet_set							Polar_facet_set;
	typedef typename V_triangulation_3::Cell_handle							Cell_handle;
	typedef typename Union_of_balls_3::Triangle_3								Triangle_3;
	typedef typename Union_of_balls_3::Segment_3								Segment_3;
	typedef typename Union_of_balls_3::Sphere_3								    Sphere_3;
	typedef typename Union_of_balls_3::Point_3									Point_3;
	typedef typename Union_of_balls_3::Vector_3									Vector_3;

	typedef typename V_triangulation_3::Edge									V_edge;
	typedef std::vector<V_edge> Edge_vector;
	typedef typename Edge_vector::iterator			Edge_vector_iterator;
	typedef std::back_insert_iterator<Edge_vector>	Insert_iterator;

	typedef typename V_triangulation_3::Facet					V_facet;
	typedef std::vector<V_facet> Facet_vector;
	typedef typename V_triangulation_3::Vertex_pair					Vertex_pair;
	
	typedef typename Facet_vector::iterator			Facet_vector_iterator;
	typedef std::back_insert_iterator<Facet_vector>	Facet_insert_iterator;

	V_topology_filtration_layer_3(const QString& name, Union_of_balls_3* m, const QString& ttip, bool front, bool visited, bool next = false, bool negation = false, bool fill = true) : GL_draw_layer_3(name, ttip), parent(m) {
		do_front = front;
		do_visited = visited;
		do_negation = negation;
		do_next = next;
		do_fill = fill;

		Application_settings::add_double_setting("lambda-limit", 0.01);
		Application_settings::add_double_setting("angle-limit", 10);
		Application_settings::add_int_setting("angle-id", 10);
		Application_settings::add_double_setting("scale-limit", 1);
		Application_settings::add_bool_setting("ignore-incomplete-v-voronoi-faces-for-medial-axis", false);
	}

	bool draw_inner_voronoi_face(const V_edge& e, V_triangulation_3* t) {
		Cell_circulator c_start, c_circ = t->incident_cells(e);
		c_start = c_circ;
		std::list<Point_3> polygon;
		bool all_interior = true;
		do {
			if (!t->is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
				polygon.push_back(t->voronoi_vertex(c_circ));
				if (!do_fill) *this << t->voronoi_vertex(c_circ);
			} else all_interior = false;
			c_circ++;
		} while (c_circ!=c_start);
		if (polygon.size()>2 && (all_interior==true || !Application_settings::get_bool_setting("ignore-incomplete-v-voronoi-faces-for-medial-axis"))) {
			Cell_handle c = e.first;
			Point_3 a = c->vertex(e.second)->point();
			Point_3 b = c->vertex(e.third)->point();
			double length = sqrt(CGAL::to_double(squared_distance(a,b)));
			Vector_3 normal = (b - a) / length;
			*this << NORMAL;
			*this << CGAL::ORIGIN + normal;
			*this << polygon;
			return true;
		}
		return false;
	}

	virtual void draw_commands() {
		// run the computation
		parent->get_topology_angle_filtered_medial_axis();
		// get the triangulation for debug information
		V_triangulation_3* t = parent->get_flooded_v_triangulation();
		
		if (do_fill) *this << RENDER_FILL;
		else *this << RENDER_NO_FILL;

		if (do_negation) {
			if (do_front) {
				std::multimap<double,Vertex_pair>* front = &(t->front);
				All_edges_iterator e_it, e_end = t->all_edges_end();
				int counter = 0, all_edges = 0;
				for (e_it=t->all_edges_begin(); e_it!=e_end; ++e_it, ++all_edges) {
					if (!t->is_in_front(*e_it,*front) && !t->is_visited(*e_it))
						draw_inner_voronoi_face( *e_it, t );
				}
			}
		} else {
			if (do_front) {
				std::multimap<double,Vertex_pair>* front = &(t->front);
				*this << DO_INVERSE_SCALAR;
				std::multimap<double, Vertex_pair>::iterator fr_it, fr_end = front->end();
				for (fr_it=front->begin(); fr_it!=fr_end; ++fr_it) {
					*this << fr_it->first;
					draw_inner_voronoi_face( t->create_edge_from_vertex_pair(fr_it->second), t );
				}
				*this << NO_SCALAR;
			}
			if (do_visited) {
			}
			if (do_next) {
				std::multimap<double,Vertex_pair>* front = &(t->front);
				if (front->size() > 0) 
					draw_inner_voronoi_face( t->create_edge_from_vertex_pair(front->begin()->second), t );
			}
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case SCALAR_EDITABLE:
			return do_front && !do_negation;
		case LINE_WIDTH_EDITABLE:
		case POINT_SIZE_EDITABLE:
			return !do_fill;
			break;
		default:
			return false;
			break;
		}
	}

	virtual QString get_name_user_property(Layer_property l) { 
		if (l == SCALAR_EDITABLE) 
			if (do_front) return "Stability";
		else return "";
	}

	//virtual void application_settings_changed(const QString& settings_name) {

	//	if (do_medial_axis && do_lambda && settings_name=="lambda-limit") {
	//		double lambda_limit = Application_settings::get_double_setting("lambda-limit");

	//		if (lambda_limit < 0) scalar_min = 0;
	//		else scalar_min = lambda_limit;
	//		invalidate_cache();
	//		has_new_range = false;
	//		widget->request_repaint();

	//	}

	//	if (do_medial_axis && (do_angle || do_top_angle) && settings_name=="angle-limit") {
	//		double lambda_limit = Application_settings::get_double_setting("angle-limit");

	//		if (lambda_limit < 0) scalar_min = 0;
	//		else scalar_min = lambda_limit;
	//		invalidate_cache();
	//		has_new_range = false;
	//		widget->request_repaint();

	//	}

	//	if (do_medial_axis && do_top_angle && settings_name=="angle-id") {
	//		double lambda_limit = Application_settings::get_double_setting("angle-id");

	//		if (lambda_limit < 0) scalar_max = scalar_min = 0;
	//		else scalar_max = scalar_min = lambda_limit;
	//		invalidate_cache();
	//		has_new_range = false;
	//		widget->request_repaint();

	//	}		

	//	if (do_medial_axis && do_scale && settings_name=="scale-limit") {
	//		double lambda_limit = Application_settings::get_double_setting("scale-limit");

	//		if (lambda_limit < 0) scalar_min = 0;
	//		else scalar_min = lambda_limit;
	//		invalidate_cache();
	//		has_new_range = false;
	//		widget->request_repaint();

	//	}
	//	


	//	if (do_medial_axis && settings_name=="ignore-incomplete-v-voronoi-faces-for-medial-axis") {
	//		invalidate_cache();
	//		widget->request_repaint();
	//	}
	//}

private:
	Union_of_balls_3 *parent;
	bool do_front, do_visited, do_next,	do_fill, do_negation;
};


#endif //MESECINA_V_TOPOLOGY_FILTRATION_LAYER_3_H