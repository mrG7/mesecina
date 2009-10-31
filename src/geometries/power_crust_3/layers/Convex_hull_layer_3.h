/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Convex_hull_layer_3.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_CONVEX_HULL_LAYER_3_H
#define MESECINA_CONVEX_HULL_LAYER_3_H

#include <geometries/power_crust_3/Power_crust_3.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>


template <class Power_crust_3>
class Convex_hull_layer_3 : public GL_draw_layer_3 {
public:
	typedef typename Power_crust_3::Triangulation_3						 Triangulation_3;
	typedef typename Triangulation_3::All_cells_iterator	All_cells_iterator;
	typedef typename Triangulation_3::Vertex_handle						   Vertex_handle;
	typedef typename Triangulation_3::Cell_handle						   Cell_handle;
	typedef typename Power_crust_3::Triangle_3										 Triangle_3;

	typedef std::vector<Vertex_handle> Vertex_vector;
	typedef std::back_insert_iterator<Vertex_vector> Vertex_insert_iterator;
	typedef std::vector<Cell_handle> Cell_vector;
	typedef std::back_insert_iterator<Cell_vector> Cell_insert_iterator;

	Convex_hull_layer_3(const QString& name, Power_crust_3* m, const QString& tt, bool vertex) : GL_draw_layer_3(name, tt), parent(m) {
		do_vertex = vertex;
	}

	virtual void draw_commands() {
		Triangulation_3* t = parent->get_input_triangulation();
		if (t->number_of_vertices() < 4) return;

		if (do_vertex) {

			std::cout << PROGRESS_STATUS << "Extracting convex hull faces" << std::endl;
			Vertex_vector on_convex_hull;
			Vertex_insert_iterator ii(on_convex_hull);
			t->incident_vertices( t->infinite_vertex(), ii);
			*this << POINTS_START;
			for ( Vertex_vector::iterator i = on_convex_hull.begin(); i != on_convex_hull.end(); ++i)
				*this << (*i)->point();
			*this << POINTS_END;
			std::cout << PROGRESS_DONE << std::endl;
		} else {
			std::cout << PROGRESS_STATUS << "Extracting convex hull faces" << std::endl;
			Cell_vector on_convex_hull;
			Cell_insert_iterator ii(on_convex_hull);
			t->incident_cells( t->infinite_vertex(), ii);
			*this << TRIANGLES_START;
			for ( Cell_vector::iterator i = on_convex_hull.begin(); i != on_convex_hull.end(); ++i) {
				Cell_handle c_it = *i;
				int inf_id = c_it->index(t->infinite_vertex());
				*this << Triangle_3(
						c_it->vertex((inf_id+1)%4)->point(),
						c_it->vertex((inf_id+2)%4)->point(),
						c_it->vertex((inf_id+3)%4)->point());
			}
			*this << TRIANGLES_END;
			std::cout << PROGRESS_DONE << std::endl;
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case POINT_SIZE_EDITABLE:
			return do_vertex;
			break;
		case LINE_WIDTH_EDITABLE:
		default:
			return false;
			break;
		}
	}


private:
	Power_crust_3 *parent;
	bool do_vertex;
};


#endif //MESECINA_CONVEX_HULL_LAYER_3_H