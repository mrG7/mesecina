/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Input_mesh_layer_3.h 388 2008-12-12 18:17:06Z miklosb $
 */

#ifndef MESECINA_MEDIAL_SURFACE_LAYER_3_H
#define MESECINA_MEDIAL_SURFACE_LAYER_3_H

#include "../Medial_explore_3.h"
#include <gui/gl_widget_3/GL_draw_layer_3.h>

class Medial_surface_layer_3 : public GL_draw_layer_3 {
public:


	Medial_surface_layer_3(const QString& name, Medial_explore_3* m, QString ttip, bool wireframe, bool vertex, bool angle = false, bool top_angle = false, bool front = false, bool visited = false, bool negate = false) : GL_draw_layer_3(name,ttip), parent(m) {
		do_only_wireframe = wireframe;
		do_vertex = vertex;
		do_angle = angle;
		do_top_angle = top_angle;
		do_front = front;
		do_visited = visited;
		do_negate = negate;
		Application_settings::add_bool_setting("debug-rim-vertices-cycle-computation", false);
		Application_settings::add_int_setting("rim-vertices-id",0);
		Application_settings::add_int_setting("rim-vertex-face-neighbor-id",0);
	}

	void draw_face(Face& f) {
		std::list<Point_3> polygon;
		Face_vertex_iterator fv_it, fv_end = f.vertices_end();
		for (fv_it = f.vertices_begin(); fv_it!=fv_end; ++fv_it) {
			polygon.push_back((*fv_it)->point());
		}
		*this << NORMAL;
		*this << CGAL::ORIGIN + f.get_normal();
//		std::cout << CGAL::ORIGIN + f.get_normal() << std::endl;
		*this << polygon;
	}

	void draw_edge(const Edge& e) {
		*this << Segment_3(e.first->point(), e.second->point());
	}

	virtual void draw_commands() {
//		std::cout << "drawing " << do_only_wireframe << std::endl;
		Medial_axis_transform_3* mat = parent->get_medial_axis();
		if (do_top_angle || do_front || do_visited || do_only_wireframe) mat = parent->get_topology_filtered_medial_axis();
		if (do_only_wireframe) {
			*this << DO_INVERSE_SCALAR;
			Edge_iterator e_it, e_end = mat->edges_end();
			for (e_it = mat->edges_begin(); e_it!=e_end; ++e_it) {
				int nf = mat->edge_number_of_left_neighbor_faces(e_it->first);
				if (nf>0) {
					*this << nf;
					draw_edge(e_it->first);
				}
			}
			*this << NO_SCALAR;
		} else if (do_vertex) {
//			std::cout << "do_vertex branch, rim size: " << mat->rim_vertices.size() << std::endl;
			Rim_vertices_map::iterator vs_it, vs_end = mat->rim_vertices.end(); int counter=0;
			for (vs_it = mat->rim_vertices.begin(); vs_it!=vs_end; ++vs_it, ++counter) {
				if (Application_settings::get_bool_setting("debug-rim-vertices-cycle-computation")) {
					if (counter == Application_settings::get_int_setting("rim-vertices-id")) {
						*this << vs_it->first->point();
						Vertex* v = vs_it->first;

						// collect unvisited neighbor faces
						Face_pointer_set unvisited_neighbor_faces;
						Neighbor_face_iterator nf_it, nf_end = mat->vertices_map[v].end();
						for (nf_it = mat->vertices_map[v].begin(); nf_it!=nf_end; ++nf_it) {
							if (mat->visited.find(*nf_it)==mat->visited.end())
								unvisited_neighbor_faces.insert(*nf_it);
						}
						std::cout << LOG_BLUE << "Unvisited neighbor faces: " << unvisited_neighbor_faces.size() << std::endl;

						// identify the face we are looking for
						Face* f = 0;
						Neighbor_face_iterator unf_it, unf_end = unvisited_neighbor_faces.end(); int fcounter =0;
						for (unf_it=unvisited_neighbor_faces.begin(); unf_it!=unf_end; unf_it++, fcounter++) {
							if (fcounter==Application_settings::get_int_setting("rim-vertex-face-neighbor-id"))
								f = *unf_it;
						}

						// test it
						if (f!=0) {
							*this << RENDER_FILL;
							draw_face(*f);

							bool more_comp = true;
							bool has_cycle = mat->vertex_has_cycle_with_face(v, f, more_comp);
							if (has_cycle) std::cout << "It has cycle, it is NOT RIM" << std::endl;
							else if (more_comp) std::cout << "It has more components, it is NOT RIM" << std::endl;
							else std::cout << "It IS RIM" << std::endl;
						}
					}
				}
				else {
					*this << vs_it->first->point();
				}
			}
		} else {
			*this << RENDER_FILL;
			if (do_front) {
				*this << DO_INVERSE_SCALAR;
				Front_iterator fr_it, fr_end = mat->front.end();
				for (fr_it = mat->front.begin(); fr_it!=fr_end; ++fr_it) {
					draw_face(*(fr_it->second));
				}
				*this << NO_SCALAR;
			} else if (do_visited) {
				if (!do_negate) {
					Face_set::iterator f_it, f_end = mat->visited.end();
					for (f_it=mat->visited.begin(); f_it!=f_end; ++f_it) {
						draw_face(*(*f_it));
					}
				} else {
					Face_iterator f_it, f_end = mat->faces_end();
					for (f_it = mat->faces_begin(); f_it!=f_end; ++f_it) {
						if (mat->visited.find(&(*f_it)) == mat->visited.end())
							draw_face(*f_it);
					}
				}
			} else {
				//std::cout << "face loop rendering for " << name.toStdString() << std::endl;
				if (do_angle || do_top_angle) *this << DO_INVERSE_SCALAR;
				Face_iterator f_it, f_end = mat->faces_end();
				for (f_it = mat->faces_begin(); f_it!=f_end; ++f_it) {
					if (do_angle) *this << f_it->angle;
					if (do_top_angle) {
						*this << f_it->top_angle;
						// *this << (mat->is_front(&(*f_it)) ? 1 : 0);
						// std::cout << f_it->top_angle;
					}
					draw_face(*f_it);
				}
				if (do_angle || do_top_angle) *this << NO_SCALAR;
			}
		}

	}

