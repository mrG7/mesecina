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


	Medial_surface_layer_3(const QString& name, Medial_explore_3* m, QString ttip, bool wireframe, bool vertex, bool angle = false, bool top_angle = false,bool front = false, bool visited = false, bool negate = false, bool scale_balls = false, bool lambda = false) : GL_draw_layer_3(name,ttip), parent(m) {
		do_only_wireframe = wireframe;
		do_vertex = vertex;
		do_angle = angle;
		do_lambda = lambda;
		do_top_angle = top_angle;
		do_front = front;
		do_visited = visited;
		do_negate = negate;
		do_scale_balls = scale_balls;
		Application_settings::add_bool_setting("debug-rim-vertices-cycle-computation", false);
		Application_settings::add_int_setting("rim-vertices-id",0);
		Application_settings::add_int_setting("rim-vertex-face-neighbor-id",0);
	}

	void draw_face(Face& f) {
		std::list<Point_3> polygon;
		Face_vertex_iterator fv_it, fv_end = f.vertices.end();
		for (fv_it = f.vertices.begin(); fv_it!=fv_end; ++fv_it) {
			polygon.push_back((*fv_it)->p);
		}
		*this << NORMAL;
		*this << CGAL::ORIGIN + f.normal;
//		std::cout << CGAL::ORIGIN + f.get_normal() << std::endl;
		*this << polygon;
//		*this << f.intersection_point;
	}

	void draw_edge(const Edge& e) {
		*this << Segment_3(e.first->p, e.second->p);
	}

	virtual void draw_commands() {
//		std::cout << "drawing " << do_only_wireframe << std::endl;
		Medial_axis_transform_3* mat = parent->get_medial_axis();
		if (do_top_angle || do_front || do_visited || do_only_wireframe) mat = parent->get_topology_filtered_medial_axis();
		if (do_only_wireframe) {
			*this << DO_INVERSE_SCALAR;
			Edge_iterator e_it, e_end = mat->edges_end();
			for (e_it = mat->edges_begin(); e_it!=e_end; ++e_it) {
//				int nf = mat->edge_number_of_left_neighbor_faces(e_it->first);
//				if (nf>0) {
//					*this << nf;
					draw_edge(e_it->first);
//				}
			}
			*this << NO_SCALAR;
		} else if (do_vertex) {
			Vertex_vector::iterator mv_it, mv_end = mat->vertices.end();
			*this << POINTS_START;
			for (mv_it = mat->vertices.begin(); mv_it!=mv_end; mv_it++) {
				*this << mv_it->p;
			}
			*this << POINTS_END;
/*
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
				
			} */
		} else if (do_scale_balls){
			*this << RENDER_FILL;
//			std::cout << "scale_axis_balls size: " << mat->scale_axis_balls.size() << std::endl;
			std::vector<Ball> balls;
			parent->get_sampled_balls();
			if (do_angle) {
				mat->fill_balls_with_criteria(CRIT_ANGLE, Application_settings::get_double_setting("angle-limit"), balls);
			} else if (do_lambda) {
				mat->fill_balls_with_criteria(CRIT_LAMBDA, Application_settings::get_double_setting("lambda-limit"), balls);
			} else if (do_top_angle) {
				parent->get_sheet_topology_angle_stability();
				mat->fill_balls_with_criteria(CRIT_SHEET_TOPANGLE, Application_settings::get_double_setting("topology-filtration-angle-lower-limit"), balls);
			} else {
				std::vector<Ball>::iterator b_it, b_end = mat->scale_axis_balls.end();
				for (b_it=mat->scale_axis_balls.begin(); b_it!=b_end; ++b_it) {
					*this << Sphere_3(b_it->first, b_it->second*b_it->second);
				}
			}
			if (do_angle || do_lambda || do_top_angle) {
				std::vector<Ball>::iterator b_it, b_end = balls.end();
				for (b_it=balls.begin(); b_it!=b_end; ++b_it) {
					*this << Sphere_3(b_it->first, b_it->second*b_it->second);
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
				if (do_angle || do_top_angle || do_lambda) *this << DO_INVERSE_SCALAR;

				Face_iterator f_it, f_end = mat->faces_end();
				for (f_it = mat->faces_begin(); f_it!=f_end; ++f_it) {
					if (do_angle) *this << f_it->angle;
					if (do_lambda) *this << f_it->lambda;
					if (do_top_angle) {
						*this << f_it->top_angle;
						// *this << (mat->is_front(&(*f_it)) ? 1 : 0);
						// std::cout << f_it->top_angle;
					}
					draw_face(*f_it);
				}

				//std::cout << LOG_GREEN << "drawing triangles" << std::endl;
				//Face_iterator f_it, f_end = mat->triangles.end();
				//for (f_it = mat->triangles.begin(); f_it!=f_end; ++f_it) {

				//	draw_face(*f_it);
				//}

				if (do_angle || do_top_angle || do_lambda) *this << NO_SCALAR;
			}
		}

	}

	double face_distance(Face& f, double x, double y, double z) {
		double d = 0; Point_3 q(x,y,z);
		Face_vertex_iterator fv_it, fv_end = f.vertices.end();int n=0;
		for (fv_it = f.vertices.begin(); fv_it!=fv_end; ++fv_it) {
			Point_3 p = (*fv_it)->p;
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

			Face_vertex_iterator fv_it, fv_end = closest_face->vertices.end();
			for (fv_it = closest_face->vertices.begin(); fv_it!=fv_end; ++fv_it) {
				Point_3 p = (*fv_it)->p;
				widget->add_point_to_selection(p.x(), p.y(), p.z());
			}

			{
				Face* f = closest_face;

				// collect edges
				Edge_set face_edges;
				Face_vertex_iterator fv_it, fv_next, fv_end =f->vertices.end();
				fv_next = fv_it = f->vertices.begin();
				fv_next++;
				for (; fv_next!=fv_end; ++fv_it, ++fv_next) {
					Vertex* v1 = *fv_it;
					Vertex* v2 = *fv_next;
					Edge e = make_edge(v1,v2);
					face_edges.insert(e);
				}
				// close the loop with the end -> begin
				Edge e = make_edge(*fv_it,*(f->vertices.begin()));
				face_edges.insert(e);

				std::cout << "Number of edges: " << face_edges.size() << std::endl;

				// go through edges
				Edge_set_iterator e_it, e_end = face_edges.end();
				for (e_it = face_edges.begin(); e_it!=e_end; ++e_it) {
					// consider neighbor only if edge has two neighbor faces
					std::cout << "edge neghbor faces: " << mat->edges[*e_it].size() << std::endl;
					//if (edges[*e_it].size()==2) {
					//	Neighbor_face_iterator nf_it, nf_end = edges[*e_it].end();
					//	for (nf_it=edges[*e_it].begin(); nf_it!=nf_end; ++nf_it) {
					//		Face* neighbor_face = *nf_it;
					//		if (last_sheet_flooded.find(neighbor_face)==last_sheet_flooded.end()) {
					//			neighbor_face->set_sheet_index(i); // set sheet index
					//			last_sheet_flooded.insert(neighbor_face); // add to flooded so that we don't visit it again
					//			last_sheet_faces.push(neighbor_face); // add him to the queue to visit neighbors
					//			sheet_unvisited.erase(neighbor_face); // make sure we don't choose this as seed for a new sheet
					//		}
					//	}
					//}
				}

				//closest_face->compute_normal();
				//std::cout << "computed normal" << std::endl;
			}
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case USER_PROPERTY_1:
		case USER_PROPERTY_2:
			return true;
			break;
		case GLUT_SPHERE:
		case USER_PROPERTY_3:
			return do_scale_balls;
		case SCALAR_EDITABLE:
			return do_angle || do_top_angle || do_only_wireframe  || do_lambda;
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
		if (l == SCALAR_EDITABLE) 
			if (do_lambda) return "Lambda";
			else return "Angle";
		if (l == USER_PROPERTY_1) return "Save to OFF";
		if (l == USER_PROPERTY_2) return "Save to OBJ";
		if (l == USER_PROPERTY_3) return "Save to medial balls";
		else return "";
	}


	virtual void execute_user_property(Layer_property l) {

		if (l == USER_PROPERTY_1) {

			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose an off filename to save medial axis to",
				settings.value("last-data-directory",QString()).toString(),
				"OFF (*.off)");
			if (file_name=="") return;
			if (!file_name.endsWith(".off")) file_name += ".off";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);

			if (do_angle) {
				parent->get_medial_axis()->write_to_off(file_name.toStdString().c_str());

			} else if (do_lambda) {
			} else	parent->get_medial_axis()->write_to_off(file_name.toStdString().c_str());
		}
		if (l == USER_PROPERTY_2) {

			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose an off filename to save medial axis to",
				settings.value("last-data-directory",QString()).toString(),
				"OBJ (*.obj)");
			if (file_name=="") return;
			if (!file_name.endsWith(".obj")) file_name += ".obj";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);

			parent->get_medial_axis()->write_to_obj(file_name.toStdString().c_str());
		}
		if (l == USER_PROPERTY_3) {

			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose an off filename to save medial balls to",
				settings.value("last-data-directory",QString()).toString(),
				"WOFF (*.woff)");
			if (file_name=="") return;
			if (!file_name.endsWith(".woff")) file_name += ".woff";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);
			
			std::vector<Ball> balls;
			parent->get_sampled_balls();
			if (do_angle) {
				parent->mat.fill_balls_with_criteria(CRIT_ANGLE, Application_settings::get_double_setting("angle-limit"), balls);
			} else if (do_lambda) {
				parent->mat.fill_balls_with_criteria(CRIT_LAMBDA, Application_settings::get_double_setting("lambda-limit"), balls);
			} else if (do_top_angle) {
				parent->get_sheet_topology_angle_stability();
				parent->mat.fill_balls_with_criteria(CRIT_SHEET_TOPANGLE, Application_settings::get_double_setting("topology-filtration-angle-lower-limit"), balls);
			} else {
				std::vector<Ball>::iterator b_it, b_end = parent->mat.scale_axis_balls.end();
				for (b_it=parent->mat.scale_axis_balls.begin(); b_it!=b_end; ++b_it) {
					*this << Sphere_3(b_it->first, b_it->second*b_it->second);
				}
			}

			parent->write_mesh_balls(file_name.toStdString(), &balls);
		}
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

		if ((do_lambda) && settings_name=="lambda-limit") {
			double lambda_limit = Application_settings::get_double_setting("lambda-limit");

			if (lambda_limit < 0) scalar_min = 0;
			else scalar_min = lambda_limit;
			invalidate_cache();
			has_new_range = false;
			widget->request_repaint();

		}
	}

private:
	Medial_explore_3 *parent;
	bool do_only_wireframe, do_vertex, do_angle, do_top_angle, do_front, do_visited, do_negate, do_scale_balls, do_lambda;

};


#endif //MESECINA_MEDIAL_SURFACE_LAYER_3_H