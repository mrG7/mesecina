/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Lambda_onestep_layer_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef FILTERED_MEDIAL_AXIS_LAYER_2_H
#define FILTERED_MEDIAL_AXIS_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>
#include <gui/app/static/Application_settings.h>

template <class Power_crust_2>
class Filtered_medial_axis_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Power_crust_2::Triangulation_2						  Triangulation_2;
	typedef typename Triangulation_2::Finite_edges_iterator			Finite_edges_iterator;
	typedef typename Triangulation_2::Finite_faces_iterator		Finite_faces_iterator;
	typedef typename Triangulation_2::Face_handle							  Face_handle;
	typedef typename Power_crust_2::Point_2										  Point_2;
	typedef typename Power_crust_2::Segment_2									Segment_2;
	typedef typename Power_crust_2::Line_2									Line_2;
	typedef typename Power_crust_2::Ray_2									Ray_2;
	typedef typename Power_crust_2::Circle_2									Circle_2;
	Filtered_medial_axis_layer_2(const QString& name, Power_crust_2* m, QString ttip = "", bool filter = false, bool only_interior = true, bool balls = false, bool fill = true, bool lambda = true, bool scale = false ) : 
			GL_draw_layer_2(name, ttip), parent(m), do_filter(filter), do_lambda(lambda), do_interior(only_interior), do_balls(balls), do_fill(fill), do_scale(scale) {
		if (filter)
			if (do_lambda) Application_settings::add_double_setting("lambda-limit", 10);
			else if (!do_scale) Application_settings::add_double_setting("angle-limit", 5);
			else Application_settings::add_double_setting("scale-limit", 1);
	}

	virtual void draw_commands() {
		Triangulation_2* t = parent->get_crust_classified_triangulation();
		if (do_filter) {
			t = parent->get_lambda_angle_classified_triangulation();
			*this << DO_INVERSE_SCALAR;
		}
		if (!do_interior) *this << NO_BOUNDING;


		if (do_balls) { // drawing balls
			if (do_fill) *this << RENDER_FILL;
			Finite_faces_iterator f_it, f_end = t->finite_faces_end();
			for (f_it = t->finite_faces_begin(); f_it!=f_end; f_it++) {
				if (!do_interior || f_it->get_onestep_classification() == O_INTERIOR) {
					Point_2 center = t->dual(f_it);
					if (do_filter && do_lambda) *this << f_it->get_vertex_lambda();
					if (do_filter && !do_lambda && !do_scale) *this << f_it->get_vertex_angle();
					if (do_filter && !do_lambda && do_scale) *this << f_it->get_vertex_scale();
					*this << Circle_2(center, squared_distance(center, f_it->vertex(0)->point()));
				}
			}
			if (do_fill) *this << RENDER_NO_FILL;
		} else { // drawing segments
			for (Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
				Face_handle f = e_it->first;
				int id = e_it->second;
				Face_handle nF = f->neighbor(id);
				if ((do_interior && f->get_onestep_classification() == O_INTERIOR && nF->get_onestep_classification() == O_INTERIOR)
					|| (!do_interior && f->get_edge_classification(id) != C_CRUST) ) {
						if (do_filter && do_lambda) *this << t->lambda_edge(*e_it);
						if (do_filter && !do_lambda && !do_scale) *this << t->angle_edge(*e_it);
						if (do_filter && !do_lambda && do_scale) {
							*this << t->scale_edge(*e_it);
//							std::cout << "edge scale value: " << t->scale_edge(*e_it) << std::endl;
						}
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

		if (!do_interior) *this << DO_BOUNDING;
		if (do_filter) *this << NO_SCALAR;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case LINE_WIDTH_EDITABLE:
			return true;
			break;
		case SCALAR_EDITABLE:
			return do_filter;
			break;
		case POINT_SIZE_EDITABLE:
		default:
			return false;
			break;
		}
	}

	// this tells the display name of the custom action
	virtual QString get_name_user_property(Layer_property l) { 
		if (l == SCALAR_EDITABLE) 
			if (do_lambda) return "Lambda";
			else if (!do_scale) return "Angle";
			else return "Scale";
		return "";
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if (do_filter && do_lambda && settings_name=="lambda-limit") {
			double lambda_limit = Application_settings::get_double_setting("lambda-limit");

			if (lambda_limit < 0) scalar_min = 0;
			else scalar_min = lambda_limit;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}
		if (do_filter && !do_lambda && !do_scale && settings_name=="angle-limit") {
			double angle_limit = Application_settings::get_double_setting("angle-limit");

			if (angle_limit < 0) scalar_min = 0;
			else scalar_min = angle_limit;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}

		if (do_filter && !do_lambda && do_scale && settings_name=="scale-limit") {
			double angle_limit = Application_settings::get_double_setting("scale-limit");

			if (angle_limit < 0) scalar_min = 0;
			else scalar_min = angle_limit;
			invalidate_cache();
			has_new_range = false;
			if (!Application_settings::get_bool_setting("ball-medial-axis-follow-scale-limit")) widget->request_repaint();

		}
	}

private:
	Power_crust_2 *parent;
	bool do_filter, do_lambda, do_interior, do_balls, do_fill, do_scale;
};


#endif //FILTERED_MEDIAL_AXIS_LAYER_2_H