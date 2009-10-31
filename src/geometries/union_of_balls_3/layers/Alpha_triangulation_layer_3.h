/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Dual_faces_layer_3.h 373 2008-07-11 02:20:13Z miklosb $
 */

#ifndef MESECINA_ALPHA_TRIANGULATION_LAYER_3_H
#define MESECINA_ALPHA_TRIANGULATION_LAYER_3_H

#include <geometries/power_crust_3/Power_crust_3.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>

template <class Union_of_balls_3>
class Alpha_triangulation_layer_3 : public GL_draw_layer_3 {
public:
	typedef typename Union_of_balls_3::Alpha_shape_3				 Alpha_shape_3;
	typedef typename Alpha_shape_3::Finite_vertices_iterator	Finite_vertices_iterator;
	typedef typename Alpha_shape_3::Facet							Facet;
	typedef typename Alpha_shape_3::Edge							Edge;
	typedef typename Alpha_shape_3::Classification_type				Alpha_classification;
	typedef typename Union_of_balls_3::Triangle_3										 Triangle_3;
	typedef typename Union_of_balls_3::Segment_3										 Segment_3;
	typedef typename Union_of_balls_3::Sphere_3											Sphere_3;

	

	Alpha_triangulation_layer_3(const QString& name, Union_of_balls_3* m, const QString& ttip, bool vertices, bool edges, bool faces, bool ball, bool fill, bool dual, bool classified = false, Alpha_classification u = Alpha_shape_3::REGULAR) : GL_draw_layer_3(name, ttip), parent(m) {
		uc = u;
		do_vertices = vertices;
		do_edges = edges;
		do_faces = faces;
		do_ball = ball;
		do_fill = fill;
		do_dual = dual;
		do_classified = classified;
	}

	virtual void draw_commands() {
//		std::cout <<LOG_BLUE << "Drawing layer " << name.toStdString() << std::endl;
		Alpha_shape_3* a = parent->get_alpha_shape();
		if (do_fill) *this << RENDER_FILL;
		else {
//			std::cout << name.toStdString() << " no fill" << std::endl;
			*this << RENDER_NO_FILL;
		}

		if (do_classified) {
			if (do_faces) {
				std::list<Facet>       facets;
				if (a->number_of_vertices() > 3)
					a->get_alpha_shape_facets(std::back_inserter(facets),uc);

//				std::cout << "layer " << name.toStdString() << " has " << facets.size() << " triangles" << std::endl;
				if (facets.size() > 0) {
					std::list<Facet>::iterator f_it = facets.begin(), f_end = facets.end();
					for (;f_it!=f_end; f_it++) {
						if (do_fill)
							*this << Triangle_3(
								f_it->first->vertex((f_it->second+1)%4)->point(),
								f_it->first->vertex((f_it->second+2)%4)->point(),
								f_it->first->vertex((f_it->second+3)%4)->point());
						else {
							*this << Segment_3(f_it->first->vertex((f_it->second+1)%4)->point(), f_it->first->vertex((f_it->second+2)%4)->point());
							*this << Segment_3(f_it->first->vertex((f_it->second+3)%4)->point(), f_it->first->vertex((f_it->second+2)%4)->point());
							*this << Segment_3(f_it->first->vertex((f_it->second+1)%4)->point(), f_it->first->vertex((f_it->second+3)%4)->point());
						}
					}
				}
			} else if (do_edges) {
				std::list<Edge>       edges;
				if (a->number_of_vertices() > 3)
				a->get_alpha_shape_edges(std::back_inserter(edges),uc);

				if (edges.size() > 0) {
					std::list<Edge>::iterator e_it = edges.begin(), e_end = edges.end();
					for (;e_it!=e_end; e_it++) {
						*this << Segment_3(
							e_it->first->vertex(e_it->second)->point(),
							e_it->first->vertex(e_it->third)->point());
					}
				}
			}
		} else {
			if (do_vertices) {
				Finite_vertices_iterator v_it, v_end = a->finite_vertices_end();
				for (v_it = a->finite_vertices_begin(); v_it != v_end; ++v_it) {
					if (do_ball) {
						*this << Sphere_3(v_it->point().point(), v_it->point().weight());
					} else *this << v_it->point();
				}
			} else if (do_edges) {

			} else if (do_faces) {

			}
		}
		*this << RENDER_FILL;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case USER_PROPERTY_1:
			return true;
			break;
		case POINT_SIZE_EDITABLE:
			return do_vertices && !do_ball;
			break;
		case GLUT_SPHERE:
			return do_ball;
			break;
		case LINE_WIDTH_EDITABLE:
			return do_edges || (!do_fill && do_faces);
		default:
			return false;
			break;
		}
	}

	// this tells the display name of the custom action
	virtual QString get_name_user_property(Layer_property l) { 
		if (l == USER_PROPERTY_1) return "Alpha balls to povray file";
		return "";
	}

	// this is called when a custom action is clicked by the user
	virtual void execute_user_property(Layer_property l) {

		if (l == USER_PROPERTY_1) {

			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose a povray filename to save balls to",
				settings.value("last-povray-directory",QString()).toString(),
				"OFF (*.txt)");
			if (file_name=="") return;
			if (!file_name.endsWith(".txt")) file_name += ".txt";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-povray-directory",save_directory);

			std::ofstream myfile;
			myfile.open (file_name.toStdString().c_str());

			myfile << "camera {" << std::endl;
			qglviewer::Vec pos = widget->camera()->position();
			myfile << "  location <" <<pos.x<<", "<<pos.y<<", "<<pos.z<<">"<<std::endl;
			qglviewer::Vec up = widget->camera()->upVector();
			myfile << "  up <" <<up.x<<", "<<up.y<<", "<<up.z<<">"<<std::endl;
			qglviewer::Vec view = widget->camera()->viewDirection();
			myfile << "  direction <" <<view.x<<", "<<view.y<<", "<<view.z<<">"<<std::endl;
			qglviewer::Vec right = widget->camera()->rightVector();
			myfile << "  right <" <<right.x<<", "<<right.y<<", "<<right.z<<">"<<std::endl;
			myfile << "  look_at <0.0, 0.0,  0.0>"<<std::endl;
			myfile << "}" << std::endl;

			myfile << "merge {" << std::endl;
			Alpha_shape_3* a = parent->get_alpha_shape();
			Finite_vertices_iterator v_it, v_end = a->finite_vertices_end();
			for (v_it = a->finite_vertices_begin(); v_it != v_end; ++v_it) {
				Point_3 center = v_it->point().point();
				double radius = sqrt(CGAL::to_double(v_it->point().weight()));
				myfile << "sphere { <" <<center.x()<< ", " <<center.y()<< ", " <<center.z()<< ">, " << radius << "}" << std::endl;
			}
			myfile << "}" << std::endl;

			myfile.close();	

		}


	}

private:
	Union_of_balls_3 *parent;
	Alpha_classification uc;
	bool do_vertices, do_edges, do_faces, do_ball, do_fill, do_dual, do_classified;
	//std::string classification_string;
};


#endif //MESECINA_ALPHA_TRIANGULATION_LAYER_3_H