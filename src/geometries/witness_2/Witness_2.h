/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_WITNESS_2_H
#define MESECINA_WITNESS_2_H

#include <geometries/Geometry.h>

#include <list>
#include <vector>
#include <string>

template <class K, int N = 2>
class Witness_landmark_set_2;


template <class K> // Kernel used in all computations
class Witness_2 : public Geometry {

public:
	typedef Witness_landmark_set_2<K>								Witness_landmark_set;
	typedef typename Witness_landmark_set::Witness_triangulation	Witness_triangulation;
	typedef typename K::Point_2										Point_2;
	typedef typename K::Vector_2									Vector_2;
	typedef typename K::FT											Coord_type;
	typedef typename Witness_landmark_set::Witness_iterator			Witness_iterator;
	typedef typename Witness_landmark_set::Landmark_list			Landmark_list;
	typedef typename Witness_landmark_set::Witness_handle			Witness_handle;


	
	typedef typename K::Segment_2									Segment_2;
	typedef typename K::Line_2										Line_2;
	typedef typename K::Ray_2										Ray_2;

	typedef typename Witness_landmark_set::Witness_complex_triangulation	Witness_complex_triangulation;


	Witness_2();
	virtual ~Witness_2();
	virtual Geometry* clone();
	
	// methods to communicate with other geometries
	virtual std::list<std::string> offer_structures();
	virtual void* give_structure(const std::string& name);
	virtual void receive_structure_changed(const std::string& name);
	
	//// points communication with user interface
	//virtual void add_point(double x, double y);
	//virtual void add_points(std::list<QPointF>* points);
	//virtual std::list<QPointF>* get_points();

	//// ball communication with user interface
	//virtual void add_weighted_point(Point3D point);
	//virtual void add_weighted_points(std::list<Point3D>* points);
	//virtual std::list<Point3D>* get_weighted_points();

	// receive application settings changes
	virtual void application_settings_changed(const QString&);
	// modification for evolutions
	virtual void apply_modification(const std::string& );

	Witness_landmark_set* get_witness_landmark_set();
	Witness_complex_triangulation* get_witness_complex_triangulation();

	void invalidate_cache();

private:
//	std::list<Vertex_handle> inserted_landmarks;
	Witness_landmark_set witness_landmark_set;
	//std::list<QPointF> points;
	bool has_points;

	//std::list<Point_2> witness_point_list;
};

#endif //MESECINA_WITNESS_2_H