/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_triangulation_layer_3.h 373 2008-07-11 02:20:13Z miklosb $
 */

#ifndef MESECINA_CLASSIFIED_V_TRIANGULATION_LAYER_3_H
#define MESECINA_CLASSIFIED_V_TRIANGULATION_LAYER_3_H

#include <gui/gl_widget_3/GL_draw_layer_3.h>
#include <gui/app/static/Application_settings.h>

template <class Union_of_balls_3>
class Classified_v_triangulation_layer_3 : public GL_draw_layer_3 {

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

	typedef typename V_triangulation_3::Facet									V_facet;
	typedef std::vector<V_facet> Facet_vector;
	typedef typename Facet_vector::iterator			Facet_vector_iterator;
	typedef std::back_insert_iterator<Facet_vector>	Insert_iterator;

	Classified_v_triangulation_layer_3(const QString& name, Union_of_balls_3* m, const QString& ttip, bool vertex, C_classification c) : GL_draw_layer_3(name, ttip), parent(m) {
		do_vertex = vertex;
		classification = c;
		if (do_vertex) Application_settings::add_int_setting("v-vertex-id", 0);
		Application_settings::add_bool_setting("v-crossing-edges-show-dual", false);
	}

	virtual void draw_commands() {
		V_triangulation_3* t = parent->get_classified_v_triangulation();
		//Point_3 p, p1, p2, p3, mid;

		Finite_vertices_iterator v_it, v_end = t->finite_vertices_end(); int i=0;
		*this << DO_SCALAR;
		*this << NO_BOUNDING;
		for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++, i++) {
			*this << i;
			Facet_vector facets; Insert_iterator ii(facets);
			t->incident_facets(v_it, ii);
			Facet_vector_iterator f_it, f_end = facets.end();
			for (f_it = facets.begin(); f_it != f_end; f_it++) {
				Cell_handle c = f_it->first;
				int id = f_it->second;
				Cell_handle n_c = c->neighbor(id);
//				int n_id = n_c->index(c);
				if (c->get_crossing_classification(id) == classification)
					if (!t->is_infinite(c) && !t->is_infinite(n_c))
						*this << Segment_3( t->dual(c), t->dual(n_c));
					else
						if (Application_settings::get_bool_setting("v-crossing-edges-show-dual"))
							*this << Triangle_3(c->vertex((id+1)%4)->point(), c->vertex((id+2)%4)->point(), c->vertex((id+3)%4)->point());
//						std::cout << "didn't display a crossing edge because it is line or halfline" << std::endl;
			}
		}
		*this << DO_BOUNDING;
		*this << NO_SCALAR;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case LINE_WIDTH_EDITABLE:
			return !do_vertex;
			break;
		case SCALAR_EDITABLE:
			return !do_vertex;
		case POINT_SIZE_EDITABLE:
			return do_vertex;
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
		if (settings_name=="v-vertex-id") {
			int id = Application_settings::get_int_setting("v-vertex-id");

			scalar_min = scalar_max = id;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}
	}

private:
	Union_of_balls_3 *parent;
	bool do_vertex;
	C_classification classification;
};


#endif //MESECINA_CLASSIFIED_V_TRIANGULATION_LAYER_3_H