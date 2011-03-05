/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Balls_layer_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_BALLS_LAYER_2_H
#define MESECINA_BALLS_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>
#include <geometries/union_of_balls_2/Union_of_balls_2.h>
#include <QtGui/QClipboard>

#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <list> 
#include <algorithm>

template <class Union_of_balls_2>
class Balls_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Union_of_balls_2::Ball_triangulation_2						 Ball_triangulation_2;
	typedef typename Ball_triangulation_2::Vertex_handle						 Vertex_handle;

	typedef typename Ball_triangulation_2::Finite_vertices_iterator			   Finite_vertices_iterator;
	typedef typename Ball_triangulation_2::Hidden_vertices_iterator				Hidden_vertices_iterator;
	typedef typename Union_of_balls_2::Point_2										 Point_2;
	typedef typename Union_of_balls_2::Circle_2										 Circle_2;
	typedef typename Union_of_balls_2::NT									NT;
	Balls_layer_2(const QString& name, Union_of_balls_2* m, const QString& tt = "", bool fill = false, bool only_center = false) : GL_draw_layer_2(name, tt), parent(m) {
		this->fill = fill;
		this->only_center = only_center;
		selected_vertices.clear();
//		std::cout << name.toStdString() << " " << only_center << std::endl;

		Application_settings::add_bool_setting("random-ball-order", false);
		Application_settings::add_int_setting("balls-id", 10);
		Application_settings::add_bool_setting("disks-use-balls-id", true);
		Application_settings::add_bool_setting("centers-use-balls-id", true);
		Application_settings::add_bool_setting("centers-accumulate-balls-id", true);		

	}

	virtual void draw_commands() {
		bool do_random = Application_settings::get_bool_setting("random-ball-order");
		Ball_triangulation_2* t = parent->get_ball_triangulation();
		Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
		Hidden_vertices_iterator hv_it, hv_end = t->hidden_vertices_end();
		std::vector<int>* random_tokens = parent->get_ball_random_tokens();

		if (fill) *this << RENDER_FILL;
		radii.clear(); xs.clear(); ys.clear();
		*this << DO_SCALAR; int i=0;
		for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++, i++) {
			//if (!shrink || v_it->get_classification() == D_IS) {
				NT r = v_it->point().weight();
				double sqrtr = sqrt(CGAL::to_double(r));
				if (do_random) *this << (*random_tokens)[i%random_tokens->size()] + i/random_tokens->size();
				else *this << sqrtr;
				if (only_center) *this << v_it->point().point();
				else *this << Circle_2(v_it->point().point(), r);
				radii.push_back(sqrtr);
				xs.push_back(CGAL::to_double(v_it->point().point().x()));
				ys.push_back(CGAL::to_double(v_it->point().point().y()));
			//}
		}
		for (hv_it = t->hidden_vertices_begin(); hv_it!=hv_end; hv_it++, i++) {
			//if (!shrink || hv_it->get_classification() == D_IS) {
				NT r = hv_it->point().weight();
				double sqrtr = sqrt(CGAL::to_double(r));
				if (do_random) *this << (*random_tokens)[i%random_tokens->size()] + i/random_tokens->size();
				else *this << sqrtr;
				if (only_center) *this << hv_it->point().point();
				else *this << Circle_2(hv_it->point().point(), r);
				radii.push_back(sqrtr);
				xs.push_back(CGAL::to_double(hv_it->point().point().x()));
				ys.push_back(CGAL::to_double(hv_it->point().point().y()));
			//}
		}		
		*this << NO_SCALAR;
		if (fill) *this << RENDER_NO_FILL;
	}

	void clear_selection() {
		selected_vertices.clear();
	}

	virtual void box_selection(double xmin, double ymin, double xmax, double ymax, bool* hit) {
		if (only_center) {
			Ball_triangulation_2* t = parent->get_ball_triangulation();
			Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
			Hidden_vertices_iterator hv_it, hv_end = t->hidden_vertices_end();
			for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++) {
				double px = CGAL::to_double(v_it->point().x());
				double py = CGAL::to_double(v_it->point().y());
				if  (xmin <= px && px <= xmax && ymin <= py && py <= ymax && !is_vertex_selected(v_it)) {
					widget->add_point_to_selection(px, py);
					Vertex_handle v = v_it;
//					std::cout << "selection on vertex: " << v->point() << std::endl;
					selected_vertices.push_back(v);
					*hit = true;
				}
			}
			for (hv_it = t->hidden_vertices_begin(); hv_it!=hv_end; hv_it++) {
				double px = CGAL::to_double(hv_it->point().x());
				double py = CGAL::to_double(hv_it->point().y());
				if  (xmin <= px && px <= xmax && ymin <= py && py <= ymax && !is_vertex_selected(v_it)) {
					widget->add_point_to_selection(px, py);
					//				Vertex_handle v = *v_it;
					selected_vertices.push_back(hv_it);
					*hit = true;
				}
			}
				
		}
	}

	void update_orig_points() {
		Ball_triangulation_2* t = parent->get_ball_triangulation();
		std::list<Point3D>* orig_points = parent->get_weighted_points();

		std::list<Point3D> new_orig_points;
		new_orig_points.clear();
		Finite_vertices_iterator fv_it, fv_end = t->finite_vertices_end();
		Hidden_vertices_iterator hv_it, hv_end = t->hidden_vertices_end();
		for (fv_it = t->finite_vertices_begin(); fv_it!=fv_end; fv_it++) {
			double px = CGAL::to_double(fv_it->point().x());
			double py = CGAL::to_double(fv_it->point().y());
			double radius = sqrt(CGAL::to_double(fv_it->point().weight()));
			new_orig_points.push_back(Point3D(px,py,radius));
		}
		for (hv_it = t->hidden_vertices_begin(); hv_it!=hv_end; hv_it++) {
			double px = CGAL::to_double(hv_it->point().x());
			double py = CGAL::to_double(hv_it->point().y());
			double radius = sqrt(CGAL::to_double(hv_it->point().weight()));
			new_orig_points.push_back(Point3D(px,py,radius));
		}
		orig_points->clear();
		orig_points->insert(orig_points->end(), new_orig_points.begin(), new_orig_points.end());
	}

	virtual bool selection_deleted() {
		if (only_center) {
			if (selected_vertices.size() < 1) return false;
			std::cout << "selection size: " << selected_vertices.size() << std::endl;
			Ball_triangulation_2* t = parent->get_ball_triangulation();
			typedef typename std::list<Vertex_handle>::iterator Vertex_handle_list_iterator;
			Vertex_handle_list_iterator  v_it, v_end = selected_vertices.end();
			// save coordinates of selected vertices, and remove them from triangulation
			for (v_it = selected_vertices.begin(); v_it!=v_end; v_it++) {
				Vertex_handle v = *v_it;
				t->remove(v);
			}

			update_orig_points();

			selected_vertices.clear();
			parent->invalidate_cache(true);
			return true;
		}
		return false;
	}

	virtual void selection_moved_by(double x, double y) {
		if (only_center) {
			if (selected_vertices.size() < 1) return;
			Ball_triangulation_2* t = parent->get_ball_triangulation();

			std::list<Point3D> orig_coordinates;
			typedef typename std::list<Vertex_handle>::iterator Vertex_handle_list_iterator;
			Vertex_handle_list_iterator  v_it, v_end = selected_vertices.end();
			// save coordinates of selected vertices, and remove them from triangulation
			for (v_it = selected_vertices.begin(); v_it!=v_end; v_it++) {
				Vertex_handle v = *v_it;
//				std::cout << "selection move: " << v->point() << std::endl;
				double px = CGAL::to_double(v->point().point().x());
				double py = CGAL::to_double(v->point().point().y());
				double radius = CGAL::to_double(v->point().weight());
				orig_coordinates.push_back(Point3D(px,py,radius));
				t->remove(v);
			}

			selected_vertices.clear();
			// add translated vertices to the triangulation and the selected list
			std::list<Point3D>::iterator p_it, p_end = orig_coordinates.end();
			for (p_it = orig_coordinates.begin(); p_it!=p_end; p_it++) {
				selected_vertices.push_back(t->insert(Ball_triangulation_2::Point(Point_2(p_it->x+x, p_it->y+y),p_it->z)));
			}

			update_orig_points();
			parent->invalidate_cache(true);
		}
	}

	void wheelEvent(QWheelEvent *e) {
		if (only_center && selected_vertices.size() >0) {
			int numDegrees = e->delta() / 8;
//			std::cout << "numdegrees: " << numDegrees << std::endl;

			double shrinking = 1.0+(double)(numDegrees)/500.0;
			shrinking = shrinking*shrinking;
//			std::cout << "shrinking: " << shrinking << std::endl;

			Ball_triangulation_2* t = parent->get_ball_triangulation();
			std::list<Point3D> orig_coordinates;
			typedef typename std::list<Vertex_handle>::iterator Vertex_handle_list_iterator;
			Vertex_handle_list_iterator  v_it, v_end = selected_vertices.end();
			// save coordinates of selected vertices, and remove them from triangulation
			for (v_it = selected_vertices.begin(); v_it!=v_end; v_it++) {
				Vertex_handle v = *v_it;
//				std::cout << "selection move: " << v->point() << std::endl;
				double px = CGAL::to_double(v->point().point().x());
				double py = CGAL::to_double(v->point().point().y());
				double radius = CGAL::to_double(v->point().weight());
				orig_coordinates.push_back(Point3D(px,py,radius));
				t->remove(v);
			}

			selected_vertices.clear();
			// add translated vertices to the triangulation and the selected list
			std::list<Point3D>::iterator p_it, p_end = orig_coordinates.end();
			for (p_it = orig_coordinates.begin(); p_it!=p_end; p_it++) {
				selected_vertices.push_back(t->insert(Ball_triangulation_2::Point(Point_2(p_it->x, p_it->y),p_it->z*shrinking)));
			}

			update_orig_points();
			parent->invalidate_cache(true);
			widget->repaintGL();

		}
	}

	bool is_vertex_selected(Vertex_handle v) {
		typedef typename std::list<Vertex_handle>::iterator Vertex_handle_list_iterator;
		Vertex_handle_list_iterator  v_it = std::find(selected_vertices.begin(), selected_vertices.end(), v);
		return (v_it != selected_vertices.end());
	}


	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case USER_PROPERTY_1:
		case USER_PROPERTY_2:
			return !only_center; break;
		case COLOR_EDITABLE:
		case SCALAR_EDITABLE:
			return true;
			break;
		case LINE_WIDTH_EDITABLE:
			return !fill && !only_center; break;
		case POINT_SIZE_EDITABLE:
			return only_center; break;
		default:
			return false;
			break;
		}
	}

	virtual void execute_user_property(Layer_property l) {
		if (l == USER_PROPERTY_1) {
			QClipboard *clipboard = QApplication::clipboard();
			QString text = "[";
			for (int i=0; i<(int)radii.size(); i++) {
				text = text + tr(" %1").arg(radii[i]);
			}
			text = text + "]";
			clipboard->setText(text);
			std::cout << "Balls radii: " << text.toStdString() << std::endl;
		}
		if (l == USER_PROPERTY_2) {
			QClipboard *clipboard = QApplication::clipboard();
			QString text = "[";
			for (int i=0; i<(int)radii.size(); i++) {
				text = text + tr("%1 %2 %3").arg(xs[i]).arg(ys[i]).arg(radii[i]);
				if (i!=radii.size()-1) text = text + "; ";
			}
			text = text + "]";
			clipboard->setText(text);
			std::cout << "Balls: " << text.toStdString() << std::endl;
		}

	}
	virtual QString get_name_user_property(Layer_property l) { 
		if (l == USER_PROPERTY_1)
			return "Radii to clipboard";
		if (l == USER_PROPERTY_2)
			return "Balls to clipboard";
		if (l== SCALAR_EDITABLE) {
			if (Application_settings::get_bool_setting("random-ball-order")) return "Ball id";
			else return "Radii range";
		}
		return "";
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if (settings_name=="random-ball-order") {
			invalidate_cache();
			widget->request_repaint();
		}
		if (settings_name=="balls-id") {
			int ball_id = Application_settings::get_int_setting("balls-id");

			if (only_center) {
				if (Application_settings::get_bool_setting("centers-use-balls-id")) {
					if (ball_id < 0) scalar_max = 0;
					else scalar_max = ball_id;
					if (!Application_settings::get_bool_setting("centers-accumulate-balls-id")) {
						scalar_min = ball_id;
					}
				}
			}
			else
			if (fill) {
				if (Application_settings::get_bool_setting("disks-use-balls-id")) {
					if (ball_id < 0) scalar_max = 0;
					else scalar_max = ball_id;
				}
			} else {
				if (ball_id < 0) scalar_max = 0;
				else scalar_max = ball_id;
				if (!fill) scalar_min = ball_id;
			}
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}

	}



private:
	Union_of_balls_2 *parent;
	bool fill;
	bool only_center;
	std::vector<float> radii;
	std::vector<float> xs;
	std::vector<float> ys;
	std::list<Vertex_handle> selected_vertices;
};


#endif //MESECINA_BALLS_LAYER_2_H