/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_triangulation_layer_3.h 373 2008-07-11 02:20:13Z miklosb $
 */

#ifndef MESECINA_POISSON_SURFACE_LAYER_3_H
#define MESECINA_POISSON_SURFACE_LAYER_3_H

#pragma comment(lib,"psapi.lib")

#include <gui/gl_widget_3/GL_draw_layer_3.h>
#include <gui/app/static/Application_settings.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>
#include <CGAL/IO/output_surface_facets_to_polyhedron.h>
#include <CGAL/Poisson_reconstruction_function.h>
#include <CGAL/Point_with_normal_3.h>
#include <CGAL/property_map.h>
#include <CGAL/IO/read_xyz_points.h>
#include <CGAL/compute_average_spacing.h>



template <class Union_of_balls_3>
class Poisson_surface_layer_3 : public GL_draw_layer_3 {
	typedef typename Union_of_balls_3::Alpha_shape_3 Alpha_shape_3;

	typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
	typedef Kernel::FT FT;
	typedef Kernel::Point_3 Point;
	typedef CGAL::Point_with_normal_3<Kernel> Point_with_normal;
	typedef Kernel::Sphere_3 Sphere;
	typedef std::vector<Point_with_normal> PointList;
	typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
	typedef CGAL::Poisson_reconstruction_function<Kernel> Poisson_reconstruction_function;
	typedef CGAL::Surface_mesh_default_triangulation_3 STr;
	typedef CGAL::Surface_mesh_complex_2_in_triangulation_3<STr> C2t3;
	typedef CGAL::Implicit_surface_3<Kernel, Poisson_reconstruction_function> Surface_3;

public:

	Poisson_surface_layer_3(const QString& name, Union_of_balls_3* m, const QString& ttip, bool wireframe) : GL_draw_layer_3(name, ttip), parent(m) {
		do_wireframe = wireframe;
		Application_settings::add_double_setting("poisson-mesher-bound-angle",20);
		Application_settings::add_double_setting("poisson-mesher-bound-radius-wrt-point-spacing",30);
		Application_settings::add_double_setting("poisson-mesher-bound-distance-wrt-point-spacing",0.375);
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
//		Alpha_shape_3* alpha_shape = parent->get_alpha_shape();

		FT sm_angle = Application_settings::get_double_setting("poisson-mesher-bound-angle");
		FT sm_radius = Application_settings::get_double_setting("poisson-mesher-bound-radius-wrt-point-spacing");
		FT sm_distance = Application_settings::get_double_setting("poisson-mesher-bound-distance-wrt-point-spacing");

		// Reads the point set file in points[].
		// Note: read_xyz_points_and_normals() requires an iterator over points
		// + property maps to access each point's position and normal.
		// The position property map can be omitted here as we use iterators over Point_3 elements.
		PointList points;
		std::ifstream stream("c:\\data\\kitten.xyz");
		if (!stream ||
			!CGAL::read_xyz_points_and_normals(
			stream,
			std::back_inserter(points),
			CGAL::make_normal_of_point_with_normal_pmap(std::back_inserter(points))))
		{
			std::cout << "Error: cannot read file data/kitten.xyz" << std::endl;
			return;
		}
		std::cout << "Loaded data\\kitten.xyz" << std::endl;

		// Creates implicit function from the read points.
		// Note: this method requires an iterator over points
		// + property maps to access each point's position and normal.
		// The position property map can be omitted here as we use iterators over Point_3 elements.
		Poisson_reconstruction_function function(
			points.begin(), points.end(),
			CGAL::make_normal_of_point_with_normal_pmap(points.begin()));

		// Computes the Poisson indicator function f()
		// at each vertex of the triangulation.
		if ( ! function.compute_implicit_function() )
			return;

		std::cout << "Computed indicator function" << std::endl;

		// Computes average spacing
		FT average_spacing = CGAL::compute_average_spacing(points.begin(), points.end(),
			6 /* knn = 1 ring */);

		// Gets one point inside the implicit surface
		// and computes implicit function bounding sphere radius.
		Point inner_point = function.get_inner_point();
		Sphere bsphere = function.bounding_sphere();
		FT radius = std::sqrt(bsphere.squared_radius());

		// Defines the implicit surface: requires defining a
		// conservative bounding sphere centered at inner point.
		FT sm_sphere_radius = 5.0 * radius;
		FT sm_dichotomy_error = sm_distance*average_spacing/1000.0; // Dichotomy error must be << sm_distance
		Surface_3 surface(function,
			Sphere(inner_point,sm_sphere_radius*sm_sphere_radius),
			sm_dichotomy_error/sm_sphere_radius);

		// Defines surface mesh generation criteria
		CGAL::Surface_mesh_default_criteria_3<STr> criteria(sm_angle,  // Min triangle angle (degrees)
			sm_radius*average_spacing,  // Max triangle size
			sm_distance*average_spacing); // Approximation error

		// Generates surface mesh with manifold option
		STr tr; // 3D Delaunay triangulation for surface mesh generation
		C2t3 c2t3(tr); // 2D complex in 3D Delaunay triangulation
		CGAL::make_surface_mesh(c2t3,                                 // reconstructed mesh
			surface,                              // implicit surface
			criteria,                             // meshing criteria
			CGAL::Manifold_with_boundary_tag());  // require manifold mesh
		std::cout << "Mesh generated: " << tr.number_of_vertices() << std::endl;

		if(tr.number_of_vertices() == 0)
			return;

		// saves reconstructed surface mesh
		std::ofstream out("c:\\data\\kitten_poisson-20-30-0.375.off");
		if (!out) std::cout << LOG_ERROR << "Could not open file to write results" << std::endl;
		Polyhedron output_mesh;
		CGAL::output_surface_facets_to_polyhedron(c2t3, output_mesh);
		*this << RENDER_FILL;
		*this << output_mesh;
//		std::cout << "Mesh written" << std::endl;



		//if (alpha_shape->number_of_vertices() < 4) return;

		//double angle = Application_settings::get_double_setting("implicit-mesher-bound-angle"); angle = angle>30 ? 30 : angle;
		//double radius = Application_settings::get_double_setting("implicit-mesher-bound-radius");
		//double distance = Application_settings::get_double_setting("implicit-mesher-bound-distance");

		//std::cout << "Meshing implicit with angle: " << angle << " radius: " << radius << " distance: " << distance << std::endl;

		//Tr tr;            // 3D-Delaunay triangulation
		//C2t3 c2t3 (tr);   // 2D-complex in 3D-Delaunay triangulation

		//std::cout << PROGRESS_STATUS << "Implicit meshing" << std::endl;
		//// defining the surface
		//Union_surface<GT, Union_of_balls_3> surface(alpha_shape, Sphere_3(CGAL::ORIGIN, 2.));
		////Surface_3 surface(surface_function, Sphere_3(CGAL::ORIGIN, 2.));

		//// defining meshing criteria
		//CGAL::Surface_mesh_default_criteria_3<Tr> criteria(angle, radius, distance);

		//// meshing surface
		//CGAL::make_surface_mesh(c2t3, surface, criteria, CGAL::Non_manifold_tag());
		//std::cout << PROGRESS_DONE << std::endl;

		//*this << DO_BOUNDING;
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

	virtual void application_settings_changed(const QString& settings_name) {

		if (settings_name.startsWith("poisson-mesher-bound")) {
			invalidate_cache();
			widget->request_repaint();
		}
	}

private:
	Union_of_balls_3 *parent;
	bool do_wireframe;
//	M_classification classification;
};


#endif //MESECINA_POISSON_SURFACE_LAYER_3_H