	double face_distance(Face& f, double x, double y, double z) {
		double d = 0; Point_3 q(x,y,z);
		Face_vertex_iterator fv_it, fv_end = f.vertices_end();int n=0;
		for (fv_it = f.vertices_begin(); fv_it!=fv_end; ++fv_it) {
			Point_3 p = (*fv_it)->point();
			d = d+ sqrt(squared_distance(p,q));
			n++;
		}
		return d/n;
	}

	virtual void selection_at(double x, double y, double z) {
		if (!do_vertex && !do_only_wireframe) {
//			std::cout << "layer got selection at " << x <<"," << y <<"," << z <<std::endl;
			widget->clear_selection();
			Face* closest_face = 0; double closest_distance = 0;
			Medial_axis_transform_3* mat = parent->get_medial_axis();
			Face_iterator f_it, f_end = mat->faces_end();
			for (f_it = mat->faces_begin(); f_it!=f_end; ++f_it) {
				if (closest_face ==0 || face_distance(*f_it, x,y,z) < closest_distance) {
					closest_face = &(*f_it);
					closest_distance = face_distance(*f_it, x,y,z);
				}
			}

			Face_vertex_iterator fv_it, fv_end = closest_face->vertices_end();
			for (fv_it = closest_face->vertices_begin(); fv_it!=fv_end; ++fv_it) {
				Point_3 p = (*fv_it)->point();
				widget->add_point_to_selection(p.x(), p.y(), p.z());
			}

			closest_face->compute_normal();
			std::cout << "computed normal" << std::endl;
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case SCALAR_EDITABLE:
			return do_angle || do_top_angle || do_only_wireframe;
			break;
		case LINE_WIDTH_EDITABLE:
			return do_only_wireframe;
		case POINT_SIZE_EDITABLE:
			return do_vertex;
		default:
			return false;
			break;
		}
	}

	virtual QString get_name_user_property(Layer_property l) { 
		if (l == SCALAR_EDITABLE) return "Angle";
		else return "";
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if (settings_name.indexOf("rim-vert")!=-1 && do_vertex) {
			invalidate_cache();
			widget->request_repaint();
		}

		if ((do_top_angle || do_angle) && settings_name=="angle-limit") {
			double lambda_limit = Application_settings::get_double_setting("angle-limit");

			if (lambda_limit < 0) scalar_min = 0;
			else scalar_min = lambda_limit;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}
	}

private:
	Medial_explore_3 *parent;
	bool do_only_wireframe, do_vertex, do_angle, do_top_angle, do_front, do_visited, do_negate;

};


#endif //MESECINA_MEDIAL_SURFACE_LAYER_3_H