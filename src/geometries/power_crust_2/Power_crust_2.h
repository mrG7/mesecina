/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Power_crust_2.h 531 2009-02-05 22:02:06Z miklosb $
 */

#ifndef MESECINA_POWER_CRUST_2_H
#define MESECINA_POWER_CRUST_2_H

#include <geometries/Geometry.h>



#include <list>
#include <vector>

template <class K>
class Classified_triangulation_2;

template <class K, class NT>
class Polar_triangulation_2;

template <class K> // Numeric type used in all computations
class Power_crust_2 : public Geometry {


private:
	typedef typename K::FT												NT;


public:
	// delaunay triangulation
	typedef Classified_triangulation_2<K>				   Triangulation_2;
	typedef Polar_triangulation_2<K,NT>				 Polar_triangulation_2;

	typedef typename K::Point_2									   Point_2;
	typedef typename K::Vector_2                                  Vector_2;
	typedef typename K::Circle_2                                  Circle_2;
	typedef typename K::Triangle_2                              Triangle_2;
	typedef typename K::Segment_2                                Segment_2;
	typedef typename K::Line_2										Line_2;
	typedef typename K::Ray_2										 Ray_2;
	typedef typename Triangulation_2::Finite_vertices_iterator Finite_vertices_iterator;

public:
	// the Geometry interface
	Power_crust_2();

	virtual ~Power_crust_2();
	
	virtual Geometry* clone();
	
	virtual std::list<std::string> offer_structures();
	virtual void* give_structure(const std::string&);
	
	virtual void add_point(double x, double y);
	virtual void add_points(std::list<QPointF>* points);
	virtual std::list<QPointF>* get_points();

	void invalidate_cache();

	Triangulation_2* get_input_triangulation();
	Polar_triangulation_2* get_polar_triangulation();
	Polar_triangulation_2* get_classified_polar_triangulation();
	virtual void apply_modification(const std::string& );

	Triangulation_2* get_crust_classified_triangulation();
	Triangulation_2* get_lambda_angle_classified_triangulation();

	std::list<Point_2>* get_point_list();

//private:
	Triangulation_2 input_triangulation;
	Polar_triangulation_2 polar_triangulation;
	bool has_polar_triangulation;

	std::list<QPointF> points;
	std::list<Point_2> point_list;
};


#endif //MESECINA_POWER_CRUST_2_H