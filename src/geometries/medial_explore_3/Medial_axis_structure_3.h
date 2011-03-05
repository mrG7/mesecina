/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_cell_base_3.h 378 2008-08-03 18:50:59Z miklosb $
 */

#ifndef MESECINA_MEDIAL_AXIS_STRUCTURE_3
#define MESECINA_MEDIAL_AXIS_STRUCTURE_3

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Delaunay_triangulation_3.h>

#include <string>

#include <set>
#include <list>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#ifdef MESECINA_GEOMETRY_LIBRARY
#define MV_API   __declspec( dllexport )
#else
#define MV_API   __declspec( dllimport )
#endif

namespace medial {

	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	
	typedef K::Point_3 Point_3;
	typedef K::Vector_3 Vector_3;
	typedef K::Line_3 Line_3;
	typedef K::Segment_3 Segment_3;
	typedef K::Sphere_3 Sphere_3;
	typedef std::pair<Point_3,double> Ball;

	class Face;
	class Vertex;
	struct Triangle {
		Vertex* p[3];
		
		double angle, lambda;
		Point_3 intersection_point;
	};

	typedef std::map<Triangle*, Face*> Triangle_to_face_map;

	typedef CGAL::Triangulation_vertex_base_with_info_3<std::pair<double, std::set<Triangle*> >, K> Vb;
	typedef CGAL::Triangulation_data_structure_3<Vb>                    Tds;
	typedef CGAL::Delaunay_triangulation_3<K, Tds>                      Delaunay_for_unique;

	typedef Delaunay_for_unique::Vertex_handle Unique_vertex_handle;


	typedef std::vector<Vertex> Vertex_vector;
	typedef Vertex_vector::iterator Vertex_iterator;
	typedef std::vector<Face> Face_vector;
	typedef Face_vector::iterator Face_iterator;

	typedef std::vector<Vertex*> Vertex_pointer_vector;
	typedef Vertex_pointer_vector::iterator Face_vertex_iterator;
	typedef std::vector<Face*> Face_pointer_vector;
	typedef Face_pointer_vector::iterator Face_pointer_iterator;
	typedef std::set<Face*> Face_pointer_set;
	typedef Face_pointer_set::iterator Neighbor_face_iterator;

	typedef std::pair<Vertex*,Vertex*> Edge;
	Edge make_edge(Vertex *v1, Vertex *v2);

	struct Edge_hash : public std::unary_function<Edge, std::size_t> {
		std::size_t operator()(Edge const& e) const {
			std::size_t seed = 0;
			boost::hash_combine(seed, e.first);
			boost::hash_combine(seed, e.second);
			return seed;
		}
	};
	typedef boost::unordered_map<Edge, Face_pointer_set, Edge_hash> Edges_map;
	typedef boost::unordered_map<Vertex*, Face_pointer_set> Vertices_map;
	typedef Edges_map::iterator Edge_iterator;
	typedef boost::unordered_set<Edge> Edge_set;
	typedef Edge_set::iterator Edge_set_iterator;
	typedef boost::unordered_map<Vertex*, Edge_set> Rim_vertices_map;
	typedef boost::unordered_map<Face*, Edge_set> Face_connectivity_map;
	typedef boost::unordered_set<Face*> Face_set;
	typedef boost::unordered_set<Vertex*> Vertex_set;
	typedef std::multimap<double, Face*> Front;
	typedef Front::iterator Front_iterator;
	typedef std::multimap<double, int> Sheet_front;
	typedef Sheet_front::iterator Sheet_front_iterator;
	typedef CGAL::Triple<Vertex*, Vertex*, Point_3> Boundary_neighbor_info;
	typedef std::map<Vertex*, std::pair<Boundary_neighbor_info, Point_3> > Boundary_vertex_map;





	typedef std::map<Vertex*, std::list<Vertex*> > Sheet_boundary;

	class Vertex {
	public:
		Point_3 p;
		double radius_sq;
		Vertex(const Point_3& _p, double _radius_sq);


		double radius();
	}; // Vertex


	class Face {
	public:
		Vector_3 normal;
		unsigned sheet_index;
		Vertex_pointer_vector vertices;

		Face(const Vertex_pointer_vector& _vertices);
		void compute_normal();

