
#ifndef MESECINA_VANISHING_LAYER_2_H
#define MESECINA_VANISHING_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <list>
#include <algorithm>

template <class Moebius_2>
class Vanishing_layer_2 : public GL_draw_layer_2 {
public:

	Vanishing_layer_2(const QString& name, Moebius_2* m, QString ttip) : GL_draw_layer_2(name, ttip), parent(m) {
	}

	typedef typename Moebius_2::RT_3 RT_3;
	typedef typename Moebius_2::Point_2 Point_2;
	typedef typename Moebius_2::Circle_2 Circle_2;



	virtual void draw_commands() {

		const RT_3* rt_3 = parent->get_triangulation_with_vanishing_points();

		if (!parent->enough_spheres()) return;

		typename RT_3::Finite_vertices_iterator v_it, v_end = rt_3->finite_vertices_end();
		*this << NO_BOUNDING;
		*this << DO_SCALAR;
		for (v_it = rt_3->finite_vertices_begin(); v_it!=v_end; v_it++) {
			*this << v_it->vanishing_date;
			*this << v_it->vanishing_point;
			//for(std::vector<Point_2>::const_iterator a = v_it->extremal_points.begin(); a != v_it->extremal_points.end(); ++a) {
			//	*this << *a;
			//}
		}
		*this << NO_SCALAR;
		*this << DO_BOUNDING;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case SCALAR_EDITABLE:
		case POINT_SIZE_EDITABLE:
			return true;
			break;
		case LINE_WIDTH_EDITABLE:
		default:
			return false;
			break;
		}
	}

	// this tells the display name of the custom action
	virtual QString get_name_user_property(Layer_property l) { 
		if (l == SCALAR_EDITABLE) 
			return "Vanishing time";
		return "";
	}

private:
	Moebius_2 *parent;
};


#endif //MESECINA_VANISHING_LAYER_2_H