/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_triangulation_layer_3.h 373 2008-07-11 02:20:13Z miklosb $
 */

#ifndef MESECINA_V_CELL_CLASSIFICATION_LAYER_3_H
#define MESECINA_V_CELL_CLASSIFICATION_LAYER_3_H

#include <gui/gl_widget_3/GL_draw_layer_3.h>
#include <gui/app/static/Application_settings.h>

template <class Union_of_balls_3>
class V_cell_classification_layer_3 : public GL_draw_layer_3 {

public:
	typedef typename Union_of_balls_3::V_triangulation_3						 V_triangulation_3;
	typedef typename V_triangulation_3::Finite_vertices_iterator				Finite_vertices_iterator;
	typedef typename V_triangulation_3::Finite_cells_iterator					Finite_cells_iterator;
	typedef typename V_triangulation_3::Finite_edges_iterator					Finite_edges_iterator;
	typedef typename V_triangulation_3::All_edges_iterator						All_edges_iterator;
	typedef typename V_triangulation_3::Cell_circulator					Cell_circulator;
	typedef typename V_triangulation_3::Polar_facet								Polar_facet;
	typedef typename V_triangulation_3::Polar_facet_set							Polar_facet_set;
	typedef typename V_triangulation_3::Cell_handle							Cell_handle;
	typedef typename Union_of_balls_3::Triangle_3								Triangle_3;
	typedef typename Union_of_balls_3::Segment_3								Segment_3;
	typedef typename Union_of_balls_3::Point_3									Point_3;
	typedef typename Union_of_balls_3::Vector_3									Vector_3;

	typedef std::vector<Cell_handle> Cell_vector;
	typedef typename Cell_vector::iterator			Cell_vector_iterator;
	typedef std::back_insert_iterator<Cell_vector>	Insert_iterator;

	typedef typename V_triangulation_3::Facet					V_facet;
	typedef std::vector<V_facet> Facet_vector;
	typedef typename Facet_vector::iterator			Facet_vector_iterator;
	typedef std::back_insert_iterator<Facet_vector>	Face_insert_iterator;

	V_cell_classification_layer_3(const QString& name, Union_of_balls_3* m, const QString& ttip, M_classification c, bool dual_cell = false, bool dual_face = false, bool front_face = false) : GL_draw_layer_3(name, ttip), parent(m) {
		classification = c;
		do_dual_cell = dual_cell;
		do_dual_face = dual_face;
		do_front_face = front_face;
		if (do_front_face) Application_settings::add_int_setting("v-voronoi-face-id",0);
		if (do_vertex) Application_settings::add_int_setting("v-vertex-id", 0);
		Application_settings::add_int_setting("v-vertex-voronoi-vertex-id", -1);
	}

