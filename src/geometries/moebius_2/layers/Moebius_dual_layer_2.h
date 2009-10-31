
#ifndef MESECINA_MOEBIUS_DUAL_LAYER_2_H
#define MESECINA_MOEBIUS_DUAL_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <list>
#include <algorithm>

template <class Moebius_2>
class Moebius_dual_layer_2 : public GL_draw_layer_2 {
public:

	Moebius_dual_layer_2(const QString& name, Moebius_2* m, QString ttip, bool critical_point, bool vertex, bool fill) : GL_draw_layer_2(name, ttip), parent(m) {
		do_critical_point = critical_point;
		do_vertex = vertex;
		do_fill = fill;
		Application_settings::add_bool_setting("moebius-dual-segments-limited-by-growth-setting",false);
	}

	typedef typename Moebius_2::RT_3 RT_3;
	typedef typename RT_3::Vertex_handle Vertex_handle;
	typedef typename Moebius_2::Critical_point_and_hull Critical_point_and_hull;
	typedef typename Moebius_2::Point_2 Point_2;
	typedef typename Moebius_2::Triangle_2 Triangle_2;
	typedef typename Moebius_2::Segment_2 Segment_2;


	virtual void draw_commands() {
		if (!parent->enough_spheres()) return;

		const std::vector<Critical_point_and_hull>* moebius_vertices = parent->get_moebius_vertices();
		std::vector<Critical_point_and_hull>::const_iterator mv_it, mv_end = moebius_vertices->end();
		*this << DO_SCALAR;
		for (mv_it = moebius_vertices->begin(); mv_it!=mv_end; ++mv_it) {
			Critical_point_and_hull cph = *mv_it;
			Vertex_handle ball_0 = cph.second.get<0>();
			Vertex_handle ball_1 = cph.second.get<1>();
			Vertex_handle ball_2 = cph.second.get<2>();
			double appear = ball_1->multiplicative_distance_to(cph.first);
			*this << appear;
			if (do_critical_point) 
				*this << cph.first;
			else {
				if (do_vertex) {
					*this << ball_1->input_center();
					*this << ball_2->input_center();
					*this << ball_0->input_center();
				} else {
					if (do_fill) {
						*this << Triangle_2(ball_0->input_center(), ball_1->input_center(), ball_2->input_center());
					} else {
						*this << Segment_2(ball_0->input_center(), ball_1->input_center());
						*this << Segment_2(ball_2->input_center(), ball_1->input_center());
						*this << Segment_2(ball_2->input_center(), ball_0->input_center());
					}
				}
			}
		}
		*this << NO_SCALAR;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case SCALAR_EDITABLE:
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

	virtual QString get_name_user_property(Layer_property l) { 
		if (l == SCALAR_EDITABLE) 
			return "Multiplicative birth";
		return "";
	}

	virtual void application_settings_changed(const QString& settings_name) {
		bool segments = !do_critical_point && !do_vertex && !do_fill;
		if (settings_name=="moebius-balls-scale-factor" && 
			(!segments || Application_settings::get_bool_setting("moebius-dual-segments-limited-by-growth-setting"))) {
			double mult_limit = Application_settings::get_double_setting("moebius-balls-scale-factor");

			if (mult_limit < 0) scalar_min = 0;
			else scalar_max = mult_limit;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}
	}


private:
	Moebius_2 *parent;
	bool do_critical_point, do_fill, do_vertex;
};


#endif //MESECINA_MOEBIUS_DUAL_LAYER_2_H