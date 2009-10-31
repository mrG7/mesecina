/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Input_mesh_layer_3.h 388 2008-12-12 18:17:06Z miklosb $
 */

#ifndef MESECINA_MESH_LAYER_3_H
#define MESECINA_MESH_LAYER_3_H

#include <geometries/mesh_view_3/Mesh_view_3.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>

class Mesh_layer_3 : public GL_draw_layer_3 {
public:
	typedef  Mesh_view_3::Polyhedron				Polyhedron;
	typedef  Polyhedron::Halfedge_around_facet_circulator HFC;
	typedef  Polyhedron::Traits::Point_3                  Point;
	typedef  Polyhedron::Traits::Triangle_3                  Triangle_3;

	Mesh_layer_3(const QString& name, Mesh_view_3* m, QString ttip, bool wireframe, bool vertex, bool count = false) : GL_draw_layer_3(name,ttip), parent(m) {
		do_only_wireframe = wireframe;
		do_vertex = vertex;
		do_count = count;
	}

	virtual void draw_commands() {
//		std::cout << "drawing " << do_only_wireframe << std::endl;
		Polyhedron* p = parent->get_polyhedron();
		
		if (do_vertex) {
			Polyhedron::Vertex_iterator v_it, v_end = p->vertices_end();
			for (v_it = p->vertices_begin(); v_it!=v_end; ++v_it) {
				*this << v_it->point();
			}
		} else if (do_count) {


			*this << TRIANGLES_START;
			*this << DO_SCALAR;
			int ind = 0, i;	Point pts[3];
			for(Polyhedron::Facet_iterator fi = p->facets_begin();
				fi != p->facets_end(); ++fi, ++ind) {
					//std::cout << PROGRESS_VALUE << ind << std::endl;
					HFC hc = fi->facet_begin();
					HFC hc_end = hc; i = 0;
					do {
						pts[i] = (*hc).vertex()->point();
						i++;
					} while (++hc != hc_end);
					*this << ind;
					*this << Triangle_3(pts[0], pts[1], pts[2]);
			}
			*this << NO_SCALAR;
			*this << TRIANGLES_END;


		} else {
			if (do_only_wireframe) *this << RENDER_NO_FILL;
			else *this << RENDER_FILL;
			*this << *p;
			if (!do_only_wireframe) *this << RENDER_NO_FILL;
		}

	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case SCALAR_EDITABLE:
			return do_count;
			break;
		case LINE_WIDTH_EDITABLE:
			return do_only_wireframe && !do_vertex;
		case POINT_SIZE_EDITABLE:
			return do_vertex;
		default:
			return false;
			break;
		}
	}

	virtual QString get_name_user_property(Layer_property l) { 
		if (l == SCALAR_EDITABLE) return "Index";
		else return "";
	}

private:
	Mesh_view_3 *parent;
	bool do_only_wireframe, do_vertex, do_count;
};


#endif //MESECINA_MESH_LAYER_3_H