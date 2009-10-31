/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_POWER_CRUST_3_H
#define MESECINA_POWER_CRUST_3_H

#include <geometries/Geometry.h>

#include "Classified_triangulation_3.h"
#include "Polar_triangulation_3.h"

#include <list>
#include <vector>
#include <string>


template <class K> // Kernel used in all computations
class Power_crust_3 : public Geometry {


public:
	typedef typename K::FT													NT;
	typedef Polar_triangulation_3<K,NT>							Polar_triangulation_3;
	typedef typename Polar_triangulation_3::Vertex_handle		Polar_vertex_handle;
	typedef std::vector<Polar_vertex_handle>						Polar_vertex_vector;
	typedef typename Polar_vertex_vector::iterator					Polar_vertex_vector_iterator;
	typedef Classified_triangulation_3<K,Polar_vertex_handle>		Triangulation_3;
	typedef typename Triangulation_3::Vertex_handle					Vertex_handle;
	typedef typename Triangulation_3::Cell_handle					Classified_cell_handle;
	typedef typename Triangulation_3::Cell					Classified_cell;
	typedef typename K::Point_3									   Point_3;
	typedef typename K::Vector_3                                  Vector_3;
	typedef typename K::Triangle_3                              Triangle_3;
	typedef typename K::Segment_3                                Segment_3;
	typedef typename K::Line_3										Line_3;
	typedef typename K::Ray_3										 Ray_3;
	typedef typename K::Sphere_3								  Sphere_3;
	typedef typename Triangulation_3::Finite_vertices_iterator Finite_vertices_iterator;

	Power_crust_3();
	virtual ~Power_crust_3();
	virtual Geometry* clone();
	
	//// methods to communicate with other geometries
	virtual std::list<std::string> offer_structures();
	virtual void* give_structure(const std::string& name);
	//virtual void receive_structure_changed(const std::string& name);
	
	// points communication with user interface
	virtual void add_points(std::list<Point3D>*);
	virtual std::list<Point3D>* get_points();

	//// ball communication with user interface
	//virtual void add_weighted_points(std::list<Point4D>*);
	//virtual std::list<Point4D>* get_weighted_points();

	//// directly handle file io
	//virtual void load_generic_file(const std::string&);
	//virtual void save_generic_file(const std::string&);

	//// receive application settings changes
	//virtual void application_settings_changed(const QString&);
	//// modification for evolutions
	//virtual void apply_modification(const std::string& );

	void invalidate_cache();

	Triangulation_3* get_input_triangulation();
	Triangulation_3* get_pole_classified_triangulation();
	Polar_triangulation_3* get_polar_triangulation();
	Polar_triangulation_3* get_classified_polar_triangulation();

	void label_poles_improved_with_heuristic();
	Polar_vertex_vector_iterator top_polar_vertex(Polar_vertex_vector* vec);

private:
	Triangulation_3 input_triangulation;
	Polar_triangulation_3 polar_triangulation;
	bool has_polar_triangulation;

	std::list<Point3D> input_points;
};

#endif //MESECINA_POWER_CRUST_3_H