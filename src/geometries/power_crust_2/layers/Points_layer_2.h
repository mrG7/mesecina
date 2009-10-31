/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Points_layer_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_POINTS_LAYER_2_H
#define MESECINA_POINTS_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <list>
#include <algorithm>

template <class Power_crust_2>
class Points_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Power_crust_2::Triangulation_2						 Triangulation_2;
	typedef typename Triangulation_2::Finite_vertices_iterator	Finite_vertices_iterator;
	typedef typename Triangulation_2::Vertex_handle						   Vertex_handle;
	typedef typename Power_crust_2::Point_2										 Point_2;
	Points_layer_2(const QString& name, Power_crust_2* m, QString ttip = "") : GL_draw_layer_2(name, ttip), parent(m) {
	}

	virtual void draw_commands() {
		
		Triangulation_2* t = parent->get_input_triangulation();
		Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
		*this << DO_SCALAR;
		for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++) {
			*this << CGAL::to_double(v_it->point().x());
			*this << v_it->point();
		}
		*this << NO_SCALAR;
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

	void clear_selection() {
		selected_vertices.clear();
	}

	virtual void ray_selection(double x, double y, bool* hit) {
		Triangulation_2* t = parent->get_input_triangulation();
		if (t->number_of_vertices() < 1) return;
		Vertex_handle v = t->nearest_vertex(Point_2(x,y));
		Point_2 p = v->point();
		Point_2 s(x,y);
		double dist = sqrt(CGAL::to_double(CGAL::squared_distance(p,s)));
		if (widget->x_pixel_dist_d(dist) < point_size/2) {
			if (!is_vertex_selected(v)) {
				widget->add_point_to_selection(CGAL::to_double(p.x()), CGAL::to_double(p.y()));
				selected_vertices.push_back(v);
				// save it for myself for later, reference to the selected object
//				DEBUG_MSG("Points_layer_2: ray_selection added a point");
			} else {
				widget->remove_point_from_selection(CGAL::to_double(p.x()), CGAL::to_double(p.y()));
				selected_vertices.remove(v);
//				DEBUG_MSG("Points_layer_2: ray_selection removed a point");
			}
			*hit = true;
		}
	}
	
	virtual void disk_selection(double , double , double , bool* ) {
//		DEBUG_MSG("disk_selection at points layer");
	}
	
	virtual void box_selection(double xmin, double ymin, double xmax, double ymax, bool* hit) {
		Triangulation_2* t = parent->get_input_triangulation();
		Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
		for (v_it = t->finite_vertices_begin(); v_it!=v_end; v_it++) {
			double px = CGAL::to_double(v_it->point().x());
			double py = CGAL::to_double(v_it->point().y());
			if  (xmin <= px && px <= xmax && ymin <= py && py <= ymax && !is_vertex_selected(v_it)) {
				widget->add_point_to_selection(px, py);
//				Vertex_handle v = *v_it;
				selected_vertices.push_back(v_it);
				*hit = true;
			}
		}
//		DEBUG_MSG("box_selection at points layer");
	}

	virtual void selection_moved_by(double x, double y) {
		if (selected_vertices.size() == 0) return;
		Triangulation_2* t = parent->get_input_triangulation();
		std::list<QPointF> orig_coordinates;
		typedef typename std::list<Vertex_handle>::iterator Vertex_handle_list_iterator;
		Vertex_handle_list_iterator v_it, v_end = selected_vertices.end();
		// save coordinates of selected vertices, and remove them from triangulation
		for (v_it = selected_vertices.begin(); v_it!=v_end; v_it++) {
			Vertex_handle v = *v_it;
			orig_coordinates.push_back(QPointF(CGAL::to_double(v->point().x()),
											   CGAL::to_double(v->point().y())));
			t->remove(v);
		}
		// clean selected list;
		selected_vertices.clear();
		// add translated vertices to the triangulation and the selected list
		std::list<QPointF>::iterator p_it, p_end = orig_coordinates.end();
		for (p_it = orig_coordinates.begin(); p_it!=p_end; p_it++) {
			selected_vertices.push_back(t->insert(Point_2(p_it->x()+x, p_it->y()+y)));
		}
		parent->invalidate_cache();
	}

	virtual bool selection_deleted() {
		if (selected_vertices.size() < 1) return false;
		Triangulation_2* t = parent->get_input_triangulation();
		typedef typename std::list<Vertex_handle>::iterator Vertex_handle_list_iterator;
		Vertex_handle_list_iterator  v_it, v_end = selected_vertices.end();
		// save coordinates of selected vertices, and remove them from triangulation
		for (v_it = selected_vertices.begin(); v_it!=v_end; v_it++) {
			Vertex_handle v = *v_it;
			t->remove(v);
		}
		selected_vertices.clear();
		parent->invalidate_cache();
		return true;
	}


	bool is_vertex_selected(Vertex_handle v) {
		typedef typename std::list<Vertex_handle>::iterator Vertex_handle_list_iterator;
		Vertex_handle_list_iterator  v_it = std::find(selected_vertices.begin(), selected_vertices.end(), v);
		return (v_it != selected_vertices.end());
	}

	virtual QString get_name_user_property(Layer_property l) { 
		if (l == SCALAR_EDITABLE) return "x range";
		else return "";
	}

private:
	Power_crust_2 *parent;
	std::list<Vertex_handle> selected_vertices;
};


#endif //MESECINA_POINTS_LAYER_2_H