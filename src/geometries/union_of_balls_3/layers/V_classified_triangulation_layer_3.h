/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_triangulation_layer_3.h 373 2008-07-11 02:20:13Z miklosb $
 */

#ifndef MESECINA_V_CLASSIFIED_TRIANGULATION_LAYER_3_H
#define MESECINA_V_CLASSIFIED_TRIANGULATION_LAYER_3_H

#include <gui/gl_widget_3/GL_draw_layer_3.h>
#include <gui/app/static/Application_settings.h>

#include <boost/foreach.hpp>

template <class Union_of_balls_3>
class V_classified_triangulation_layer_3 : public GL_draw_layer_3 {

public:
	typedef typename Union_of_balls_3::V_triangulation_3						 V_triangulation_3;
	typedef typename Union_of_balls_3::Edges_with_doubles						 Edges_with_doubles;
	typedef typename Union_of_balls_3::Edges_set								 Edges_set;
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
	typedef typename Facet_vector::iterator			Facet_vector_iterator;
	typedef std::back_insert_iterator<Facet_vector>	Facet_insert_iterator;

	V_classified_triangulation_layer_3(const QString& name, Union_of_balls_3* m, const QString& ttip, bool vertex, M_classification c, bool medial_axis = false, bool scale_balls = false, bool fill=true, bool lambda = false, bool angle = false, bool scale = false, bool top_angle = false) : GL_draw_layer_3(name, ttip), parent(m) {
		do_medial_axis = medial_axis;
		do_vertex = vertex;
		do_scale_balls = scale_balls;
		do_fill = fill;
		do_lambda = lambda;
		do_angle = angle;
		do_scale = scale;
		do_top_angle = top_angle;
		classification = c;
		Application_settings::add_double_setting("lambda-limit", 0.01);
		Application_settings::add_double_setting("angle-limit", 10);
		Application_settings::add_int_setting("angle-id", 10);
		Application_settings::add_double_setting("scale-limit", 1);
//		Application_settings::add_bool_setting("ignore-incomplete-v-voronoi-faces-for-medial-axis", false);
	}

