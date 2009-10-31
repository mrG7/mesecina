/* This source file is part of Mesecina, a software for visualization and studying of
* the medial axis and related computational geometry structures.
* More info: http://www.agg.ethz.ch/~miklosb/mesecina
* Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
*
* $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
*/

#ifndef MESECINA_MOEBIUS_2_H
#define MESECINA_MOEBIUS_2_H

//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>

#include <CGAL/Regular_triangulation_3.h>
#include <CGAL/Triangulation_data_structure_3.h>
#include <CGAL/Triangulation_vertex_base_3.h>
#include <CGAL/Regular_triangulation_cell_base_3.h>
#include <CGAL/Regular_triangulation_filtered_traits_3.h>

#include <CGAL/Delaunay_triangulation_2.h>

#include <geometries/Geometry.h>

#include <boost/iterator.hpp>
#include <boost/tuple/tuple.hpp>

#include <list>
#include <vector>
#include <string>

class GL_draw_layer_2;

template <class K, class RTV = CGAL::Triangulation_vertex_base_3<K> >
class RT_vertex : public RTV {
public: 
	typedef typename K::Point_2		Point_2;
	Point3D												input_point;
	std::vector<Point_2>					extremal_points;
	Point_2												vanishing_point;
	double												vanishing_date;

	typedef typename RTV::Cell_handle Cell_handle;
	typedef typename RTV::Point Point;

	Point_2 input_center() {
		return Point_2(input_point.x, input_point.y);
	}

	void set_vanishing_date() {
		vanishing_date = multiplicative_distance_to(vanishing_point);
	}

	double multiplicative_distance_to(const Point_2& p) {
		return CGAL::to_double(CGAL::sqrt(CGAL::squared_distance(input_center(), p)))/input_point.z;
	}

	template <class TDS2>
	struct Rebind_TDS {
		typedef typename RTV::template Rebind_TDS<TDS2>::Other		RTV2;
		typedef RT_vertex<K, RTV2>																Other;
	};

	RT_vertex() : RTV() {}
	RT_vertex(const Point & p) : RTV(p) {}
	RT_vertex(const Point & p, Cell_handle c) : RTV(p, c) {}
	RT_vertex(Cell_handle c) : RTV(c) {}


};

template <class K, class RTC = CGAL::Regular_triangulation_cell_base_3<CGAL::Regular_triangulation_euclidean_traits_3<K> > >
class RT_cell : public RTC {
public:
	typedef typename K::Point_3										P3;
	typedef typename K::FT												FT;

	P3																						dual_vertex;
	FT																						dual_value;
	std::map<RT_cell<K, RTC>*, std::vector<P3> >	edge_intersection_points;


	typedef typename RTC::Vertex_handle Vertex_handle;
	typedef typename RTC::Cell_handle Cell_handle;

	template <class TDS2>
	struct Rebind_TDS {
		typedef typename RTC::template Rebind_TDS<TDS2>::Other		RTC2;
		typedef RT_cell<K, RTC2>																	Other;
	};

	RT_cell() : RTC() {}
	RT_cell(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2, Vertex_handle v3) : RTC(v0, v1, v2, v3) {}
	RT_cell(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2, Vertex_handle v3,
					Cell_handle n0, Cell_handle n1, Cell_handle n2, Cell_handle n3) : RTC(v0, v1, v2, v3, n0, n1, n2, n3) {}

};

template <class K> // Kernel used in all computations
class Moebius_2 : public Geometry {
public:
	typedef typename K::Point_3																						Point_3;
	typedef typename K::Point_2																						Point_2;
	typedef typename K::Segment_2																					Segment_2;
	typedef typename K::Triangle_2																					Triangle_2;
	typedef typename K::Vector_3																					Vector_3;
	typedef typename K::Circle_2																					Circle_2;

	typedef typename K::FT																								FT;


	typedef typename CGAL::Regular_triangulation_3<CGAL::Regular_triangulation_euclidean_traits_3<K>, 
		CGAL::Triangulation_data_structure_3<RT_vertex<CGAL::Regular_triangulation_filtered_traits_3<K> >, RT_cell<K> > >		RT_3;

	typedef typename RT_3::Weighted_point																	Weighted_point_3;
	typedef typename RT_3::Cell																						Rtc;
	typedef typename RT_3::Cell_handle																		Cell_handle;
	typedef typename RT_3::Vertex																					Rtv;
	typedef typename RT_3::Vertex_handle																	Vertex_handle;

	typedef typename boost::tuple<Vertex_handle, Vertex_handle, Vertex_handle> V_triple;
	typedef typename std::pair<Point_2, V_triple>													Critical_point_and_hull;

