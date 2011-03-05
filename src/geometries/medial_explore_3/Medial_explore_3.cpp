#include <QString>

#include <gui/app/static/Application_settings.h>

#include "Medial_explore_3.h"
#include "layers/Medial_surface_layer_3.h"
#include "layers/Medial_sheets_layer_3.h"

#include <limits>
typedef std::numeric_limits< double > dbl;

Medial_explore_3::Medial_explore_3() : has_topology_filtration(false), has_sheets(false), has_mat(false), has_balls(false) {
	GL_draw_layer_3* faces_layer = new Medial_surface_layer_3("Medial faces",this, "Faces in the medial surface", false, false, false);
	add_layer(faces_layer);
	GL_draw_layer_3* edges_layer = new Medial_surface_layer_3("Medial edges",this, "Edges in the medial surface", true, false, false);
	add_layer(edges_layer);

	GL_draw_layer_3* niv_layer = new Medial_surface_layer_3("Medial vertices",this, "Vertices detected as not interior", false, true, false);
	add_layer(niv_layer);

	GL_draw_layer_3* an_layer = new Medial_surface_layer_3("Angle faces",this, "Angle filtered faces in the medial surface", false, false, true, false);
	add_layer(an_layer);
	GL_draw_layer_3* la_layer = new Medial_surface_layer_3("Lambda faces",this, "Lambda filtered faces in the medial surface", false, false, false, false, false, false, false, false, true);
	add_layer(la_layer);
	GL_draw_layer_3* tan_layer = new Medial_surface_layer_3("Top angle faces",this, "Topology angle filtered faces in the medial surface", false, false, false, true);
	add_layer(tan_layer);

	GL_draw_layer_3* fr_layer = new Medial_surface_layer_3("Front faces",this, "Topology angle filtered faces in the medial surface", false, false, false, false, true);
	add_layer(fr_layer);
	GL_draw_layer_3* vis_layer = new Medial_surface_layer_3("Visited faces",this, "Topology angle filtered faces in the medial surface", false, false, false, false, false, true);
	add_layer(vis_layer);
	GL_draw_layer_3* visn_layer = new Medial_surface_layer_3("Left faces",this, "Topology angle filtered faces in the medial surface", false, false, false, false, false, true, true);
	add_layer(visn_layer);

	GL_draw_layer_3* scale_balls_layer = new Medial_surface_layer_3("Dense scale axis balls",this, "Finite set of balls approximating the scale axis", false, false, false, false, false, false, false, true);
	add_layer(scale_balls_layer);

	GL_draw_layer_3* lambda_balls_layer = new Medial_surface_layer_3("Dense lambda balls",this, "Finite set of balls approximating the scale axis", false, false, false, false, false, false, false, true, true);
	add_layer(lambda_balls_layer);
	GL_draw_layer_3* angle_balls_layer = new Medial_surface_layer_3("Dense angle balls",this, "Finite set of balls approximating the scale axis", false, false, true, false, false, false, false, true);
	add_layer(angle_balls_layer);
	GL_draw_layer_3* topangle_balls_layer = new Medial_surface_layer_3("Dense topangle balls",this, "Finite set of balls approximating the scale axis", false, false, false, true, false, false, false, true);
	add_layer(topangle_balls_layer);


	// medial sheets
	GL_draw_layer_3* med_sheets_layer = new Medial_sheets_layer_3("Medial sheets",this, "Medial sheets segmented by rim and seam points", false);
	add_layer(med_sheets_layer);
	GL_draw_layer_3* med_sheets_wf_layer = new Medial_sheets_layer_3("Medial wireframes",this, "The wireframe of sheets", true);
	add_layer(med_sheets_wf_layer);

	GL_draw_layer_3* med_topa_sheets_layer = new Medial_sheets_layer_3("Medial topangle sheets",this, "Medial sheets filtered by topology preserving angle filtration", false, false, false, true);
	add_layer(med_topa_sheets_layer);
	

	GL_draw_layer_3* med_vert_sheets_layer = new Medial_sheets_layer_3("Medial sheets vertices",this, "Medial sheets vertices, currently misused for finding boundary vertices", false, true, false, true);
	add_layer(med_vert_sheets_layer);

	Application_settings::add_int_setting("medial-explore-front-steps",1);
	Application_settings::add_bool_setting("debug-medial-explore-front", false);

	Application_settings::add_int_setting("medial-explore-sheet-front-steps",1);
	Application_settings::add_bool_setting("debug-medial-explore-sheet-front", false);

	Application_settings::add_double_setting("scale-axis-balls-center-max-distance",0.000000001, "Minimum squared distance between centers as of balls before they'll be considered duplicate -- relative to bounding diagonal");
	Application_settings::add_double_setting("scale-axis-balls-min-intersection-depth",140, "Minimum intersection depth of two balls connected by a segment -- otherwise triangle will be subdivided into 4");

	Application_settings::add_double_setting("angle-limit",10);
	Application_settings::add_double_setting("lambda-limit",0);

	Application_settings::add_int_setting("medial-surface-boundary-smoothing-steps", 0);
	Application_settings::add_int_setting("scale-surface-boundary-smoothing-steps", 0);
	Application_settings::add_double_setting("boundary-smoothing-neighbor-edge-max-squared-ratio",10);
	Application_settings::add_double_setting("boundary-smoothing-max-vertex-movement",0.05);

}
Medial_explore_3::~Medial_explore_3() {}
Geometry* Medial_explore_3::clone() {
	Medial_explore_3* new_medial_explore = new Medial_explore_3();
	return new_medial_explore;
}

