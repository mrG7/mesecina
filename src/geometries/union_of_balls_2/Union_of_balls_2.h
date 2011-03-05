/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_UNION_OF_BALLS_2_H
#define MESECINA_UNION_OF_BALLS_2_H

//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>

#include <geometries/Geometry.h>

#include <list>
#include <vector>
#include <string>

template <class K, class NT>
class Ball_triangulation_2;

template <class K, class Rtds>
class V_triangulation_2;


template <class K> // Numeric type used in all computations
class Union_of_balls_2 : public Geometry {

//public typedefs
public:

	typedef typename K::FT												NT;
	typedef Ball_triangulation_2<K,NT>									  Ball_triangulation_2;
	typedef typename Ball_triangulation_2::Triangulation_data_structure				  Rtds;
	typedef V_triangulation_2<K, Rtds>										 V_triangulation_2;

	typedef typename Ball_triangulation_2::Weighted_point   Weighted_point;
	
	typedef typename K::Point_2									   Point_2;
	typedef typename K::Vector_2                                  Vector_2;
	typedef typename K::Line_2										Line_2;
	typedef typename K::Circle_2                                  Circle_2;
	typedef typename K::Triangle_2                              Triangle_2;
	typedef typename K::Segment_2                                Segment_2;
	typedef typename K::Ray_2										 Ray_2;

	typedef typename Ball_triangulation_2::Finite_edges_iterator Finite_edges_iterator;
	typedef typename Ball_triangulation_2::Face_handle Face_handle;
	typedef typename Ball_triangulation_2::Edge Edge;

	typedef typename V_triangulation_2::Face_handle V_face_handle;
	
	double medial_ball_intersection_cosinus;


public:
	// the Geometry interface
	Union_of_balls_2(std::string b_source, std::string pre);
	virtual ~Union_of_balls_2();
	virtual Geometry* clone();

	virtual std::list<std::string> offer_structures();
	virtual void* give_structure(const std::string& name);

	void invalidate_cache(bool set_has_ball_triangulation = false);

	virtual void application_settings_changed(const QString& settings_name);
	virtual void apply_modification(const std::string& name);
	// specific to this "Geometry"

	virtual void receive_structure_changed(const std::string&);

	virtual void add_weighted_points(const std::list<Point3D>* points);
	virtual void add_weighted_point(const Point3D& points);
	virtual std::list<Point3D>* get_weighted_points();

	Ball_triangulation_2* get_ball_triangulation();
	std::vector<int>*  get_ball_random_tokens();
	Ball_triangulation_2* get_dual_ball_triangulation();
	V_triangulation_2* get_v_triangulation();
	V_triangulation_2* get_classified_v_triangulation();
	V_triangulation_2* get_flooded_v_triangulation();
	std::list<Circle_2>* get_medial_balls();
	std::list<Circle_2>* get_convex_balls();
	std::list<Circle_2>* get_orig_medial_balls(bool not_compute = false);

	void save_balls(QString file_name,std::list<Point3D>* points);
private:

	void add_medial_balls_between(Point_2& c1, NT& w1, Point_2& c2, NT& w2, Point_2& intersection, std::list<Circle_2>* container = 0);
	void add_medial_balls_between(Point_2& c1, NT& w1, Point_2& c2, NT& w2, std::list<Circle_2>* container = 0);	
	void fill_v_triangulation();
	static bool intersect_ball_ball(const Weighted_point &wp0, const Weighted_point &wp1, Point_2& ip1, Point_2 &ip2);



private:
	Ball_triangulation_2 ball_triangulation;
	V_triangulation_2 v_triangulation;
	std::list<Circle_2> medial_balls, orig_medial_balls, convex_balls;
	std::list<Point3D> orig_points;
	std::list<Point3D> medial_points;
	std::vector<int> random_tokens;
	bool has_ball_triangulation;

	bool has_v_triangulation;
	bool has_medial_balls;
	bool has_convex_balls;

	std::string prefix;
	std::string ball_source; //SHARED_INNER_POLES vs SHARED_INNER_VORONOI_BALLS
};

////typedef CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt Exact_kernel;
//typedef CGAL::Filtered_kernel<CGAL::Simple_cartesian<CGAL::Lazy_exact_nt<CORE::Expr> > > Exact_kernel;
//typedef CGAL::Exact_predicates_inexact_constructions_kernel Double_kernel;

#endif //MESECINA_UNION_OF_BALLS_2_H