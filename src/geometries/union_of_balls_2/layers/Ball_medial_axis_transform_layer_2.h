/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Ball_medial_axis_transform_layer_2.h 528 2009-01-26 20:56:08Z miklosb $
 */

#ifndef MESECINA_BALL_MEDIAL_AXIS_TRANSFORM_LAYER_2_H
#define MESECINA_BALL_MEDIAL_AXIS_TRANSFORM_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>
#include <geometries/union_of_balls_2/Union_of_balls_2.h>

template <class Union_of_balls_2>
class Ball_medial_axis_transform_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Union_of_balls_2::V_triangulation_2				 Triangulation_2;
	typedef typename Union_of_balls_2::Ball_triangulation_2			Ball_triangulation_2;
	typedef typename Ball_triangulation_2::Vertex_handle			Ball_vertex_handle;
	typedef typename Ball_triangulation_2::Face_handle Ball_face_handle;
	typedef typename Ball_triangulation_2::Finite_edges_iterator Ball_finite_edges_iterator;
	typedef typename Triangulation_2::Finite_edges_iterator		   Finite_edges_iterator;
	typedef typename Triangulation_2::Face_handle							 Face_handle;
	typedef typename Union_of_balls_2::Segment_2 Segment_2;
	typedef typename Union_of_balls_2::Point_2										 Point_2;
	typedef typename Union_of_balls_2::Circle_2										 Circle_2;

	Ball_medial_axis_transform_layer_2(const QString& name, Union_of_balls_2* m, const QString& tt = "", bool axis = true, bool vertex = false, bool fill = true, bool scale_axis=false, bool convex_balls=false) : GL_draw_layer_2(name, tt), parent(m)
	{
		do_axis = axis;
		do_vertex = vertex;
		do_fill = fill;
		do_scale_axis = scale_axis;
		do_convex_balls = convex_balls;

		Application_settings::add_double_setting("scale-axis-shrinking-back-percentage",100);
		Application_settings::add_bool_setting("ignore-non-complete-v-voronoi edge", false);
	}

	virtual void draw_commands() {

		if (do_axis) {
			Triangulation_2* t = parent->get_flooded_v_triangulation();
			Ball_triangulation_2* bt = parent->get_dual_ball_triangulation();

			std::cout << PROGRESS_STATUS << "Extracting medial axis edges" << std::endl;
			for (Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
				Face_handle f = e_it->first;
				int id = e_it->second;
				Face_handle nF = f->neighbor(id);
				if (f->get_classification() == V_INTERIOR && nF->get_classification() == V_INTERIOR) {
					CGAL::Object o = t->dual(e_it);
					Segment_2 s;
					if (CGAL::assign(s,o)) *this << s;
				} else if (!Application_settings::get_bool_setting("ignore-non-complete-v-voronoi edge") && f->get_classification(id) == V_CROSSING) {
					Ball_vertex_handle mid = f->get_ball_voronoi_edge(id);
					// this is a crossing edge
					if (f->get_classification() != V_INTERIOR) {
						Face_handle temp = f;
						f = nF;
						nF = temp;
					}
					if (f->get_classification() == V_INTERIOR && nF->get_classification() == V_EXTERIOR) {
						// f has to be interior end vertex, and nF exterior
						// if the associated ball point where this edge has to be split is not an endvertex
						// add it to the medial axis
						if (mid != f->get_ball_voronoi_vertex() && mid != nF->get_ball_voronoi_vertex()) {
							*this << Segment_2(mid->point(), t->dual(f));
						}
					} else {
						//						std::cout << LOG_ERROR << "V_CROSSING edge is not really consistent" << std::endl;
					}
				}
			}

			// sigular edges
			for (Ball_finite_edges_iterator e_it = bt->finite_edges_begin(), e_end = bt->finite_edges_end(); e_it != e_end; e_it++) {
				Ball_face_handle f = e_it->first;
				int id = e_it->second; 
				if (f->get_classification(id) == U_SINGULAR) {
					*this << Segment_2(f->vertex((id+1)%3)->point(),f->vertex((id+2)%3)->point());
				}
			}
			std::cout << PROGRESS_DONE << std::endl;
		} else if (do_convex_balls) {
			std::list<Circle_2>* mb = parent->get_convex_balls();
			std::list<Circle_2>::iterator ball_it, ball_end = mb->end();
			for(ball_it = mb->begin(); ball_it != ball_end; ball_it++) {
				Circle_2 circ = *ball_it;
				if (do_vertex) {
					*this << circ.center();
				} else {
					double r = CGAL::to_double(circ.squared_radius());
					*this << Circle_2(circ.center(), r);
				}
			}
			if (do_fill) *this << RENDER_NO_FILL;

		} else {
			if (do_fill) *this << RENDER_FILL;
			double shrink_ratio = 1;
			if (do_scale_axis) {
				Ball_triangulation_2* bt = parent->get_dual_ball_triangulation();
				double inv_perc = 1.0 - Application_settings::get_double_setting("scale-axis-shrinking-back-percentage")/100.0;
				shrink_ratio = CGAL::to_double(bt->get_shrink_ratio());
				shrink_ratio = shrink_ratio + (1 - shrink_ratio) * inv_perc;
				shrink_ratio *= shrink_ratio;
			}

			std::list<Circle_2>* mb = parent->get_medial_balls();
			std::list<Circle_2>::iterator ball_it, ball_end = mb->end();
			for(ball_it = mb->begin(); ball_it != ball_end; ball_it++) {
				Circle_2 circ = *ball_it;
				if (do_vertex) {
					*this << circ.center();
				} else {
					double r = shrink_ratio *  CGAL::to_double(circ.squared_radius());
					*this << Circle_2(circ.center(), r);
				}
			}
			if (do_fill) *this << RENDER_NO_FILL;
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
		case USER_PROPERTY_1:
			return do_convex_balls;
		case USER_PROPERTY_2:
			return !do_convex_balls;
//		case SCALAR_EDITABLE:
		case POINT_SIZE_EDITABLE:
			return do_vertex;
			break;
		case LINE_WIDTH_EDITABLE:
			return do_axis || !do_fill; break;
		default:
			return false;
			break;
		}
	}

	virtual QString get_name_user_property(Layer_property l) { 
		if (l== USER_PROPERTY_1)
			return "Balls to WPOFF";
		if (l== USER_PROPERTY_2)
			return "Balls to WPOFF";
		return "";
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if ((!do_axis && settings_name=="medial-ball-sampling-min-angle-for-scale-axis") ||
			(do_scale_axis && settings_name=="scale-axis-shrinking-back-percentage")) {
				invalidate_cache();
				widget->request_repaint();
		}
		if (do_axis && settings_name=="ignore-non-complete-v-voronoi edge") {
			invalidate_cache();
			widget->request_repaint();
		}
	}


	virtual void execute_user_property(Layer_property l) {
		if (l == USER_PROPERTY_2) {
			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose an off filename to save balls to",
				settings.value("last-data-directory",QString()).toString(),
				"WPOFF (*.wpoff)");
			if (file_name=="") return;
			if (!file_name.endsWith(".wpoff")) file_name += ".wpoff";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);

			std::list<Point3D> medial_points;

			std::list<Circle_2>::iterator b_it, b_end = parent->get_medial_balls()->end();
			for (b_it = parent->get_medial_balls()->begin(); b_it!=b_end; b_it++)
				medial_points.push_back(Point3D(CGAL::to_double(b_it->center().x()),
				CGAL::to_double(b_it->center().y()),
				sqrt(CGAL::to_double(b_it->squared_radius()))));

			parent->save_balls(file_name, &medial_points);
		}
		if (l == USER_PROPERTY_1) {
			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose an off filename to save balls to",
				settings.value("last-data-directory",QString()).toString(),
				"WPOFF (*.wpoff)");
			if (file_name=="") return;
			if (!file_name.endsWith(".wpoff")) file_name += ".wpoff";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);

			std::list<Point3D> convex_points;
			std::list<Circle_2>::iterator b_it, b_end = parent->get_convex_balls()->end();
			for (b_it = parent->get_convex_balls()->begin(); b_it!=b_end; b_it++)
				convex_points.push_back(Point3D(CGAL::to_double(b_it->center().x()),
				CGAL::to_double(b_it->center().y()),
				sqrt(CGAL::to_double(b_it->squared_radius()))));

			b_it, b_end = parent->get_medial_balls()->end();
			for (b_it = parent->get_medial_balls()->begin(); b_it!=b_end; b_it++)
				convex_points.push_back(Point3D(CGAL::to_double(b_it->center().x()),
				CGAL::to_double(b_it->center().y()),
				sqrt(CGAL::to_double(b_it->squared_radius()))));

			
//			convex_points.insert(convex_points.end(),parent->get_weighted_points()->begin(), parent->get_weighted_points()->end());

			

			parent->save_balls(file_name, &convex_points);
		}
	}

private:
	Union_of_balls_2 *parent;
	bool do_axis, do_vertex, do_fill, do_scale_axis, do_convex_balls;

};


#endif //MESECINA_BALL_MEDIAL_AXIS_TRANSFORM_LAYER_2_H