void Medial_explore_3::receive_structure_changed(const std::string& name) {
	if (name == SHARED_MOFF_STRING) {
		std::cout << LOG_GREEN << "New moff string" << std::endl;
		invalidate_cache();
		has_mat = false;
	}
}

void Medial_explore_3::load_generic_file(const std::string& file_name) {
	if (QString(file_name.c_str()).endsWith(".off")) {
		mat.read_from_off(file_name.c_str());
		mat.compute_face_normals();
		mat.smooth_medial_surface_boundary(Application_settings::get_int_setting("medial-surface-boundary-smoothing-steps"));
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
		mat.smooth_medial_surface_boundary(Application_settings::get_int_setting("medial-surface-boundary-smoothing-steps"));
		add_variable("Faces - medial explore", mat.faces.size());
		add_variable("Vertices - medial explore", mat.vertices.size());
		add_variable("Edges - medial explore", mat.edges.size());
		has_mat = true;
		invalidate_cache();
//		widget->repaint();
	}
}

Medial_axis_transform_3* Medial_explore_3::get_medial_axis() {
	if (!has_mat) {
		void* p = request_structure(SHARED_MOFF_STRING);
		if (!p) std::cout << LOG_ERROR << "Error getting moff string in Medial_explore_3 geometry" << std::endl;
		else {
			std::string* moff_string = static_cast<std::string*> (p);
			std::stringstream ss(*moff_string);
			mat.read_from_moff(ss);
			mat.compute_face_normals();
			mat.smooth_medial_surface_boundary(Application_settings::get_int_setting("medial-surface-boundary-smoothing-steps"));
		}
		has_mat = true;
	}
	return &mat;
}

Medial_axis_transform_3* Medial_explore_3::get_topology_filtered_medial_axis() {
	if (!has_topology_filtration) {
		get_medial_axis();
//		mat.compute_topology_angle_filtration();
		has_topology_filtration = true;
	}
	return &mat;
}

std::vector<double>* Medial_explore_3::get_sheet_topology_angle_stability() {
	get_medial_axis_sheets();
	if (!has_topology_sheets) {
		mat.compute_topology_angle_sheet_filtration();
		has_topology_sheets = true;
	}
	return &(mat.sheet_top_angle);

}

std::vector<std::set<Face*> >* Medial_explore_3::get_medial_axis_sheets() {
	if (!has_sheets) {
		get_medial_axis();
		mat.identify_sheets();
		add_variable("Sheets - medial explore", mat.sheets.size());
		has_sheets = true;
	}
	return &(mat.sheets);
	
}

std::vector<Ball>* Medial_explore_3::get_sampled_balls() {
	if (!has_balls) {
		get_medial_axis();
		mat.sample_balls();
		has_balls = true;
	}
	return &(mat.scale_axis_balls);
}



void Medial_explore_3::application_settings_changed(const QString& name) {
	if (name == "medial-explore-front-steps" || name == "debug-medial-explore-front") {
		if (Application_settings::get_bool_setting("debug-medial-explore-front")) {
			invalidate_cache();
			do_widget_repaint();
		}
	}
	if (name == "medial-surface-boundary-smoothing-steps") {
		invalidate_cache();
		has_mat = false;
		do_widget_repaint();
	}
}

void Medial_explore_3::invalidate_cache() {
	invalidate_all_layers();
	has_topology_sheets = has_sheets = has_topology_filtration = has_balls = false;
//	std::cout << "invalidate layers" << std::endl;
}

void Medial_explore_3::write_mesh_balls(const std::string& file_name, std::vector<Ball>* balls) {
//		get_mesh_balls();

		std::ofstream myfile;
		myfile.open (file_name.c_str());
		myfile.precision(dbl::digits10);

		// write header
		myfile << "WOFF " << balls->size() << " 0 0" << std::endl;

		// write vertices
		std::vector<Ball>::iterator p_it, p_end = balls->end();
		for (p_it=balls->begin(); p_it!=p_end; ++p_it) {
			myfile << std::fixed << p_it->first.x() << " " 
				   << std::fixed << p_it->first.y() << " " 
				   << std::fixed << p_it->first.z() << " " 
				   << std::fixed << p_it->second << std::endl;
		}
		std::cout << "Written " << balls->size() << " balls." << std::endl;
		myfile.close();
}