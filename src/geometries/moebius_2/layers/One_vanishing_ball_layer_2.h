
#ifndef MESECINA_ONE_VANISHING_BALL_LAYER_2_H
#define MESECINA_ONE_VANISHING_BALL_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <list>
#include <algorithm>

template <class Moebius_2>
class One_vanishing_ball_layer_2 : public GL_draw_layer_2 {
public:

	One_vanishing_ball_layer_2(const QString& name, Moebius_2* m, QString ttip, bool vanishing_point, bool fill) : GL_draw_layer_2(name, ttip), parent(m) {
		do_vanishing_point = vanishing_point;
		do_fill = fill;

		Application_settings::add_int_setting("moebius-vanishing-event-index",0);
	}

	typedef typename Moebius_2::RT_3 RT_3;
	typedef typename RT_3::Vertex_handle Vertex_handle;
	typedef typename Moebius_2::Point_2 Point_2;
	typedef typename Moebius_2::Circle_2 Circle_2;



	virtual void draw_commands() {
		if (!parent->enough_spheres()) return;

		size_t index = Application_settings::get_int_setting("moebius-vanishing-event-index");
		const std::vector<Vertex_handle>* vhs = parent->get_handles_sorted_by_date();
		if (index>=vhs->size()) return;

		Vertex_handle v_it = (*vhs)[index];

		*this << NO_BOUNDING;
		if (do_fill) *this << RENDER_FILL;
		if (do_vanishing_point) 
			*this << v_it->vanishing_point;
		else {
			Point_2 center(v_it->input_point.x, v_it->input_point.y);
			*this << Circle_2(center, v_it->input_point.z*v_it->input_point.z*v_it->vanishing_date*v_it->vanishing_date);
		}
		if (do_fill) *this << RENDER_NO_FILL;
		*this << DO_BOUNDING;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case POINT_SIZE_EDITABLE:
			return do_vanishing_point;
			break;
		case LINE_WIDTH_EDITABLE:
			return !do_vanishing_point && !do_fill;
		default:
			return false;
			break;
		}
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if (settings_name=="moebius-vanishing-event-index") {
			size_t index = Application_settings::get_int_setting("moebius-vanishing-event-index");
			const std::vector<Vertex_handle>* vhs = parent->get_handles_sorted_by_date();
			if (index < vhs->size()) {
				Vertex_handle v_it = (*vhs)[index];
				Application_settings::set_setting("moebius-balls-scale-factor",v_it->vanishing_date);
				parent->invalidate_scaled_balls_layer();
			}

			invalidate_cache();
			widget->request_repaint();
		}
	}


private:
	Moebius_2 *parent;
	bool do_vanishing_point, do_fill;
};


#endif //MESECINA_ONE_VANISHING_BALL_LAYER_2_H