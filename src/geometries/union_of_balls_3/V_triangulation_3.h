/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_triangulation_3.h 379 2008-08-12 17:58:07Z miklosb $
 */

#ifndef MESECINA_V_TRIANGULATION_3_H
#define MESECINA_V_TRIANGULATION_3_H

#include <constants.h>
#include "V_cell_base_3.h"
#include "V_vertex_base_3.h"
#include <CGAL/Weighted_point.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Delaunay_triangulation_3.h>

#include <queue>
#include <utility>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

//#define MAX_FLOAT_DISTANCE_FOR_ZERO 0.000000001
#define MAX_FLOAT_DISTANCE_FOR_ZERO 0
#define PI 3.14159265358979323846

#include <limits>
typedef std::numeric_limits< double > dbl;

template <class Gt, class Polar_tds>
class V_triangulation_3 : public CGAL::Delaunay_triangulation_3 < 
	Gt,
	CGAL::Triangulation_data_structure_3 < 
		V_vertex_base_3<Gt, CGAL::Triangulation_vertex_base_3<Gt>, Polar_tds > ,
		V_cell_base_3<Gt, CGAL::Triangulation_cell_base_with_circumcenter_3<Gt>, Polar_tds > 
	> 
>
{
public:
	typedef typename 
			CGAL::Triangulation_data_structure_3 < 
		V_vertex_base_3<Gt, CGAL::Triangulation_vertex_base_3<Gt>, Polar_tds > ,
		V_cell_base_3<Gt, CGAL::Triangulation_cell_base_with_circumcenter_3<Gt>, Polar_tds > 
	>	Tds;
	typedef typename CGAL::Delaunay_triangulation_3 <Gt,Tds>	Base;
	typedef typename Geom_traits::FT			Coord_type;
	typedef typename Geom_traits::Point_3       Point_3;
	typedef typename CGAL::Weighted_point<Point_3,Coord_type> Weighted_point;
	typedef typename Geom_traits::Vector_3       Vector_3;
	typedef typename Geom_traits::Segment_3     Segment_3;
	typedef typename Geom_traits::Triangle_3    Triangle_3;
	typedef typename Tds::Vertex_handle				Vertex_handle;
	typedef typename Tds::Cell_handle				Cell_handle;
	typedef typename Polar_tds::Facet		Polar_facet;
	typedef typename Polar_tds::Vertex_handle		Polar_vertex_handle;
	typedef typename Polar_tds::Cell_handle		Polar_cell_handle;
	typedef typename V_vertex_base_3<Gt, CGAL::Triangulation_vertex_base_3<Gt>, Polar_tds >::Polar_facet_set Polar_facet_set;

	typedef std::vector<Cell_handle>				Cell_vector;
	typedef typename Cell_vector::iterator			Cell_vector_iterator;
	typedef std::back_insert_iterator<Cell_vector>	Cell_insert_iterator;

	typedef std::set<Polar_vertex_handle>			Balls_set;

	//typedef std::pair<Cell_handle, int>				Pair_edge;
	typedef std::pair<Vertex_handle, Vertex_handle>     Vertex_pair;

	Vertex_pair create_vertex_pair_from_edge(const Edge& e) const {
		Vertex_handle v1 = e.first->vertex(e.second);
		Vertex_handle v2 = e.first->vertex(e.third);
		if ( &(*(v1)) > &(*(v2)) ) {
			std::swap(v1,v2);
		}
		return std::make_pair(v1,v2);
	}

	Edge create_edge_from_vertex_pair(const Vertex_pair& vp) const {
		Cell_handle c; int i,j;
		if (!is_edge(vp.first, vp.second, c,i,j))
			std::cout << LOG_ERROR << "Vertex pair does not correspond to edge from triangulation" << std::endl;
		return Edge(c,i,j);

	}

	struct Vertex_pair_hash : public std::unary_function<Vertex_pair, std::size_t> {
		std::size_t operator()(Vertex_pair const& vp) const {
			std::size_t seed = 0;
//			boost::hash<Vertex*> hasher;
			boost::hash_combine(seed, &(*(vp.first)));
			boost::hash_combine(seed, &(*(vp.second)));
			return seed;
		}
	};

	struct Cell_handle_hash : public std::unary_function<Cell_handle, std::size_t> {
		std::size_t operator()(Cell_handle const& ch) const {
//			std::size_t seed = 0;
			boost::hash<Cell*> hasher;
			return hasher(&(*ch));
		}
	};

	struct Polar_vertex_handle_hash : public std::unary_function<Polar_vertex_handle, std::size_t> {
		std::size_t operator()(Polar_vertex_handle const& v) const {
//			std::size_t seed = 0;
			boost::hash<void*> hasher;
			return hasher(&(*v));
		}
	};	

	typedef boost::unordered_map<Vertex_pair, double, Vertex_pair_hash > Edges_with_doubles;
	typedef boost::unordered_set<Vertex_pair, Vertex_pair_hash > Edges_set;


	//	typedef typename Tds::All_faces_iterator		All_faces_iterator;
//	typedef typename Tds::Finite_vertices_iterator	Finite_vertices_iterator;
//	typedef typename Tds::Face_circulator			Face_circulator;
//	typedef typename Tds::Vertex_circulator			Vertex_circulator;
//	typedef typename Tds::Edge_circulator			Edge_circulator;


	V_triangulation_3(const Gt& gt = Gt())
		: CGAL::Delaunay_triangulation_3<Gt,Tds>(gt) { 
			classified = false; flooded = false;
	}

	V_triangulation_3(const V_triangulation_3<Gt, Polar_tds> &tr) : CGAL::Delaunay_triangulation_3<Gt,Tds>(tr)
	{   
	//CGAL::CGAL_triangulation_postcondition( is_valid() ); /// \todo check what to do, removed because of mac version
	classified = tr.classified; 
	flooded = tr.flooded;
	}

	void set_dirty() { classified = false; flooded = false; } /// \todo check if we could make it so that it is done by the insert...

	bool is_dirty() { return !classified; }
	bool is_flooded() { return flooded; }

	Vertex_handle insert(Point_3 p, Polar_facet f) {
		Vertex_handle v = Base::insert(p);
		v->add_dual_source_facet(f);
		if (v->get_dual_source_facets().size() > 1) 
			std::cout << LOG_BLUE << "more than one v vertex origin" << std::endl;
//		else std::cout << LOG_BLUE << "One v vertex origin" << std::endl;


		if (MAX_FLOAT_DISTANCE_FOR_ZERO > 0 && number_of_vertices() > 1) {
			std::vector<Vertex_handle> iv; std::vector<Vertex_handle>::iterator v_it, v_end;
			std::back_insert_iterator< std::vector<Vertex_handle> > ii(iv);
			incident_vertices(v, ii);
			for (v_it = iv.begin(); v_it != iv.end(); v_it++) {
				Coord_type sq_dist = squared_distance((*v_it)->point(), v->point());
				if (sq_dist < MAX_FLOAT_DISTANCE_FOR_ZERO) {
					Polar_facet_set pfs = v->get_dual_source_facets(); Polar_facet_set::iterator pf_it, pf_end = pfs.end();
					for (pf_it = pfs.begin(); pf_it!=pf_end; pf_it++) {
						(*v_it)->add_dual_source_facet(*pf_it);
					}
					remove(v);
					v = *v_it;
					return v;
				}
			}
		}
		return v;
	}

	void collect_balls_from_polar_facet_set(Polar_facet_set& pf_set, Balls_set& bs) {
//		double alpha_value = Application_settings::get_double_setting("alpha-value-for-dual-of-union-of-balls");
		Polar_facet_set::iterator pf_it;
		for (pf_it = pf_set.begin(); pf_it!=pf_set.end(); pf_it++) {
			Polar_cell_handle ac, n_ac;
			ac = pf_it->first;
			n_ac = pf_it->first->neighbor(pf_it->second);
//			if (ac->get_alpha() <= alpha_value || n_ac->get_alpha() <= alpha_value)
				for (int i=1; i<4; i++) {
					bs.insert(pf_it->first->vertex((pf_it->second+i)%4));
				}
//			else std::cout << LOG_BLUE << "Source polar face vertex not added" << std::endl;
		}
	}

	Point_3 voronoi_vertex(const Cell_handle& ch) {
		if (is_infinite(ch)) {
			std::cout << LOG_ERROR << "getting dual of infinite cell?" << std::endl;
			return Point_3(0.0,0.0,0.0);
		}
		if (ch->is_flat()) {
			Point_3 sum(0.0,0.0,0.0); int n=0;
			for (int i=0; i<4; i++) {
				if (!is_infinite(ch->neighbor(i))) {
					sum = sum + (dual(ch->neighbor(i)) - CGAL::ORIGIN);
					n++;
				}
				else std::cout << LOG_WARNING << "a flat tet's dual vertex has infinite neighbor, position optimisation might have failed" << std::endl;
			}
			return CGAL::ORIGIN + ( (sum-CGAL::ORIGIN)*1.0/float(n));
		} else return dual(ch);
	}

	bool voronoi_vertex_is_ball_center(const Cell_handle& ch, Polar_vertex_handle& ball) {
		if (!is_infinite(ch)) {
			Vertex_handle v0 = ch->vertex(0); Balls_set bs0;
			Vertex_handle v1 = ch->vertex(1); Balls_set bs1;
			Vertex_handle v2 = ch->vertex(2); Balls_set bs2;
			Vertex_handle v3 = ch->vertex(3); Balls_set bs3;

			collect_balls_from_polar_facet_set(v0->get_dual_source_facets(), bs0);
			collect_balls_from_polar_facet_set(v1->get_dual_source_facets(), bs1);
			collect_balls_from_polar_facet_set(v2->get_dual_source_facets(), bs2);
			collect_balls_from_polar_facet_set(v3->get_dual_source_facets(), bs3);

			Balls_set i01, i23, i0123;
			set_intersection(bs0.begin(), bs0.end(), bs1.begin(), bs1.end(), std::inserter(i01, i01.begin()));
			set_intersection(bs2.begin(), bs2.end(), bs3.begin(), bs3.end(), std::inserter(i23, i23.begin()));
			set_intersection(i01.begin(), i01.end(), i23.begin(), i23.end(), std::inserter(i0123, i0123.begin()));

			if (i0123.size() == 1) {
				ball = *i0123.begin();
				return true;
			}
			if (i0123.size() > 1) {
				// flat vertex, a tetrahedron at the intersecion of two balls
				return false;
			}
		}
		return false;
	}

	typedef boost::unordered_map<Cell_handle, int, Cell_handle_hash > Cell_map;
	
	void create_unique_vertices_container(std::list<Point_3>& unique_vertices, std::list<double>& unique_radii, Cell_map& vertices) {
		typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
		typedef CGAL::Triangulation_vertex_base_with_info_3<int, K> Vb;
		typedef CGAL::Triangulation_data_structure_3<Vb>                    Tds;
		typedef CGAL::Delaunay_triangulation_3<K, Tds>                      Delaunay_with_info;
		typedef Delaunay_with_info::Vertex_handle							D_vertex_handle;
		Delaunay_with_info unique_triangulation;

		double max_squared_distance = Application_settings::get_double_setting("medial-axis-export-vertex-squared-distance-collapse");

		typedef boost::unordered_map<Polar_vertex_handle, int, Polar_vertex_handle_hash > Polar_ball_set;
		Polar_ball_set ball_centers;

		// collect unique vertices and create the map Cell_handle to index
		Finite_cells_iterator c_it, c_end = finite_cells_end(); int counter = 0;
		for (c_it = finite_cells_begin(); c_it!=c_end; ++c_it) {
			if (c_it->get_medial_classification()==M_INTERIOR) {
				Cell_handle c = c_it;
				Polar_vertex_handle ball;
				if (voronoi_vertex_is_ball_center(c, ball)) {
					// this vertex corresponds to a ball center
					if (ball_centers.find(ball) != ball_centers.end()) {
						// ball center has an index already
						vertices[c] = ball_centers[ball];
					} else {
						// ball center was not written
						Point_3 p = ball->point().point();
						double radius = sqrt(CGAL::to_double( ball->point().weight()));
						D_vertex_handle v = unique_triangulation.nearest_vertex(p);
//						std::cout << "Distance to closest vertex: " << squared_distance(v->point(),p) << std::endl;
						if (v == D_vertex_handle() || squared_distance(v->point(),p) > max_squared_distance) {
							// this is the first vertex or the nearest vertex is far
							ball_centers[ball] = counter++;
							vertices[c] = ball_centers[ball];
							unique_vertices.push_back(p);
							unique_radii.push_back(radius);
							if (v == D_vertex_handle()) v = unique_triangulation.insert(p);
							else v = unique_triangulation.insert(p, v->cell());
							v->info() = vertices[c];
						} else {
							// vertex is close, take the index of the close vertex
							// assign it to the ball_center and to the cell
//							std::cout << "collapsing ball vertex" << std::endl;
							int v_index = v->info();
							ball_centers[ball] = v_index;
							vertices[c] = ball_centers[ball];
						}
					}
				} else {
					// this vertex does not correspond to a ball center
					Point_3 p = voronoi_vertex(c);
					double radius = sqrt(CGAL::to_double(squared_distance(p, c->vertex(0)->point())));
					D_vertex_handle v = unique_triangulation.nearest_vertex(p);
//					std::cout << "Distance to closest vertex: " << squared_distance(v->point(),p) << std::endl;
					if (v == D_vertex_handle() || squared_distance(v->point(),p) > max_squared_distance) {
						// this is the first vertex or the nearest vertex is far
						vertices[c] = counter++;
						unique_vertices.push_back(p);
						unique_radii.push_back(radius);
						v = unique_triangulation.insert(p);
						v->info() = vertices[c];
					} else {
//						std::cout << "collapsing vertex" << std::endl;
						// vertex is close, take the index of the close vertex
						// assign it to the ball_center and to the cell
						int v_index = v->info();
						vertices[c] = v_index;
					}
				}
//				std::cout << vertices.size() << std::endl;
			}
		}
		std::cout << "Found " << unique_vertices.size() << " unique vertices" << std::endl;
		if (unique_radii.size() != unique_vertices.size()) std::cout << LOG_ERROR << "Number of unique radii and vertices in medial axis is not equal" << std::endl;


	}


	void write_simplified_medial_axis_to_off(const std::string& file_name) {
		// Delaunay triangulations to collapse geometrically close points

		std::cout << "Write simplified medial axis mesh file" << std::endl;

		Cell_map vertices;
		std::list<Point_3> unique_vertices;
		std::list<double> unique_radii;
		create_unique_vertices_container(unique_vertices, unique_radii, vertices);

		std::list< std::list<int> > valid_polygons;


		// collect valid (non-zero volume) polygons as list of referenced vertices
		All_edges_iterator e_it, e_end = all_edges_end();// int counter = 0;
		for (e_it=all_edges_begin(); e_it!=e_end; ++e_it) {
			if (is_medial_interior_voronoi_facet(*e_it)) {
				Cell_circulator c_start, c_circ = incident_cells(*e_it);
				c_start = c_circ; 
				std::list<int> pol; int previous_index = -1;
				do {
					if (!is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
						int index = vertices[c_circ];
						if (index != previous_index) {
							pol.push_back(index);
							previous_index = index;
						}
					} 
					c_circ++;
				} while (c_circ!=c_start);
				if (previous_index == *pol.begin()) pol.pop_back();
				if (pol.size() > 2)
					valid_polygons.push_back(pol);
			}
		}

		// open file
		std::ofstream myfile;
		myfile.open (file_name.c_str());
		myfile.precision(dbl::digits10);

		// write header
		myfile << "OFF " << unique_vertices.size() << " " << valid_polygons.size() << " 0" << std::endl;

		// write vertices
		std::list<Point_3>::iterator p_it, p_end = unique_vertices.end();
		for (p_it=unique_vertices.begin(); p_it!=p_end; ++p_it) {
			myfile << std::fixed << p_it->x() << " " 
				   << std::fixed << p_it->y() << " " 
				   << std::fixed << p_it->z() << std::endl;
		}

		// write polygons
		std::list< std::list<int> >::iterator pol_it, pol_end = valid_polygons.end();
		for (pol_it = valid_polygons.begin(); pol_it!=pol_end; pol_it++) {
			std::list<int>& pol = *pol_it;
			myfile << pol.size();
			std::list<int>::iterator p_it, p_end = pol.end();
			for (p_it=pol.begin(); p_it!=p_end; ++p_it) {
				myfile << " " << *p_it;
			}
			myfile << std::endl;
		}

		// close file
		myfile.close();	
	}

	void write_less_medial_balls(const std::string& file_name)  {
		Cell_map vertices;
		std::list<Point_3> unique_vertices;
		std::list<double> unique_radii;
		create_unique_vertices_container(unique_vertices, unique_radii, vertices);

		std::ofstream myfile;
		myfile.open (file_name.c_str());
		myfile.precision(dbl::digits10);

		// write header
		myfile << "WOFF " << unique_vertices.size() << " 0 0" << std::endl;

		// write vertices
		std::list<Point_3>::iterator p_it, p_end = unique_vertices.end();
		std::list<double>::iterator r_it, r_end = unique_radii.end();
		for (r_it =unique_radii.begin(), p_it=unique_vertices.begin(); p_it!=p_end; ++p_it, ++r_it) {
			myfile << std::fixed << p_it->x() << " " 
				   << std::fixed << p_it->y() << " " 
				   << std::fixed << p_it->z() << " " 
				   << std::fixed << *r_it << std::endl;
		}
		std::cout << "Written " << unique_vertices.size() << " balls." << std::endl;
		myfile.close();

	}

	std::string create_sorted_polygon_string(std::vector<double> polygon) {
		std::ostringstream stm;
		std::sort(polygon.begin(),polygon.end()); // sort is temporary because we pass polygon by valu
		std::vector<double>::iterator p_it = polygon.begin();
		for (int i=0; i < polygon.size(); i++, p_it++) {
			stm << *p_it;
			stm << " ";
		}
		return stm.str();
	}

	void write_medial_axis_to_moff(std::ostream& myfile, double growth_ratio) {
		std::cout << "Write simplified medial axis to MOFF file" << std::endl;

		Cell_map vertices;
		std::list<Point_3> unique_vertices;
		std::list<double> unique_radii;
		create_unique_vertices_container(unique_vertices, unique_radii, vertices);

		std::list< std::vector<double> > valid_polygons;

		std::set<std::string> unique_polygons;
		// collect valid (non-zero volume) polygons as list of referenced vertices
		All_edges_iterator e_it, e_end = all_edges_end(); int counter = 0;
		for (e_it=all_edges_begin(); e_it!=e_end; ++e_it) {
			if (is_medial_interior_voronoi_facet(*e_it)) {
				Cell_circulator c_start, c_circ = incident_cells(*e_it);
				c_start = c_circ; 
				std::vector<double> pol; int previous_index = -1;
				bool all_interior = true;
				do {
					if (!is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
						int index = vertices[c_circ];
						if (index != previous_index) {
							pol.push_back(index);
							previous_index = index;
						}
					} else all_interior = false;
					c_circ++;
				} while (c_circ!=c_start);
				if (previous_index == *pol.begin()) pol.pop_back();
				if (pol.size() > 2 &&
					(all_interior==true || !Application_settings::get_bool_setting("ignore-incomplete-v-voronoi-faces-for-medial-axis"))) {
					Edge e = *e_it;
					std::string pol_str = create_sorted_polygon_string(pol);
					if (unique_polygons.find(pol_str) == unique_polygons.end()) {
						pol.push_back(angle_values[create_vertex_pair_from_edge(e)]);
						pol.push_back(lambda_value(e));
						Point p = e.first->vertex(e.second)->point();
						pol.push_back(CGAL::to_double(p.x()));
						pol.push_back(CGAL::to_double(p.y()));
						pol.push_back(CGAL::to_double(p.z()));
						valid_polygons.push_back(pol);
						unique_polygons.insert(pol_str);
					} else {
//						std::cout << LOG_GREEN << "duplicate polygon eliminated " << pol_str << std::endl;
					}
				}
			}
		}
		std::cout << "Found " << valid_polygons.size() << " unique polygons" << std::endl;


		myfile.precision(dbl::digits10);

		// write header
		myfile << "MOFF " << unique_vertices.size() << " " << valid_polygons.size() << " " << growth_ratio << std::endl;

		// write vertices
		std::list<Point_3>::iterator p_it, p_end = unique_vertices.end();
		std::list<double>::iterator r_it, r_end = unique_radii.end();
		for (r_it =unique_radii.begin(), p_it=unique_vertices.begin(); p_it!=p_end; ++p_it, ++r_it) {
			myfile << std::fixed << p_it->x() << " " 
				   << std::fixed << p_it->y() << " " 
				   << std::fixed << p_it->z() << " " 
				   << std::fixed << *r_it << std::endl;
		}

		// write polygons
		std::list< std::vector<double> >::iterator pol_it, pol_end = valid_polygons.end();
		for (pol_it = valid_polygons.begin(); pol_it!=pol_end; pol_it++) {
			std::vector<double>& pol = *pol_it;
			int pol_vert_number = pol.size()-5;
			myfile << pol_vert_number;
			std::vector<double>::iterator p_it, p_end = pol.end(); int counter =0;
			for (p_it=pol.begin(); p_it!=p_end; ++p_it, ++counter) {
				if (counter<pol_vert_number)
					myfile << " " << (int)(*p_it);
				else
					myfile << " " << *p_it;
			}
			myfile <<  std::endl;
		}
	}

	void write_medial_axis_to_moff(const std::string& file_name, double growth_ratio) {
		// open file
		std::ofstream myfile;
		myfile.open (file_name.c_str());

		write_medial_axis_to_moff(myfile, growth_ratio);

		// close file
		myfile.close();	

	}

	void  write_medial_axis_to_off(const std::string& file_name) {
		std::cout << "Write medial axis mesh file" << std::endl;
		typedef boost::unordered_map<Cell_handle, int, Cell_handle_hash > Cell_set;
		Cell_set vertices;

		Finite_cells_iterator c_it, c_end = finite_cells_end(); int counter = 0;
		for (c_it = finite_cells_begin(); c_it!=c_end; ++c_it) {
			if (c_it->get_medial_classification()==M_INTERIOR) {
				Cell_handle c = c_it;
				vertices[c] = counter++;
//				std::cout << vertices.size() << std::endl;
			}
		}

		All_edges_iterator e_it, e_end = all_edges_end(); counter = 0;
		for (e_it=all_edges_begin(); e_it!=e_end; ++e_it) {
			if (is_medial_interior_voronoi_facet(*e_it)) {
				counter++;
			}
		}


		std::ofstream myfile;
		myfile.open (file_name.c_str());

		myfile << "OFF " << vertices.size() << " " << counter << " 0" << std::endl;

		counter = 0;
		for (c_it = finite_cells_begin(); c_it!=c_end; ++c_it) {
			if (c_it->get_medial_classification()==M_INTERIOR) {
				Cell_handle c = c_it;
				Point_3 v = voronoi_vertex(c);
				myfile << v.x() << " " << v.y() << " " << v.z() << std::endl;
				if (vertices[c]!=counter++) {
					std::cout << LOG_WARNING << "Medial axis write to off has mixed up vertex index "<< vertices[c] << " with counter " << counter << std::endl;

				}
			}
		}

		counter = 0;
		for (e_it=all_edges_begin(); e_it!=e_end; ++e_it) {
			if (is_medial_interior_voronoi_facet(*e_it)) {
				Cell_circulator c_start, c_circ = incident_cells(*e_it);
				c_start = c_circ; int polygon_length = 0;
				do {
					if (!is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
						polygon_length++;
					} 
					c_circ++;
				} while (c_circ!=c_start);
				myfile << polygon_length;
				c_start = c_circ;
				do {
					if (!is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
						Cell_handle c = c_circ;
						myfile << " " << vertices[c];

					} 
					c_circ++;
				} while (c_circ!=c_start);
				myfile << std::endl;
			}
		}


		myfile.close();	
	}

	void compute_classification() {
		Vertex_handle v1, v2, v3;
		Polar_facet f1, f2, f3, pf;
		Polar_vertex_handle common_ball;

		std::cout << PROGRESS_STATUS << "Initializing V Voronoi classification and detecting flat tetrahedra" << std::endl;
		All_cells_iterator c_it, c_end = all_cells_end(); int fc_counter = 0;
		for (c_it = all_cells_begin(); c_it!= c_end; c_it++) {
			c_it->set_medial_classification(M_UNCLASSIFIED);
			c_it->set_crossing_vertex(0, 0);
			c_it->set_crossing_vertex(1, 0);
			c_it->set_crossing_vertex(2, 0);
			c_it->set_crossing_vertex(3, 0);
			c_it->set_crossing_classification(0, C_UNCLASSIFIED);
			c_it->set_crossing_classification(1, C_UNCLASSIFIED);
			c_it->set_crossing_classification(2, C_UNCLASSIFIED);
			c_it->set_crossing_classification(3, C_UNCLASSIFIED);

			if (!is_infinite(c_it)) {
				Vertex_handle v0 = c_it->vertex(0); Balls_set bs0;
				Vertex_handle v1 = c_it->vertex(1); Balls_set bs1;
				Vertex_handle v2 = c_it->vertex(2); Balls_set bs2;
				Vertex_handle v3 = c_it->vertex(3); Balls_set bs3;

				collect_balls_from_polar_facet_set(v0->get_dual_source_facets(), bs0);
				collect_balls_from_polar_facet_set(v1->get_dual_source_facets(), bs1);
				collect_balls_from_polar_facet_set(v2->get_dual_source_facets(), bs2);
				collect_balls_from_polar_facet_set(v3->get_dual_source_facets(), bs3);

				Balls_set i01, i23, i0123;
				set_intersection(bs0.begin(), bs0.end(), bs1.begin(), bs1.end(), std::inserter(i01, i01.begin()));
				set_intersection(bs2.begin(), bs2.end(), bs3.begin(), bs3.end(), std::inserter(i23, i23.begin()));
				set_intersection(i01.begin(), i01.end(), i23.begin(), i23.end(), std::inserter(i0123, i0123.begin()));

				if (i0123.size() > 1) {
					c_it->set_flat(true);
					fc_counter++;
				}
			}
		}
		std::cout << PROGRESS_DONE << std::endl;
		std::cout << fc_counter << " v voronoi vertices are flat" << std::endl;

		std::cout << PROGRESS_STATUS << "Classifying V Voronoi edges to find crossing and alpha" << std::endl;
		//std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		//std::cout << PROGRESS_MAXIMUM <<  number_of_finite_facets () << std::endl;
		Finite_facets_iterator f_it, f_end = finite_facets_end(); int i = 0;
		for (f_it = finite_facets_begin(); f_it!=f_end; f_it++, i++) {
//			std::cout << PROGRESS_VALUE << i << std::endl;
			Cell_handle c = f_it->first;
			int id = f_it->second;
			Cell_handle n_c = c->neighbor(id);
			int n_id = n_c->index(c);
			Polar_facet_set pf_set; Polar_facet_set::iterator pf_it;

			//if (squared_distance(dual(c),dual(n_c)) < MAX_FLOAT_DISTANCE_FOR_ZERO) {
			//	std::cout << LOG_BLUE << "V Voronoi edge set to unclassified because too short" << std::endl;
			//	c->set_crossing_vertex(id, 0);
			//	n_c->set_crossing_vertex(n_id, 0);
			//	c->set_crossing_classification(id, C_UNCLASSIFIED);
			//	n_c->set_crossing_classification(n_id, C_UNCLASSIFIED);
			//	common_ball = 0;
			//} else {
				v1 = c->vertex((id+1)%4); Balls_set bs1;
				v2 = c->vertex((id+2)%4); Balls_set bs2;
				v3 = c->vertex((id+3)%4); Balls_set bs3;

				collect_balls_from_polar_facet_set(v1->get_dual_source_facets(), bs1);
				collect_balls_from_polar_facet_set(v2->get_dual_source_facets(), bs2);
				collect_balls_from_polar_facet_set(v3->get_dual_source_facets(), bs3);

				Balls_set i12, i123;
				set_intersection(bs1.begin(), bs1.end(), bs2.begin(), bs2.end(), std::inserter(i12, i12.begin()));
				set_intersection(i12.begin(), i12.end(), bs3.begin(), bs3.end(), std::inserter(i123, i123.begin()));

				if (i123.size() > 2) std::cout << LOG_RED << "more common balls: " << i123.size() << std::endl;
				if (i123.size()==1 ) {
					common_ball = *(i123.begin());
					c->set_crossing_vertex(id, common_ball);
					c->set_crossing_classification(id, C_CROSSING);
					n_c->set_crossing_vertex(n_id, common_ball);
					n_c->set_crossing_classification(n_id, C_CROSSING);
				} else if (i123.size()==2) {
					//Balls_set::iterator b_it = i123.begin();
					//Polar_vertex_handle b1 = *b_it; ++b_it;
					//Polar_vertex_handle b2 = *b_it;
					//Point v1 = dual(c);
					//Point v2 = dual(n_c);


					//if (squared_distance(dual(c),common_ball->point().point()) < MAX_FLOAT_DISTANCE_FOR_ZERO)

					c->set_crossing_vertex(id, 0);
					c->set_crossing_classification(id, C_ALPHA);
					n_c->set_crossing_vertex(n_id, 0);
					n_c->set_crossing_classification(n_id, C_ALPHA);				
				}
//			}


//			f1 = v1->get_dual_source_facet();
//			f2 = v2->get_dual_source_facet(); 
//			f3 = v3->get_dual_source_facet(); 
//			common_ball = 0;
//			for (int i=1; i<4; i++)
//				for (int j=1; j<4; j++) 
//					if (f1.first->vertex((f1.second+i)%4) == f2.first->vertex((f2.second+j)%4))
//						for (int k=1; k<4; k++)
//							if (f1.first->vertex((f1.second+i)%4) == f3.first->vertex((f3.second+k)%4))
//								common_ball = f1.first->vertex((f1.second+i)%4);
//			c->set_crossing_vertex(id, common_ball);
//			n_c->set_crossing_vertex(n_id, common_ball);
//

			// setting balls which are centers of balls as interior
//			if (common_ball != 0) {
//				Vertex_handle v = c->vertex(id);
//				if (!is_infinite(v) && c->get_medial_classification() != M_INTERIOR) {
//					pf_set = v->get_dual_source_facets();
//					for (pf_it = pf_set.begin(); pf_it!=pf_set.end(); pf_it++) {
//						for (int i=1; i<4; i++)
//							if (pf_it->first->vertex((pf_it->second+i)%4) == common_ball) {
//								if (squared_distance(dual(c),common_ball->point().point()) < MAX_FLOAT_DISTANCE_FOR_ZERO) {
//									c->set_medial_classification(M_INTERIOR);
//									c->set_polar_vertex(common_ball);
//								}
//								//								std::cout << LOG_WARNING << "m interior set for voronoi vertex" << std::endl;
//							}
//					}
//				}
//				v = n_c->vertex(n_id);
//				if (!is_infinite(v) && c->get_medial_classification() != M_INTERIOR) {
//					pf_set = v->get_dual_source_facets();
//					for (pf_it = pf_set.begin(); pf_it!=pf_set.end(); pf_it++) {
//						for (int i=1; i<4; i++)
//							if (pf_it->first->vertex((pf_it->second+i)%4) == common_ball) {
//								if (squared_distance(dual(c),common_ball->point().point()) < MAX_FLOAT_DISTANCE_FOR_ZERO) {
//									c->set_medial_classification(M_INTERIOR);
//									c->set_polar_vertex(common_ball);
//								}
//								//								std::cout << LOG_WARNING << "m interior set for voronoi vertex" << std::endl;
//							}
//					}
//				}
//			} else {
////				c->set_medial_classification(M_UNCLASSIFIED);
////				n_c->set_medial_classification(M_UNCLASSIFIED);
//			}
		}
		std::cout << PROGRESS_DONE << std::endl;


		classified = true;
	}

	void flood_cell_based_voronoi_classification() {
		if (number_of_vertices() < 4) return;


		std::cout << PROGRESS_STATUS << "Setting initial classification and collecting infinite voronoi cells" << std::endl;
		std::queue<Vertex_handle> VQ; //the vertices whose voronoi cells recently received classifying info
		std::set<Vertex_handle> visited_vertex; // vertices whose voronoi cells is already have been queued to processed
		// reset cells to unclassified and 
		// set infinite cells to exterior and put the corresponding vertices 
		// in queue and the visited vertices
		All_cells_iterator ac_it, ac_end = all_cells_end();
		for (ac_it = all_cells_begin(); ac_it!=ac_end; ac_it++) {
			if (is_infinite(ac_it)) {
				ac_it->set_medial_classification(M_EXTERIOR);
				for (int i=0; i<4; i++) {
					Vertex_handle v = ac_it->vertex(i);
					if (!is_infinite(v) && visited_vertex.find(v) == visited_vertex.end()) {
						VQ.push(v);
						visited_vertex.insert(v);
					}
				}
			}
			else ac_it->set_medial_classification(M_UNCLASSIFIED);
		}
		std::cout << PROGRESS_DONE << std::endl;

		std::cout << PROGRESS_STATUS << "Flooding V voronoi cell by cell" << std::endl; size_t vertex_counter = 0;
		// this is the flooding
		while (!VQ.empty()/* && vertex_counter < number_of_vertices() * 2*/) {
			vertex_counter++;
			Vertex_handle v = VQ.front(); VQ.pop();

			std::queue<Cell_handle> CQ; // the voronoi vertices which are recently classified
			std::set<Cell_handle> visited_cells; // the voronoi vertices which have been classified in the processing of this voronoi cell

			// get voronoi vertices and collect exterior classified vertices
			Cell_vector v_cells; Cell_insert_iterator ii(v_cells);
			incident_cells(v,ii);
			Cell_vector_iterator c_it, c_end = v_cells.end();
			for (c_it = v_cells.begin(); c_it!=c_end; ++c_it) {
				if ((*c_it)->get_medial_classification() == M_EXTERIOR) {
					CQ.push(*c_it);
					visited_cells.insert(*c_it);
				}
			}

			if (CQ.empty()) {
				std::cout << LOG_ERROR << "flood_cell_based_voronoi_classification() found a voronoi cell which has no exterior classified vertex" << std::endl;
			}

			// flood the voronoi face to find exterior vertices
			int voronoi_cell_vertex_counter = 0;
			while (!CQ.empty()) {
				voronoi_cell_vertex_counter++;
				// std::cout << "--- flooding pop" << std::endl;
				Cell_handle c = CQ.front(); CQ.pop();
				assert(c->get_medial_classification() != M_UNCLASSIFIED);
				for (int i=0; i<4; i++) {
					Cell_handle n_c = c->neighbor(i);
					if (n_c->has_vertex(v)) {
						// std::cout << "neighbor " << i << " is Voronoi vertex of the cell" << std::endl;
						if (visited_cells.find(n_c) == visited_cells.end() && // not visited through the cell flooding
							(c->get_crossing_classification(i) == C_UNCLASSIFIED || n_c->is_flat()) ) {
							// std::cout << "propagate classification, edge was unclassified" << std::endl;
								if (n_c->get_medial_classification() != M_UNCLASSIFIED && n_c->get_medial_classification() != c->get_medial_classification()) {
									std::cout << LOG_WARNING << "Flooding gives conflicting information for a voronoi vertex classification" << std::endl;
									n_c->set_medial_classification(M_EXTERIOR);
								} else
									n_c->set_medial_classification(c->get_medial_classification());
							// adjacent Voronoi cell add to queue (to further flooding) if not visited already
							if (!Application_settings::get_bool_setting("v-cell-flooding-only-infinite-cells")) {
								for (int j=0; j<4; j++) {
									Vertex_handle cv = n_c->vertex(j);
									if (!is_infinite(cv) && visited_vertex.find(cv) == visited_vertex.end()) {
										VQ.push(cv);
										visited_vertex.insert(cv);
									}
								}
							}
							CQ.push(n_c);
							visited_cells.insert(n_c);
						}
					}// else std::cout << "neighbor " << i << " does not belong to Voronoi cell" << std::endl;
				}
			}

			// rest of voronoi vertices are interior
			for (c_it = v_cells.begin(); c_it!=c_end; ++c_it) {
				if (visited_cells.find(*c_it) == visited_cells.end()) {
					if ((*c_it)->get_medial_classification() == M_EXTERIOR)
						std::cout << LOG_WARNING << "Flooding gives wants to overwrite a previously exterior vertex with interior label" << std::endl;
					(*c_it)->set_medial_classification(M_INTERIOR);
				}
			}

		}
		std::cout << PROGRESS_DONE << std::endl;
	}

	void collect_medial_edges() {
		std::cout << PROGRESS_STATUS << "Collecting medial edges" << std::endl;
		medial_edges.clear();
		All_edges_iterator e_it, e_end = all_edges_end();
		int counter = 0, all_edges = 0;
		for (e_it=all_edges_begin(); e_it!=e_end; ++e_it, ++all_edges) {

			Cell_circulator c_start, c_circ = incident_cells(*e_it);
			c_start = c_circ;
			std::list<Point_3> polygon;
			bool all_interior = true;
			do {
				if (!is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
					polygon.push_back(voronoi_vertex(c_circ));
					//						std::cout << t->dual(c_circ) << std::endl;
				} else all_interior = false;
				c_circ++;
			} while (c_circ!=c_start);
			if (polygon.size()>2 && (all_interior==true || !Application_settings::get_bool_setting("ignore-incomplete-v-voronoi-faces-for-medial-axis"))) {
				medial_edges.insert(create_vertex_pair_from_edge(*e_it));
			}
		}


		std::cout << PROGRESS_DONE << std::endl;
	}

	double lambda_value(Edge& e) {
		return sqrt(CGAL::to_double(squared_distance(e.first->vertex(e.second)->point(), e.first->vertex(e.third)->point())));
	}

	//double angle_value(Edge& e) {
	//	return e.first->get_angle_value(edge_index_in_cell(&e));
	//}

	void compute_local_filtration_values() {
		angle_values.clear();
		All_edges_iterator e_it, e_end = all_edges_end();
		for (e_it=all_edges_begin(); e_it!=e_end; ++e_it) {
			// compute the baricenter of the voronoi face part of the medial axis
			Cell_circulator c_start, c_circ = incident_cells(*e_it);
			c_start = c_circ;
			Point_3 sum = Point_3(0.0, 0.0, 0.0); int i=0;
//			bool all_interior = true;
			do {
				if (!is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
					sum = sum + (voronoi_vertex(c_circ)-CGAL::ORIGIN);
					i++;
				}
				c_circ++;
			} while (c_circ!=c_start);
			if (i>2) {
				sum = CGAL::ORIGIN + ( (sum-CGAL::ORIGIN)*1.0/float(i));

				// compute the angle stability at that point
				double lambda_sq = CGAL::to_double(segment(*e_it).squared_length())/4;
				double dist_sq = CGAL::to_double(squared_distance(e_it->first->vertex(e_it->second)->point(),sum));
				double angle = asin(sqrt(lambda_sq/dist_sq))*(180.0/PI);

				angle_values[create_vertex_pair_from_edge(*e_it)] = angle;
			}
			//Edge e = *e_it;
			//e_it->first->set_angle_value(edge_index_in_cell(&e), angle);
		}

	}

	void compute_topology_angle_filtration(int steps = -1) {
		topology_angle_values.clear();
		visited_edges.clear();
		front.clear();

		front_value = 1;

		std::cout << PROGRESS_STATUS << "Flooding to fix topology" << std::endl;

		//fill front with the inital front faces
//		std::cout << "compute_topology angle filtration" << std::endl;
		All_edges_iterator e_it, e_end = all_edges_end();// int medial =0;
		for (e_it=all_edges_begin(); e_it!=e_end; ++e_it) {
//			if (is_medial_interior_voronoi_facet(*e_it)) medial++;
			if (is_front_voronoi_facet(*e_it)) {

				Vertex_pair vp = create_vertex_pair_from_edge(*e_it);
				if (angle_values.find(vp) == angle_values.end())
					std::cout << LOG_ERROR << "Front voronoi face has no angle value" << std::endl;

				add_to_front(*e_it,front);

				//double angle = angle_values[vp];
				//front.insert(std::make_pair(angle, vp));
//				topology_angle_values[vp] = angle;
//				std::cout << "added to front" << std::endl;
//				topology_angle_values[create_vertex_pair_from_edge(*e_it)] = 1;
			}
//			else std::cout << "not front" << std::endl;
		}

		//std::multimap<double, Vertex_pair>::iterator fr_it, fr_end = front.end();
		//for (fr_it = front.begin(); fr_it!=fr_end; ++fr_it) {
		//	std::cout << "angle: " << fr_it->first << std::endl;
		//}


		////// --- BEGIN OF FRONT DETECTION DEBUGGING
		//if (Application_settings::get_bool_setting("debug-front-propagation-instead-of-topology-filtering")) {
		//	std::cout << LOG_RED << "DEBUG FILTERING" << std::endl;
		//	std::multimap<double, Vertex_pair>::iterator fr_it, fr_end = front.end();
		//	for (fr_it = front.begin(); fr_it!=fr_end; ++fr_it) {
		//		Vertex_pair vp = fr_it->second;
		//		topology_angle_values[vp] = 1;
		//		visited_edges.insert(vp);
		//	}

		//	for (int front_number = 2; front_number < 10; front_number++) {
		//		front.clear();
		//		for (e_it=all_edges_begin(); e_it!=e_end; ++e_it) {
		//			if (is_front_voronoi_facet(*e_it)) {
		//				Vertex_pair vp = create_vertex_pair_from_edge(*e_it);
		//				if (angle_values.find(vp) == angle_values.end())
		//					std::cout << LOG_ERROR << "Front voronoi face has no angle value" << std::endl;
		//				double angle = angle_values[vp];
		//				front.insert(std::make_pair(angle, vp));
		//				//					topology_angle_values[vp] = angle;
		//				//				std::cout << "added to front" << std::endl;
		//				//				topology_angle_values[create_vertex_pair_from_edge(*e_it)] = 1;
		//			}
		//			//			else std::cout << "not front" << std::endl;
		//		}

		//		//		std::multimap<double, Vertex_pair>::iterator fr_it, fr_end = front.end();
		//		for (fr_it = front.begin(); fr_it!=fr_end; ++fr_it) {
		//			Vertex_pair vp = fr_it->second;
		//			topology_angle_values[vp] = front_number;
		//			visited_edges.insert(vp);
		//		} 
		//	}
		//	return;
		//}
		///// ---- END OF FRONT DETECTION DEBUGGING


		// traverse the medial axis and compute stability values
		std::cout << "Visited edges size before flooding: " << visited_edges.size() << std::endl;
//		int counter=0;
		int front_counter = 0;
		front_value = -1;

		while (!front.empty() && (steps==-1 || front_counter<steps)) {
			std::multimap<double, Vertex_pair>::iterator i;
			i = front.begin(); 
//			front_value = front_value + 1;
			front_value = std::max((*i).first, front_value);
//			std::cout << "value: " << (*i).first << std::endl;
			Vertex_pair vp = (*i).second;
			front.erase(i);
			Edge e = create_edge_from_vertex_pair(vp);
			if (visited_edges.find(vp)!=visited_edges.end()) {
				std::cout << LOG_ERROR << "FLOODING: the highest face in the front has been visited already" << std::endl;
				continue; // this edge has been visited already, we keep its stability the earlier one, that is for sure not larger than the current value
			}
			if (!is_front_voronoi_facet(e)) {
				std::cout << LOG_ERROR << "FLOODING, the highest face in the front is not detected as front currently" << std::endl;
				continue;
			}
			visited_edges.insert(vp);
			topology_angle_values[vp] = front_value;
			// FOR DEBUGGING THE FRONT THING
			// topology_angle_values[vp] = front_counter++;

			// circulate around voronoi edges to find for every of them
			// the two neighboring voronoi facets
			Facet_circulator f_start, f_circ = incident_facets(e);
			f_start = f_circ;
			Vertex_handle v1 = e.first->vertex(e.second);
			Vertex_handle v2 = e.first->vertex(e.third);
			do {
				int v1_index=5, v2_index=5;
				if (!has_vertex(*f_circ,v1,v1_index) || !has_vertex(*f_circ,v2,v2_index)) 
					std::cout << LOG_ERROR << "Flooding Voronoi facets, a vertex does not belong to a circulating face around its edge" << std::endl;
				Cell_handle c = f_circ->first;
				int id = f_circ->second;
				int last = 6 - id - v1_index - v2_index; //the fourth vertex has index 6 minus the other three
//				std::cout << 

				// first neighboring voronoi facet, if it is front now -> add it to front
				Edge n_e = Edge(c,last,v1_index);
				if (is_front_voronoi_facet(n_e)) add_to_front(n_e,front);
				else remove_from_front(n_e,front);

				// second neighboring voronoi facet, if it is front now -> add it to front
				n_e = Edge(c,last,v2_index);
				if (is_front_voronoi_facet(n_e)) add_to_front(n_e,front);
				else remove_from_front(n_e,front);

				f_circ++;
			} while (f_circ!=f_start);

			front_counter++;

		}
		
		std::cout << PROGRESS_DONE << std::endl;
		std::cout << LOG_BLUE << "Front end size: " << front.size() << std::endl;

//		All_edges_iterator e_it, e_end = t->all_edges_end();
		for (e_it=all_edges_begin(); e_it!=e_end; ++e_it) {
			if (is_medial_interior_voronoi_facet(*e_it) && !is_visited(*e_it)) {
				topology_angle_values[create_vertex_pair_from_edge(*e_it)] = 91;
			}
		}

	}

	void check_edges_with_doubles(const Edges_with_doubles& ewd) const {
//		std::cout << "Checking edges unordered_map" << std::endl;
		Edges_with_doubles::const_iterator vp_it, vp_end = ewd.end(); bool problem = false;
		for (vp_it = ewd.begin(); vp_it!=vp_end; ++vp_it) {
			Vertex_handle v1 = vp_it->first.first;
			Vertex_handle v2 = vp_it->first.second;
			Cell_handle c; int i,j;
			if (!is_edge(v1,v2,c,i,j)) {
				std::cout << LOG_ERROR << "unordered_map has edges which are not edges of the triangulation" << std::endl;
				problem = true;
			}
		}
		if (!problem) std::cout << LOG_GREEN << "Edges_with_doubles contains only edges of the triangulation" << std::endl;
//		std::cout << "Checking done" << std::endl;
	}


//	void insert_and_increase_value(const Edge& n_e, std::multimap<double, Vertex_pair>& front) {
//		Vertex_pair vp = create_vertex_pair_from_edge(n_e);
//		double angle = angle_values[vp];
////		double angle = std::max(angle_values[vp], front_value);
////		if (angle > angle_values[vp]) std::cout << "angle value has been increased from " << angle_values[vp] << " to " << angle << std::endl;
//		if (topology_angle_values.find(vp) != topology_angle_values.end()) {
//			// this 
//			std::cout << LOG_WARNING << "face to be added to the front has been added before" << std::endl;
//		}
////		topology_angle_values[vp] = angle; // TEMPORARY REMOVE DEBUG
//		front.insert(std::make_pair(angle,vp));
//	}

	bool is_in_front(const Edge& n_e, std::multimap<double, Vertex_pair>& front) {
		// get the angle corresponding to this face
		Vertex_pair vp = create_vertex_pair_from_edge(n_e);
		double angle = angle_values[vp];
		//angle = 0;
		
		// check if we have it in the front already
		typedef std::multimap<double, Vertex_pair>::iterator Vp_it;
		std::pair<Vp_it, Vp_it> range = front.equal_range(angle);
		Vp_it vp_it, vp_end = range.second; bool found = false;
		for (vp_it = range.first; vp_it!=vp_end; ++vp_it) {
			if (vp_it->second == vp) {
					found = true;
					break;
			}
		}
		return found;
	}

	bool is_visited(const Edge& n_e) {
		// get the angle corresponding to this face
		Vertex_pair vp = create_vertex_pair_from_edge(n_e);
		return visited_edges.find(vp) != visited_edges.end();
	}

	void add_to_front(const Edge& n_e, std::multimap<double, Vertex_pair>& front) {
		// get the angle corresponding to this face
		Vertex_pair vp = create_vertex_pair_from_edge(n_e);
		double angle = angle_values[vp];
		//angle = 0;
		
		// check if we have it in the front already
		typedef std::multimap<double, Vertex_pair>::iterator Vp_it;
		std::pair<Vp_it, Vp_it> range = front.equal_range(angle);
		Vp_it vp_it, vp_end = range.second; bool found = false;
		for (vp_it = range.first; vp_it!=vp_end; ++vp_it) {
			if (vp_it->second == vp) {
					found = true;
					break;
			}
		}
		// if we didn't found it, add it
		if (!found) {
			front.insert(std::make_pair(angle,vp));
		}
	}

	void remove_from_front(const Edge& n_e, std::multimap<double, Vertex_pair>& front) {
		// get the angle corresponding to this face
		Vertex_pair vp = create_vertex_pair_from_edge(n_e);
		double angle = angle_values[vp];
		//angle = 0;

		// check if we have it in the front already
		typedef std::multimap<double, Vertex_pair>::iterator Vp_it;
		std::pair<Vp_it, Vp_it> range = front.equal_range(angle);
		Vp_it vp_it, vp_end = range.second; bool found = false;
		for (vp_it = range.first; vp_it!=vp_end; ++vp_it) {
			if (vp_it->second == vp) {
					found = true;
					break;
			}
		}
		// if we found it, remove it
		if (found) {
			front.erase(vp_it);
		}
	}

//	void remove_from_front_if_exists(const Edge& n_e, std::multimap<double, Vertex_pair>& front) {
//		Vertex_pair vp = create_vertex_pair_from_edge(n_e);
//		double angle = 0;
//		if (topology_angle_values.find(vp) != topology_angle_values.end()) {
//			angle = topology_angle_values[vp];
//			typedef std::multimap<double, Vertex_pair>::iterator Vp_it;
//			std::pair<Vp_it, Vp_it> range = front.equal_range(angle);
//			Vp_it vp_it, vp_end = range.second; bool found = false;
//			for (vp_it = range.first; vp_it!=vp_end; ++vp_it) {
//				if (vp_it->second == vp) {
//					front.erase(vp_it);
////					std::cout << " ~!~!~! removed something from the front" << std::endl;
//					found = true;
//					break;
//				}
//			}
////			if (!found) std::cout << "Had topology angle, but did not find in front" << std::endl;
//		} else {
//			std::cout << LOG_ERROR << "the vertex pair to be removed from front is not in the topology_angle_values" << std::endl;
//		}// if has no topology_angle_value then it cannot be in the front
//	}

	// the voronoi facet dual to e is a medial axis facet (or at least a part of it)
	bool is_medial_interior_voronoi_facet(const Edge& e) const {
//		std::cout << "                 checking facet: " << &(*(e.first)) << " " << e.second << " " << e.third << std::endl;
		Cell_circulator c_start, c_circ = incident_cells(e);
		c_start = c_circ;
		int i=0;
		do {
			if (!is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
				i++;
			}
			c_circ++;
		} while (c_circ!=c_start);
//		std::cout << "the i value: " << i << std::endl;
		if (i>2) return true;
		return false;
	}

	// the voronoi edge dual to f is in the interior of the medial axis
	// i.e. returns true if at least two interior voronoi facets are touching it
	bool is_medial_interior_voronoi_edge(const Facet& f) const {
		Cell_handle c = f.first;
		int id = f.second;
		Edge edge_of_facet; int interior_edge = 0;
		edge_of_facet = make_triple(c, (id+1)%4, (id+2)%4);
		if (visited_edges.find(create_vertex_pair_from_edge(edge_of_facet)) == visited_edges.end() &&
			is_medial_interior_voronoi_facet(edge_of_facet)) interior_edge++;
		edge_of_facet = make_triple(c, (id+2)%4, (id+3)%4);
		if (visited_edges.find(create_vertex_pair_from_edge(edge_of_facet)) == visited_edges.end() &&
			is_medial_interior_voronoi_facet(edge_of_facet)) interior_edge++;
		edge_of_facet = make_triple(c, (id+3)%4, (id+1)%4);
		if (visited_edges.find(create_vertex_pair_from_edge(edge_of_facet)) == visited_edges.end() &&
			is_medial_interior_voronoi_facet(edge_of_facet)) interior_edge++;
//		if (interior_edge == 2)	std::cout << LOG_BLUE << "    medial voronoi edge has adjacent voronoi facets: " << interior_edge << std::endl;
//		if (interior_edge == 1) std::cout << LOG_RED <<  "    medial voronoi edge has adjacent voronoi facets: " << interior_edge << std::endl;
//		std::cout << "    medial voronoi edge has adjacent voronoi facets: " << interior_edge << std::endl;
		return interior_edge > 1;
	}

	// returns true if the medial facet is considered 
	bool is_front_voronoi_facet(const Edge& e) const {
		if (visited_edges.find(create_vertex_pair_from_edge(e)) != visited_edges.end()) return false;
		if (!is_medial_interior_voronoi_facet(e)) return false;
		Facet_circulator f_start, f_circ = incident_facets(e);
		f_start = f_circ;
		int status = 0, first_status = 0; // 0 unintialized, 1 interior medial axis, 2 boundary medial axis, 
		int change = 0;
//		std::cout << LOG_GREEN << "------------- CHECKING ONE VORONOI FACET FOR FRONT" << std::endl;
		do {
//			std::cout << "new voronoi edge being looked at" << std::endl;
			Cell_handle vor_vert1 = f_circ->first;
			Cell_handle vor_vert2 = f_circ->first->neighbor(f_circ->second);
			if (!is_infinite(vor_vert1) && vor_vert1->get_medial_classification() == M_INTERIOR && !is_infinite(vor_vert2) && vor_vert2->get_medial_classification() == M_INTERIOR) {
//				std::cout << "medial voronoi edge" << std::endl;
				if (is_medial_interior_voronoi_edge(*f_circ)) {
//					std::cout << "    interior voronoi edge" << std::endl;
					if (status!=0 && status!=1) change++;
					if (status==0) first_status = 1;
					status = 1;
				} else {
//					std::cout << LOG_RED << "    boundary voronoi edge" << std::endl;
					if (status!=0 && status!=2) change++;
					if (status==0) first_status = 2;
					status = 2;
				}
//				std::cout << "status: " << status << " change: " << change << std::endl;
			} else {
//				std::cout << "not medial voronoi edge" << std::endl;
				// not interior so it corresponds to boundary medial axis face if we find any interior vertices
				// if we don't find any interior vertices, all status will be 2 and no change -> not front
				if (status!=0 && status!=2) change++;
				if (status==0) first_status = 2;
				status = 2;

			}
//			std::cout << "status: " << status << " change: " << change << std::endl;
			f_circ++;
		} while (f_circ!=f_start);
		if (first_status!=status) 
			change++;

//		std::cout << "Done" << std::endl;

		
//		if (change%2 == 1) std::cout << LOG_WARNING << "Detecting if an voronoi face is front has given strange stuff" << std::endl;
//		std::cout << "Change value: " << change << std::endl;
		if (change>2 || change == 0) return false;
		return true;
	}

	typedef typename CGAL::Triple<Edge, Point_3, double> Edge_with_ball;

	struct radius_comparator {
		bool operator()(const Edge_with_ball& e1, const Edge_with_ball& e2) const {
			// compare by radius and
			// if radii are the same sort by cell handle pointers
			if (e1.third != e2.third) {
				return e1.third > e2.third;
//				std::cout << "radii different" << std::endl;
			} else {
//				std::cout << "radii same" << std::endl;
				return &(*(e1.first.first)) > &(*(e2.first.first));
			}
		}
	};


	void compute_scale_filtration_values() {


		std::set<Edge_with_ball, radius_comparator> medial_edges;
		std::cout << PROGRESS_STATUS << "Computing radiuses for Voronoi faces and sorting them by radius" << std::endl;
		All_edges_iterator e_it, e_end = all_edges_end();
		for (e_it=all_edges_begin(); e_it!=e_end; ++e_it) {
			// compute the baricenter of the voronoi face part of the medial axis
			Cell_circulator c_start, c_circ = incident_cells(*e_it);
			c_start = c_circ;
			Point_3 sum = Point_3(0.0, 0.0, 0.0); int i=0;
//			bool all_interior = true;
			do {
				if (!is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
					sum = sum + (voronoi_vertex(c_circ)-CGAL::ORIGIN);
					i++;
				}
				c_circ++;
			} while (c_circ!=c_start);
			sum = CGAL::ORIGIN + ( (sum-CGAL::ORIGIN)*1.0/float(i));

			if (i>2) {
				double radius = sqrt(CGAL::to_double(squared_distance(sum, e_it->first->vertex(e_it->second)->point())));
				Edge_with_ball ewb = make_triple(*e_it, sum, radius);
				medial_edges.insert(ewb);
			}
		}

		std::cout << PROGRESS_DONE << std::endl;

		scale_values.clear();

		std::cout << PROGRESS_STATUS << "Comparing ball coverages ... ugly n squared" << std::endl;
		std::cout << PROGRESS_MAXIMUM << medial_edges.size() << std::endl;
		std::set<Edge_with_ball, radius_comparator>::iterator ewb_it, ewb_end = medial_edges.end(); int count = 0;
		for (ewb_it = medial_edges.begin(); ewb_it!=ewb_end; ++ewb_it, count++) {
			std::cout << PROGRESS_VALUE << count << std::endl;
			Point_3 large_center = ewb_it->second;
			double large_radius = ewb_it->third;
//			std::cout << "center: " << large_center << " radius: " << large_radius << std::endl;
			std::set<Edge_with_ball, radius_comparator>::iterator s_ewb_it, s_ewb_end = medial_edges.end();
			for (s_ewb_it = ewb_it; s_ewb_it!=s_ewb_end; ++s_ewb_it) {
				Point_3 small_center = s_ewb_it->second;
				double small_radius = s_ewb_it->third;
				double distance = sqrt(CGAL::to_double(squared_distance(small_center, large_center)));
				double scale = distance/(large_radius-small_radius);
				if (scale!=scale) scale = 100;
//				Pair_edge pe = create_pair_edge(&(s_ewb_it->first));
//				scale_values[s_ewb_it->first] = scale;
				Vertex_pair vp = create_vertex_pair_from_edge(s_ewb_it->first);
				if (scale_values.find(vp) == scale_values.end()) {
					scale_values[vp] = scale;
				} else {
					double old_scale = scale_values[vp];
					scale_values[vp] = scale < old_scale ? scale : old_scale;
				}
			}
		}
		std::cout << PROGRESS_DONE << std::endl;

	}


	//Pair_edge create_pair_edge(Edge* e) {
	//	return make_pair(e->first, e->second*10 + e->third);
	//}

	//int edge_index_in_cell(Edge* e) {
	//	int small_i = e->second;
	//	int large_i = e->third;
	//	if (small_i > large_i) {
	//		small_i = e->third;
	//		large_i = e->second;
	//	}
	//	
	//	// 0,1 -> 0
	//	// 1,2 -> 1
	//	// 2,3 -> 2
	//	if (large_i-small_i == 1) return small_i; 

	//	// 0,2 -> 5
	//	if (large_i+small_i == 2) return 5;

	//	// 0,3 -> 3
	//	// 1,3 -> 4
	//	return large_i+small_i;
	//}

	void flood_voronoi_classification() {
		if (number_of_vertices() < 4) return;

		std::cout << PROGRESS_STATUS << "Flooding Voronoi diagram to classify voronoi vertices" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM << "0" << std::endl;

		std::queue<Cell_handle> CQ;

		//// infinite cells labeled as exterior
		Cell_vector cells; Insert_iterator ii(cells); incident_cells(infinite_vertex(), ii);
		Cell_vector_iterator c_it, c_end = cells.end();
		for (c_it = cells.begin(); c_it != c_end; c_it++) {
			std::cout << PROGRESS_VALUE << 0 << std::endl;
			(*c_it)->set_medial_classification(M_EXTERIOR);
			CQ.push(*c_it);
		}

		// put the classified cells in the queue
		Finite_cells_iterator fc_it, fc_end = finite_cells_end();
		for (fc_it = finite_cells_begin(); fc_it!=fc_end; fc_it++) {
			if (fc_it->get_medial_classification() == M_EXTERIOR /*!= M_UNCLASSIFIED*/)
				CQ.push(fc_it);
		}

		// flood through Voronoi diagram to classify the vertices which can be reached from the classified vertices
		while (!CQ.empty()) {
			std::cout << PROGRESS_VALUE << 0 << std::endl;
			Cell_handle c = CQ.front(); CQ.pop();
			assert(c->get_medial_classification() != M_UNCLASSIFIED);
			for (int i=0; i<4; i++) {
				Cell_handle n_c = c->neighbor(i);
				if (c->get_crossing_classification(i) == C_UNCLASSIFIED && n_c->get_medial_classification() == M_UNCLASSIFIED) {
					n_c->set_medial_classification(c->get_medial_classification());
					CQ.push(n_c);
				}
			}
		}


//		std::deque <Vertex_handle> Q;
//		std::deque <Vertex_handle> P;
//		
//		// populate queue with all vertices
//		Finite_vertices_iterator f_it, f_end = finite_vertices_end();
//		for (f_it = finite_vertices_begin(); f_it != f_end; f_it++) {
//			Q.push_back(f_it);
//		}
//
//		// process queue
//		int last_size; // how many vertices we started with
//		bool completely_classified;
//		do {
//			last_size = Q.size();
//			std::cout << LOG_BLUE << "Q size in flooding: "<< Q.size() << std::endl;
//			P.clear();
//			while (!Q.empty()) {
//				std::cout << PROGRESS_VALUE << 0 << std::endl;
////				std::cout << LOG_BLUE << "a vertex popped" << std::endl;
//				Vertex_handle v = Q.front(); Q.pop_front();
//				// get a classified cell (Voronoi vertex)
//				Cell_handle root_c = root_cell_for_vertex(v, completely_classified);
//				if (root_c!=0) {
//					//std::cout << LOG_GREEN << "Root cell with classification: " << 
//					//             ((root_c->get_medial_classification() == M_INTERIOR) ? "INT" : 
//					//			 (root_c->get_medial_classification() == M_EXTERIOR) ? "EXT" : "UNCLASSIFIED");
//					//std::cout << " and it "<< (completely_classified ? "IS" : "is NOT") <<" completely classified "<< std::endl;
//				}
//				else std::cout << LOG_GREEN << "Root cell not found" << std::endl;
//				// if we have a root_c, classify the rest of the voronoi cell
//				if (root_c!=0 && !completely_classified) classify_voronoi_cell(v, root_c);
//				// if we didn't have a root_c or it was interior keep the cell to have it classified again
//				if (root_c==0 || (!completely_classified && root_c->get_medial_classification() == M_INTERIOR) )
//					P.push_back(v);
//			}
//			Q = P;
//		} while (0 < Q.size() && Q.size() < last_size);
//		if (!Q.empty()) 
//			std::cout << LOG_ERROR << "Flooding for V Voronoi classification is not complete, " << Q.size() << " cells have unclassified vertices" << std::endl;
//		else std::cout << LOG_BLUE << "Flooding for V Voronoi classification finished" << std::endl;


		std::cout << PROGRESS_DONE << std::endl;

		flooded = true;
	}

	Cell_handle root_cell_for_vertex(Vertex_handle v, bool& completely_classified) {
		completely_classified = true; Cell_handle exterior_root = 0, interior_root = 0;
		Cell_vector cells; Insert_iterator ii(cells); incident_cells(v, ii);
		Cell_vector_iterator c_it, c_end = cells.end();
		for (c_it = cells.begin(); c_it != c_end; c_it++) {
			if ((*c_it)->get_medial_classification() == M_INTERIOR) {
				interior_root = *c_it;
//				std::cout << LOG_BLUE << "interior root set" << std::endl;
			}
			else if ((*c_it)->get_medial_classification() == M_EXTERIOR) {
				exterior_root = *c_it;
//				std::cout << LOG_BLUE << "exterior root set" << std::endl;
			}
			else {
//				std::cout << LOG_BLUE << "unclassified found" << std::endl;
				completely_classified = false;
			}
		}
		if (exterior_root != 0) {
//			std::cout << "return exterior node" << std::endl;
			return exterior_root;
		}
		else {
//			std::cout << "return interior node" << std::endl;
			return interior_root;
		}

	}

	void classify_voronoi_cell(Vertex_handle v, Cell_handle root_c) {
		int id = root_c->index(v);
		M_classification l = root_c->get_medial_classification();
		std::queue<Facet> Q;
		std::set<Facet> visited_facets;
		std::set<Facet>::iterator visited_facets_end = visited_facets.end();

//		std::cout << LOG_RED << "classify voronoi cell which has vertex as index: " << id << std::endl;

		// add the three voronoi edges to the queue if they are not crossing and do not lead to 
		for (int i=1; i<4; i++) {
			int index = (id+i)%4;
			Cell_handle nc = root_c->neighbor(index);
			if (root_c->get_crossing_vertex(index) == 0) {
				int nindex = root_c->index(nc);
				Q.push(Facet(nc,nindex));
				visited_facets.insert(Facet(nc,nindex));
				visited_facets.insert(Facet(root_c,index));
			}
		}

		// flood until one gets to crossing or classified edges
		while (!Q.empty()) {
			// set the label to the new Voronoi vertex
			Facet f = Q.front(); Q.pop();
			Cell_handle new_c = f.first;
			int id = f.second;
			new_c->set_medial_classification(l);
//			std::cout << LOG_RED << "medial classification set to " << ((l == M_INTERIOR) ? "INT" : 
//								 (l == M_EXTERIOR) ? "EXT" : "UNCLASSIFIED") << std::endl;
			int v_id = new_c->index(v);
//			std::cout << LOG_RED << "vertex index for this cell is: " << v_id << std::endl;
			for (int i=1; i<4; i++) {
				int index = (v_id+i)%4;
				Cell_handle neighbor_cell = new_c->neighbor(index);
				int neighbor_id = neighbor_cell->index(new_c);
				if (new_c->get_crossing_vertex(index)==0 &&
					visited_facets.find(Facet(neighbor_cell,neighbor_id))==visited_facets.end() &&
					visited_facets.find(Facet(new_c,index))==visited_facets.end()
					) {
						Q.push(Facet(neighbor_cell,neighbor_id));
						visited_facets.insert(Facet(neighbor_cell,neighbor_id));
						visited_facets.insert(Facet(new_c,index));
//						std::cout << LOG_GREEN << "REAL FLOOD facet added to the queue" << std::endl;
				}
			}
		}

		if (l==M_EXTERIOR) {
			Cell_vector cells; Insert_iterator ii(cells); incident_cells(v, ii);
			Cell_vector_iterator c_it, c_end = cells.end();
			for (c_it = cells.begin(); c_it != c_end; c_it++) {
				if ((*c_it)->get_medial_classification() == M_UNCLASSIFIED) {
					if ( (*c_it)->get_crossing_vertex(0)==0 && (*c_it)->get_crossing_vertex(1)==0 &&
						 (*c_it)->get_crossing_vertex(2)==0 && (*c_it)->get_crossing_vertex(3)==0)
							(*c_it)->set_medial_classification(M_INTERIOR);
//					std::cout << LOG_RED << "medial classification set to INT in the extra stage" << std::endl;
				}
			}

		}


	}


	Edges_with_doubles scale_values;
	Edges_with_doubles angle_values;
	Edges_with_doubles topology_angle_values;
	Edges_set visited_edges;
	Edges_set medial_edges;
	std::multimap<double, Vertex_pair> front;

private:
	bool classified;
	bool flooded;
	double front_value;
//	stdext::hash_map<Edge, double> scale_values;
};

#endif //MESECINA_V_TRIANGULATION_3_H