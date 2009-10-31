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

#include <string>

#include <set>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>



namespace medial {

	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	
	typedef K::Point_3 Point_3;
	typedef K::Vector_3 Vector_3;
	typedef K::Segment_3 Segment_3;

	class Face;
	class Vertex;

	typedef std::vector<Vertex> Vertex_vector;
	typedef std::vector<Face> Face_vector;
	typedef Face_vector::iterator Face_iterator;

	typedef std::vector<Vertex*> Vertex_pointer_vector;
	typedef Vertex_pointer_vector::iterator Face_vertex_iterator;
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

	class Vertex {
		Point_3 p;
		double radius_sq;
	public:
		Vertex(const Point_3& _p, double _radius_sq);

		Point_3& point();
		void set_point(const Point_3& mp);

		double radius();
		double squared_radius();
		void set_squared_radius(double rsq);
	}; // Vertex

	class Face {
		Vector_3 normal;
	public:
		Vertex_pointer_vector vertices;

		Face(const Vertex_pointer_vector& _vertices);
		Vector_3 get_normal();
		void compute_normal();
		void set_normal(const Vector_3& v);
		Face_vertex_iterator vertices_begin();
		Face_vertex_iterator vertices_end();

		double angle, lambda, scale, top_angle, top_scale;
	}; // Face

	class Medial_axis_transform_3 {
		Vertex_vector vertices;
		Face_vector faces;
		Edges_map edges;

	public:
		Vertices_map vertices_map;
		Face_set visited;
		Rim_vertices_map rim_vertices;
		Front front;

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

		int edge_number_of_left_neighbor_faces(const Edge& e);
		bool is_edge_interior(const Edge& e);
		bool is_edge_boundary(const Edge& e);

		bool find_prev_next_vertex_in_face(Vertex* v, Face* f, Vertex*&prev, Vertex*&next);
		bool vertex_has_cycle_with_face(Vertex *v, Face *f, bool& more_components);
		bool found_cycle_to(Face_connectivity_map& face_connections, Face* current, Face* target, Edge_set& visited);
		void print_connectivity_map(Face_connectivity_map& face_connections, Edge_set& cycle_visited);

		void update_rim_vertex_set(const Edge& e);
		bool is_vertex_interior_alone(Vertex* v);
		bool is_vertex_interior_for_face(Vertex* v, Face *f);
		bool is_front(Face* f);

		Front_iterator find_in_front(Face* f);
		void add_to_front(Face* f);
		void remove_from_front(Face* f);

		void compute_topology_angle_filtration();

	}; // Structure

} // end of Medial namespace


#endif //MESECINA_MEDIAL_AXIS_STRUCTURE_3