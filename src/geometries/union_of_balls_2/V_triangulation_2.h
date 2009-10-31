/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_triangulation_2.h 531 2009-02-05 22:02:06Z miklosb $
 */

#ifndef MESECINA_V_TRIANGULATION_2_H
#define MESECINA_V_TRIANGULATION_2_H

#include "V_face_base_2.h"
#include "V_vertex_base_2.h"

#include <CGAL/Triangulation_vertex_base_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <queue>

template <class Gt, class Rtds>
class V_triangulation_2 : public CGAL::Delaunay_triangulation_2 < Gt, 
		CGAL::Triangulation_data_structure_2 < 
			V_vertex_base_2<Gt, CGAL::Triangulation_vertex_base_2<Gt>, Rtds> ,
			V_face_base_2<Gt, CGAL::Triangulation_face_base_2<Gt>, Rtds>
		> 
	>
{
public:
	typedef Gt									Geom_traits;
	typedef typename CGAL::Triangulation_data_structure_2 < 
			V_vertex_base_2<Gt, CGAL::Triangulation_vertex_base_2<Gt>, Rtds> ,
			V_face_base_2<Gt, CGAL::Triangulation_face_base_2<Gt>, Rtds>
		>												Tds;
	typedef typename Geom_traits::FT			 Coord_type;
	typedef typename Geom_traits::Point_2			Point_2;
	typedef typename Geom_traits::Vector_2       Vector_2;
	typedef typename Geom_traits::Segment_2     Segment_2;
	typedef typename Geom_traits::Triangle_2    Triangle_2;
	typedef typename Rtds::Edge					Ball_edge;
	typedef typename Rtds::Vertex_handle	    Ball_vertex_handle;
	typedef typename Rtds::Face_handle	    Ball_face_handle;
	typedef typename Tds::Vertex_handle Vertex_handle;
	typedef typename Tds::Face_handle Face_handle;
	typedef typename CGAL::Delaunay_triangulation_2<Gt,Tds>::Finite_edges_iterator Finite_edges_iterator;
	typedef typename CGAL::Delaunay_triangulation_2<Gt,Tds>::All_faces_iterator All_faces_iterator;
	typedef typename CGAL::Delaunay_triangulation_2<Gt,Tds>::Face_circulator Face_circulator;
	typedef typename CGAL::Delaunay_triangulation_2<Gt,Tds>::Edge_circulator Edge_circulator;
	typedef typename std::list<Ball_vertex_handle> Ball_vertex_list;
	typedef typename Ball_vertex_list::iterator Ball_vertex_list_iterator;
	typedef typename Ball_vertex_list::const_iterator Ball_vertex_list_const_iterator;
	typedef typename std::list<Ball_edge> Ball_edge_list;
	typedef typename Ball_edge_list::const_iterator Ball_edge_list_const_iterator;
	


	V_triangulation_2(const Gt& gt = Gt())
		: CGAL::Delaunay_triangulation_2<Gt,Tds>(gt) { classified = false; flooded = false; }

	V_triangulation_2(const V_triangulation_2<Gt, Rtds> &tr) : CGAL::Delaunay_triangulation_2<Gt,Tds>(tr)
	{   
//	CGAL::CGAL_triangulation_postcondition( is_valid() );  /// \todo removed for Mac version
	classified = tr.classified; flooded = tr.flooded; 
	}

	void set_dirty() { classified = false; flooded = false; }
	bool is_dirty() { return !classified; }
	bool is_flooded() { return flooded; }

	void compute_classification() {
		// classify edges of v voronoi diagram
		std::cout << PROGRESS_STATUS << "Classifying V-voronoi edges" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM <<  tds().number_of_edges()-1 << std::endl;
		Finite_edges_iterator e_it, e_end = finite_edges_end(); int i = 0;
		for (e_it = finite_edges_begin(); e_it!=e_end; ++e_it, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			Face_handle f = e_it->first;
			int id = e_it->second;
			Face_handle nF = f->neighbor(id);
			if (nF==0) return;
			int nId = nF->index(f);

			Vertex_handle v1 = f->vertex((id+1)%3); // first v vertex inducing voronoi edge
			Vertex_handle v2 = f->vertex((id+2)%3); // second v vertex inducing voronoi edge

			std::list<Ball_edge> pes1 = v1->get_origin_edges(); // origin polar edges (regular or singular) of v1
			std::list<Ball_edge> pes2 = v2->get_origin_edges(); // origin polar edges (regular or singular) of v2

			std::list<Ball_vertex_handle> intersection;
			intersect_edge_lists(pes1, pes2, &intersection);

			if (intersection.size() > 1) {
				/// \todo maybe not safe enough!!!!! TODO.. THINK ABOUT it <- it was a comment from mepos source code
				f->set_classification(id, V_ALPHA);
				nF->set_classification(nId, V_ALPHA);
				std::list<Ball_vertex_handle>::iterator bi, bi_end = intersection.end();
				//for (bi = intersection.begin(); bi!=bi_end; bi++) {
				//	Ball_vertex_handle bvh = *bi;
				//	std::cout << LOG_BLUE<< bvh->point() << std::endl;
				//}

				bi = intersection.begin();
				Ball_vertex_handle common_vertex = *bi;
				f->set_ball_voronoi_edge(id, common_vertex);
				nF->set_ball_voronoi_edge(nId, common_vertex);
				bi++;
				common_vertex = *bi;
				f->set_ball_voronoi_edge_second(id, common_vertex);
				nF->set_ball_voronoi_edge_second(nId, common_vertex);

			} else if (intersection.size() == 1) {
				f->set_classification(id, V_CROSSING);
				nF->set_classification(nId, V_CROSSING);
				Ball_vertex_handle common_vertex = *(intersection.begin());
				//			assert(common_vertex!=0);
				f->set_ball_voronoi_edge(id, common_vertex);
				nF->set_ball_voronoi_edge(nId, common_vertex);
			} else {
				// it is a non-crossing edge
			}

			// if this "crossing" edge is crossing only in an endpoint set it as touching
			//Ball_vertex_handle int_vertex = *(intersection.begin());
			//if (f->get_ball_voronoi_vertex() == int_vertex || nF->get_ball_voronoi_vertex() == int_vertex) {
			//	f->set_classification(id, V_TOUCHING);
			//	nF->set_classification(nId, V_TOUCHING);
			//}
		}
		std::cout << PROGRESS_DONE << std::endl;
		classified = true;
	}

	void flood_voronoi_classification() {
		if (number_of_vertices() < 2) return;
		std::queue<Vertex_handle> Q;
		std::queue<Vertex_handle> post_process;

		// go through all infinite faces and their vertices. if the vertex has an unclassified voronoi vertex, add it to the queue
		All_faces_iterator af_it, af_end = all_faces_end();
		for (af_it = all_faces_begin(); af_it != af_end; af_it++) af_it->set_classification(V_UNCLASSIFIED);
		Face_circulator	f_circ, f_start = incident_faces(infinite_vertex());
		f_circ = f_start;
		do {
			//if (is_infinite(f_circ)) std::cout << "INF" << " " << 
			//	((f_circ->get_classification()==V_INTERIOR) ? "INT" : "EXT") << std::endl;
			//else std::cout << CGAL::to_double(dual(f_circ).x()) << " " << CGAL::to_double(dual(f_circ).y()) << " " <<
			//	((f_circ->get_classification()==V_INTERIOR) ? "INT" : "EXT") << std::endl;
			if (vertex_has_unclassified_face(f_circ->vertex(0))) Q.push(f_circ->vertex(0));
			if (vertex_has_unclassified_face(f_circ->vertex(1))) Q.push(f_circ->vertex(1));
			if (vertex_has_unclassified_face(f_circ->vertex(2))) Q.push(f_circ->vertex(2));
			f_circ++;
		} while (f_circ != f_start);

		std::cout << PROGRESS_STATUS << "Flooding Voronoi diagram to classify voronoi vertices" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM << "0" << std::endl;
		int last_q_length = Q.size(); bool to_continue = true; int flood_loops = 0;
		do {
			int i=0;
			while (!Q.empty() /*&& i < 3*/) {
				i++;
				std::cout << PROGRESS_VALUE << 0 << std::endl;
				//std::cout << "--------------------------" << std::endl;
				// pop vertex from queue
				Vertex_handle v = Q.front(); Q.pop();
				Face_circulator f_root, f_circ, f_start = incident_faces(v);
				f_circ = f_start; bool found_classified = false; bool found_external = false;
				// go through the incident faces of the vertex until one classified face is found, when external found keep that
				do {
					if (is_infinite(f_circ) || f_circ->get_classification() != V_UNCLASSIFIED) {
						found_classified = true;
						if (f_circ->get_classification() == V_EXTERIOR || is_infinite(f_circ)) 
							found_external = true;
						f_root = f_circ;
					}
					f_circ++;
				} while (f_circ!=f_start && !found_external);

				if (found_classified && vertex_has_unclassified_face(v)) {
					V_classification root_classification = is_infinite(f_root) ? V_EXTERIOR : f_root->get_classification();
					assert(root_classification!=V_UNCLASSIFIED);


					// one goes clockwise and anticlockwise along unclassified faces and set the encountered vertices to 
					// the root classification

					//if (root_classification==V_INTERIOR) { // for interior root
					//	std::cout << "interior root" << std::endl;
					//} 
					//else { // for exterior root
					//std::cout << "ROOT: ";
					//if (is_infinite(f_root)) std::cout << "INF";
					//else std::cout << CGAL::to_double(dual(f_root).x()) << " " << CGAL::to_double(dual(f_root).y());
					//std::cout << " - " << (root_classification == V_INTERIOR ? "INT" : "EXT") << std::endl;
					Edge_circulator e_circ, e_start = incident_edges(v, f_root);
					// go counterclockwise until reach crossing
					e_circ = e_start; //e_circ--;
					Face_handle ef, aef;
					int eid, aeid;
					bool need_go_further;
					do {
						ef = e_circ->first;
						eid = e_circ->second;
						need_go_further = (ef->get_classification(eid) == V_UNCLASSIFIED);

						//Face_handle nef = ef->neighbor(eid);
						//std::cout << "CCW STUD: ";
						//if (is_infinite(ef)) std::cout << "INF";
						//else std::cout << CGAL::to_double(dual(ef).x()) << " " << CGAL::to_double(dual(ef).y());
						//std::cout << " - ";
						//if (is_infinite(nef)) std::cout << "INF";
						//else std::cout << CGAL::to_double(dual(nef).x()) << " " << CGAL::to_double(dual(nef).y());
						if (need_go_further ) { // here was unclassified
							//std::cout << "  not" << std::endl;
							if (ef->get_classification()==V_UNCLASSIFIED) {
								ef->set_classification(root_classification);
							}
							if (ef->neighbor(eid)->get_classification()==V_UNCLASSIFIED) {
								ef->neighbor(eid)->set_classification(root_classification); // just to make sure, can be buggy!!!
							}
						} else {
							//std::cout << "  crossing" << std::endl;
						}
						e_circ++;
					} while (e_circ != e_start && need_go_further); // here was not corssing

					// go clockwise until reach crossing
					e_circ = e_start; e_circ--;
					ef = e_circ->first;
					eid = e_circ->second;
					do {
						ef = e_circ->first;
						eid = e_circ->second;
						need_go_further = (ef->get_classification(eid) == V_UNCLASSIFIED);

						//Face_handle nef = ef->neighbor(eid);
						//std::cout << "CW STUD: ";
						//if (is_infinite(ef)) std::cout << "INF";
						//else std::cout << CGAL::to_double(dual(ef).x()) << " " << CGAL::to_double(dual(ef).y());
						//std::cout << " - ";
						//if (is_infinite(nef)) std::cout << "INF";
						//else std::cout << CGAL::to_double(dual(nef).x()) << " " << CGAL::to_double(dual(nef).y());
						if (need_go_further ) { // here was not corssing
							//						std::cout << "  not" << std::endl;
							if (ef->get_classification()==V_UNCLASSIFIED) 
								ef->set_classification(root_classification);
							if (ef->neighbor(eid)->get_classification()==V_UNCLASSIFIED) 
								ef->neighbor(eid)->set_classification(root_classification); // just to make sure, can be buggy!!!
						} else {
							//						std::cout << "  crossing" << std::endl;
						}
						e_circ--;
					} while (e_circ != e_start && need_go_further); // here was not corssing 

					// check if we have two consecutive
					Edge_circulator e_adv;
					bool has_consecutive_crossing = false;
					if (root_classification == V_INTERIOR) {
						e_circ = e_start; e_adv = e_circ; e_adv++;
						do {
							ef = e_circ->first;
							eid = e_circ->second;
							aef = e_adv->first;
							aeid = e_adv->second;
							if (ef->get_classification(eid) == V_CROSSING && aef->get_classification(aeid) == V_CROSSING)
								has_consecutive_crossing = true;
							e_circ++; e_adv++;
						} while (e_circ!=e_start);
					}

					if (!has_consecutive_crossing || root_classification != V_INTERIOR) {

						// go through all the faces and mark unclassified as opposite from root
						V_classification opp_classification = root_classification==V_INTERIOR ? V_EXTERIOR : V_INTERIOR;
						f_circ = f_start = f_root;
						//std::cout << "Voronoi vertices: " << std::endl;
						do {
							if (f_circ->get_classification() == V_UNCLASSIFIED) {
								f_circ->set_classification(opp_classification);
							}
							//if (is_infinite(f_circ)) std::cout << "INF" << " " << 
							//	((f_circ->get_classification()==V_INTERIOR) ? "INT" : "EXT") << std::endl;
							//else std::cout << CGAL::to_double(dual(f_circ).x()) << " " << CGAL::to_double(dual(f_circ).y()) << " " <<
							//	((f_circ->get_classification()==V_INTERIOR) ? "INT" : "EXT") << std::endl;
							f_circ++;
						} while (f_circ != f_start);
					}


					if (root_classification == V_INTERIOR && has_consecutive_crossing) {
						f_circ = f_start = incident_faces(v);

						//std::cout << "Voronoi vertices: " << std::endl;
						//do {
						//	//if (is_infinite(f_circ)) std::cout << "INF" << " " << 
						//	//	((f_circ->get_classification()==V_INTERIOR) ? "INT" : "EXT") << std::endl;
						//	//else std::cout << CGAL::to_double(dual(f_circ).x()) << " " << CGAL::to_double(dual(f_circ).y()) << " " <<
						//	//	((f_circ->get_classification()==V_INTERIOR) ? "INT" : "EXT") << std::endl;
						//	if (f_circ->get_classification() == V_EXTERIOR) f_circ->set_classification(V_UNCLASSIFIED);
						//	f_circ++;
						//} while (f_circ != f_start);
						post_process.push(v);
					} else {
						// go through all faces and all vertices to find vertices to be put in queue
						f_circ = f_start = incident_faces(v);
						//std::cout << "Voronoi vertices: " << std::endl;
						do {
							//if (is_infinite(f_circ)) std::cout << "INF" << " " << 
							//	((f_circ->get_classification()==V_INTERIOR) ? "INT" : "EXT") << std::endl;
							//else std::cout << CGAL::to_double(dual(f_circ).x()) << " " << CGAL::to_double(dual(f_circ).y()) << " " <<
							//	((f_circ->get_classification()==V_INTERIOR) ? "INT" : "EXT") << std::endl;
							if (vertex_has_unclassified_face(f_circ->vertex(0))) Q.push(f_circ->vertex(0));
							if (vertex_has_unclassified_face(f_circ->vertex(1))) Q.push(f_circ->vertex(1));
							if (vertex_has_unclassified_face(f_circ->vertex(2))) Q.push(f_circ->vertex(2));
							f_circ++;
						} while (f_circ != f_start);
					}


					//				} //end exterior root

				} else f_root = f_start;
			}

			//std::cout << "post process size: " << post_process.size() << std::endl;
			while (!post_process.empty()) {
				Vertex_handle v = post_process.front(); post_process.pop();
				if (vertex_has_unclassified_face(v)) Q.push(v);
			}
			//std::cout << "Q size: " << Q.size() << std::endl;
			//std::cout << LOG_BLUE << "last_q_length: " << last_q_length << std::endl;
			if (last_q_length==Q.size())
				to_continue = false;
			else last_q_length = Q.size();
		} while (to_continue && flood_loops < 10);

		af_end = all_faces_end(); int counter = 0;
		for (af_it = all_faces_begin(); af_it != af_end; af_it++) 
			if (af_it->get_classification() == V_UNCLASSIFIED) {
				af_it->set_classification(V_EXTERIOR);
				counter++;
			}
		std::cout << "Flooding did not reach " << counter << " voronoi vertices, which are considered exterior now" << std::endl;

		std::cout << PROGRESS_DONE << std::endl;
		flooded = true;
	}

	void intersect_edge_lists(const std::list<Ball_edge>& es1, const std::list<Ball_edge>& es2, std::list<Ball_vertex_handle>* result) {
			std::list<Ball_vertex_handle> v_list = std::list<Ball_vertex_handle>();
			result->clear();
			get_vertex_list(es1, &v_list);
			intersect_vertex_edge_list(v_list, es2, result);
	}

	void get_vertex_list(const std::list<Ball_edge>& es, std::list<Ball_vertex_handle>* result) {
		Ball_vertex_list_iterator v_it, v_end;
		Ball_edge_list_const_iterator e_it, e_end = es.end();
		for (e_it = es.begin(); e_it != e_end; e_it++) {
			Ball_face_handle pf = e_it->first;
			int pid = e_it->second;
			Ball_vertex_handle v1 = pf->vertex((pid+1)%3); // first endpoint of e_it
			Ball_vertex_handle v2 = pf->vertex((pid+2)%3); // second endpoint of e_it

			v_end = result->end();
			bool v1_found = false, v2_found = false;
			for (v_it = result->begin(); v_it != v_end; v_it++) {
				if (*v_it == v1) v1_found = true;
				if (*v_it == v2) v2_found = true;
			}
			if (!v1_found) result->push_back(v1);
			if (!v2_found) result->push_back(v2);
		}
	}

	void intersect_vertex_edge_list(const std::list<Ball_vertex_handle>& vs,
		const std::list<Ball_edge>& es, std::list<Ball_vertex_handle>* result) {
			result->clear();
			Ball_vertex_list_const_iterator v_it, v_end = vs.end();
			for (v_it = vs.begin(); v_it != v_end; v_it++) { // for all vertices in vs
				Ball_edge_list_const_iterator e_it, e_end = es.end();
				for (e_it = es.begin(); e_it != e_end; e_it++) { // for all edges in es
					bool add_v1 = false, add_v2 = false;
					Ball_face_handle pf = e_it->first;
					int pid = e_it->second;
					Ball_vertex_handle v1 = pf->vertex((pid+1)%3); // first endpoint of e_it
					Ball_vertex_handle v2 = pf->vertex((pid+2)%3); // second endpoint of e_it
					Ball_vertex_handle v = *v_it;
					if (v1==v) add_v1 = true; // if they intersect mark it for addition
					if (v2==v) add_v2 = true;

					if (add_v1 || add_v2) { // if we found intersection
						Ball_vertex_list_iterator vv_it, vv_end;
						vv_end = result->end();
						bool v1_found = false, v2_found = false; // see if we already have them in list
						for (vv_it = result->begin(); vv_it != vv_end; vv_it++) {
							if (*vv_it == v1) v1_found = true;
							if (*vv_it == v2) v2_found = true;
						}
						if (!v1_found && add_v1) result->push_back(v1); // if not in the list and we need
						if (!v2_found && add_v2) result->push_back(v2); // to add them, then add them :)
					}
				}
			}
	}

	Vertex_handle safe_insert(const Point_2 &p, const Ball_edge& e) {
		Vertex_handle v = insert(p);
		v->add_origin_edge(e);

		/// \todo remove this collapsing after the flooding of the v voronoi diagram is stable
		//if (MAX_CROSSING_V_VORONOI_SQUARED > 0 && number_of_vertices() > 1) {
		//	Vertex_circulator vv_circ, vv_begin;
		//	vv_begin = vv_circ = incident_vertices(v);
		//	do {
		//		Coord_type sq_dist = squared_distance(vv_circ->point(), v->point());
		//		if (sq_dist < MAX_CROSSING_V_VORONOI_SQUARED) {
		//			std::list<Ball_edge> es = v->get_origin_edges();
		//			std::list<Ball_edge>::iterator e_it, e_end = es.end();
		//			for (e_it = es.begin(); e_it != e_end; e_it++)
		//				vv_circ->add_origin_edge(*e_it);
		//			remove(v);
		//			v = vv_circ;
		//			vv_circ = vv_begin; // to exit the loop
		//		}	else vv_circ++;
		//	} while(vv_circ != vv_begin);
		//}

		return v;
	}

	bool vertex_has_unclassified_face(Vertex_handle v) {
		Face_circulator f_circ, f_start = incident_faces(v);
		f_circ = f_start;
		bool unclassified = false;
		do {
			if (f_circ->get_classification()==V_UNCLASSIFIED && !is_infinite(f_circ)) unclassified = true;
			f_circ++;
		} while (f_circ!=f_start && !unclassified);
		//if (!unclassified)
		//	int a = 2;
		return unclassified;
	}

	void set_voronoi_vertex_is_added(Face_handle f) {
		f->set_added_to_medial_balls(true);
		if (is_infinite(f)) return;
		Point_2 vor = f->get_circumcenter();
		double vx = CGAL::to_double(vor.x());
		double vy = CGAL::to_double(vor.y());
		Face_handle nF = f->neighbor(0);
		if (!is_infinite(nF)) {
			double fx = CGAL::to_double(nF->get_circumcenter().x());
			double fy = CGAL::to_double(nF->get_circumcenter().y());
			if (((vx-fx)*(vx-fx) + (vy-fy)*(vy-fy) < MAX_CROSSING_V_VORONOI_SQUARED) &&
				nF->get_added_to_medial_balls() == false)
				set_voronoi_vertex_is_added(nF);
		}
		nF = f->neighbor(1);
		if (!is_infinite(nF)) {
			double fx = CGAL::to_double(nF->get_circumcenter().x());
			double fy = CGAL::to_double(nF->get_circumcenter().y());
			if (((vx-fx)*(vx-fx) + (vy-fy)*(vy-fy) < MAX_CROSSING_V_VORONOI_SQUARED) &&
				nF->get_added_to_medial_balls() == false)
				set_voronoi_vertex_is_added(nF);
		}
		nF = f->neighbor(2);
		if (!is_infinite(nF)) {
			double fx = CGAL::to_double(nF->get_circumcenter().x());
			double fy = CGAL::to_double(nF->get_circumcenter().y());
			if (((vx-fx)*(vx-fx) + (vy-fy)*(vy-fy) < MAX_CROSSING_V_VORONOI_SQUARED) &&
				nF->get_added_to_medial_balls() == false)
				set_voronoi_vertex_is_added(nF);
		}
	}

	//bool voronoi_vertex_is_added(Face_handle f) {
	//	if (is_infinite(f)) return true;
	//	if (f->get_added_to_medial_balls()) {
	//		return true;
	//	}
	//	Point_2 vor = f->get_circumcenter();
	//	double vx = CGAL::to_double(vor.x());
	//	double vy = CGAL::to_double(vor.y());
	//	Face_handle nF = f->neighbor(0);
	//	if (!is_infinite(nF)) {
	//		double fx = CGAL::to_double(nF->get_circumcenter().x());
	//		double fy = CGAL::to_double(nF->get_circumcenter().y());
	//		if (((vx-fx)*(vx-fx) + (vy-fy)*(vy-fy) < MAX_CROSSING_V_VORONOI_SQUARED) && 
	//			voronoi_vertex_is_added(nF)) {
	//				f->set_added_to_medial_balls(true);	return true;
	//		}
	//	}
	//	nF = f->neighbor(1);
	//	if (!is_infinite(nF)) {
	//		double fx = CGAL::to_double(nF->get_circumcenter().x());
	//		double fy = CGAL::to_double(nF->get_circumcenter().y());
	//		if (((vx-fx)*(vx-fx) + (vy-fy)*(vy-fy) < MAX_CROSSING_V_VORONOI_SQUARED) && 
	//			voronoi_vertex_is_added(nF)) {
	//				f->set_added_to_medial_balls(true);	return true;
	//		}
	//	}
	//	nF = f->neighbor(2);
	//	if (!is_infinite(nF)) {
	//		double fx = CGAL::to_double(nF->get_circumcenter().x());
	//		double fy = CGAL::to_double(nF->get_circumcenter().y());
	//		if (((vx-fx)*(vx-fx) + (vy-fy)*(vy-fy) < MAX_CROSSING_V_VORONOI_SQUARED) && 
	//			voronoi_vertex_is_added(nF)) {
	//				f->set_added_to_medial_balls(true);	return true;
	//		}
	//	}
	//	return false;
	//}


private:
	bool classified;
	bool flooded;
};

#endif //MESECINA_V_TRIANGULATION_2_H
