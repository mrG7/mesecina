/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_triangulation_layer_3.h 373 2008-07-11 02:20:13Z miklosb $
 */

#ifndef MESECINA_IMPLICIT_MESHER_LAYER_3_H
#define MESECINA_IMPLICIT_MESHER_LAYER_3_H

#include <gui/gl_widget_3/GL_draw_layer_3.h>
#include <gui/app/static/Application_settings.h>




template <class Union_of_balls_3>
class Implicit_mesher_layer_3 : public GL_draw_layer_3 {
	typedef typename Union_of_balls_3::Alpha_shape_3 Alpha_shape_3;
	typedef typename Alpha_shape_3::Point Point_3;
	typedef typename Union_of_balls_3::Polyhedron Polyhedron;




public:

	Implicit_mesher_layer_3(const QString& name, Union_of_balls_3* m, const QString& ttip, bool wireframe) : GL_draw_layer_3(name, ttip), parent(m) {
		do_wireframe = wireframe;

	}


	//Number_type surface_function (Point_3 p) {
	//	Alpha_shape_3::Vertex_handle ball = alpha_shape->nearest_power_vertex(p);
	//	const FT x2=p.x()*p.x(), y2=p.y()*p.y(), z2=p.z()*p.z();
	//	return x2+y2+z2-1;
	//}

	//Number_type sphere_function (Point_3 p) {
	//	const Number_type x2=p.x()*p.x(), y2=p.y()*p.y(), z2=p.z()*p.z();
	//	return x2+y2+z2-1;
	//}

	virtual void draw_commands() {
		Polyhedron* p = parent->get_boundary_polyhedron();
		
		*this << DO_BOUNDING;
		if (do_wireframe) *this << RENDER_NO_FILL;
		else *this << RENDER_FILL;
		*this << *p;
		if (!do_wireframe) *this << RENDER_NO_FILL;


		
		//if (do_wireframe) {
		//	*this << SEGMENTS_START;
		//	C2t3::Edge_iterator e_it, e_end = c2t3.edges_end();
		//	for (e_it=c2t3.edges_begin(); e_it!=e_end; ++e_it) {
		//		C2t3::Cell_handle c = e_it->first;
		//		int id1 = e_it->second;
		//		int id2 = e_it->third;
		//		*this << Segment_3(c->vertex(id1)->point(), c->vertex(id2)->point());
		//	}
		//	*this << SEGMENTS_END;
		//} else {
		//	*this << TRIANGLES_START;
		//	C2t3::Facet_iterator f_it, f_end = c2t3.facets_end();
		//	for (f_it=c2t3.facets_begin(); f_it!=f_end; ++f_it) {
		//		C2t3::Cell_handle c = f_it->first;
		//		int id = f_it->second;
		//		*this << Triangle_3(c->vertex((id+1)%4)->point(), c->vertex((id+2)%4)->point(), c->vertex((id+3)%4)->point());
		//	}
		//	*this << TRIANGLES_END;
		//}



	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case USER_PROPERTY_1:
			return true;
			break;
		case LINE_WIDTH_EDITABLE:
			return do_wireframe;
			break;
		default:
			return false;
			break;
		}
	}

	//virtual QString get_name_user_property(Layer_property l) { 
	//	if (l == SCALAR_EDITABLE) return "Id";
	//	else return "";
	//}

	//virtual void application_settings_changed(const QString& settings_name) {

	//	if (settings_name=="implicit-mesher-bound-angle" || settings_name=="implicit-mesher-bound-radius" || settings_name=="implicit-mesher-bound-distance") {
	//		invalidate_cache();
	//		widget->request_repaint();
	//	}
	//}
	virtual QString get_name_user_property(Layer_property l) { 
		if (l == USER_PROPERTY_1) return "Save to OFF";
		else return "";
	}

	virtual void execute_user_property(Layer_property l) {

		if (l == USER_PROPERTY_1) {

			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose an off filename to save surface mesh to",
				settings.value("last-data-directory",QString()).toString(),
				"OFF (*.off)");
			if (file_name=="") return;
			if (!file_name.endsWith(".off")) file_name += ".off";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);

			Polyhedron* p = parent->get_boundary_polyhedron();
			std::ofstream outfile;
			outfile.open(file_name.toStdString().c_str());
			outfile << *p;
			outfile.close();

		}
	}


private:
	Union_of_balls_3 *parent;
	bool do_wireframe;
	M_classification classification;
};


#endif //MESECINA_IMPLICIT_MESHER_LAYER_3_H