#include "Medrep_view_3.h"
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <gui/app/static/Application_settings.h>
#include <constants.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>

#include "layers/Medrep_input_mesh_layer_3.h"



Medrep_view_3::Medrep_view_3() : Geometry() {
	medrep = 0;
	active_reconstruction_mesh = active_skeleton_mesh = "";

	GL_draw_layer_3* input_mesh_layer = new Medrep_input_mesh_layer_3("MR input",this, "Medrep input mesh", false);
	add_layer(input_mesh_layer);
	GL_draw_layer_3* input_wf_layer = new Medrep_input_mesh_layer_3("MR input wireframe",this, "Medrep input mesh", true);
	add_layer(input_wf_layer);

	GL_draw_layer_3* sk_mesh_layer = new Medrep_skeleton_mesh_layer_3("MR medial",this, "Medrep skeletal mesh", false);
	add_layer(sk_mesh_layer);
	GL_draw_layer_3* sk_wf_layer = new Medrep_skeleton_mesh_layer_3("MR medial wireframe",this, "Medrep skeletal mesh", true);
	add_layer(sk_wf_layer);
	
	GL_draw_layer_3* rc_mesh_layer = new Medrep_reconstruction_mesh_layer_3("MR reconstruct",this, "Medrep reconstruction based on the current skeleton mesh", false);
	add_layer(rc_mesh_layer);
	GL_draw_layer_3* rc_wf_layer = new Medrep_reconstruction_mesh_layer_3("MR reconstruct wireframe",this, "Medrep reconstruction wireframe based on the current skeleton mesh", true);
	add_layer(rc_wf_layer);

	
}

Medrep_view_3::~Medrep_view_3() {
	if (medrep) delete medrep;
}

void Medrep_view_3::load_mesh_from_medrep_entry(const QString& medrep_setting_name) {
	if (medrep ==0 || ! medrep->contains(medrep_setting_name)) return;
	QString input_file_name = medrep->value(medrep_setting_name).toString();
	input_file_name = QObject::tr("%1/%2").arg(out_directory).arg(input_file_name);

	GLMesh mesh;
	loaded_meshes[medrep_setting_name.toStdString()] = GLMesh();

	loaded_meshes[medrep_setting_name.toStdString()].import_from_file(input_file_name);
}

void Medrep_view_3::load_generic_file(const std::string& file_name) {
	std::cout << "Medrep_view loading " << file_name << std::endl;
	QString medrep_filename(file_name.c_str());
	medrep = new QSettings(medrep_filename, QSettings::IniFormat);
	out_directory = medrep_filename;
	out_directory.truncate(out_directory.lastIndexOf('/'));

	load_mesh_from_medrep_entry("input");
	//QString input_file_name = medrep->value("input").toString();
	//input_file_name = QObject::tr("%1/%2").arg(out_directory).arg(input_file_name);

	//input_mesh.import_from_file(input_file_name);
//	std::cout << "Input file: " << input_file_name.toStdString() << std::endl;

//	QString scale_1_1_mesh_file_name = medrep->value("scale-1.1-mesh").toString();
//	scale_1_1_mesh_file_name = QObject::tr("%1/%2").arg(out_directory).arg(scale_1_1_mesh_file_name);
//	std::cout << "1.1 scale axis file: " << scale_1_1_mesh_file_name.toStdString() << std::endl;

	double m2b2m = medrep->value("m2b2m").toDouble();
	add_variable("mesh-2-ball-2-mesh conversion precision", m2b2m);

	QStringList all_keys = medrep->allKeys(); medial_meshes.clear();
	Application_settings::remove_settings_ending_with("-mesh");
	QStringList::iterator s_it, s_end = all_keys.end();
	for (s_it = all_keys.begin(); s_it!=s_end; ++s_it) {
		QString key = *s_it;
		if (key.endsWith("-mesh") && key!="volume-mesh") {
			Application_settings::add_bool_setting(key.toStdString().c_str(), false);
			medial_meshes.push_back(key.toStdString());
//			add_variable(key.toStdString().c_str(), "");
		}
	}

	invalidate_cache();

	std::list<std::string>::iterator ms_it, ms_end = medial_meshes.end();
	for (ms_it=medial_meshes.begin(); ms_it!=ms_end; ++ms_it) {
		QString sn(ms_it->c_str());
		if (Application_settings::get_bool_setting(sn)) {
			active_skeleton_mesh = sn;
			//active_reconstruction_mesh = active_skeleton_mesh.left(active_skeleton_mesh.lastIndexOf("-"));
			//active_reconstruction_mesh = tr("%1-balls-surface").arg(active_reconstruction_mesh);
			//std::cout << "active_reconstruction_mesh " << active_reconstruction_mesh.toStdString() << std::endl;
			application_settings_changed(sn);
			return;
		}
	}

}



void Medrep_view_3::application_settings_changed(const QString& name) {
	if (name.endsWith("-mesh")) {
		std::list<std::string>::iterator s_it, s_end = medial_meshes.end();
		for (s_it=medial_meshes.begin(); s_it!=s_end; ++s_it) {
			QString sn(s_it->c_str());
			if (sn!=name) Application_settings::set_setting(sn, false);
			else  {
				Application_settings::set_setting(sn, true);
				active_skeleton_mesh = sn;
				active_reconstruction_mesh = active_skeleton_mesh.left(active_skeleton_mesh.lastIndexOf("-"));
				active_reconstruction_mesh = tr("%1-balls-surface").arg(active_reconstruction_mesh);
			}
		}
		invalidate_all_layers();
		do_widget_repaint();
	}
}



