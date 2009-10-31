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

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>


#ifdef MESECINA_GEOMETRY_LIBRARY
#define MV_API   __declspec( dllexport )
#else
#define MV_API   __declspec( dllimport )
#endif

class MV_API Mesh_view_3 : public Geometry {

public:

	typedef CGAL::Exact_predicates_inexact_constructions_kernel		Kernel;
	typedef CGAL::Polyhedron_3<Kernel>								Polyhedron;

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
	//virtual void application_settings_changed(const QString&);
	//// modification for evolutions
	//virtual void apply_modification(const std::string& );

	void invalidate_cache();

	Polyhedron* get_polyhedron();

protected:
	Polyhedron p;
};

#endif //MESECINA_MESH_VIEW_3_H