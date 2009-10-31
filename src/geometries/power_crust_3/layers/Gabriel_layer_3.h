/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Gabriel_edges_layer_3.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_GABRIEL_LAYER_3_H
#define MESECINA_GABRIEL_LAYER_3_H

#include <geometries/power_crust_3/Power_crust_3.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>

template <class Power_crust_3>
class Gabriel_layer_3 : public GL_draw_layer_3 {
public:
	typedef typename Power_crust_3::Triangulation_3						 Triangulation_3;
	typedef typename Triangulation_3::Finite_edges_iterator	Finite_edges_iterator;
	typedef typename Triangulation_3::Finite_facets_iterator	Finite_facets_iterator;
	typedef typename Triangulation_3::Cell_handle						   Cell_handle;
	typedef typename Power_crust_3::Segment_3										 Segment_3;
	typedef typename Power_crust_3::Triangle_3										 Triangle_3;

	Gabriel_layer_3(const QString& name, Power_crust_3* m, const QString& tt, bool faces, bool force_edges = false) : GL_draw_layer_3(name,tt), parent(m) {
		do_faces = faces;
		do_force_edges = force_edges;
	}

	virtual void draw_commands() {
		Triangulation_3* t = parent->get_input_triangulation();
		if (do_faces) {
			Finite_facets_iterator f_it, f_end = t->finite_facets_end();
			std::cout << PROGRESS_STATUS << "Extracting Gabriel faces" << std::endl;
			if (!do_force_edges) *this << TRIANGLES_START;
			else *this << SEGMENTS_START;
			for (f_it = t->finite_facets_begin(); f_it!=f_end; f_it++) {
				if (t->is_Gabriel(*f_it)) {
					Cell_handle c = f_it->first;
					int id = f_it->second;
					if (!do_force_edges) {
						*this << Triangle_3(
							c->vertex((id+1)%4)->point(),
							c->vertex((id+2)%4)->point(),
							c->vertex((id+3)%4)->point());
					} else {
						*this << Segment_3(c->vertex((id+1)%4)->point(), c->vertex((id+2)%4)->point());
						*this << Segment_3(c->vertex((id+2)%4)->point(), c->vertex((id+3)%4)->point());
						*this << Segment_3(c->vertex((id+3)%4)->point(), c->vertex((id+1)%4)->point());
					}
				}
			}
			if (!do_force_edges) *this << TRIANGLES_END;
			else *this << SEGMENTS_END;
			std::cout << PROGRESS_DONE << std::endl;
		} else {
			std::cout << PROGRESS_STATUS << "Extracting Gabriel edges" << std::endl;
			*this << SEGMENTS_START;
			Finite_edges_iterator f_it, f_end = t->finite_edges_end();
			for (f_it = t->finite_edges_begin(); f_it!=f_end; f_it++) {
				if (t->is_Gabriel(*f_it)) {
					Cell_handle c = f_it->first;
					int id = f_it->second;
					int id2 = f_it->third;
					*this << Segment_3(
						c->vertex(id)->point(),
						c->vertex(id2)->point());
				}
			}
			*this << SEGMENTS_END;
			std::cout << PROGRESS_DONE << std::endl;
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
		case LINE_WIDTH_EDITABLE:
			return !do_faces || do_force_edges;
			break;
		case POINT_SIZE_EDITABLE:
		default:
			return false;
			break;
		}
	}


private:
	Power_crust_3 *parent;
	bool do_faces, do_force_edges;
};


#endif //MESECINA_GABRIEL_EDGES_LAYER_3_H