void Medrep_view_3::invalidate_cache() {
	invalidate_all_layers();
}

GLMesh* Medrep_view_3::get_mesh_from_setting(const std::string& name) {
	if (loaded_meshes.find(name) == loaded_meshes.end()) load_mesh_from_medrep_entry(name.c_str());

	//std::cout << "Vertex count: " << input_mesh.vertex_count << std::endl;
	//std::cout << "Indices count: " << input_mesh.indices_count << std::endl;
	if (loaded_meshes.find(name) == loaded_meshes.end()) return 0;
	return &loaded_meshes[name];
}

GLMesh* Medrep_view_3::get_input_mesh() {
	return get_mesh_from_setting("input");
}

GLMesh* Medrep_view_3::get_skeleton_mesh() {
	return get_mesh_from_setting(active_skeleton_mesh.toStdString());
}

GLMesh* Medrep_view_3::get_reconstruction_mesh() {
	return get_mesh_from_setting(active_reconstruction_mesh.toStdString());
}

bool GLMesh::import_from_file(const QString& file_name) {

	// open file
	QFile f(file_name);
	if ( !f.open( QIODevice::ReadOnly ) ) {
		std::cout << LOG_ERROR << "File " << file_name.toStdString() << " could not be open for reading!" << std::endl;
		return false;
	}

	QString short_filename = file_name.right(file_name.length() - file_name.lastIndexOf("/") - 1);
	std::cout << PROGRESS_STATUS << "Loading " << short_filename.toStdString() << std::endl;

	QTextStream fs( &f );
	QString head; fs >> head;
	QString extension = "OFF";
	if (QString::compare(head, extension)) {
		std::cout << LOG_ERROR << "File " << file_name.toStdString() << " has not correct header!" << std::endl;
		return false;
	}	

	unsigned nv, nt, dummy;// GLfloat x, y, z;
	fs >> nv; fs >> nt; fs >> dummy;
	vertex_count = nv*3;
	indices_count = nt*3;


	if (vertices) delete vertices;
	vertices = new GLfloat[vertex_count]; 
	
	if (indices) delete indices;
	indices = new GLuint[indices_count]; 

	if (face_normals) delete face_normals;
	face_normals = new GLfloat[indices_count];

	for (unsigned i=0; i<nv && !fs.atEnd(); i++) {
		fs >> vertices[3*i];
//		std::cout << vertices[3*i] << std::endl;
		fs >> vertices[3*i+1];
		fs >> vertices[3*i+2];	
	}

	std::set<Ordered_pair> edges;

	for (unsigned i=0; i<nt && !fs.atEnd(); i++) {
		// save indices
		fs >> dummy;
		fs >> indices[3*i];
		fs >> indices[3*i+1];
		fs >> indices[3*i+2];

		// compute normal
		unsigned vertex_index = indices[3*i];
		Point_3 p0 = Point_3(
				vertices[3*vertex_index],
				vertices[3*vertex_index+1],
				vertices[3*vertex_index+2]);
		vertex_index = indices[3*i+1];
		Point_3 p1 = Point_3(
				vertices[3*vertex_index],
				vertices[3*vertex_index+1],
				vertices[3*vertex_index+2]);
		vertex_index = indices[3*i+2];
		Point_3 p2 = Point_3(
				vertices[3*vertex_index],
				vertices[3*vertex_index+1],
				vertices[3*vertex_index+2]);
		Vector_3 normal = cross_product(p1-p0, p2-p1);
		double nl = normal.squared_length();
		if (nl>0) normal = normal / std::sqrt(nl);

		//store normal
		face_normals[3*i] = normal.x();
		face_normals[3*i+1] = normal.y();
		face_normals[3*i+2] = normal.z();
//		std::cout << face_normals[3*i] << std::endl;

		edges.insert(make_ordered_pair(indices[3*i],indices[3*i+1]));
		edges.insert(make_ordered_pair(indices[3*i+2],indices[3*i+1]));
		edges.insert(make_ordered_pair(indices[3*i+2],indices[3*i]));
		
	}

	edge_indices_count = edges.size()*2;

	if (edge_indices) delete edge_indices;
	edge_indices = new GLuint[edge_indices_count];

	std::set<Ordered_pair>::iterator e_it, e_end = edges.end(); int i = 0;
	for (e_it = edges.begin(); e_it!=e_end; ++e_it, ++i) {
		edge_indices[2*i] = e_it->first;
		edge_indices[2*i+1] = e_it->second;
	}


	std::cout << "Loaded " << nv << " vertices and " << nt << " faces from " << file_name.toStdString()<< std::endl;

	f.close();

	std::cout << PROGRESS_DONE << std::endl;


	std::cout << "Size in memory of loaded mesh: " << size_of_data() << std::endl;

	return true;

}

size_t GLMesh::size_of_data() {
	size_t s = 0;
		
	s += sizeof(vertex_count);
	s += vertex_count * sizeof(GLfloat);
	s += sizeof(indices_count);
	s += indices_count * sizeof(GLfloat);
	s += indices_count * sizeof(GLuint);
	s += sizeof(edge_indices_count);
	s += edge_indices_count * sizeof(GLuint);

	return s;
}


