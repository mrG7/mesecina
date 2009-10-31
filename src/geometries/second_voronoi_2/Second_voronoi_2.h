/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_SECOND_VORONOI_2_H
#define MESECINA_SECOND_VORONOI_2_H

#include <geometries/Geometry.h>

#include <list>
#include <vector>
#include <string>

#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Regular_triangulation_euclidean_traits_2.h>
#include <CGAL/Regular_triangulation_2.h>



template <class K, class Bt /*= CGAL::Delaunay_triangulation_2< K >*/ > // Kernel used in all computations
class Second_voronoi_2 : public Geometry {

public:
	typedef typename K::Point_2										   Point_2;
	typedef typename K::FT													FT;
	typedef typename K::Segment_2									 Segment_2;
	typedef typename K::Line_2											Line_2;
	typedef typename K::Ray_2											 Ray_2;

	typedef CGAL::Regular_triangulation_euclidean_traits_2<K, FT>		Rt;
	typedef typename Bt										Base_triangulation;
	typedef typename CGAL::Regular_triangulation_2<Rt>			 Triangulation;
	typedef typename Triangulation::Weighted_point				Weighted_point;

	Second_voronoi_2(const std::string& _triangulation_source, const std::string& _layer_name = "2nd Order Voronoi");
	virtual ~Second_voronoi_2();
	//virtual Geometry* clone();
	
	// methods to communicate with other geometries
	//virtual std::list<std::string> offer_structures();
	//virtual void* give_structure(const std::string& name);
	virtual void receive_structure_changed(const std::string& name);
	
	// points communication with user interface
	//virtual void add_point(double x, double y);
	//virtual void add_points(std::list<QPointF>* points);
	//virtual std::list<QPointF>* get_points();

	// ball communication with user interface
	//virtual void add_weighted_point(Point3D point);
	//virtual void add_weighted_points(std::list<Point3D>* points);
	//virtual std::list<Point3D>* get_weighted_points();

	// receive application settings changes
//	virtual void application_settings_changed(const QString&);
	// modification for evolutions
//	virtual void apply_modification(const std::string& );

	void invalidate_cache();

	Triangulation* get_regular_triangulation();

private:
	std::string triangulation_source, layer_name;
	Triangulation regular_tri;
	bool dirty;
	Base_triangulation * base_tri;
};

#endif //MESECINA_SECOND_VORONOI_2_H