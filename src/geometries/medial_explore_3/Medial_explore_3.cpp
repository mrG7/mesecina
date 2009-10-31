#include <QString>

#include <gui/app/static/Application_settings.h>

#include "Medial_explore_3.h"
#include "layers/Medial_surface_layer_3.h"

Medial_explore_3::Medial_explore_3() : has_topology_filtration(false) {
	GL_draw_layer_3* faces_layer = new Medial_surface_layer_3("Medial faces",this, "Faces in the medial surface", false, false, false);
	add_layer(faces_layer);
	GL_draw_layer_3* edges_layer = new Medial_surface_layer_3("Medial edges",this, "Edges in the medial surface", true, false, false);
	add_layer(edges_layer);

	GL_draw_layer_3* niv_layer = new Medial_surface_layer_3("Not interior vertices",this, "Vertices detected as not interior", false, true, false);
	add_layer(niv_layer);

	GL_draw_layer_3* an_layer = new Medial_surface_layer_3("Angle faces",this, "Angle filtered faces in the medial surface", false, false, true, false);
	add_layer(an_layer);
	GL_draw_layer_3* tan_layer = new Medial_surface_layer_3("Top angle faces",this, "Topology angle filtered faces in the medial surface", false, false, false, true);
	add_layer(tan_layer);

	GL_draw_layer_3* fr_layer = new Medial_surface_layer_3("Front faces",this, "Topology angle filtered faces in the medial surface", false, false, false, false, true);
	add_layer(fr_layer);
	GL_draw_layer_3* vis_layer = new Medial_surface_layer_3("Visited faces",this, "Topology angle filtered faces in the medial surface", false, false, false, false, false, true);
	add_layer(vis_layer);
	GL_draw_layer_3* visn_layer = new Medial_surface_layer_3("Left faces",this, "Topology angle filtered faces in the medial surface", false, false, false, false, false, true, true);
	add_layer(visn_layer);
	Application_settings::add_int_setting("medial-explore-front-steps",1);
	Application_settings::add_bool_setting("debug-medial-explore-front", false);

}
Medial_explore_3::~Medial_explore_3() {}
Geometry* Medial_explore_3::clone() {
	return 0;
}

void Medial_explore_3::load_generic_file(const std::string& file_name) {
	if (QString(file_name.c_str()).endsWith(".off")) {
		mat.read_from_off(file_name.c_str());
		mat.compute_face_normals();
		Face_iterator f_it, f_end = mat.faces_end();
		for (f_it=mat.faces_begin(); f_it!=f_end; ++f_it) {
			f_it->angle = ((double)rand())/RAND_MAX * 90.0;
//			std::cout << "Angle: " << f_it->angle << std::endl;
		}
		invalidate_cache();
	}
	if (QString(file_name.c_str()).endsWith(".moff")) {
		mat.read_from_moff(file_name.c_str());
		mat.compute_face_normals();
		invalidate_cache();
	}
}

Medial_axis_transform_3* Medial_explore_3::get_medial_axis() {
	return &mat;
}

Medial_axis_transform_3* Medial_explore_3::get_topology_filtered_medial_axis() {
	if (!has_topology_filtration) {
		mat.compute_topology_angle_filtration();
		has_topology_filtration = true;
	}
	return &mat;
}


void Medial_explore_3::application_settings_changed(const QString& name) {
	if (name == "medial-explore-front-steps" || name == "debug-medial-explore-front") {
		if (Application_settings::get_bool_setting("debug-medial-explore-front")) {
			invalidate_cache();
			do_widget_repaint();
		}
	}
}

void Medial_explore_3::invalidate_cache() {
	invalidate_all_layers();
	has_topology_filtration = false;
//	std::cout << "invalidate layers" << std::endl;
}