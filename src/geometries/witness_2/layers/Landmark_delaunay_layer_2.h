/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Armin Häberling, ETH Zurich
 *
 * $Id: Landmark_Points_layer_2.h 344 2008-02-29 12:50:20Z arminha $
 */

#ifndef MESECINA_LANDMARK_DELAUNAY_LAYER_2_H
#define MESECINA_LANDMARK_DELAUNAY_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>
//#include <geometries/union_of_balls_2/Union_of_balls_2.h>

/**
 * Layer showing the landmarks
 */
template <class Witness_2>
class Landmark_delaunay_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Witness_2::Point_2								Point_2;
	typedef typename Witness_2::Landmark_list						Point_List;
	typedef typename Witness_2::Witness_landmark_set				Witness_landmark_set;
	typedef typename Point_List::iterator							Point_iterator;
	typedef typename Witness_2::Segment_2				Segment_2;
	typedef typename Witness_2::Line_2					Line_2;
	typedef typename Witness_2::Ray_2					Ray_2;
	typedef typename Witness_2::Witness_complex_triangulation Witness_complex_triangulation;
	typedef typename Witness_complex_triangulation::Face_handle Face_handle;

	Landmark_delaunay_layer_2(const QString& name, Witness_2* m, const QString& tt, bool vertex, bool dual, bool witnessed, bool outside) : GL_draw_layer_2(name, tt), parent(m) {
		do_vertex = vertex;
		do_dual = dual;
		do_witnessed = witnessed;
		draw_outside = outside;
	}

	virtual void draw_commands() {
		Witness_landmark_set* wls = parent->get_witness_landmark_set();
		if (do_vertex) {
			//std::cout << "Drawing landmark points " << wls->landmark_list().size() << std::endl;
			Point_iterator p_it, p_end = wls->landmark_list().end();
			for (p_it = wls->landmark_list().begin(); p_it!=p_end; p_it++) {
				*this << *p_it;
			}
		} else {
			if (do_witnessed) {
				if (do_dual) {

					Witness_complex_triangulation * wc = parent->get_witness_complex_triangulation();
					// update the classification if needed
					if (!draw_outside && wc->classification_is_dirty()) {
						wc->compute_witness_complex_classification();
					}
					Witness_complex_triangulation::Finite_edges_iterator e_it, e_end = wc->finite_edges_end();
					*this << NO_BOUNDING;
					for (e_it = wc->finite_edges_begin();	e_it != e_end; ++e_it) {
						if (!wc->is_witness_complex_edge(*e_it)) {
							// draw the dual
							Face_handle f = e_it->first;
//							int id = e_it->second;
							if (draw_outside || (f->get_witness_complex_classification() == WC_INTERIOR)) {
//								n++;
								CGAL::Object o = wc->dual(e_it);
								Line_2  l;
								Ray_2   r;
								Segment_2 s;
								if (CGAL::assign(s,o)) *this << s;
								if (CGAL::assign(r,o)) *this << r;
								if (CGAL::assign(l,o)) *this << l;
							}
						}
					}
					*this << DO_BOUNDING;

				} else {

					Witness_complex_triangulation * wc = parent->get_witness_complex_triangulation();
					Witness_complex_triangulation::Finite_edges_iterator e_it, e_end = wc->finite_edges_end();
					for (e_it = wc->finite_edges_begin();	e_it != e_end; ++e_it) {
						if (wc->is_witness_complex_edge(*e_it)) {
							*this << wc->segment(*e_it);
						}
					}
				}
			} else {
				Witness_complex_triangulation * wc = parent->get_witness_complex_triangulation();
				if (do_dual) {
					Witness_complex_triangulation::Finite_edges_iterator e_it, e_end = wc->finite_edges_end();
					*this << NO_BOUNDING;
					for (e_it = wc->finite_edges_begin();	e_it != e_end; ++e_it) {
						// draw the dual
						CGAL::Object o = wc->dual(e_it);
						Line_2  l;
						Ray_2   r;
						Segment_2 s;
						if (CGAL::assign(s,o)) *this << s;
						if (CGAL::assign(r,o)) *this << r;
						if (CGAL::assign(l,o)) *this << l;
					}
					*this << DO_BOUNDING;
				} else *this << *wc;
			}
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
		case POINT_SIZE_EDITABLE:
			return do_vertex;
		case LINE_WIDTH_EDITABLE:
			return !do_vertex;
		case USER_PROPERTY_1:
			return true;
			break;
		default:
			return false;
			break;
		}
	}

	// this is called when a custom action is clicked by the user
	virtual void execute_user_property(Layer_property l) {
		if (l == USER_PROPERTY_1) {
			QClipboard *clipboard = QApplication::clipboard();
			QString text = QString("%1").arg(parent->get_witness_landmark_set()->get_epsilon());
			clipboard->setText(text);
		}
	}

	// this tells the display name of the custom action
	virtual QString get_name_user_property(Layer_property l) { 
		if (l == USER_PROPERTY_1)
			return "Epsilon to Clipboard: " + QString("%1").arg(parent->get_witness_landmark_set()->get_epsilon());
		return "";
	}
	
private:
	Witness_2 *parent;
	bool do_vertex, do_dual, do_witnessed, draw_outside;
};


#endif //MESECINA_LANDMARK_DELAUNAY_LAYER_2_H