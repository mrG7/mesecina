
#ifndef MESECINA_MOEBIUS_BALLS_LAYER_2_H
#define MESECINA_MOEBIUS_BALLS_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <list>
#include <algorithm>

template <class Moebius_2>
class Moebius_balls_layer_2 : public GL_draw_layer_2 {
public:

	Moebius_balls_layer_2(const QString& name, Moebius_2* m, QString ttip, bool center, bool fill, bool use_scale_factor = false) : GL_draw_layer_2(name, ttip), parent(m) {
		do_center = center;
		do_fill = fill;
		do_use_scale_factor = use_scale_factor;

		Application_settings::add_double_setting("moebius-balls-scale-factor",1);
	}

	typedef typename Moebius_2::RT_3 RT_3;
	typedef typename Moebius_2::Point_2 Point_2;
	typedef typename Moebius_2::Circle_2 Circle_2;



	virtual void draw_commands() {

		double squared_scale_factor = 1;
		if (do_use_scale_factor) {
			squared_scale_factor = Application_settings::get_double_setting("moebius-balls-scale-factor");
			squared_scale_factor = squared_scale_factor * squared_scale_factor;
		}

		const RT_3* rt_3 = parent->get_lifted_triangulation();
		if (do_fill) *this << RENDER_FILL;

		typename RT_3::Finite_vertices_iterator v_it, v_end = rt_3->finite_vertices_end();
		for (v_it = rt_3->finite_vertices_begin(); v_it!=v_end; v_it++) {

			Point_2 center(v_it->input_point.x, v_it->input_point.y);
			if (do_center)
				*this << center;
			else
				*this << Circle_2(center, v_it->input_point.z*v_it->input_point.z*squared_scale_factor);
		}
		if (do_fill) *this << RENDER_NO_FILL;

	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
		case POINT_SIZE_EDITABLE:
			return do_center;
		case LINE_WIDTH_EDITABLE:
			return !do_center && !do_fill;
			break;
		default:
			return false;
			break;
		}
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if (do_use_scale_factor && settings_name=="moebius-balls-scale-factor") {
			invalidate_cache();
			widget->request_repaint();
		}
	}


private:
	Moebius_2 *parent;
	bool do_center, do_fill, do_use_scale_factor;
};


#endif //MESECINA_MOEBIUS_BALLS_LAYER_2_H