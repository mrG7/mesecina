#include "Mesh_view_3.h"
#include "layers/Mesh_layer_3.h"

#include <CGAL/IO/Polyhedron_iostream.h>

Mesh_view_3::Mesh_view_3() : Geometry() {
	GL_draw_layer_3* mesh_layer = new Mesh_layer_3("Input mesh",this, "Mesh of the file read from the OFF file", false, false);
	add_layer(mesh_layer);
	GL_draw_layer_3* wireframe_layer = new Mesh_layer_3("Input wireframe",this, "Wireframe of the file read from the OFF file", true, false);
	add_layer(wireframe_layer);
	GL_draw_layer_3* point_layer = new Mesh_layer_3("Input vertices",this, "Points in the file read from the OFF file", true, true);
	add_layer(point_layer);
	GL_draw_layer_3* faces_layer = new Mesh_layer_3("Input faces",this, "Faces in the file read from the OFF file", true, false, true);
	add_layer(faces_layer);
}

Mesh_view_3::~Mesh_view_3() {}

Geometry* Mesh_view_3::clone() {
	Mesh_view_3* new_mesh_view = new Mesh_view_3();
	new_mesh_view->p = p;
	std::cout << "Mesh view cloned" << std::endl;
	return new_mesh_view;
}

void Mesh_view_3::load_generic_file(const std::string& filename) {
	if (QString(filename.c_str()).endsWith(".off")) {
		std::ifstream f(filename.c_str());
		p.clear();
		f >> p;
		f.close();
		std::cout << "Mesh_view_3: Polyhedron loaded from " << filename << " and contains " << p.size_of_facets () <<" faces and " << p.size_of_vertices() << " vertices." << std::endl;
		invalidate_cache();
	}
}
void Mesh_view_3::save_generic_file(const std::string& filename) {
	if (QString(filename.c_str()).endsWith(".off")) {
		std::ofstream outfile;
		outfile.open(filename.c_str());
		outfile << p;
		outfile.close();
		std::cout << "Mesh_view_3: File written: " << filename << std::endl;
	}
}

void Mesh_view_3::invalidate_cache() {
	invalidate_all_layers();
}

Mesh_view_3::Polyhedron* Mesh_view_3::get_polyhedron() {
	return &p;
}
