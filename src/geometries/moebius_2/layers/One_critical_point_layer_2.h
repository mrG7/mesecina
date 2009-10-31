
#ifndef MESECINA_ONE_CRITICAL_POINT_LAYER_2_H
#define MESECINA_ONE_CRITICAL_POINT_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <list>
#include <algorithm>

template <class Moebius_2>
class One_critical_point_layer_2 : public GL_draw_layer_2 {
public:

	One_critical_point_layer_2(const QString& name, Moebius_2* m, QString ttip, bool critical_point, bool vertex, bool fill, bool scaled) : GL_draw_layer_2(name, ttip), parent(m) {
		do_critical_point = critical_point;
		do_vertex = vertex;
		do_fill = fill;
		do_scaled = scaled;
		Application_settings::add_int_setting("moebius-critical-event-index",0);
	}

	typedef typename Moebius_2::RT_3 RT_3;
	typedef typename RT_3::Vertex_handle Vertex_handle;
	typedef typename Moebius_2::Critical_point_and_hull Critical_point_and_hull;
	typedef typename Moebius_2::Point_2 Point_2;
	typedef typename Moebius_2::Circle_2 Circle_2;



	virtual void draw_commands() {
		if (!parent->enough_spheres()) return;

		size_t index = Application_settings::get_int_setting("moebius-critical-event-index");
		const std::vector<Critical_point_and_hull>* critical_points = parent->get_critical_points();
		if (index>=critical_points->size()) return;

		Critical_point_and_hull cph = (*critical_points)[index];

		*this << NO_BOUNDING;
		if (do_fill) *this << RENDER_FILL;
		if (do_critical_point) 
			*this << cph.first;
		else {
			Vertex_handle ball_0 = cph.second.get<0>();
			Vertex_handle ball_1 = cph.second.get<1>();
			Vertex_handle ball_2 = cph.second.get<2>();
			if (do_vertex) {
				*this << ball_1->input_center();
				*this << ball_2->input_center();
				if (ball_0 != ball_1) *this << ball_0->input_center();
			} else {
				double square_scale = 1;
				if (do_scaled) {
					square_scale = ball_1->multiplicative_distance_to(cph.first);
					square_scale = square_scale *square_scale;
				}
				*this << Circle_2(ball_1->input_center(), ball_1->input_point.z*ball_1->input_point.z*square_scale);
				*this << Circle_2(ball_2->input_center(), ball_2->input_point.z*ball_2->input_point.z*square_scale);
				if (ball_0 != ball_1) *this << Circle_2(ball_0->input_center(), ball_0->input_point.z*ball_0->input_point.z*square_scale);
			}
		}
		if (do_fill) *this << RENDER_NO_FILL;
		*this << DO_BOUNDING;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
		case POINT_SIZE_EDITABLE:
			return do_critical_point || do_vertex;
			break;
		case LINE_WIDTH_EDITABLE:
			return !do_critical_point && !do_vertex && !do_fill;
		default:
			return false;
			break;
		}
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if (settings_name=="moebius-critical-event-index") {

			size_t index = Application_settings::get_int_setting("moebius-critical-event-index");
			const std::vector<Critical_point_and_hull>* critical_points = parent->get_critical_points();
			if (index < critical_points->size()) {
				Critical_point_and_hull cph = (*critical_points)[index];
				Vertex_handle ball_1 = cph.second.get<1>();
				double md_event = ball_1->multiplicative_distance_to(cph.first);
				double epsilon = md_event/1000.0;
				if (index+1 < critical_points->size()) {
					cph = (*critical_points)[index+1];
					Vertex_handle ball_1 = cph.second.get<1>();
					epsilon = (ball_1->multiplicative_distance_to(cph.first) - md_event)/1000.0;
				}

				Application_settings::set_setting("moebius-balls-scale-factor",md_event+epsilon);
				parent->invalidate_scaled_balls_layer();
			}

			invalidate_cache();
			widget->request_repaint();
		}
	}


private:
	Moebius_2 *parent;
	bool do_critical_point, do_fill, do_scaled, do_vertex;
};


#endif //MESECINA_ONE_CRITICAL_POINT_LAYER_2_H