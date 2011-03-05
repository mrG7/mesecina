/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Input_mesh_layer_3.h 388 2008-12-12 18:17:06Z miklosb $
 */

#ifndef MESECINA_DOMAIN_MESH_LAYER_3_H
#define MESECINA_DOMAIN_MESH_LAYER_3_H

#include <geometries/mesh_view_3/Mesh_view_3.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>
//#include "../Mesh_template_defintions.h"

class Domain_mesh_layer_3 : public GL_draw_layer_3 {
public:
	typedef  Mesh_view_3::C3t3				C3t3;
	typedef  C3t3::Facet_iterator	Facet_iterator;
	typedef  C3t3::Cell_iterator	Cell_iterator;
	typedef  C3t3::Cell_handle	Cell_handle;	

	typedef  Mesh_view_3::Polyhedron				Polyhedron;
	//typedef  Polyhedron::Halfedge_around_facet_circulator HFC;
	typedef  Polyhedron::Traits::Point_3                  Point;
	typedef  Polyhedron::Traits::Triangle_3                  Triangle_3;
	typedef  Polyhedron::Traits::Segment_3                  Segment_3;
	typedef  Polyhedron::Traits::Sphere_3                  Sphere_3;

	Domain_mesh_layer_3(const QString& name, Mesh_view_3* m, QString ttip, bool wireframe, bool surface, bool balls = false, bool vertex = false, bool count = false) : GL_draw_layer_3(name,ttip), parent(m) {
		do_only_wireframe = wireframe;
		do_balls = balls;
		do_vertex = vertex;
		do_surface = surface;
		do_count = count;
	}

	virtual void draw_commands() {
//		std::cout << "drawing " << do_only_wireframe << std::endl;
		C3t3* dm = parent->get_domain_mesh();

		if (do_balls) {
			if (!do_only_wireframe) *this << RENDER_FILL;
			else { *this << RENDER_NO_FILL;			}
			std::list<Point4D>* balls = parent->get_mesh_balls();
			std::list<Point4D>::iterator b_it, b_end = balls->end();
			for (b_it = balls->begin(); b_it!=b_end; b_it++) {
				Point p(b_it->x, b_it->y, b_it->z);
				*this << Sphere_3(p, b_it->w);
			}
		
		} else if (do_surface) {
			//if (do_only_wireframe) *this << RENDER_NO_FILL;
			//else *this << RENDER_FILL;
			Facet_iterator f_it =  dm->facets_begin(), f_end = dm->facets_end();
			for (/*f_it = dm->facets_begin()*/; f_it!=f_end; f_it++) {
				if (do_only_wireframe) {
					Cell_handle c = f_it->first;
					int i = f_it->second;
					Point p1 = c->vertex((i+1)%4)->point();
					Point p2 = c->vertex((i+2)%4)->point();
					Point p3 = c->vertex((i+3)%4)->point();
					*this << Segment_3(p1,p2);
					*this << Segment_3(p1,p3);
					*this << Segment_3(p3,p2);

				} else *this << dm->triangulation().triangle(*f_it);
			}

		} else {
			Cell_iterator c_it =  dm->cells_begin(), c_end = dm->cells_end();
			for (/*f_it = dm->facets_begin()*/; c_it!=c_end; c_it++) {
				Point p0 = c_it->vertex(0)->point();
				Point p1 = c_it->vertex(1)->point();
				Point p2 = c_it->vertex(2)->point();
				Point p3 = c_it->vertex(3)->point();
				if (do_only_wireframe) {
					*this << Segment_3(p1,p2);
					*this << Segment_3(p1,p3);
					*this << Segment_3(p3,p2);
					*this << Segment_3(p3,p0);
					*this << Segment_3(p2,p0);
					*this << Segment_3(p1,p0);
				} else {
					*this << Triangle_3(p0,p1,p2);
					*this << Triangle_3(p3,p1,p2);
					*this << Triangle_3(p0,p3,p2);
					*this << Triangle_3(p0,p1,p3);
				}
			}
		}
		

	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case USER_PROPERTY_1:
			return true;
			break;
		case USER_PROPERTY_2:
		case GLUT_SPHERE:
			return do_balls;
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
		if (l == USER_PROPERTY_1) return "Save to Medit";
		if (l == USER_PROPERTY_2) return "Save to mesh balls";
		else return "";
	}

	virtual void execute_user_property(Layer_property l) {

		if (l == USER_PROPERTY_1) {

			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose a medit filename to save mesh to",
				settings.value("last-data-directory",QString()).toString(),
				"Mesh (*.mesh)");
			if (file_name=="") return;
			if (!file_name.endsWith(".mesh")) file_name += ".mesh";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);

			C3t3* dm = parent->get_domain_mesh();
			std::ofstream medit_file(file_name.toStdString().c_str());
			dm->output_to_medit(medit_file);
			medit_file.close();

			std::cout << "Medit file written to " << file_name.toStdString() << 
				" contains " << dm->number_of_cells() << " cells and " << 
				dm->number_of_facets() << " surface facets." << std::endl;

		}

		if (l == USER_PROPERTY_2) {

			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose a filename to save mesh balls to",
				settings.value("last-data-directory",QString()).toString(),
				"WOFF (*.woff)");
			if (file_name=="") return;
			if (!file_name.endsWith(".woff")) file_name += ".woff";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);

			parent->write_mesh_balls(file_name.toStdString(), parent->get_mesh_balls());

			std::cout << "Balls file written to " << file_name.toStdString() << std::endl;

		}
	}

private:
	Mesh_view_3 *parent;
	bool do_only_wireframe, do_vertex, do_count, do_surface, do_balls;
};


#endif //MESECINA_DOMAIN_MESH_LAYER_3_H