	virtual void draw_commands() {
		V_triangulation_3* t = parent->get_classified_v_triangulation();
		int id = Application_settings::get_int_setting("v-vertex-id");
		//Point_3 p, p1, p2, p3, mid;

		if (do_front_face) {
			parent->get_topology_angle_filtered_medial_axis();
			id = Application_settings::get_int_setting("v-voronoi-face-id");

			All_edges_iterator e_it, e_end = t->all_edges_end(); int counter = 0;
			for (e_it=t->all_edges_begin(); e_it!=e_end; ++e_it, counter++) {
				if (counter==id) {
					if (t->is_medial_interior_voronoi_facet(*e_it)) {
						bool is_front = t->is_front_voronoi_facet(*e_it);
						std::cout << "this face is front: " << (is_front? "TRUE" : "FALSE") << std::endl;





						Cell_circulator c_start, c_circ = t->incident_cells(*e_it);
						c_start = c_circ;
						std::list<Point_3> polygon;
						bool all_interior = true;
						do {
							if (!t->is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
								polygon.push_back(t->voronoi_vertex(c_circ));
								Cell_handle c = c_circ;
								std::cout << "Vor vertex address: " << &(*c) << std::endl;
								*this << polygon.back();
								//						std::cout << t->dual(c_circ) << std::endl;
							} else all_interior = false;
							c_circ++;
						} while (c_circ!=c_start);
						std::cout << "This face has " << polygon.size() << " vertices" << std::endl;
						if (polygon.size()>2 && (all_interior==true || !Application_settings::get_bool_setting("ignore-incomplete-v-voronoi-faces-for-medial-axis"))) {
							Cell_handle c = e_it->first;
							Point_3 a = c->vertex(e_it->second)->point();
							Point_3 b = c->vertex(e_it->third)->point();
							double length = sqrt(CGAL::to_double(squared_distance(a,b)));
							Vector_3 normal = (b - a) / length;
							*this << NORMAL;
							*this << CGAL::ORIGIN + normal;
							*this << polygon;
						}


					} else 
						std::cout << "Voronoi face id " << id << " is not medial" << std::endl;
				}
			}
			return;
		}

		Finite_vertices_iterator v_it, v_end = t->finite_vertices_end(); int counter=0;
//		*this << NO_BOUNDING;
		for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++, counter++) {
			if (counter==id) {
				std::cout << LOG_BLUE << "V vertex " << counter << std::endl;
				Cell_vector cells; Insert_iterator ii(cells);
				t->incident_cells(v_it,ii);
				Cell_vector_iterator c_it, c_end = cells.end();
				Cell_handle infinite_cell = 0;

				// display the class we are interested in
				int cc = 0;
				for (c_it = cells.begin(); c_it!=c_end; ++c_it, ++cc) {
					if (!t->is_infinite(*c_it)) {
						if (do_dual_cell && cc==Application_settings::get_int_setting("v-vertex-voronoi-vertex-id")) {
							Point_3 v0 = (*c_it)->vertex(0)->point();
							Point_3 v1 = (*c_it)->vertex(1)->point();
							Point_3 v2 = (*c_it)->vertex(2)->point();
							Point_3 v3 = (*c_it)->vertex(3)->point();
							std::cout << "V0: " << v0 << "\n V1: " << v1 << "\n V2: " << v2 << "\n V3: " << v3 << std::endl;
							*this << Triangle_3(v0,v1,v2);
							*this << Triangle_3(v0,v1,v3);
							*this << Triangle_3(v2,v1,v3);
							*this << Triangle_3(v2,v0,v3);
							*this << Segment_3(t->dual(*c_it), v0);
						}

					}
				}


				// set every vertex to unclassified, only infinite will be exterior
				std::set<Cell_handle> visited_cells;
				std::queue<Cell_handle> CQ;
				for (c_it = cells.begin(); c_it!=c_end; ++c_it) {
					if (!t->is_infinite(*c_it))
						(*c_it)->set_medial_classification(M_UNCLASSIFIED);
					else {
						CQ.push(*c_it);
						visited_cells.insert(*c_it);
						(*c_it)->set_medial_classification(M_EXTERIOR);
					}
				}


				if (CQ.empty()) {
					std::cout << "We have no infinite voronoi vertex for this v point" << std::endl;

				}

				// while neighboring voronoi vertices can be reached mark them exterior
				while (!CQ.empty()) {
					std::cout << "--- flooding pop" << std::endl;
					Cell_handle c = CQ.front(); CQ.pop();
					assert(c->get_medial_classification() != M_UNCLASSIFIED);
					for (int i=0; i<4; i++) {
						Cell_handle n_c = c->neighbor(i);
						if (n_c->has_vertex(v_it)) {
							std::cout << "neighbor " << i << " is Voronoi vertex of the cell" << std::endl;
							if (visited_cells.find(n_c) == visited_cells.end() && // not visited through the cell flooding
								c->get_crossing_classification(i) == C_UNCLASSIFIED || n_c->is_flat() ) {
//							if (c->get_crossing_classification(i) == C_UNCLASSIFIED && visited_cells.find(n_c) == visited_cells.end()) {
								std::cout << "propagate classification, edge was unclassified" << std::endl;
								n_c->set_medial_classification(c->get_medial_classification());
								CQ.push(n_c);
								visited_cells.insert(n_c);
							}
						} else std::cout << "neighbor " << i << " does not belong to Voronoi cell" << std::endl;
					}
				}
			
				// rest of voronoi vertices are interior
				for (c_it = cells.begin(); c_it!=c_end; ++c_it) {
					if (visited_cells.find(*c_it) == visited_cells.end())
						(*c_it)->set_medial_classification(M_INTERIOR);
				}
		

//				V_triangulation_3::Balls_set bs;
//				t->collect_balls_from_polar_facet_set(v_it->get_dual_source_facets(), bs);
//				V_triangulation_3::Balls_set::iterator bs_it, bs_end = bs.end();
//				for (bs_it = bs.begin(); bs_it!=bs_end; ++bs_it) {
////					std::cout << " a ball" << std::endl;
//					*this << (*bs_it)->point().point();
//					std::cout << "source ball: " << (*bs_it)->point().point() << std::endl;
//				}

				
				cc = 0;
				for (c_it = cells.begin(); c_it!=c_end; ++c_it, ++cc) {
					if (!t->is_infinite(*c_it) && (*c_it)->get_medial_classification() == classification) {
						*this << t->dual(*c_it);			
					}
				}
			}
//			Facet_vector facets; Insert_iterator ii(facets);
//			t->incident_facets(v_it, ii);
//			Facet_vector_iterator f_it, f_end = facets.end();
//			for (f_it = facets.begin(); f_it != f_end; f_it++) {
//				Cell_handle c = f_it->first;
//				int id = f_it->second;
//				Cell_handle n_c = c->neighbor(id);
//				int n_id = n_c->index(c);
//				if (c->get_crossing_classification(id) == classification)
//					if (!t->is_infinite(c) && !t->is_infinite(n_c))
//						*this << Segment_3( t->dual(c), t->dual(n_c));
//					else
//					*this << Triangle_3(c->vertex((id+1)%4)->point(), c->vertex((id+2)%4)->point(), c->vertex((id+3)%4)->point());
////						std::cout << "didn't display a crossing edge because it is line or halfline" << std::endl;
//			}
		}
//		*this << DO_BOUNDING;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:

			return true;
			break;
		case POINT_SIZE_EDITABLE:
			return !do_dual_cell;
		case SCALAR_EDITABLE:
			return false;
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

		if (settings_name=="v-voronoi-face-id") {
//			int id = Application_settings::get_int_setting("v-vertex-id");
			invalidate_cache();
			widget->request_repaint();

		}

		if (settings_name=="v-vertex-id") {
//			int id = Application_settings::get_int_setting("v-vertex-id");
			invalidate_cache();
			widget->request_repaint();

		}
		if (settings_name=="v-vertex-voronoi-vertex-id") {
//			int id = Application_settings::get_int_setting("v-vertex-voronoi-vertex-id");
			invalidate_cache();
			widget->request_repaint();

		}
	}

private:
	Union_of_balls_3 *parent;
	bool do_vertex, do_dual_cell, do_dual_face, do_front_face;
	M_classification classification;
};


#endif //MESECINA_V_CELL_CLASSIFICATION_LAYER_3_H