	virtual void draw_commands() {
		V_triangulation_3* t = parent->get_flooded_v_triangulation();
		Edges_set* medial_edges = parent->get_medial_axis();
//		if (do_angle) t = parent->get_local_filtered_medial_axis();
		Edges_with_doubles* edges_with_stability;
		if (do_scale) edges_with_stability = parent->get_scale_filtered_medial_axis();
		if (do_angle) edges_with_stability = parent->get_angle_filtered_medial_axis();
		if (do_top_angle) edges_with_stability = parent->get_topology_angle_filtered_medial_axis();
		
		if (do_fill) *this << RENDER_FILL;
		else *this << RENDER_NO_FILL;
		if (do_vertex || do_scale_balls) {
			*this << NO_BOUNDING;
			double scale_back = parent->growth_ratio * parent->growth_ratio;
			if (do_scale_balls)
				if (do_fill) *this << RENDER_FILL;
				else *this << RENDER_NO_FILL;
			Finite_cells_iterator c_it, c_end = t->finite_cells_end();
			for (c_it = t->finite_cells_begin(); c_it!=c_end; ++c_it) {
				if (c_it->get_medial_classification()==classification) {
					//if (c_it->is_flat()) {
					//	std::cout << "--- flat vertex" << std::endl;
					//	for (int i=0; i<4; i++) {

					//		std::cout << i << " neighbor vertex: " << t->dual(c_it->neighbor(i)) << std::endl;
					//	}
					//}
					//*this << t->dual(c_it);
					if (do_scale_balls) {
						Point_3 c = t->voronoi_vertex(c_it);
						double sqradius = CGAL::to_double(squared_distance(c, c_it->vertex(0)->point()));
						*this << Sphere_3(c, sqradius/scale_back);
					}
					*this << t->voronoi_vertex(c_it);
				}
			}
			*this << DO_BOUNDING;
		}
		if (do_medial_axis) {
			if (/*do_medial_axis ||*/ do_lambda || do_angle || do_scale || do_top_angle) *this << DO_INVERSE_SCALAR;
			if (do_scale || do_angle || do_top_angle) {
				std::cout << (do_fill ? "polygons" : "polygon edges") << " to render: " << edges_with_stability->size() << std::endl;
				BOOST_FOREACH(Edges_with_doubles::value_type i, *edges_with_stability) {
					*this << i.second;
					V_edge e = t->create_edge_from_vertex_pair(i.first);
//					std::cout << "stability of face: " << i.second << std::endl;
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
					}

				}

			} else {
				std::cout << "medial axis from the new container" << std::endl;
				BOOST_FOREACH(Edges_set::value_type i, *medial_edges) {
					V_edge e = t->create_edge_from_vertex_pair(i);

					Cell_circulator c_start, c_circ = t->incident_cells(e);
					c_start = c_circ;
					std::list<Point_3> polygon;
					bool all_interior = true;
					do {
						if (!t->is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
							polygon.push_back(t->voronoi_vertex(c_circ));
							//						std::cout << t->dual(c_circ) << std::endl;
						} else all_interior = false;
						c_circ++;
					} while (c_circ!=c_start);
					if (polygon.size()>2 && (all_interior==true || !Application_settings::get_bool_setting("ignore-incomplete-v-voronoi-faces-for-medial-axis"))) {
						//*this << counter++;
						if (do_lambda) {
//							V_triangulation_3::Edge e = *e_it;
							*this << t->lambda_value(e);
						}
						//if (do_angle) {
						//	V_triangulation_3::Edge e = *e_it;
						//	*this << t->angle_value(e);
						//}
						Cell_handle c = e.first;
						Point_3 a = c->vertex(e.second)->point();
						Point_3 b = c->vertex(e.third)->point();
						double length = sqrt(CGAL::to_double(squared_distance(a,b)));
						Vector_3 normal = (b - a) / length;
						//					*this << Segment_3(a,b);
						*this << NORMAL;
						*this << CGAL::ORIGIN + normal;
						*this << polygon;
					}
				}
//				std::cout << "There are " << all_edges << " edges in the whole triangulation and " << counter << "  have been found as medial and sent to display" << std::endl;
			}
			if (/*do_medial_axis ||*/ do_lambda|| do_angle || do_scale || do_top_angle) *this << NO_SCALAR;
		}

	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case GLUT_SPHERE:
			return do_scale_balls;
			break;
		case USER_PROPERTY_1:
		case USER_PROPERTY_2:
		case USER_PROPERTY_3:
		case USER_PROPERTY_4:
			return do_medial_axis;
		case SCALAR_EDITABLE:
			return /*do_medial_axis ||*/ do_lambda|| do_angle || do_scale || do_top_angle;
		case LINE_WIDTH_EDITABLE:
			return !do_fill;
			break;
		case POINT_SIZE_EDITABLE:
			return do_vertex || !do_fill;
			break;
		default:
			return false;
			break;
		}
	}

	virtual QString get_name_user_property(Layer_property l) { 
		if (l == SCALAR_EDITABLE) 
			if (do_lambda) return "Lambda";
			else if (do_angle || do_top_angle) return "Angle";
				else return "Scale";
		if (l == USER_PROPERTY_1) return "Medial axis to off";
		if (l == USER_PROPERTY_2) return "Simplified MA to off";
		if (l == USER_PROPERTY_3) return "Write MOFF file";
		if (l == USER_PROPERTY_4) return "Write medial balls file";
		else return "";
	}


	// this is called when a custom action is clicked by the user
	virtual void execute_user_property(Layer_property l) {

		if (l == USER_PROPERTY_1) {

			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose an off filename to save balls to",
				settings.value("last-data-directory",QString()).toString(),
				"OFF (*.off)");
			if (file_name=="") return;
			if (!file_name.endsWith(".off")) file_name += ".off";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);

			parent->write_medial_axis_to_off(file_name.toStdString());
		}

		if (l == USER_PROPERTY_2) {

			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose an off filename to save balls to",
				settings.value("last-data-directory",QString()).toString(),
				"OFF (*.off)");
			if (file_name=="") return;
			if (!file_name.endsWith(".off")) file_name += ".off";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);

			parent->write_simplified_medial_axis_to_off(file_name.toStdString());
		}		

		if (l == USER_PROPERTY_3) {

			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose an off filename to save balls to",
				settings.value("last-data-directory",QString()).toString(),
				"OFF (*.moff)");
			if (file_name=="") return;
			if (!file_name.endsWith(".moff")) file_name += ".moff";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);

			parent->write_medial_axis_to_moff(file_name.toStdString());
		}	

		if (l == USER_PROPERTY_4) {

			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose an off filename to save balls to",
				settings.value("last-data-directory",QString()).toString(),
				"WOFF (*.woff)");
			if (file_name=="") return;
			if (!file_name.endsWith(".woff")) file_name += ".woff";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);

			parent->write_less_medial_balls(file_name.toStdString());
		}

	}

	virtual void application_settings_changed(const QString& settings_name) {

		if (do_medial_axis && do_lambda && settings_name=="lambda-limit") {
			double lambda_limit = Application_settings::get_double_setting("lambda-limit");

			if (lambda_limit < 0) scalar_min = 0;
			else scalar_min = lambda_limit;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}

		if (do_medial_axis && (do_angle || do_top_angle) && settings_name=="angle-limit") {
			double lambda_limit = Application_settings::get_double_setting("angle-limit");

			if (lambda_limit < 0) scalar_min = 0;
			else scalar_min = lambda_limit;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}

		if (do_medial_axis && do_top_angle && settings_name=="angle-id") {
			double lambda_limit = Application_settings::get_double_setting("angle-id");

			if (lambda_limit < 0) scalar_max = scalar_min = 0;
			else scalar_max = scalar_min = lambda_limit;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}		

		if (do_medial_axis && do_scale && settings_name=="scale-limit") {
			double lambda_limit = Application_settings::get_double_setting("scale-limit");

			if (lambda_limit < 0) scalar_min = 0;
			else scalar_min = lambda_limit;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}
		


		//if (do_medial_axis && settings_name=="ignore-incomplete-v-voronoi-faces-for-medial-axis") {
		//	invalidate_cache();
		//	widget->request_repaint();
		//}
	}

private:
	Union_of_balls_3 *parent;
	bool do_vertex, do_medial_axis, do_scale_balls, do_fill, do_lambda, do_angle, do_scale, do_top_angle;
	M_classification classification;
};


#endif //MESECINA_V_CLASSIFIED_TRIANGULATION_LAYER_3_H