		double angle, lambda, scale, top_angle;//, top_scale;
		Point_3 intersection_point;
	}; // Face


	enum Filtering_citeria {CRIT_LAMBDA, CRIT_ANGLE, CRIT_SHEET_TOPANGLE };

	class MV_API Medial_axis_transform_3 {



	public:
		double growth_ratio;
		double bounding_diagonal_radius;
		Vertex_vector vertices;
		Face_vector faces;
		std::vector<Triangle> triangles;
		Triangle_to_face_map triangle_source;

		Edges_map edges;
		Vertices_map vertices_map;
		Face_set visited;
		Rim_vertices_map rim_vertices;
		Front front;
		Delaunay_for_unique scale_axis_balls_delaunay;
		std::vector<Ball> scale_axis_balls;

		Medial_axis_transform_3();
		
		Face_iterator faces_begin();
		Face_iterator faces_end();
		Edge_iterator edges_begin();
		Edge_iterator edges_end();

		bool read_from_off(std::istream& infile);
		bool read_from_off(const char* file_name);
		bool read_from_moff(std::istream& infile);
		bool read_from_moff(const char* file_name);
		void compute_face_normals();

		void compute_vertex_normals();

		bool write_to_off(const char* file_name, std::vector<double>* sheet_stability = 0, double limit = 0);
		bool write_to_obj(const char* file_name, std::vector<double>* sheet_stability = 0, double limit = 0);

		void add_vertex_to_unique_del(Point_3 p, double r, double max_squared_distance, Triangle* source_face);
		void sample_triangle_with_balls(Point_3& p1, double r1, Point_3& p2, double r2, Point_3& p3, double r3, Point_3& bp, double max_squared_distance, double inters_cos, Triangle* source_face);
		bool does_triangle_fulfill_criteria(Triangle* f, Filtering_citeria crit, double limit);
		void fill_balls_with_criteria(Filtering_citeria crit, double limit, std::vector<Ball>& balls);

		int edge_number_of_left_neighbor_faces(const Edge& e);
		bool is_edge_interior(const Edge& e);
		bool is_edge_boundary(const Edge& e);

		int edge_number_of_left_neighbor_sheets(const Edge& e);
		bool is_edge_sheet_interior(const Edge& e);

		bool is_on_sheet_boundary(const Edge& e, int sheet_index, std::set<int>& neighbor_sheets);
		bool is_sheet_on_front(unsigned sheet_index);

		bool find_prev_next_vertex_in_face(Vertex* v, Face* f, Vertex*&prev, Vertex*&next);
		bool vertex_has_cycle_with_face(Vertex *v, Face *f, bool& more_components);
		bool found_cycle_to(Face_connectivity_map& face_connections, Face* current, Face* target, Edge_set& visited);
		void print_connectivity_map(Face_connectivity_map& face_connections, Edge_set& cycle_visited);

		void update_rim_vertex_set(const Edge& e);
		bool is_vertex_interior_alone(Vertex* v);
		bool is_vertex_interior_for_face(Vertex* v, Face *f);
		bool is_front(Face* f);
		void identify_sheets();

		Front_iterator find_in_front(Face* f);
		void add_to_front(Face* f);
		void remove_from_front(Face* f);

		Sheet_front_iterator find_in_sheet_front(int i, double stability);
		void add_to_sheet_front(int i, double stability);
		void remove_from_sheet_front(int i, double stability);

		void compute_topology_angle_filtration();
		void compute_topology_angle_sheet_filtration();
		void sample_balls();
		void add_to_boundary_map(Vertex* source, Vertex* target, Point_3 bp, Boundary_vertex_map& bond_vertex, bool force_out);
		void smooth_medial_surface_boundary(int steps, std::vector<double>* sheet_stability = 0, double limit = 0);

		std::set<int> sheet_visited;
		Sheet_front sheet_front;

		std::vector<Vector_3> vertex_normals;

		std::map<Face*, bool> face_flip;
		std::vector<std::set<Face*> > sheets;
		std::map<int, std::set<int> > sheet_connectivity;
		std::vector<double> sheet_top_angle;

	}; // Structure

} // end of Medial namespace


#endif //MESECINA_MEDIAL_AXIS_STRUCTURE_3