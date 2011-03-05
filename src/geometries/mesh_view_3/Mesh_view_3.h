/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_MESH_VIEW_3_H
#define MESECINA_MESH_VIEW_3_H

#include <geometries/Geometry.h>

#include <list>
#include <vector>
#include <string>

#include <CGAL/AABB_intersections.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_3/Robust_intersection_traits_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Delaunay_triangulation_3.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/refine_mesh_3.h>

#include "Mesh_polar_vertex_base.h"


#ifdef MESECINA_GEOMETRY_LIBRARY
#define MV_API   __declspec( dllexport )
#else
#define MV_API   __declspec( dllimport )
#endif



class MV_API Mesh_view_3 : public Geometry {

public:

	typedef CGAL::Exact_predicates_inexact_constructions_kernel		Kernel;
//	typedef CGAL::Polyhedron_3<Kernel>								Polyhedron;

	typedef CGAL::Mesh_3::Robust_intersection_traits_3<Kernel> Geom_traits;
	typedef CGAL::Polyhedron_3<Geom_traits> Polyhedron;
	typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron, Geom_traits> Mesh_domain;

	typedef  Polyhedron::Traits::Point_3                  Point;
	typedef  Polyhedron::Traits::Vector_3                  Vector_3;

	// Triangulation
	typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
	typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

	// Mesh Criteria
	typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
	typedef Mesh_criteria::Facet_criteria Facet_criteria;
	typedef Mesh_criteria::Cell_criteria Cell_criteria;

	// Mesh polar triangulation
	typedef CGAL::Triangulation_data_structure_3<Mesh_polar_vertex_base<Kernel> >    Tds;
	typedef CGAL::Delaunay_triangulation_3<Kernel, Tds>                      Mesh_polar_triangulation;
	typedef Mesh_polar_triangulation::Vertex_handle                     Vertex_handle;

	Mesh_view_3();
	virtual ~Mesh_view_3();
	virtual Geometry* clone();
	
	//// methods to communicate with other geometries
	//virtual std::list<std::string> offer_structures();
	//virtual void* give_structure(const std::string& name);
	//virtual void receive_structure_changed(const std::string& name);

	//// points communication with user interface
	//virtual void add_point(double x, double y);
	//virtual void add_points(std::list<QPointF>* points);
	//virtual std::list<QPointF>* get_points();

	// directly handle file io
	virtual void load_generic_file(const std::string&);
	virtual void save_generic_file(const std::string&);

	// ball communication with user interface
	//virtual void add_weighted_point(Point3D point);
	//virtual void add_weighted_points(std::list<Point3D>* points);
	//virtual std::list<Point3D>* get_weighted_points();

	// receive application settings changes
	virtual void application_settings_changed(const QString&);
	//// modification for evolutions
	//virtual void apply_modification(const std::string& );

	void invalidate_cache();

	Polyhedron* get_polyhedron();
	std::list<Point4D>* get_mesh_balls();
	C3t3* get_domain_mesh();
	Mesh_polar_triangulation* get_mesh_polar_triangulation();
	std::list<Point4D>* get_mesh_polar_balls();

	void write_mesh_balls(const std::string& file_name, std::list<Point4D>* balls);

	bool is_finite_number(double x);

protected:
	void compute_mesh_polar_triangulation();
	void compute_domain_mesh();
	void collect_balls();

	Polyhedron p; double bounding_radius;
	C3t3 domain_mesh;
	Mesh_polar_triangulation mesh_polar_triangulation;
	bool has_domain_mesh, has_mesh_balls, has_mesh_polar_triangulation;
	std::list<Point4D> mesh_balls, mesh_polar_balls;
};

#endif //MESECINA_MESH_VIEW_3_H