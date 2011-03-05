/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Input_mesh_layer_3.h 388 2008-12-12 18:17:06Z miklosb $
 */

#ifndef MESECINA_MEDIAL_SHEETS_LAYER_3_H
#define MESECINA_MEDIAL_SHEETS_LAYER_3_H

#include "../Medial_explore_3.h"
#include <gui/gl_widget_3/GL_draw_layer_3.h>

class Medial_sheets_layer_3 : public GL_draw_layer_3 {
public:


	Medial_sheets_layer_3(const QString& name, Medial_explore_3* m, QString ttip, bool wireframe, bool vertex = false, bool angle = false, bool top_angle = false, bool front = false, bool visited = false, bool negate = false) : GL_draw_layer_3(name,ttip), parent(m) {
		do_only_wireframe = wireframe;
		do_vertex = vertex;
		do_angle = angle;
		do_top_angle = top_angle;
		do_front = front;
		do_visited = visited;
		do_negate = negate;
		Application_settings::add_int_setting("debug-sheet-index",0);
		Application_settings::add_bool_setting("colorcode-sheets-by-front", false);
		Application_settings::add_double_setting("topology-filtration-angle-lower-limit", 30);
		/*Application_settings::add_bool_setting("show-only-boundary-sheet-edges", false);*/
		//Application_settings::add_bool_setting("debug-rim-vertices-cycle-computation", false);
		//Application_settings::add_int_setting("rim-vertices-id",0);
		//Application_settings::add_int_setting("rim-vertex-face-neighbor-id",0);
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
	}

	void draw_edge(const Edge& e) {
		*this << Segment_3(e.first->p, e.second->p);
	}

