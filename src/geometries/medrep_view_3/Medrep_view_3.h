/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_MEDREP_VIEW_3_H
#define MESECINA_MEDREP_VIEW_3_H

#include <geometries/Geometry.h>

//#include <list>
//#include <vector>
#include <string>
#include <QSettings>
#include <QGLWidget>
//#include <GL/glext.h>
//#include <GL/wglext.h>


#ifdef MESECINA_GEOMETRY_LIBRARY
#define MV_API   __declspec( dllexport )
#else
#define MV_API   __declspec( dllimport )
#endif

typedef CGAL::Exact_predicates_inexact_constructions_kernel Ker;
typedef Ker::Point_3 Point_3;
typedef Ker::Vector_3 Vector_3;
typedef Ker::Triangle_3 Triangle_3;
typedef Ker::Segment_3 Segment_3;

struct GLMesh {
	GLfloat* vertices; GLsizei vertex_count;
	GLuint* indices; GLsizei indices_count;
	GLuint* edge_indices; GLsizei edge_indices_count;
	GLfloat* face_normals; 

	GLMesh() {
		vertices = 0; vertex_count = 0;
		indices = 0; indices_count = 0;
		edge_indices = 0; edge_indices_count = 0;
		face_normals = 0;
	}

	bool import_from_file(const QString& file_name);

	~GLMesh() {
		if (vertices) delete vertices;
		if (indices) delete indices;
		if (edge_indices) delete edge_indices;
		if (face_normals) delete face_normals;
	}

	size_t size_of_data();
};

typedef std::pair<int,int> Ordered_pair;

Ordered_pair make_ordered_pair(int a, int b) {
	if (a>b) std::swap(a,b);
	return Ordered_pair(a,b);
}



class MV_API Medrep_view_3 : public Geometry {

public:
	Medrep_view_3();
	virtual ~Medrep_view_3();
//	virtual Geometry* clone();
	
	// methods to communicate with other geometries
	//virtual std::list<std::string> offer_structures();
	//virtual void* give_structure(const std::string& name);
	//virtual void receive_structure_changed(const std::string& name);
	
	// points communication with user interface
	//virtual void add_points(std::list<Point3D>*);
	//virtual std::list<Point3D>* get_points();

	// ball communication with user interface
	//virtual void add_weighted_points(std::list<Point4D>*);
	//virtual std::list<Point4D>* get_weighted_points();

	// directly handle file io
	virtual void load_generic_file(const std::string&);
	void load_mesh_from_medrep_entry(const QString& medrep_setting_name);
//	virtual void save_generic_file(const std::string&);

	// receive application settings changes
	virtual void application_settings_changed(const QString&);
	// modification for evolutions
	//virtual void apply_modification(const std::string& );

	void invalidate_cache();
	GLMesh* get_mesh_from_setting(const std::string& name);
	GLMesh* get_input_mesh();
	GLMesh* get_skeleton_mesh();
	GLMesh* get_reconstruction_mesh();

private:

	QString out_directory;
	QString active_skeleton_mesh, active_reconstruction_mesh;
	QSettings* medrep;
	std::list<std::string> medial_meshes;
	std::map<std::string, GLMesh> loaded_meshes;
//	GLMesh input_mesh;
//	GLMesh medial_mesh;
};

#endif //MESECINA_MEDREP_VIEW_3_H