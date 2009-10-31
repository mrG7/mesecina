
#ifndef MESECINA_CHRISTOPHE_MOBEIUS_LAYER_2_H
#define MESECINA_CHRISTOPHE_MOBEIUS_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <list>
#include <algorithm>

#include <CGAL/Cartesian.h>
#include <CGAL/Moebius_diagram_2.h>
#include <CGAL/Moebius_diagram_euclidean_traits_2.h>

template <class Moebius_2>
class Christophe_moebius_layer_2 : public GL_draw_layer_2 {
public:

	Christophe_moebius_layer_2(const QString& name, Moebius_2* m, QString ttip) : GL_draw_layer_2(name, ttip), parent(m) {
	}

	typedef typename CGAL::Cartesian<double> MK;
	typedef double MW;
	typedef CGAL::Moebius_diagram_euclidean_traits_2<MK,MW> MGt;
	typedef CGAL::Moebius_diagram_2<MGt> M;
	typedef typename M::Moebius_edge Moebius_edge;
	typedef typename M::RT_3 MRt;

	void draw_moebius_segment(const Moebius_edge& e) {
		typedef typename Moebius_edge::Point Point;
		typedef typename Moebius_edge::Ray Ray;
		typedef typename Moebius_edge::Segment Segment;
		typedef typename Moebius_edge::Direction Direction;
		typedef Arc_2<MK> Arc_2;

		typedef typename Moebius_edge::Const_iterator Iterator;
		Iterator it = e.begin ();
		Iterator end = e.end ();
//		std::cout << "drawing a moebius segment" << std::endl;
		if (it == end) {
			if (e.is_line ()) {
				*this << e.line ();
			} else {
				*this << e.circle ();
			}
		}

		while (it != end) {
			Point *start = *(it++);
			//		CGAL_assertion (it != end);
			Point *stop = *(it++);

			if (e.is_line ()) {
				Direction dir (e.line ());
				if (start == NULL) {
					*this << Ray (*stop, -dir);
				} else if (stop == NULL) {
					*this << Ray (*start, dir);
				} else if (e.are_ordered (*start, *stop)) {
					*this << Segment (*start, *stop);
				} else {

					*this << Ray (*start, dir);
					*this << Ray (*stop, -dir);
				}
			} else {
				*this << *start;
				*this << *stop;
				*this << Arc_2(e.circle(), *start, *stop);
			}
		}

	}


	virtual void draw_commands() {
		parent->get_lifted_triangulation(); // make sure we have points
		std::list<Point3D>* points = parent->get_weighted_points();
		std::list<MGt::Point_2> moebius_points;

		std::list<Point3D>::iterator p_it, p_end = points->end();
		for (p_it = points->begin(); p_it!=p_end; ++p_it) {
			moebius_points.push_back(MGt::Point_2(MK::Point_2(p_it->x, p_it->y),1/p_it->z/p_it->z,0));
//			std::cout << moebius_points.back() << std::endl;
		}


//		std::cout << "Starting" << std::endl;

		//std::ifstream in ("c:\\data\\moebius.cin");
		//std::istream_iterator<MGt::Point_2> start (in);
		//std::istream_iterator<MGt::Point_2> stop;

		//std::cout << "File opened" << std::endl;

		M dia;

//		std::cout << "Created" << std::endl;

		std::list<MGt::Point_2>::iterator istart = moebius_points.begin();
		std::list<MGt::Point_2>::iterator iend = moebius_points.end();

		int n = dia.init (istart, iend);

//		std::cout << "Initialized " << n << std::endl;

		MRt rt (dia.rt ());

//		std::cout << "Copied" << std::endl;

		rt.is_valid ();

//		std::cout << "Validated" << std::endl;

		dia.build ();
		dia.construct();

//		std::cout << "Built" << std::endl;

		*this << NO_BOUNDING;
		typedef typename M::Moebius_edge_const_iterator Iterator;
		Iterator i = dia.moebius_edges_const_begin (), end = dia.moebius_edges_const_end ();
		while (i != end) {
			draw_moebius_segment(*i);
			++i;
		}
		*this << DO_BOUNDING;


//		const std::vector<Critical_point_and_hull>* critical_points = parent->get_critical_points();
//
//		if (!parent->enough_spheres()) return;
//

//		*this << DO_SCALAR;
//		for (size_t i = 0; i < critical_points->size(); ++i) {
//			*this << (*critical_points)[i].second.get<1>()->multiplicative_distance_to((*critical_points)[i].first);
//			*this << (*critical_points)[i].first;
////			*this << k++;
////			*this << (*critical_points)[i].second.get<2>()->input_center();
////			*this << (*critical_points)[i].second.get<1>()->input_center();
////			if((*critical_points)[i].second.get<0>() != (*critical_points)[i].second.get<1>())
////				*this << (*critical_points)[i].second.get<0>()->input_center();
//			//for(std::vector<Point_2>::const_iterator a = v_it->extremal_points.begin(); a != v_it->extremal_points.end(); ++a) {
//			//	*this << *a;
//			//}
//		}
//		*this << NO_SCALAR;

	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case USER_PROPERTY_1:
		case LINE_WIDTH_EDITABLE:
		case POINT_SIZE_EDITABLE:
			return true;
			break;
		case SCALAR_EDITABLE:
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

	//virtual void execute_user_property(Layer_property lp) { 
	//	if (lp == USER_PROPERTY_1) {
	//		const std::vector<Critical_point_and_hull>* critical_points = parent->get_critical_points();
	//		std::vector<Critical_point_and_hull>::const_iterator cp_it, cp_end = critical_points->end();
	//		QString text_x = "[";
	//		QString text_y = "[";
	//		for (cp_it=critical_points->begin(); cp_it!=cp_end; ++cp_it) {
	//			Vertex_handle ball = cp_it->second.get<1>();
	//			Point_2 cp =  cp_it->first;
	//			text_x = tr("%1%2, ").arg(text_x).arg(ball->multiplicative_distance_to(cp));
	//			text_y = tr("%1%2, ").arg(text_y).arg(1);
	//		}
	//		text_x.chop(2);
	//		text_x = tr("%1]").arg(text_x);
	//		text_y.chop(2);
	//		text_y = tr("%1]").arg(text_y);
	//		std::cout << "Topological events happen at growth factors of: " << text_x.toStdString() << std::endl;
	//		text_x = tr("hold on\n scatter(%1,%2,'x')\n a = axis\n a(1)=1\n axis(a)\n hold off\n").arg(text_x).arg(text_y);
	//		QApplication::clipboard()->setText(text_x);
	//	}
	//}

private:
	Moebius_2 *parent;
};


#endif //MESECINA_CHRISTOPHE_MOBEIUS_LAYER_2_H