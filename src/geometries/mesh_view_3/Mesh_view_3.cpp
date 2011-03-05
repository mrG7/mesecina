#include "Mesh_view_3.h"

#include "layers/Mesh_layer_3.h"
#include "layers/Domain_mesh_layer_3.h"
#include "layers/Mesh_polar_triangulation_layer_3.h"

#include <gui/app/static/Application_settings.h>

#include <CGAL/IO/Polyhedron_iostream.h>

#include <limits>
typedef std::numeric_limits< double > dbl;

Mesh_view_3::Mesh_view_3() : Geometry(), has_domain_mesh(false), has_mesh_balls(false), has_mesh_polar_triangulation(false) {
	// input surface mesh
	GL_draw_layer_3* mesh_layer = new Mesh_layer_3("Input mesh",this, "Mesh of the file read from the OFF file", false, false);
	add_layer(mesh_layer);
	GL_draw_layer_3* wireframe_layer = new Mesh_layer_3("Input wireframe",this, "Wireframe of the file read from the OFF file", true, false);
	add_layer(wireframe_layer);
	GL_draw_layer_3* point_layer = new Mesh_layer_3("Input vertices",this, "Points in the file read from the OFF file", true, true);
	add_layer(point_layer);
	GL_draw_layer_3* faces_layer = new Mesh_layer_3("Input faces",this, "Faces in the file read from the OFF file", true, false, true);
	add_layer(faces_layer);

	// meshed surface
	GL_draw_layer_3* surf_mesh_layer = new Domain_mesh_layer_3("Surface remesh",this, "Mesh of the surface after volume meshing this shape", false, true);
	add_layer(surf_mesh_layer);
	GL_draw_layer_3* surfw_mesh_layer = new Domain_mesh_layer_3("Surface remesh edges",this, "Wireframe of the surface after volume meshing this shape", true, true);
	add_layer(surfw_mesh_layer);

	GL_draw_layer_3* vol_mesh_layer = new Domain_mesh_layer_3("Volume remesh",this, "Mesh of the volume after volume meshing this shape", false, false);
	add_layer(vol_mesh_layer);
	GL_draw_layer_3* volw_mesh_layer = new Domain_mesh_layer_3("Volume remesh edges",this, "Wireframe of the volume after volume meshing this shape", true, false);
	add_layer(volw_mesh_layer);

	GL_draw_layer_3* ball_mesh_layer = new Domain_mesh_layer_3("Mesh balls",this, "Circumscribed balls of the tets in volume mesh - surface", false, false, true);
	add_layer(ball_mesh_layer);
	GL_draw_layer_3* ballw_mesh_layer = new Domain_mesh_layer_3("Mesh spheres",this, "Circumscribed balls of the tets in volume mesh - wireframe", true, false, true);
	add_layer(ballw_mesh_layer);

	// Mesh polar triangulation
	GL_draw_layer_3* poi_mesh_layer = new Mesh_polar_triangulation_layer_3("Surface points",this, "Surface samples of the volume meshing", false, false, true);
	add_layer(poi_mesh_layer);
	GL_draw_layer_3* sd_mesh_layer = new Mesh_polar_triangulation_layer_3("Surface Delaunay",this, "Delaunay of the surface samples from the volume meshing", true, false, false);
	add_layer(sd_mesh_layer);

	GL_draw_layer_3* mpb_mesh_layer = new Mesh_polar_triangulation_layer_3("Mesh polar balls",this, "Polar balls defined based on mesh", false, true, false);
	add_layer(mpb_mesh_layer);
	



	Application_settings::add_double_setting("meshing-surface-angle", 25);
	Application_settings::add_double_setting("meshing-surface-size", 0.15);
	Application_settings::add_double_setting("meshing-surface-approximation", 0.008);

	Application_settings::add_double_setting("meshing-tet-radius-edge-ratio", 4);
	Application_settings::add_double_setting("meshing-tet-size", 0.1);

	Application_settings::add_bool_setting("ignore-mesh-balls-centered-outside", true);
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
		Polyhedron::Vertex_iterator v_it, v_end = p.vertices_end();
		double minx, miny, minz, maxx, maxy, maxz;
		for (v_it = p.vertices_begin(); v_it!=v_end; ++v_it) {
			double x = v_it->point().x();
			double y = v_it->point().y();
			double z = v_it->point().z();
			if (v_it == p.vertices_begin()) {
				minx = maxx = x;
				miny = maxy = y;
				minz = maxz = z;
			} else {
				if (x < minx) minx = x; if (x > maxx) maxx =x;
				if (y < miny) miny = y; if (y > maxy) maxy =y;
				if (z < minz) minz = z; if (z > maxz) maxz =z;
			}
		}

		std::cout << "Bounding box: (" << minx << "," << miny << "," << minz << ") - (" << maxx << "," << maxy << "," << maxz << ")" << std::endl;
		double dx = maxx-minx, dy = maxy-miny, dz = maxz - minz;
		bounding_radius = sqrt(dx*dx + dy*dy + dz*dz) / 2.0;
		add_variable("Input bounding box radius", bounding_radius);
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

void Mesh_view_3::application_settings_changed(const QString& setting_name) {
	if (setting_name.startsWith("meshing-")) {
		invalidate_cache();
		emit do_widget_repaint();
	}
	if (setting_name == "ignore-mesh-balls-centered-outside") {
		invalidate_all_layers();
		has_mesh_balls = false;
		emit do_widget_repaint();
	}
}

void Mesh_view_3::invalidate_cache() {
	invalidate_all_layers();
	has_domain_mesh = false;
	has_mesh_balls = false;
	has_mesh_polar_triangulation = false;
}

Mesh_view_3::Polyhedron* Mesh_view_3::get_polyhedron() {
	return &p;
}

Mesh_view_3::C3t3* Mesh_view_3::get_domain_mesh() {
	if (!has_domain_mesh) {
		if (!p.empty())
			compute_domain_mesh();
		has_domain_mesh = true;
	}
	return &domain_mesh;
}

std::list<Point4D>* Mesh_view_3::get_mesh_balls() {
	get_domain_mesh();
	if (!has_mesh_balls) {
		collect_balls();
		has_mesh_balls = true;
	}
	return &mesh_balls;
}

Mesh_view_3::Mesh_polar_triangulation* Mesh_view_3::get_mesh_polar_triangulation() {
	get_domain_mesh();
	if (!has_mesh_polar_triangulation) {
		compute_mesh_polar_triangulation();
		has_mesh_polar_triangulation = true;
	}
	return &mesh_polar_triangulation;
}

std::list<Point4D>* Mesh_view_3::get_mesh_polar_balls() {
	get_mesh_polar_triangulation();
	return &mesh_polar_balls;
}

bool Mesh_view_3::is_finite_number(double x) {
	 return (x <= DBL_MAX && x >= -DBL_MAX); 
}


void Mesh_view_3::compute_mesh_polar_triangulation() {
	std::cout << PROGRESS_STATUS << "Delaunay triangulation of the surface points" << std::endl;

	const Tr& t = domain_mesh.triangulation();
	Tr::Finite_vertices_iterator dv_it, dv_end = t.finite_vertices_end();
	for (dv_it = t.finite_vertices_begin(); dv_it!=dv_end; dv_it++) {
		if (domain_mesh.in_dimension(dv_it) == 2)
			mesh_polar_triangulation.insert(dv_it->point());
	}

	std::cout << PROGRESS_DONE << std::endl;

//	std::cout << "starting interior poles " << mesh_polar_triangulation.number_of_cells() << std::endl;
	std::cout << PROGRESS_STATUS << "Finding mesh interior poles" << std::endl;
	Mesh_polar_triangulation::Finite_cells_iterator c_it, c_end = mesh_polar_triangulation.finite_cells_end(); int counter =0;
	for (c_it = mesh_polar_triangulation.finite_cells_begin(); c_it!=c_end; c_it++, counter++) {
//		std::cout << "cell " << counter << std::endl;
		Point center = mesh_polar_triangulation.dual(c_it);
		if (!is_finite_number(center.x()) || !is_finite_number(center.x()) || !is_finite_number(center.x())) {
			std::cout << LOG_WARNING << "couldn't compute a voronoi vertex, ignoring for poles computation" << std::endl;
		} else {
//			std::cout << " dual center: " << center << std::endl;
			Tr::Cell_handle location = domain_mesh.triangulation().locate(center);
			if (domain_mesh.is_in_complex(location)) {
//				std::cout << " inside" << std::endl;
				for (int i=0; i<4; i++) {
//					std::cout << "  vertex " << i << std::endl;
					Mesh_polar_triangulation::Vertex_handle vh = c_it->vertex(i);
					//				std::cout << "vh " << vh->point() << std::endl;
					//				std::cout << "  center: " << center << std::endl;
//					std::cout << "  vh->point: " << vh->point() << std::endl;
					Vector_3 cp = center - vh->point();
//					std::cout << "  cp: " << cp << std::endl;
//					std::cout << "  mesh->pole: " << &(*vh->mesh_pole) << std::endl;
					if (vh->mesh_pole==0 || squared_distance(vh->point(), mesh_polar_triangulation.dual(vh->mesh_pole)) < cp.squared_length()) {
						vh->mesh_pole = c_it;
//						std::cout << "  pole changed" << std::endl;
					} //else { std::cout << "  same pole" << std::endl; }
				}
			} //else { std::cout << " outside" << std::endl; }
		}
	}
//	std::cout << "done poles" << std::endl;

	mesh_polar_balls.clear();
	Mesh_polar_triangulation::Finite_vertices_iterator v_it, v_end = mesh_polar_triangulation.finite_vertices_end(); counter = 0;
	for (v_it=mesh_polar_triangulation.finite_vertices_begin(); v_it!=v_end; v_it++, counter++) {
//		std::cout << "vector " << counter << std::endl;
		if (v_it->mesh_pole != 0) {
			Point c = mesh_polar_triangulation.dual(v_it->mesh_pole);
			double sq_r = squared_distance(c, v_it->mesh_pole->vertex(0)->point());
			mesh_polar_balls.push_back(Point4D(c.x(), c.y(), c.z(), sq_r));
		}
	}

	add_variable("Number of balls in polar mesh", mesh_polar_balls.size());
	std::cout << PROGRESS_DONE << std::endl;

}

void Mesh_view_3::compute_domain_mesh() {
	// Create domain
	std::cout << PROGRESS_STATUS << "Meshing the volume" << std::endl;

	Mesh_domain domain(p);

	// Set mesh criteria
	double surface_angle = Application_settings::get_double_setting("meshing-surface-angle");
	double surface_size = Application_settings::get_double_setting("meshing-surface-size")*bounding_radius;
	double surface_approximation = Application_settings::get_double_setting("meshing-surface-approximation")*bounding_radius;

	double tet_ratio = Application_settings::get_double_setting("meshing-tet-radius-edge-ratio");
	double tet_size = Application_settings::get_double_setting("meshing-tet-size")*bounding_radius;

	Facet_criteria facet_criteria(surface_angle, surface_size, surface_approximation); // angle, size, approximation
	Cell_criteria cell_criteria(tet_ratio, tet_size); // radius-edge ratio, size
	Mesh_criteria criteria(facet_criteria, cell_criteria);

	// Mesh generation
	domain_mesh = CGAL::make_mesh_3<C3t3>(domain, criteria);
	std::cout << PROGRESS_DONE << std::endl;

	add_variable("Number of tets in volume mesh", domain_mesh.number_of_cells());
	add_variable("Number of faces in surface mesh", domain_mesh.number_of_facets());




}

void Mesh_view_3::collect_balls() {
	//Save balls to mesh_balls
	std::cout << PROGRESS_STATUS << "Collecting balls" << std::endl;
	mesh_balls.clear();
	typedef C3t3::Cell_iterator Cell_iterator;
	typedef C3t3::Cell_handle Cell_handle;
	typedef  Polyhedron::Traits::Point_3                  Point;
	Cell_iterator c_it =  domain_mesh.cells_begin(), c_end = domain_mesh.cells_end();
	for (/*f_it = dm->facets_begin()*/; c_it!=c_end; c_it++) {
		Point p0 = c_it->vertex(0)->point();
		Point center = domain_mesh.triangulation().dual(c_it);
		Cell_handle location = domain_mesh.triangulation().locate(center, c_it);
		if (Application_settings::get_bool_setting("ignore-mesh-balls-centered-outside") &&
			!domain_mesh.is_in_complex(location)) {
				//skip
		} else {
			double radius_sq = squared_distance(p0,center);
			mesh_balls.push_back(Point4D(center.x(), center.y(), center.z(), radius_sq));
		}
	}
	std::cout << PROGRESS_DONE << std::endl;

	add_variable("Number of balls in surface mesh", mesh_balls.size());


}

void Mesh_view_3::write_mesh_balls(const std::string& file_name, std::list<Point4D>* balls) {
//		get_mesh_balls();

		std::ofstream myfile;
		myfile.open (file_name.c_str());
		myfile.precision(dbl::digits10);

		// write header
		myfile << "WOFF " << balls->size() << " 0 0" << std::endl;

		// write vertices
		std::list<Point4D>::iterator p_it, p_end = balls->end();
		for (p_it=balls->begin(); p_it!=p_end; ++p_it) {
			myfile << std::fixed << p_it->x << " " 
				   << std::fixed << p_it->y << " " 
				   << std::fixed << p_it->z << " " 
				   << std::fixed << sqrt(p_it->w) << std::endl;
		}
		std::cout << "Written " << balls->size() << " balls." << std::endl;
		myfile.close();
}