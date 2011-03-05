/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Input_mesh_layer_3.h 388 2008-12-12 18:17:06Z miklosb $
 */

#ifndef MESECINA_MESH_POLAR_LAYER_3_H
#define MESECINA_MESH_POLAR_LAYER_3_H

#include <geometries/mesh_view_3/Mesh_view_3.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>

class Mesh_polar_triangulation_layer_3 : public GL_draw_layer_3 {
public:
	typedef  Mesh_view_3::Mesh_polar_triangulation				Mesh_polar_triangulation;
	typedef  Mesh_polar_triangulation::Finite_vertices_iterator	Finite_vertices_iterator;
	typedef  Mesh_polar_triangulation::Cell_iterator	Cell_iterator;
	typedef  Mesh_polar_triangulation::Cell_handle	Cell_handle;	

	typedef  Mesh_view_3::Polyhedron				Polyhedron;
	//typedef  Polyhedron::Halfedge_around_facet_circulator HFC;
	typedef  Polyhedron::Traits::Point_3                  Point;
	typedef  Polyhedron::Traits::Triangle_3                  Triangle_3;
	typedef  Polyhedron::Traits::Segment_3                  Segment_3;
	typedef  Polyhedron::Traits::Sphere_3                  Sphere_3;

	Mesh_polar_triangulation_layer_3(const QString& name, Mesh_view_3* m, QString ttip, bool wireframe,bool balls = false, bool vertex = false) : GL_draw_layer_3(name,ttip), parent(m) {
		do_only_wireframe = wireframe;
		do_balls = balls;
		do_vertex = vertex;
	}

	virtual void draw_commands() {
		//C3t3* dm = parent->get_domain_mesh();
		Mesh_polar_triangulation* t = parent->get_mesh_polar_triangulation();
		if (do_vertex) {
			Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
			for (v_it=t->finite_vertices_begin(); v_it!=v_end; v_it++) {
				*this << v_it->point();
			}
		} else if (do_balls) {
			if (do_only_wireframe) *this << RENDER_NO_FILL;
			else *this << RENDER_FILL;
			std::list<Point4D>* balls = parent->get_mesh_polar_balls();
			std::list<Point4D>::iterator b_it, b_end = balls->end();
			for (b_it = balls->begin(); b_it!=b_end; b_it++) {
				Point p(b_it->x, b_it->y, b_it->z);
				*this << Sphere_3(p, b_it->w);
			}


		} else {
			*this << *t;
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case USER_PROPERTY_1:
		case GLUT_SPHERE:
			return do_balls;
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
		if (l == USER_PROPERTY_1) return "Save mesh balls";
		else return "";
	}

	virtual void execute_user_property(Layer_property l) {

		if (l == USER_PROPERTY_1) {

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

			parent->write_mesh_balls(file_name.toStdString(),parent->get_mesh_polar_balls());

			std::cout << "Balls file written to " << file_name.toStdString() << std::endl;

		}
	}

private:
	Mesh_view_3 *parent;
	bool do_only_wireframe, do_vertex, do_balls;
};


#endif //MESECINA_MESH_POLAR_LAYER_3_H