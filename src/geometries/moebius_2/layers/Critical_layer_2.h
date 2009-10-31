
#ifndef MESECINA_CRITICAL_LAYER_2_H
#define MESECINA_CRITICAL_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <list>
#include <algorithm>

template <class Moebius_2>
class Critical_layer_2 : public GL_draw_layer_2 {
public:

	Critical_layer_2(const QString& name, Moebius_2* m, QString ttip) : GL_draw_layer_2(name, ttip), parent(m) {
		Application_settings::add_double_setting("critical-point-plotting-limit-max-matlab",6);
	}


	typedef typename Moebius_2::RT_3 RT_3;
	typedef typename RT_3::Vertex_handle Vertex_handle;
	typedef typename Moebius_2::Point_2 Point_2;
	typedef typename Moebius_2::Circle_2 Circle_2;
	typedef typename Moebius_2::V_triple V_triple;
	typedef typename Moebius_2::Critical_point_and_hull Critical_point_and_hull;


	virtual void draw_commands() {

		const std::vector<Critical_point_and_hull>* critical_points = parent->get_critical_points();

		if (!parent->enough_spheres()) return;

		*this << NO_BOUNDING;
		*this << DO_SCALAR;
		for (size_t i = 0; i < critical_points->size(); ++i) {
			*this << (*critical_points)[i].second.get<1>()->multiplicative_distance_to((*critical_points)[i].first);
			*this << (*critical_points)[i].first;
//			*this << k++;
//			*this << (*critical_points)[i].second.get<2>()->input_center();
//			*this << (*critical_points)[i].second.get<1>()->input_center();
//			if((*critical_points)[i].second.get<0>() != (*critical_points)[i].second.get<1>())
//				*this << (*critical_points)[i].second.get<0>()->input_center();
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
		case USER_PROPERTY_1:
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
			return "Multiplicative distance";
		if (l== USER_PROPERTY_1)
			return "Events to clipboard";
		return "";
	}

	virtual void execute_user_property(Layer_property lp) { 
		if (lp == USER_PROPERTY_1) {
			const std::vector<Critical_point_and_hull>* critical_points = parent->get_critical_points();
			std::vector<Critical_point_and_hull>::const_iterator cp_it, cp_end = critical_points->end();
			QString text_x = "[";
			QString text_y = "[";
			for (cp_it=critical_points->begin(); cp_it!=cp_end; ++cp_it) {
				Vertex_handle ball = cp_it->second.get<1>();
				Point_2 cp =  cp_it->first;
				text_x = tr("%1%2, ").arg(text_x).arg(ball->multiplicative_distance_to(cp));
				text_y = tr("%1%2, ").arg(text_y).arg(1);
			}
			text_x.chop(2);
			text_x = tr("%1]").arg(text_x);
			text_y.chop(2);
			text_y = tr("%1]").arg(text_y);
			std::cout << "Topological events happen at growth factors of: " << text_x.toStdString() << std::endl;
			double max_limit = Application_settings::get_double_setting("critical-point-plotting-limit-max-matlab");
			text_x = tr("hold on\n scatter(%1,%2,'x')\n a = axis\n a(1)=1\n a(2)=%3\n axis(a)\n hold off\n").arg(text_x).arg(text_y).arg(max_limit);
			QApplication::clipboard()->setText(text_x);
		}
	}

private:
	Moebius_2 *parent;
};


#endif //MESECINA_CRITICAL_LAYER_2_H