	const RT_3* get_lifted_triangulation();
	const RT_3* get_triangulation_with_vanishing_points();
	const std::vector<Vertex_handle>*  get_handles_sorted_by_date();
	const std::vector<Critical_point_and_hull>*  get_critical_points();
	const std::vector<Critical_point_and_hull>*  get_moebius_vertices();

	//RT_3* get_critical_points();
	struct Critical_date_comparison {
		bool operator() (const Critical_point_and_hull& a, const Critical_point_and_hull& b) {
			return a.second.get<0>()->multiplicative_distance_to(a.first) < b.second.get<0>()->multiplicative_distance_to(b.first);
		}
	};

	struct Date_comparison {
		bool operator() (const Vertex_handle& a, const Vertex_handle& b) {
			if(a->extremal_points.empty() && b->extremal_points.empty())
				return &(*a) < &(*b);
			if(a->extremal_points.empty())
				return false;
			if(b->extremal_points.empty())
				return true;
			return a->vanishing_date < b->vanishing_date;
		}
	};

	template<class Iterator>
	class Iterator_wrapper: public boost::iterator_facade< Iterator_wrapper <Iterator>,
		Iterator,
		boost::forward_traversal_tag,
		Iterator> // & omitted on purpose 
	{
		Iterator it;
	public:
		Iterator_wrapper(): it() {}
		explicit Iterator_wrapper(Iterator *c): it(*c) {}
		Iterator_wrapper(Iterator _it): it(_it) {}

	private:
		friend class boost::iterator_core_access;
		void increment() {++it;}
		bool equal(const Iterator_wrapper& other) const {
			return it == other.it;
		}
		Iterator dereference() const {return it;}
	};

	Moebius_2();
	virtual ~Moebius_2();
	//virtual Geometry* clone();

	// methods to communicate with other geometries
	virtual std::list<std::string> offer_structures();
	virtual void* give_structure(const std::string& name);
	virtual void receive_structure_changed(const std::string& name);

	// points communication with user interface
	//virtual void add_point(double x, double y);
	//virtual void add_points(std::list<QPointF>* points);
	//virtual std::list<QPointF>* get_points();

	// ball communication with user interface
	virtual void add_weighted_point(const Point3D& point);
	virtual void add_weighted_points(const std::list<Point3D>* points);
	virtual std::list<Point3D>* get_weighted_points();

	void clear_rt_3();
	void post_process();
	void sort_critical_points();
	bool enough_spheres() const;
	static Weighted_point_3 lift(const Point3D& point);
	void compute_vanishing_points();
	void compute_facets_and_aggregate();
	void compute_handles_sorted_by_date();
	void compute_critical_points();
	void check_moebius_vertices(Cell_handle a);
    static V_triple common_facet(Rtc* a, Rtc* b);
	static bool is_inside_triangle(const Point_2& mv, const V_triple& facet);
	static bool is_inside_convex_hull(const Point3D& p, const std::vector<Point3D>& hull);
	bool compute_facets_around(typename RT_3::Finite_vertices_iterator a);
	void aggregate(typename RT_3::Finite_vertices_iterator a);
	static Point_2 furthest_point(const Point_2& source, const std::vector<Point_2>& targets);
	static void transfer_points(const std::vector<Point_3>& points3, std::vector<Point_2>& points2);
	bool closer_to_edge(const Point_2& p, const typename RT_3::Edge& e) const;
	static typename RT_3::Vertex_handle third_vertex(const typename RT_3::Facet& f, const typename RT_3::Edge& e);
	static Point_2 extrem_on_bisector(const Point3D& a, const Point3D& n);
	static Point_2 contact_on_bisector(const Point3D& a, const Point3D& n);
	void compute_cells_values();
	void compute_cell_value(typename RT_3::Finite_cells_iterator a);
	void compute_edges_values();
	void compute_edges_around(typename RT_3::Finite_cells_iterator a);
	std::vector<Point_3> intersection_points(typename RT_3::Cell_handle a, typename RT_3::Cell_handle b) const;
	static std::vector<Point_3> solve_segment_intersection(const Point_3& a, const Point_3& b);
	static std::vector<Point_3> solve_ray_intersection(const Point_3& a, const Point_3& b);



	// receive application settings changes
	virtual void application_settings_changed(const QString&);
	// modification for evolutions
	//virtual void apply_modification(const std::string& );

	void invalidate_cache();

	bool has_balls;
	bool has_vanishing_points;
	bool has_critical_points;
	bool has_handles_sorted_by_date;

	std::list<GL_draw_layer_2*> scaled_balls_layers;
	void invalidate_scaled_balls_layer();

	std::list<Point3D> input_points;
	std::list<Point3D> grown_input_points;
	std::vector<Vertex_handle> handles_sorted_by_date;
	std::vector<Critical_point_and_hull> moebius_vertices;
	std::vector<Critical_point_and_hull> critical_points;
	RT_3 rt_3;
};




#endif //MESECINA_DUMMY_2_H