	virtual void draw_commands() {
		std::vector<std::set<Face*> >* sheets = parent->get_medial_axis_sheets();
		Medial_axis_transform_3* mat = parent->get_medial_axis();
		std::vector<double>* stability;
		if (do_top_angle) stability = parent->get_sheet_topology_angle_stability();

		if (do_only_wireframe) *this << RENDER_NO_FILL;
		else *this << RENDER_FILL;
		*this << DO_INVERSE_SCALAR;

		if (do_only_wireframe) {
			int sh_index = Application_settings::get_int_setting("debug-sheet-index");
			Edge_iterator e_it, e_end = mat->edges_end();
			for (e_it = mat->edges_begin(); e_it!=e_end; ++e_it) {
				std::set<int> neighbor_sheets;
				if (/*!Application_settings::get_bool_setting("show-only-boundary-sheet-edges") ||*/ mat->is_on_sheet_boundary(e_it->first, sh_index, neighbor_sheets)) {
					*this << (mat->is_edge_sheet_interior(e_it->first) ? 1 : 0);
					draw_edge(e_it->first);
				}
			}
		} else {
			if (do_vertex) {
				//Boundary_vertex_map bond_vertex;
				//double limit = Application_settings::get_double_setting("topology-filtration-angle-lower-limit");
				////	std::cout << "limit: " << limit << std::endl;


				//Edge_iterator e_it, e_end = mat->edges_end();
				//for (e_it =mat-> edges_begin(); e_it != e_end; e_it++) {
				//	bool is_on_boundary = (stability==0 && e_it->second.size() == 1);
				//	if (stability!=0) {
				//		Face_pointer_set nf = e_it->second;
				//		Neighbor_face_iterator nf_it, nf_end = nf.end(); int neighbor_count=0;
				//		for (nf_it = nf.begin(); nf_it!=nf_end; ++nf_it) {
				//			int sheet_index = (*nf_it)->sheet_index;
				//			if ((*stability)[sheet_index] > limit) neighbor_count++;
				//		}
				//		if (neighbor_count==1) is_on_boundary = true;
				//		else is_on_boundary = false;
				//	}
				//	if (is_on_boundary){ // we are on boundary of face
				//		Edge e = e_it->first;
				//		Vertex* v1 = e.first;
				//		Vertex* v2 = e.second;
				//		Face* f = *(e_it->second.begin());
				//		Point_3 bp = f->intersection_point;
				//		mat->add_to_boundary_map(v1, v2, bp, bond_vertex);
				//		mat->add_to_boundary_map(v2, v1, bp, bond_vertex);
				//	}
				//}

				//// collect marked inputs (with more than two boundary edges)
				//std::list<Boundary_vertex_map::iterator> to_delete;
				//Boundary_vertex_map::iterator bvm_it, bvm_end = bond_vertex.end();
				//for (bvm_it = bond_vertex.begin(); bvm_it!=bvm_end; ++bvm_it) {
				//	if (bvm_it->second.second == Point_3(-1,-1,-1) || bvm_it->second.first.second == (Vertex*)(0))
				//		to_delete.push_back(bvm_it);
				//	else {
				//		Vertex* v = bvm_it->first;
				//		Boundary_neighbor_info bni = bvm_it->second.first;
				//		Vertex* n1 = bni.first;
				//		Vertex* n2 = bni.second;
				//		double distance_ratio_sq = CGAL::squared_distance(n1->p,v->p)/CGAL::squared_distance(n2->p,v->p);
				//		double ratio = Application_settings::get_double_setting("boundary-smoothing-neighbor-edge-max-squared-ratio");
				//		if (distance_ratio_sq > ratio || distance_ratio_sq < 1.0/ratio)
				//			to_delete.push_back(bvm_it);
				//	}
				//}

				//std::cout << "Boundary vertices before removing more then 2 boundary edge vertices: " << bond_vertex.size() << std::endl;

				//// delete marked inputs
				//std::list<Boundary_vertex_map::iterator>::iterator d_it, d_end = to_delete.end();
				//for (d_it=to_delete.begin(); d_it!=d_end; d_it++) {
				//	bond_vertex.erase(*d_it);
				//}

				//std::cout << "Size of movable boundary vertices: " << bond_vertex.size() << std::endl;


				//for (bvm_it = bond_vertex.begin(); bvm_it!=bvm_end; ++bvm_it) {
				//	Vertex* v = bvm_it->first;

				//	Boundary_neighbor_info bni = bvm_it->second.first;
				//	Vertex* n1 = bni.first;
				//	Vertex* n2 = bni.second;
				//	Point_3 bp = bni.third;
				//	Point_3 proj = Line_3(n1->p, n2->p).projection(v->p);
				//	Point_3 midpoint = CGAL::ORIGIN + (((n1->p-CGAL::ORIGIN) + (n2->p-CGAL::ORIGIN)) / 2.0);
				//	Point_3 new_point = CGAL::ORIGIN + (((midpoint-CGAL::ORIGIN) + (v->p-CGAL::ORIGIN)) / 2.0);
				//	if (sqrt(CGAL::squared_distance(new_point,v->p)) < mat->bounding_diagonal_radius * Application_settings::get_double_setting("boundary-smoothing-max-vertex-movement")) {
				//		bvm_it->second.second = new_point;
				//		*this << 0;
				//		*this << v->p;

				//		*this << 1;
				//		*this << new_point;

				//		*this << 2;
				//		*this << Segment_3(v->p,new_point);
				//	}

				//}

			} else {

				std::vector<std::set<Face*> >::iterator sh_it, sh_end = sheets->end(); int i=0;
				for (sh_it=sheets->begin(); sh_it!=sh_end; ++sh_it, i++) {
					std::set<Face*>::iterator f_it, f_end = sh_it->end();
					if (do_top_angle) {
						*this << (*stability)[i];
//						*this << i;
					}
					else {
						if (Application_settings::get_bool_setting("colorcode-sheets-by-front"))
							*this << (mat->is_sheet_on_front(i) ? 1 : 0);					
						else *this << i;
					}
					if (!do_top_angle || (*stability)[i] > Application_settings::get_double_setting("topology-filtration-angle-lower-limit"))
						for (f_it = sh_it->begin(); f_it!=f_end; ++f_it) {
							draw_face(**f_it);
						}
				}
			}

		}
		*this << NO_SCALAR;
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

			closest_face->compute_normal();
			std::cout << "computed normal" << std::endl;
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case USER_PROPERTY_1:
			return true;
			break;
		case USER_PROPERTY_2:
		case USER_PROPERTY_3:
			return do_top_angle;
		case SCALAR_EDITABLE:
			return true;
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
		if (l == SCALAR_EDITABLE) return "Sheet id";
		if (l == USER_PROPERTY_1) return "Show sheet sizes";
		if (l == USER_PROPERTY_2) return "Save to OFF";
		if (l == USER_PROPERTY_3) return "Save to OBJ";
		else return "";
	}

