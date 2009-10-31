/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_triangulation_layer_3.h 373 2008-07-11 02:20:13Z miklosb $
 */

#ifndef MESECINA_V_FLAT_VERTICES_LAYER_3_H
#define MESECINA_V_FLAT_VERTICES_LAYER_3_H

#include <gui/gl_widget_3/GL_draw_layer_3.h>
#include <gui/app/static/Application_settings.h>

template <class Union_of_balls_3>
class V_flat_vertices_layer_3 : public GL_draw_layer_3 {

public:
	typedef typename Union_of_balls_3::V_triangulation_3						 V_triangulation_3;
	typedef typename V_triangulation_3::Finite_vertices_iterator				Finite_vertices_iterator;
	typedef typename V_triangulation_3::Finite_cells_iterator					Finite_cells_iterator;
	typedef typename V_triangulation_3::Finite_edges_iterator					Finite_edges_iterator;
	typedef typename V_triangulation_3::Cell_circulator					Cell_circulator;
	typedef typename V_triangulation_3::Polar_facet								Polar_facet;
	typedef typename V_triangulation_3::Polar_facet_set							Polar_facet_set;
	typedef typename V_triangulation_3::Cell_handle							Cell_handle;
	typedef typename V_triangulation_3::Vertex_handle							Vertex_handle;
	typedef typename Union_of_balls_3::Triangle_3								Triangle_3;
	typedef typename Union_of_balls_3::Segment_3								Segment_3;
	typedef typename Union_of_balls_3::Point_3									Point_3;
	typedef typename Union_of_balls_3::Vector_3									Vector_3;


	typedef typename V_triangulation_3::Balls_set					Balls_set;

	typedef typename V_triangulation_3::Edge									V_edge;
	typedef std::vector<V_edge> Edge_vector;
	typedef typename Edge_vector::iterator			Edge_vector_iterator;
	typedef std::back_insert_iterator<Edge_vector>	Insert_iterator;

	typedef typename V_triangulation_3::Facet					V_facet;
	typedef std::vector<V_facet> Facet_vector;
	typedef typename Facet_vector::iterator			Facet_vector_iterator;
	typedef std::back_insert_iterator<Facet_vector>	Facet_insert_iterator;

	V_flat_vertices_layer_3(const QString& name, Union_of_balls_3* m, const QString& ttip, bool vertex) : GL_draw_layer_3(name, ttip), parent(m) {
		do_vertex = vertex;
		Application_settings::add_int_setting("v-flat-cell-id",0);
	}

	virtual void draw_commands() {
		V_triangulation_3* t = parent->get_v_triangulation();

		*this << DO_SCALAR;
		Finite_cells_iterator fc_it, fc_end = t->finite_cells_end(); int fc_counter = 0;
		for (fc_it = t->finite_cells_begin(); fc_it!=fc_end; fc_it++) {
			Vertex_handle v0 = fc_it->vertex(0); Balls_set bs0;
			Vertex_handle v1 = fc_it->vertex(1); Balls_set bs1;
			Vertex_handle v2 = fc_it->vertex(2); Balls_set bs2;
			Vertex_handle v3 = fc_it->vertex(3); Balls_set bs3;

			t->collect_balls_from_polar_facet_set(v0->get_dual_source_facets(), bs0);
			t->collect_balls_from_polar_facet_set(v1->get_dual_source_facets(), bs1);
			t->collect_balls_from_polar_facet_set(v2->get_dual_source_facets(), bs2);
			t->collect_balls_from_polar_facet_set(v3->get_dual_source_facets(), bs3);

			Balls_set i01, i23, i0123;
			set_intersection(bs0.begin(), bs0.end(), bs1.begin(), bs1.end(), std::inserter(i01, i01.begin()));
			set_intersection(bs2.begin(), bs2.end(), bs3.begin(), bs3.end(), std::inserter(i23, i23.begin()));
			set_intersection(i01.begin(), i01.end(), i23.begin(), i23.end(), std::inserter(i0123, i0123.begin()));

			if (i0123.size() > 1) {
				*this <<fc_counter++;
				if (do_vertex) {
					*this << t->dual(fc_it);
					Balls_set::iterator bs_it, bs_end = i0123.end();
					for (bs_it=i0123.begin(); bs_it!=bs_end; ++bs_it) {
						*this << Segment_3(t->dual(fc_it), (*bs_it)->point());
					}

				} else {
					Point_3 v0 = (fc_it)->vertex(0)->point();
					Point_3 v1 = (fc_it)->vertex(1)->point();
					Point_3 v2 = (fc_it)->vertex(2)->point();
					Point_3 v3 = (fc_it)->vertex(3)->point();
//					std::cout << "V0: " << v0 << "\n V1: " << v1 << "\n V2: " << v2 << "\n V3: " << v3 << std::endl;
					*this << Triangle_3(v0,v1,v2);
					*this << Triangle_3(v0,v1,v3);
					*this << Triangle_3(v2,v1,v3);
					*this << Triangle_3(v2,v0,v3);
//					*this << Segment_3(t->dual(fc_it), v0);
				}
//				std::cout << "Voronoi vertex " << fc_counter << " has common source(s): " << i0123.size() << std::endl;
			}
		}
		*this << NO_SCALAR;

	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case SCALAR_EDITABLE:
			return true;
			break;
		case POINT_SIZE_EDITABLE:
		case LINE_WIDTH_EDITABLE:
			return do_vertex;
			break;
		default:
			return false;
			break;
		}
	}

	virtual QString get_name_user_property(Layer_property l) { 
		if (l == SCALAR_EDITABLE) return "Id";
		else return "";
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if (settings_name=="v-flat-cell-id") {
			int id = Application_settings::get_int_setting("v-flat-cell-id");

			scalar_min = scalar_max = id;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}
	}

private:
	Union_of_balls_3 *parent;
	bool do_vertex;
	M_classification classification;
};


#endif //MESECINA_V_FLAT_VERTICES_LAYER_3_H