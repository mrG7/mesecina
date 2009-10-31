/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_DUMMY_3_H
#define MESECINA_DUMMY_3_H

#include <geometries/Geometry.h>

#include <list>
#include <vector>
#include <string>

#include <CGAL/Union_of_balls_3.h>
#include <CGAL/Skin_surface_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/mesh_skin_surface_3.h>
#include <CGAL/subdivide_skin_surface_mesh_3.h>

// Adaptor for Polyhedron_3
#include <CGAL/Surface_mesh_simplification/HalfedgeGraph_Polyhedron_3.h>

// Simplification function
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>

// Stop-condition policy
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_stop_predicate.h>



template <class K> // Kernel used in all computations
class Skin_surface_3 : public Geometry {

public:

	typedef typename CGAL::Skin_surface_traits_3<K>                              Skin_traits;
	typedef typename CGAL::Union_of_balls_3<Skin_traits>                        Skin_union_of_balls_3;
	typedef typename Skin_union_of_balls_3::Weighted_point                      Skin_weighted_point;
	typedef typename Skin_weighted_point::Point                              Skin_bare_point;
	typedef typename CGAL::Polyhedron_3<K,
		CGAL::Skin_surface_polyhedral_items_3<Skin_union_of_balls_3> >   Polyhedron;


	Skin_surface_3(std::string b_source);
	virtual ~Skin_surface_3();
	virtual Geometry* clone();
	
	//// methods to communicate with other geometries
	//virtual std::list<std::string> offer_structures();
	//virtual void* give_structure(const std::string& name);
	virtual void receive_structure_changed(const std::string& name);
	
	//// points communication with user interface
	//virtual void add_points(std::list<Point3D>*);
	//virtual std::list<Point3D>* get_points();

	//// ball communication with user interface
	//virtual void add_weighted_points(std::list<Point4D>*);
	//virtual std::list<Point4D>* get_weighted_points();

	//// directly handle file io
	//virtual void load_generic_file(const std::string&);
	//virtual void save_generic_file(const std::string&);

	// receive application settings changes
	virtual void application_settings_changed(const QString&);
	// modification for evolutions
	//virtual void apply_modification(const std::string& );

	void invalidate_cache();

	Polyhedron* get_skin_surface();

private:
	Polyhedron skin_surface;
	bool has_skin_surface;
	std::string ball_source;
};

#endif //MESECINA_DUMMY_3_H