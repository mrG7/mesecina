/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_SCALE_ANALYSIS_2_H
#define MESECINA_SCALE_ANALYSIS_2_H

#include <geometries/Geometry.h>

#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <list>
#include <vector>
#include <string>

#ifdef MESECINA_GEOMETRY_LIBRARY
#define MV_API   __declspec( dllexport )
#else
#define MV_API   __declspec( dllimport )
#endif



class MV_API Scale_analysis_2 : public Geometry {
public:
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

	typedef CGAL::Triangulation_face_base_with_info_2<double, K>		Fb;
	typedef CGAL::Triangulation_data_structure_2<CGAL::Triangulation_vertex_base_2<K>,Fb>   Tds;
	typedef CGAL::Delaunay_triangulation_2<K, Tds>                      Delaunay_with_scale;
	typedef Delaunay_with_scale::Finite_faces_iterator					Finite_faces_iterator;

	typedef K::Point_2													Point_2;
	typedef K::Circle_2													Circle_2;


	Scale_analysis_2();
	virtual ~Scale_analysis_2();
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

	// directly handle file io
	//virtual void load_generic_file(const std::string&);
	//virtual void save_generic_file(const std::string&);

	// receive application settings changes
//	virtual void application_settings_changed(const QString&);
	// modification for evolutions
//	virtual void apply_modification(const std::string& );

	Delaunay_with_scale* get_delaunay();
	Delaunay_with_scale* get_sa_balls_delaunay();
	Delaunay_with_scale* get_sa_ordered_balls_delaunay();

	void invalidate_cache();

private:
	bool has_points;
	bool has_balls;
	bool has_ordered_balls;
	Delaunay_with_scale delaunay;
};

#endif //MESECINA_SCALE_ANALYSIS_2_H