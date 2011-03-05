/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Armin Häberling, ETH Zurich
 *
 * $Id: Landmark_Points_layer_2.h 344 2008-02-29 12:50:20Z arminha $
 */

#ifndef MESECINA_SCALE_ANALYSIS_BALLS_LAYER_2_H
#define MESECINA_SCALE_ANALYSIS_BALLS_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>
#include <gui/app/static/Application_settings.h>

/**
 * Layer showing the landmarks
 */
class Scale_analysis_balls_layer_2 : public GL_draw_layer_2 {
public:
	typedef Scale_analysis_2::Point_2								Point_2;
	typedef Scale_analysis_2::Circle_2								Circle_2;

	typedef Scale_analysis_2::Delaunay_with_scale					Delaunay_with_scale;
	typedef Delaunay_with_scale::Finite_faces_iterator				Finite_faces_iterator;

	Scale_analysis_balls_layer_2(const QString& name, Scale_analysis_2* m, const QString& tt, bool circle = false, bool fill = false, bool use_scale_factor = false) : GL_draw_layer_2(name, tt), parent(m) {
		do_circle = circle;
		do_fill = fill;
		do_use_scale_factor = use_scale_factor;

		Application_settings::add_double_setting("scale-analysis-balls-scale-factor",1);
	}

	virtual void draw_commands() {
		Delaunay_with_scale* t = parent->get_sa_ordered_balls_delaunay();
//		std::cout << "Drawing scale analysis balls, size of delaunay: " <<  t->number_of_vertices() << std::endl;

		double scale_factor, squared_scale_factor = 1;
		if (do_use_scale_factor) {
//			t = parent->get_sa_ordered_balls_delaunay();
			scale_factor = Application_settings::get_double_setting("scale-analysis-balls-scale-factor");
			squared_scale_factor = scale_factor * scale_factor;
		}

		if (!do_use_scale_factor) *this << DO_INVERSE_SCALAR;
		if (do_circle && do_fill) *this << RENDER_FILL;
		if (do_circle) *this << NO_BOUNDING;
		Finite_faces_iterator f_it, f_end = t->finite_faces_end();
			for (f_it = t->finite_faces_begin(); f_it!=f_end; f_it++) {
				if ((do_use_scale_factor && f_it->info() > scale_factor) || (!do_use_scale_factor && f_it->info() >= 0)) {
					Point_2 center = t->dual(f_it);
					if (!do_use_scale_factor) *this << f_it->info();
					if (do_circle)
						*this << Circle_2(center, squared_scale_factor*squared_distance(center, f_it->vertex(0)->point()));
					else
						*this << center;
				}
			}
		if (do_circle) *this << DO_BOUNDING;
		if (do_circle && do_fill) *this << RENDER_NO_FILL;
		if (!do_use_scale_factor) *this << NO_SCALAR;

//		Witness_landmark_set* wls = parent->get_witness_landmark_set();
//		if (do_vertex) {
//			//std::cout << "Drawing landmark points " << wls->landmark_list().size() << std::endl;
//			Point_iterator p_it, p_end = wls->landmark_list().end();
//			for (p_it = wls->landmark_list().begin(); p_it!=p_end; p_it++) {
//				*this << *p_it;
//			}
//		} else {
//			if (do_witnessed) {
//				if (do_dual) {
//
//					Witness_complex_triangulation * wc = parent->get_witness_complex_triangulation();
//					// update the classification if needed
//					if (!draw_outside && wc->classification_is_dirty()) {
//						wc->compute_witness_complex_classification();
//					}
//					Witness_complex_triangulation::Finite_edges_iterator e_it, e_end = wc->finite_edges_end();
//					*this << NO_BOUNDING;
//					for (e_it = wc->finite_edges_begin();	e_it != e_end; ++e_it) {
//						if (!wc->is_witness_complex_edge(*e_it)) {
//							// draw the dual
//							Face_handle f = e_it->first;
////							int id = e_it->second;
//							if (draw_outside || (f->get_witness_complex_classification() == WC_INTERIOR)) {
////								n++;
//								CGAL::Object o = wc->dual(e_it);
//								Line_2  l;
//								Ray_2   r;
//								Segment_2 s;
//								if (CGAL::assign(s,o)) *this << s;
//								if (CGAL::assign(r,o)) *this << r;
//								if (CGAL::assign(l,o)) *this << l;
//							}
//						}
//					}
//					*this << DO_BOUNDING;
//
//				} else {
//
//					Witness_complex_triangulation * wc = parent->get_witness_complex_triangulation();
//					Witness_complex_triangulation::Finite_edges_iterator e_it, e_end = wc->finite_edges_end();
//					for (e_it = wc->finite_edges_begin();	e_it != e_end; ++e_it) {
//						if (wc->is_witness_complex_edge(*e_it)) {
//							*this << wc->segment(*e_it);
//						}
//					}
//				}
//			} else {
//				Witness_complex_triangulation * wc = parent->get_witness_complex_triangulation();
//				if (do_dual) {
//					Witness_complex_triangulation::Finite_edges_iterator e_it, e_end = wc->finite_edges_end();
//					*this << NO_BOUNDING;
//					for (e_it = wc->finite_edges_begin();	e_it != e_end; ++e_it) {
//						// draw the dual
//						CGAL::Object o = wc->dual(e_it);
//						Line_2  l;
//						Ray_2   r;
//						Segment_2 s;
//						if (CGAL::assign(s,o)) *this << s;
//						if (CGAL::assign(r,o)) *this << r;
//						if (CGAL::assign(l,o)) *this << l;
//					}
//					*this << DO_BOUNDING;
//				} else *this << *wc;
//			}
//		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
		case POINT_SIZE_EDITABLE:
			return !do_circle;
		case LINE_WIDTH_EDITABLE:
			return do_circle && !do_fill;
		case SCALAR_EDITABLE:
			return !do_use_scale_factor;
		//case USER_PROPERTY_1:
		//	return true;
		//	break;
		default:
			return false;
			break;
		}
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if (do_use_scale_factor && settings_name=="scale-analysis-balls-scale-factor") {
			invalidate_cache();
			widget->request_repaint();
		}
		if (!do_use_scale_factor && settings_name=="scale-analysis-balls-scale-factor") {
			double angle_limit = Application_settings::get_double_setting("scale-analysis-balls-scale-factor");

			if (angle_limit < 1) scalar_min = 1;
			else scalar_min = angle_limit;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}
	}

	// this is called when a custom action is clicked by the user
	//virtual void execute_user_property(Layer_property l) {
	//	if (l == USER_PROPERTY_1) {
	//		QClipboard *clipboard = QApplication::clipboard();
	//		QString text = QString("%1").arg(parent->get_witness_landmark_set()->get_epsilon());
	//		clipboard->setText(text);
	//	}
	//}

	// this tells the display name of the custom action
	//virtual QString get_name_user_property(Layer_property l) { 
	//	if (l == USER_PROPERTY_1)
	//		return "Epsilon to Clipboard: " + QString("%1").arg(parent->get_witness_landmark_set()->get_epsilon());
	//	return "";
	//}
	
private:
	Scale_analysis_2 *parent;
	bool do_circle, do_fill, do_use_scale_factor;
};


#endif //MESECINA_SCALE_ANALYSIS_BALLS_LAYER_2_H