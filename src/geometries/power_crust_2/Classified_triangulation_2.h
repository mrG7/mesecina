/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Classified_triangulation_2.h 531 2009-02-05 22:02:06Z miklosb $
 */

#ifndef MESECINA_CLASSIFIED_TRIANGULATION_2_H
#define MESECINA_CLASSIFIED_TRIANGULATION_2_H

#define PI 3.14159265358979323846

#include <constants.h>
#include "Classified_face_base_2.h"
#include "Classified_vertex_base_2.h"
#include <CGAL/Weighted_point.h>
#include <CGAL/Triangulation_vertex_base_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <queue>
#include <QVector>


template <class Gt>
class Classified_triangulation_2 : public CGAL::Delaunay_triangulation_2 < 
	Gt, CGAL::Triangulation_data_structure_2 < Classified_vertex_base_2<Gt> , Classified_face_base_2<Gt> > 
	>
{
public:
	typedef Gt Geom_traits;
	typedef typename CGAL::Triangulation_data_structure_2 < Classified_vertex_base_2<Gt> , Classified_face_base_2<Gt>  > Tds;
	typedef typename Geom_traits::FT			Coord_type;
	typedef typename Geom_traits::Point_2       Point_2;
	typedef typename CGAL::Weighted_point<Point_2,Coord_type> Weighted_point;
	typedef typename Geom_traits::Vector_2       Vector_2;
	typedef typename Geom_traits::Segment_2     Segment_2;
	typedef typename Geom_traits::Triangle_2    Triangle_2;
	typedef typename CGAL::Delaunay_triangulation_2 <Gt, Tds >::Vertex_handle Vertex_handle;
	typedef typename CGAL::Delaunay_triangulation_2 <Gt, Tds >::Face_handle Face_handle;
	typedef typename CGAL::Delaunay_triangulation_2 <Gt, Tds >::Finite_faces_iterator Finite_faces_iterator;
	typedef typename CGAL::Delaunay_triangulation_2 <Gt, Tds >::All_faces_iterator All_faces_iterator;
	typedef typename CGAL::Delaunay_triangulation_2 <Gt, Tds >::Finite_vertices_iterator Finite_vertices_iterator;
	typedef typename CGAL::Delaunay_triangulation_2 <Gt, Tds >::Finite_edges_iterator Finite_edges_iterator;
	typedef typename CGAL::Delaunay_triangulation_2 <Gt, Tds >::Face_circulator Face_circulator;
	typedef typename CGAL::Delaunay_triangulation_2 <Gt, Tds >::Vertex_circulator Vertex_circulator;
	typedef typename CGAL::Delaunay_triangulation_2 <Gt, Tds >::Edge_circulator Edge_circulator;


	Classified_triangulation_2(const Gt& gt = Gt())
		: CGAL::Delaunay_triangulation_2<Gt,Tds>(gt) { lambda_classified = crust_classified = classified = false; }

	Classified_triangulation_2(const Classified_triangulation_2<Gt> &tr) : CGAL::Delaunay_triangulation_2<Gt,Tds>(tr)
	{   
	//CGAL::CGAL_triangulation_postcondition( is_valid() ); /// \todo check what to do, removed because of mac version
		classified = tr.classified; 
		crust_classified = tr.crust_classified;
		lambda_classified = tr.lambda_classified;
	}

	void set_dirty() { lambda_classified = crust_classified = classified = false; } /// \todo check if we could make it so that it is done by the insert...

	bool is_dirty() { return !classified; }

	bool is_crust_dirty() { return !crust_classified; }

	bool is_lambda_dirty() { return !lambda_classified; }

	struct Scale_edge {
		Edge e;
		Scale_edge( const Edge& _e) : e(_e) {};

		bool operator<(const Scale_edge &o) const {
			return e.first->get_get_edge_scale(e.second) > o.e.first->get_get_edge_scale(o.e.second);
		}
	};

	void compute_lambda_angle_classification() {
		if (number_of_faces () < 2) return;
		std::cout << "Starting to compute one-step crust..." << std::endl;

		Finite_faces_iterator f_it, f_end = finite_faces_end();
		for (f_it = finite_faces_begin(); f_it != f_end; ++f_it) {
			f_it->set_vertex_lambda(0);
			f_it->set_vertex_angle(0);
			f_it->set_vertex_scale(0);
		}

		std::cout << PROGRESS_STATUS << "Computing lambda values for vertices" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM <<  tds().number_of_edges()-1 << std::endl; int i =0;
		for (Finite_edges_iterator e_it = finite_edges_begin(), e_end = finite_edges_end();	e_it != e_end; ++e_it, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			
			Face_handle f = e_it->first;
			int id = e_it->second;
			f->set_flood_visited(id, false); // this is for the scale values later
			Face_handle nF = f->neighbor(id);
			int nId = nF->index(f);
			nF->set_flood_visited(nId, false); // this is for the scale values later

			double el = lambda_edge(*e_it);
			double al = angle_edge(*e_it);
//			std::cout << LOG_GREEN << "el: " << el << std::endl;
			if (f->get_vertex_lambda() < el) f->set_vertex_lambda(el);
			if (nF->get_vertex_lambda() < el) nF->set_vertex_lambda(el);
			if (f->get_vertex_angle() < al) f->set_vertex_angle(al);
			if (nF->get_vertex_angle() < al) nF->set_vertex_angle(al);
		}
		std::cout << PROGRESS_DONE << std::endl;

		std::cout << PROGRESS_STATUS << "Computing scale values for vertices (very naive n squared)" << std::endl; i=0;
		std::cout << PROGRESS_MAXIMUM <<  number_of_faces() << std::endl;

		for (Finite_faces_iterator f_it = finite_faces_begin(), f_end = finite_faces_end();	f_it != f_end; ++f_it) {
			Point c1 = dual(f_it);
			f_it->set_circumcenter(c1);
		}


		for (Finite_faces_iterator f_it = finite_faces_begin(), f_end = finite_faces_end();	f_it != f_end; ++f_it, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			if (f_it->get_onestep_classification() == O_INTERIOR) {
				Point c1 = f_it->get_circumcenter();
				double r1_sq = CGAL::to_double(squared_distance(f_it->vertex(0)->point(), c1));
				f_it->set_vertex_scale(100000);
				for (Finite_faces_iterator f2_it = finite_faces_begin(), f2_end = finite_faces_end(); f2_it != f2_end; ++f2_it) {
					if (f2_it->get_onestep_classification() == O_INTERIOR) {
						Point c2 = f2_it->get_circumcenter();
						double r2_sq = CGAL::to_double(squared_distance(f2_it->vertex(0)->point(), c2));

						if (r2_sq > r1_sq) {
							double d = sqrt(CGAL::to_double(squared_distance(c1,c2)));
							double s = d / (sqrt(r2_sq) - sqrt(r1_sq));
							//std::cout << "circumcenter 1: " << c1 << " radius 1 " << sqrt(r1_sq) << std::endl;
							//std::cout << "circumcenter 3: " << c2 << " radius 2 " << sqrt(r2_sq) << std::endl;
							//std::cout << "d: " << d << std::endl;
							//std::cout << LOG_BLUE << "s: " << s << std::endl;


							if (s < f_it->get_vertex_scale()) {
								//						std::cout << " updating scale value of a ball" << std::endl;
								f_it->set_vertex_scale(s);
							}
						}
					}
				}
			}
		}

//		if Application_settings::get_bool_setting("Scale values fixed
		// init front and set edge scale values to the right thing
		std::multimap<double, Edge> front;
		front_value = 0;
		for (Finite_edges_iterator e_it = finite_edges_begin(), e_end = finite_edges_end();	e_it != e_end; ++e_it) {
			Face_handle f = e_it->first;
			int id = e_it->second;
			Face_handle nF = f->neighbor(id);
			int nId = nF->index(f);
			double es = f->get_vertex_scale() < nF->get_vertex_scale() ? f->get_vertex_scale() : nF->get_vertex_scale();
			f->set_edge_scale(id, es);
			nF->set_edge_scale(nId, es);

			if (is_front(*e_it)) front.insert(std::make_pair(scale_edge(*e_it),*e_it));
		}
		std::cout << LOG_BLUE << "Front init size: " << front.size() << std::endl;

		// traverse
		int counter=0;
		while (!front.empty()/* && counter < 1*/) {
			std::multimap<double, Edge>::iterator i;
			i = front.begin();
			front_value = (*i).first;
//			std::cout << "value: " << (*i).first << std::endl;
			Edge e = (*i).second;

			Face_handle f = e.first;
			int id = e.second;
			Face_handle nF = f->neighbor(id);
			int nId = nF->index(f);

			f->set_flood_visited(id,true);
			nF->set_flood_visited(nId,true);

			Edge n_e;
			n_e = make_pair(f,(id+1)%3);
			if (is_front(n_e)) instert_and_increase_scale(n_e,front);
			n_e = make_pair(f,(id+2)%3);
			if (is_front(n_e)) instert_and_increase_scale(n_e,front);

			n_e = make_pair(nF,(nId+1)%3);
			if (is_front(n_e)) instert_and_increase_scale(n_e,front);
			n_e = make_pair(nF,(nId+2)%3);
			if (is_front(n_e)) instert_and_increase_scale(n_e,front);

			front.erase(i);
			counter++;

		}
		std::cout << LOG_BLUE << "Front end size: " << front.size() << std::endl;

		// non-visited put ridiculously high stability
		int non_visited = 0;
		for (Finite_edges_iterator e_it = finite_edges_begin(), e_end = finite_edges_end();	e_it != e_end; ++e_it) {
			Face_handle f = e_it->first;
			int id = e_it->second;
			Face_handle nF = f->neighbor(id);
			int nId = nF->index(f);
			if (!f->is_flood_visited(id) && f->get_onestep_classification() == O_INTERIOR && nF->get_onestep_classification() == O_INTERIOR) {
				non_visited++;
				f->set_edge_scale(id, 10000);
				nF->set_edge_scale(nId, 10000);
			}
		}
		std::cout << LOG_GREEN << "Non_visited size: " << non_visited << std::endl;

		// adjusting vertex stabilities
		for (Finite_faces_iterator f_it = finite_faces_begin(), f_end = finite_faces_end();	f_it != f_end; ++f_it, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			if (f_it->get_onestep_classification() == O_INTERIOR) {
				double max = -1; Face_handle n_f;
				n_f = f_it->neighbor(0);
				if (n_f->get_onestep_classification() == O_INTERIOR)
					max = max < n_f->get_edge_scale(0) ?  n_f->get_edge_scale(0) : max;
				n_f = f_it->neighbor(1);
				if (n_f->get_onestep_classification() == O_INTERIOR)
					max = max < n_f->get_edge_scale(1) ?  n_f->get_edge_scale(1) : max;
				n_f = f_it->neighbor(2);
				if (n_f->get_onestep_classification() == O_INTERIOR)
					max = max < n_f->get_edge_scale(2) ?  n_f->get_edge_scale(2) : max;
				f_it->set_vertex_scale(max);
			}
		}



		//for (Finite_faces_iterator f_it = finite_faces_begin(), f_end = finite_faces_end();	f_it != f_end; ++f_it) {
		//	std::cout << "vertex scale value: " << f_it->get_vertex_scale() << std::endl;
		//}
		std::cout << PROGRESS_DONE << std::endl;


		lambda_classified = true;

	}

	void instert_and_increase_scale(const Edge& e, std::multimap<double, Edge>& front) {
		if (scale_edge(e)<front_value) {
			Face_handle f = e.first;
			int id = e.second;
			Face_handle nF = f->neighbor(id);
			int nId = nF->index(f);
			f->set_edge_scale(id, front_value);
			nF->set_edge_scale(nId, front_value);
		}
		front.insert(std::make_pair(scale_edge(e),e));
	}

	bool is_front(const Edge& e) {
		Face_handle f = e.first;
		int id = e.second;
		Face_handle nF = f->neighbor(id);
		int nId = nF->index(f);
		if (f->is_flood_visited(id)) return false;
		if (f->get_onestep_classification() == O_INTERIOR && nF->get_onestep_classification() == O_INTERIOR) {
			int f_neighbor_edges = 0;
			if (f->neighbor((id+1)%3)->get_onestep_classification() == O_INTERIOR && !f->is_flood_visited((id+1)%3)) f_neighbor_edges++;
			if (f->neighbor((id+2)%3)->get_onestep_classification() == O_INTERIOR && !f->is_flood_visited((id+2)%3)) f_neighbor_edges++;
			int nF_neighbor_edges = 0;
			if (nF->neighbor((nId+1)%3)->get_onestep_classification() == O_INTERIOR && !nF->is_flood_visited((nId+1)%3)) nF_neighbor_edges++;
			if (nF->neighbor((nId+2)%3)->get_onestep_classification() == O_INTERIOR && !nF->is_flood_visited((nId+2)%3)) nF_neighbor_edges++;
			if (nF_neighbor_edges == 0 || f_neighbor_edges == 0) return true;
		}
		return false;
	}

	void compute_crust_classification() {
		if (number_of_faces () < 2) return;
		std::cout << "Starting to compute one-step crust..." << std::endl;

		All_faces_iterator f_it, f_end = all_faces_end();
		for (f_it = all_faces_begin(); f_it != f_end; ++f_it) {
			f_it->set_onestep_classification(O_UNCLASSIFIED);
		}

		Finite_vertices_iterator v_it, v_end = finite_vertices_end();
		for (v_it = finite_vertices_begin(); v_it != v_end; ++v_it) {
			v_it->reset_count_reconstruction_edges();
		}

		std::queue<Face_handle> Q;
		std::cout << PROGRESS_STATUS << "Computing one-step crust" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM <<  tds().number_of_edges()-1 << std::endl; int i =0;
		for (Finite_edges_iterator e_it = finite_edges_begin(), e_end = finite_edges_end();	e_it != e_end; ++e_it, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			Face_handle f = e_it->first;
			int id = e_it->second;
			Face_handle nF = f->neighbor(id);
			int nId = nF->index(f);

			Point_2 p = f->vertex(id)->point();
			Point_2 q = f->vertex((id+1)%3)->point();
			Point_2 r = f->vertex((id+2)%3)->point();
			Point_2 s = nF->vertex(nId)->point();

			if (is_infinite(nF->vertex(nId))) {
				nF->set_onestep_classification(O_EXTERIOR);
				Q.push(nF);
				Point_2 vor = dual(f);
				if (CGAL::orientation(q,r,vor)==CGAL::LEFT_TURN) {
					f->set_edge_classification(id, C_CRUST);
					nF->set_edge_classification(nId, C_CRUST);
				} else {
					f->set_edge_classification(id, C_UNCLASSIFIED);
					nF->set_edge_classification(nId, C_UNCLASSIFIED);
				}
			} else if (is_infinite(f->vertex(id))) {
				f->set_onestep_classification(O_EXTERIOR);
				Q.push(f);
				Point_2 vor = dual(nF);
				if (CGAL::orientation(r,q,vor)==CGAL::LEFT_TURN) {
					f->set_edge_classification(id, C_CRUST);
					nF->set_edge_classification(nId, C_CRUST);
				} else {
					f->set_edge_classification(id, C_UNCLASSIFIED);
					nF->set_edge_classification(nId, C_UNCLASSIFIED);
				}
			} else {
				Vector_2 sq = s - q;
				Vector_2 sr = s - r;
				Vector_2 pq = p - q;
				Vector_2 pr = p - r;
				Vector_2 rq = r - q;
				Vector_2 v(rq.y(),-1*rq.x());

				if ((sq*sr) * (pq*pr) >= (-1) * (sr*v) * (pq*v)) {
					f->set_edge_classification(id, C_CRUST);
					nF->set_edge_classification(nId, C_CRUST);
				} else {
					f->set_edge_classification(id, C_UNCLASSIFIED);
					nF->set_edge_classification(nId, C_UNCLASSIFIED);
				}
			}
			if (f->get_edge_classification(id) == C_CRUST) {
				f->vertex((id+1)%3)->inc_count_reconstruction_edges();
				f->vertex((id+2)%3)->inc_count_reconstruction_edges();
			}
		}
		std::cout << PROGRESS_DONE << std::endl;

#ifndef GINA_2
		std::cout << PROGRESS_STATUS << "Flooding Voronoi diagram to classify voronoi vertices from onestep" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM << number_of_vertices()-1 << std::endl; i=0;
		inner_balls.clear();
		inner_balls_floats.clear();
		while (!Q.empty()) {
			Face_handle f = Q.front(); Q.pop();
			assert(f->get_onestep_classification()!=O_UNCLASSIFIED);
			Point_2 vor = dual(f);
			if (f->get_onestep_classification()==O_INTERIOR) {
				inner_balls_floats.push_back(Point3D(CGAL::to_double(vor.x()),
										      CGAL::to_double(vor.y()),
											  CGAL::sqrt(CGAL::to_double(squared_distance(vor, f->vertex(0)->point())))
											  ));
				inner_balls.push_back(Weighted_point(vor, squared_distance(vor, f->vertex(0)->point())));
			}

//			Coord_type sq_rad = squared_distance(vor, f->vertex(0)->point());
			for (int id=0; id<3; id++) {
				Face_handle nF = f->neighbor(id);
				if (nF->get_onestep_classification() == O_UNCLASSIFIED) {
					std::cout << PROGRESS_VALUE << i << std::endl; i++;
					if (!is_infinite(nF)) {
						if (f->get_edge_classification(id) != C_CRUST) {
							nF->set_onestep_classification(f->get_onestep_classification());
						} else {
							if (f->get_onestep_classification() == O_INTERIOR) 
								nF->set_onestep_classification(O_EXTERIOR);
							else {
								nF->set_onestep_classification(O_INTERIOR);
							}
						}
						Q.push(nF);
					} else {
						nF->set_onestep_classification(O_EXTERIOR);
						//Q.push(nF);
					}
				}
			}
		}
		std::cout << PROGRESS_DONE << std::endl;
//		std::cout << "Computed interior balls size: " << inner_balls.size() << std::endl;
#endif

		Vertex_handle current_vertex = 0;
		bool not_a_curve = false;
		ordered_points.clear();

		// check if reconstruction is one single curve
		v_end = finite_vertices_end();
		for (v_it = finite_vertices_begin(); v_it != v_end /*&& to_start == 0*/; ++v_it) {
			if (v_it->get_count_recontruction_edges() == 1) {
				current_vertex = v_it;
			} 
			if (v_it->get_count_recontruction_edges() > 2) {
				not_a_curve = true;
			}
		}

		if (not_a_curve) {std::cout << "WARNING: Reconstruction is not a curve!" << std::endl;}

		crust_classified = true;

		// order points for the curve reconstruction
		if (current_vertex==0) current_vertex = finite_vertices_begin();
		ordered_points.push_back(QPointF(CGAL::to_double(current_vertex->point().x()), CGAL::to_double(current_vertex->point().y())));
		Vertex_handle last_vertex = 0;

		int number_of_points = (int)number_of_vertices();
		bool done = true;
		while (ordered_points.size() < number_of_points+1 && done &&
			(ordered_points.size() == 1 || ordered_points[0]!=ordered_points[ordered_points.size()-1])
			) {
				Edge_circulator e_start, e_circ;
				e_start = e_circ = incident_edges(current_vertex);
				done = false;
				do {
					Face_handle f = e_circ->first;
					int id = e_circ->second;
					if (f->get_edge_classification(id) == C_CRUST) {
						Vertex_handle other_vertex = f->vertex((id+1)%3);
						if (other_vertex == current_vertex) other_vertex = f->vertex((id+2)%3);
						if (other_vertex != last_vertex) {
							last_vertex = current_vertex;
							current_vertex = other_vertex;
							ordered_points.push_back(
								QPointF(CGAL::to_double(current_vertex->point().x()), CGAL::to_double(current_vertex->point().y())));
							done = true;
						}
					}

					e_circ++;
				} while ((e_start != e_circ) && !done);
				//if (!done) std::cout << "end of curve has been found" << std::endl;
		}
		//std::cout << "given curve size: " << ordered_points.size() << std::endl;
	}

	void* get_ordered_onestep_points() {
		return &ordered_points;
	}

	//void compute_classification() {
	//	if (number_of_faces () < 2) return;
	//	std::cout << "Starting to classify voronoi vertices..." << std::endl;

	//	//cleaning up poles and classifications
	//	All_faces_iterator f_it, f_end = all_faces_end();
	//	for (f_it = all_faces_begin(); f_it != f_end; ++f_it) {
	//		f_it->set_opposite_pole(0);
	//		f_it->set_classification(M_UNCLASSIFIED);
	//	}

	//	std::cout << PROGRESS_STATUS << "Identifying poles" << std::endl;
	//	std::cout << PROGRESS_MINIMUM << "0" << std::endl;
	//	std::cout << PROGRESS_MAXIMUM <<  number_of_vertices()-1 << std::endl;
	//	//log(QObject::tr("Computing poles for %1 vertices...").arg(input_triangulation.number_of_vertices()));
	//	Face_handle inside_pole_seed = 0;  // this is the seed to separate inside/outside poles
	//	// iterate through vertices
	//	int i = 0;
	//	Finite_vertices_iterator v_it, v_end = finite_vertices_end();
	//	for (v_it = finite_vertices_begin(); v_it != v_end; ++v_it, i++) {
	//		std::cout << PROGRESS_VALUE << i << std::endl;
	//		Point_2 vertex = v_it->point();
	//		Point_2 pole = vertex;
	//		Vector_2 first_pole_direction;
	//		bool first_pole_is_infinite = false;
	//		Face_handle first_pole;
	//		Face_circulator f_it, f_begin;
	//		f_it = f_begin = incident_faces(v_it);
	//		// iterate through faces incident to v_it and find pole
	//		do {
	//			// if current voronoi vertex is the farthest, overwrite pole
	//			if (!is_infinite(f_it)) {
	//				Point_2 vor = dual(f_it);
	//				if (pole == vertex || has_larger_distance_to_point(vertex, vor, pole)) {
	//					pole = vor;
	//					first_pole = f_it;
	//				}
	//				++f_it;
	//			} else { first_pole_is_infinite = true; first_pole = f_it; }
	//		} while (f_it != f_begin && !first_pole_is_infinite);
	//		// add to regular triangulation the current pole
	//		if (first_pole_is_infinite) {
	//			Vertex_circulator vv_it = incident_vertices(v_it,f_it);
	//			while (!is_infinite(vv_it)) { 
	//				vv_it++; 
	//			}
	//			vv_it--; Point_2 ne1 = vv_it->point();
	//			Vector_2 n1(ne1 - vertex); // first convex hull neighbor vector
	//			vv_it++;
	//			assert(is_infinite(vv_it));
	//			vv_it++; Point_2 ne2 = vv_it->point();
	//			Vector_2 n2(vertex - ne2); // second convex hull neighbor vector
	//			n1 = Vector_2(-1*n1.y(), n1.x()); // rotate 90 degrees
	//			n1 = n1 / CGAL::sqrt(CGAL::to_double( n1.x()*n1.x() + n1.y()*n1.y())); // normalize
	//			n2 = Vector_2(-1*n2.y(), n2.x()); // rotate 90 degrees
	//			n2 = n2 / CGAL::sqrt(CGAL::to_double( n2.x()*n2.x() + n2.y()*n2.y())); // normalize
	//			first_pole_direction = n1 + n2; //add them together
	//		} else if (pole != vertex) {
	//			first_pole_direction = pole - vertex;
	//		}

	//		pole = vertex;
	//		f_it = f_begin = incident_faces(v_it);
	//		Face_handle inserted;
	//		do {
	//			// if current voronoi vertex is the farthest, overwrite pole
	//			if (!is_infinite(f_it)) {
	//				Point_2 vor = dual(f_it);
	//				if ( ((vor - vertex) * first_pole_direction < 0) &&
	//					(pole == vertex || has_larger_distance_to_point(vertex, vor, pole)) ) {
	//						inserted = f_it;
	//						pole = vor;
	//				}
	//			}
	//			++f_it;
	//		} while (f_it != f_begin);
	//		// add to regular triangulation the current pole
	//		if (pole != vertex) {
	//			if (inside_pole_seed == 0 && first_pole_is_infinite) {
	//				inside_pole_seed = inserted;
	//			}
	//			if (!first_pole_is_infinite) {
	//				first_pole->set_opposite_pole(inserted);
	//				inserted->set_opposite_pole(first_pole);
	//			}
	//		}
	//	}
	//	std::cout << PROGRESS_DONE << std::endl;
	//	
	//	inner_balls.clear();
	//	std::cout << PROGRESS_STATUS << "Flooding Voronoi diagram to classify voronoi vertices" << std::endl;
	//	std::cout << PROGRESS_MINIMUM << "0" << std::endl;
	//	std::cout << PROGRESS_MAXIMUM << "0" << std::endl;
	//	std::queue<Face_handle> Q;
	//	inside_pole_seed->set_classification(M_INTERIOR);
	//	inner_balls.push_back(Weighted_point(dual(inside_pole_seed), squared_distance(dual(inside_pole_seed), inside_pole_seed->vertex(0)->point())));
	//	Q.push(inside_pole_seed);

	//	while (!Q.empty()) {
	//		Face_handle f = Q.front(); Q.pop();
	//		Point_2 vor = dual(f);
	//		Coord_type sq_rad = squared_distance(vor, f->vertex(0)->point());
	//		for (int i=0; i<3; i++) {
	//			Face_handle nF = f->neighbor(i);
	//			if (nF->get_classification() == M_UNCLASSIFIED)
	//				if (!is_infinite(nF)) {
	//					Point_2 nVor = dual(nF);
	//					Coord_type n_sq_rad = squared_distance(nVor, nF->vertex(0)->point());
	//					if (squared_distance(vor, nVor) <
	//						(
	//						sq_rad + n_sq_rad -
	//						CGAL::sqrt(CGAL::to_double(2.0 * sq_rad * n_sq_rad)) /// \todo check why does it crash for MP_float
	//						) ) {
	//							nF->set_classification(f->get_classification());
	//							if (nF->is_interior()) inner_balls.push_back(Weighted_point(dual(nF), squared_distance(dual(nF), nF->vertex(0)->point())));
	//							Q.push(nF);
	//							Face_handle opp = nF->get_opposite_pole();
	//							if (opp!=0 && opp->get_classification() == M_UNCLASSIFIED) {
	//								opp->set_classification(nF->get_inverted_classification());
	//								if (opp->is_interior()) inner_balls.push_back(Weighted_point(dual(opp), squared_distance(dual(opp), opp->vertex(0)->point())));
	//								Q.push(opp);
	//							}
	//					}
	//				} else {
	//					nF->set_classification(M_EXTERIOR);
	//					//Q.push(nF);
	//				}
	//		}
	//	}
	//	std::cout << PROGRESS_DONE << std::endl;
	//	classified = true;
	//}

	std::list< Weighted_point>* get_inner_balls() {
		if (is_dirty()) compute_crust_classification();
		return &inner_balls;
	}

	std::list< Point3D>* get_inner_balls_floats() {
		if (is_dirty()) compute_crust_classification();
		return &inner_balls_floats;
	}

	Point_2 smooth_position(Vertex_handle  v, double speed_ratio, bool *has) {
		if (!crust_classified) compute_crust_classification();
		Edge_circulator e_start, e_circ;
		e_start = e_circ = incident_edges(v);
		Point_2 n1 = Point_2(0,0), n2 = Point_2(0,0); bool has_n1 = false, has_n2 = false;
		do {
			Face_handle f = e_circ->first;
			int id = e_circ->second;
			Face_handle nF = f->neighbor(id);
			//int nId = nF->index(f);
			Onestep_classification f_class = f->get_onestep_classification();
			Onestep_classification nF_class = nF->get_onestep_classification();
			if ( 
				((is_infinite(f) || f_class == O_EXTERIOR || f_class== O_UNCLASSIFIED) && nF_class == O_INTERIOR) ||
				((is_infinite(nF) || nF_class == O_EXTERIOR  || nF_class== O_UNCLASSIFIED) && f_class == O_INTERIOR)
				) {
					Vertex_handle other_vertex = f->vertex((id+1)%3);
					if (other_vertex == v) other_vertex = f->vertex((id+2)%3);
					if (has_n1) {
						n2 = other_vertex->point();
						has_n2 = true;
					} else {
						n1 = other_vertex->point();
						has_n1 = true;
					}
			}
			e_circ++;
		} while ((e_start != e_circ) && !has_n2);
		Point_2 middle = v->point(); *has = true;
		if (has_n1 && has_n2) middle = midpoint(n1,n2);
		else *has = false;

		if (speed_ratio == 0.5) return midpoint(middle, v->point());
		return middle;	
	}

	Point_2 noise_position(Vertex_handle  v, double noise_ratio, bool *has) {
		if (!crust_classified) compute_crust_classification();
		Edge_circulator e_start, e_circ;
		e_start = e_circ = incident_edges(v);
		Point_2 n1 = Point_2(0,0), n2 = Point_2(0,0); bool has_n1 = false, has_n2 = false;
		do {
			Face_handle f = e_circ->first;
			int id = e_circ->second;
			Face_handle nF = f->neighbor(id);
			//int nId = nF->index(f);
			Onestep_classification f_class = f->get_onestep_classification();
			Onestep_classification nF_class = nF->get_onestep_classification();
			if ( 
				((is_infinite(f) || f_class == O_EXTERIOR || f_class== O_UNCLASSIFIED) && nF_class == O_INTERIOR) ||
				((is_infinite(nF) || nF_class == O_EXTERIOR  || nF_class== O_UNCLASSIFIED) && f_class == O_INTERIOR)
				) {
					Vertex_handle other_vertex = f->vertex((id+1)%3);
					if (other_vertex == v) other_vertex = f->vertex((id+2)%3);
					if (has_n1) {
						n2 = other_vertex->point();
						has_n2 = true;
					} else {
						n1 = other_vertex->point();
						has_n1 = true;
					}
			}
			e_circ++;
		} while ((e_start != e_circ) && !has_n2);
		Point_2 middle = v->point(); *has = true; Vector_2 dir;
		if (has_n1 && has_n2) {
			dir = (n1-n2).perpendicular(CGAL::LEFT_TURN);
			dir = dir / CGAL::sqrt(CGAL::to_double(dir.squared_length()));
			dir = dir * CGAL::sqrt(CGAL::to_double((n1-n2).squared_length()));
		}
		else *has = false;
		
		double random_0_1 = (double)rand()/RAND_MAX;
		double random_1_1 = random_0_1*2.0 - 1.0;
		return v->point() + (dir*noise_ratio*random_1_1);
	}

	Point_2 loop_even(Vertex_handle  v, bool *has) {
		if (!crust_classified) compute_crust_classification();
		Edge_circulator e_start, e_circ;
		e_start = e_circ = incident_edges(v);
		Point_2 n1 = Point_2(0,0), n2 = Point_2(0,0); bool has_n1 = false, has_n2 = false;
		do {
			Face_handle f = e_circ->first;
			int id = e_circ->second;
			Face_handle nF = f->neighbor(id);
			//int nId = nF->index(f);
			Onestep_classification f_class = f->get_onestep_classification();
			Onestep_classification nF_class = nF->get_onestep_classification();
			if ( 
				((is_infinite(f) || f_class == O_EXTERIOR || f_class== O_UNCLASSIFIED) && nF_class == O_INTERIOR) ||
				((is_infinite(nF) || nF_class == O_EXTERIOR  || nF_class== O_UNCLASSIFIED) && f_class == O_INTERIOR)
				) {
					Vertex_handle other_vertex = f->vertex((id+1)%3);
					if (other_vertex == v) other_vertex = f->vertex((id+2)%3);
					if (has_n1) {
						n2 = other_vertex->point();
						has_n2 = true;
					} else {
						n1 = other_vertex->point();
						has_n1 = true;
					}
			}
			e_circ++;
		} while ((e_start != e_circ) && !has_n2);
		Point_2 loop_point = v->point(); *has = true;
		Vector_2 v1 = n1 - CGAL::ORIGIN;
		Vector_2 v2 = n2 - CGAL::ORIGIN;
		if (has_n1 && has_n2) 
			loop_point = CGAL::ORIGIN + (1/8.0*(v1+v2) + 3/4.0 * (v->point() - CGAL::ORIGIN));
		else *has = false;

		return loop_point;	
	}

	Point_2 loop_uneven(Face_handle f, int id) {
		return midpoint(f->vertex((id+1)%3)->point(), f->vertex((id+2)%3)->point());
	}

	double lambda_edge(Edge e) {
		if (is_infinite(e)) {
			return false;
		}
		return sqrt(CGAL::to_double(segment(e).squared_length()))/2;
	}

	double scale_edge(Edge e) {
		if (is_infinite(e)) {
			return 0;
		}

		Face_handle f = e.first;
		int id = e.second;
		return f->get_edge_scale(id);
	}

	double angle_edge(Edge e) {
		if (is_infinite(e)) {
			return 0;
		}

		double lambda_sq = CGAL::to_double(segment(e).squared_length())/4;

		Face_handle f = e.first;
		int id = e.second;
		Face_handle nF = f->neighbor(id);

		Point v1 = dual(f);
		Point v2 = dual(nF);

		double dist1_sq = CGAL::to_double(squared_distance(f->vertex((id+1)%3)->point(), v1));
		double dist2_sq = CGAL::to_double(squared_distance(f->vertex((id+1)%3)->point(), v2));

//		if (dist2_sq>dist1_sq) dist1_sq = dist2_sq;

//		std::cout << "dist_sq: " << dist1_sq << "             lambda_sq: " << lambda_sq << std::endl; 

//		return acos(sqrt(lambda_sq/dist1_sq))*(180.0/PI);
		return (asin(sqrt(lambda_sq/dist1_sq))+ asin(sqrt(lambda_sq/dist2_sq)))*(90.0/PI);
	}


private:
	bool classified;
	bool crust_classified;
	bool lambda_classified;
	std::list< Weighted_point > inner_balls;
	std::list<Point3D> inner_balls_floats;
	QVector< QPointF > ordered_points;
	double front_value;

};

#endif //MESECINA_CLASSIFIED_TRIANGULATION_2_H