	virtual void execute_user_property(Layer_property l) {
		if (l == USER_PROPERTY_1) {
			std::vector<std::set<Face*> >* sheets = parent->get_medial_axis_sheets();


			std::cout << "There are "<< sheets->size() << " sheets in the medial axis of sizes: ";
			std::vector<std::set<Face*> >::iterator sh_it, sh_end = sheets->end(); int i=0;
			for (sh_it=sheets->begin(); sh_it!=sh_end; ++sh_it, i++) {
				std::cout << sh_it->size() << " ";
			}
			std::cout << std::endl;

			//Medial_axis_transform_3* mat = parent->get_medial_axis_sheets();


			//Face_iterator f_it, f_end = mat->faces_end();
			//for (f_it = mat->faces_begin(); f_it!=f_end; ++f_it) {
			//	int sheet_index = f_it->get_sheet_index();
			//	if (sheet_sizes.find(sheet_index) == sheet_sizes.end())
			//		sheet_sizes[sheet_index] = 1;
			//	else sheet_sizes[sheet_index]++;

			//}

			//std::vector<int> sizes;
			//std::map<int, int>::iterator sh_it, sh_end = sheet_sizes.end();
			//for (sh_it = sheet_sizes.begin(); sh_it!=sh_end; sh_it++) {
			//	sizes.push_back(sh_it->second);
			//}
			//
			//std::cout << "There are "<< sizes.size() << " sheets in the medial axis of sizes: ";
			//std::sort(sizes.rbegin(), sizes.rend());
			//std::vector<int>::iterator s_it, s_end = sizes.end();
			//for (s_it=sizes.begin(); s_it!=s_end; ++s_it) {
			//	std::cout << *s_it << " ";
			//}
			//std::cout << std::endl;

			//for (sh_it = sheet_sizes.begin(); sh_it!=sh_end; sh_it++) {
			//	std::cout << sh_it->first <<"->" << sh_it->second << "   ";
			//}
			//std::cout << std::endl;
			
		}

		if (l == USER_PROPERTY_2) {

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

			parent->get_medial_axis()->write_to_off(file_name.toStdString().c_str(), 
				parent->get_sheet_topology_angle_stability(),
				Application_settings::get_double_setting("topology-filtration-angle-lower-limit"));
		}
		if (l == USER_PROPERTY_3) {

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

			parent->get_medial_axis()->write_to_obj(file_name.toStdString().c_str(), 
				parent->get_sheet_topology_angle_stability(),
				Application_settings::get_double_setting("topology-filtration-angle-lower-limit"));
		}
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if (settings_name=="debug-sheet-index") {
			if (do_only_wireframe) {
				invalidate_cache();
				widget->request_repaint();
			} else {
				//int lambda_limit = Application_settings::get_int_setting("debug-sheet-index");

				//scalar_min = scalar_max = lambda_limit;
				//invalidate_cache();
				//has_new_range = false;
				//widget->request_repaint();
			}

			std::vector<std::set<Face*> >* sheets = parent->get_medial_axis_sheets();
			Medial_axis_transform_3* mat = parent->get_medial_axis();
			std::cout << "This sheet is front : " << 
				(mat->is_sheet_on_front(Application_settings::get_int_setting("debug-sheet-index")) ? "TRUE" : "NOT") 
				<< std::endl;
		}

		if (settings_name=="colorcode-sheets-by-front" || settings_name=="boundary-smoothing-neighbor-edge-max-squared-ratio" || settings_name == "boundary-smoothing-max-vertex-movement") {
				invalidate_cache();
				widget->request_repaint();
		}
		//if (settings_name=="debug-sheet-index") {
		//	int lambda_limit = Application_settings::get_int_setting("debug-sheet-index");

		//	if (lambda_limit < 0) scalar_min = 0;
		//	else scalar_min = lambda_limit;
		//	invalidate_cache();
		//	has_new_range = false;
		//	widget->request_repaint();

		//}

		//if ((do_top_angle || do_angle) && settings_name=="angle-limit") {
		//	double lambda_limit = Application_settings::get_double_setting("angle-limit");

		//	if (lambda_limit < 0) scalar_min = 0;
		//	else scalar_min = lambda_limit;
		//	invalidate_cache();
		//	has_new_range = false;
		//	widget->request_repaint();

		//}
	}

private:
	Medial_explore_3 *parent;
	bool do_only_wireframe, do_vertex, do_angle, do_top_angle, do_front, do_visited, do_negate;

};


#endif //MESECINA_MEDIAL_SHEETS_LAYER_3_H