/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Armin Häberling, ETH Zurich
 *
 * $Id: Witness_Landmark_set_2.h 342 2008-02-29 10:51:56Z arminha $
 */

#ifndef MESECINA_WITNESS_LANDMARK_SET_2_H
#define MESECINA_WITNESS_LANDMARK_SET_2_H

#include "Witness_vertex_base_2.h"
#include "Witness_complex_triangulation_2.h"

#include <constants.h>

#include <cassert>
#include <set>
#include <vector>
#include <map>
#include <utility>

#include <CGAL/Point_set_2.h>
#include <CGAL/Triangulation_data_structure_2.h>
#include <CGAL/squared_distance_2.h>

/**
 * Manages the witness point set and the landmarks set, which is a subset of the witness points.
 * For the witness points the lists of N-nearest landmarks for each point are maintained.
 *
 * The ::nearest_k_landmarks queriy is limited to k <= N+1.
 *
 * The ::next_landmark_max_min query returns the witness point farthest from the current landmarks.
 * 
 * The ::next_landmark_alpha determines the next landmark in an adaptive manner.
 */
template <class K, int N>
class Witness_landmark_set_2 {

public:
	typedef CGAL::Point_set_2 <K, 
		CGAL::Triangulation_data_structure_2 <
			Witness_vertex_base_2 <K, N> 
		>
		>																Witness_triangulation;
	typedef typename Witness_triangulation::Point						Point;
	typedef typename K::Vector_2										Vector;
	typedef typename Witness_triangulation::Finite_vertices_iterator	Witness_iterator;
	typedef typename Witness_triangulation::Vertex_handle				Witness_handle;
	typedef typename std::list<Point>									Landmark_list;
	typedef typename std::list<Witness_handle>							Witness_list;
	typedef typename K::FT												Coord_type;
	typedef Witness_complex_triangulation_2<K>							Witness_complex_triangulation;

	/**
	 * Warning: this constructor clears the static Witness_triangulation witness_tri.
	 * Use the copy constructor to get a new Witness_Landmark_set_2 without erasing the witnesses.
	 */
	Witness_landmark_set_2() : complex_tri() {
		witness_tri.clear();
		landmarks.clear();
	}

	Witness_handle add_witness(const Point& witness) { 
		Witness_handle vh = witness_tri.insert(witness);
		clear_landmarks();
		return vh;
	}

	void remove_witness(Witness_handle witness) { 
		witness_tri.remove(witness);
		clear_landmarks();
	}

	/**
	 * Adds a witness point to the landmark set and update the witness complex.
	 */
	void add_landmark(Witness_handle landmark) {
		if (complex_tri.number_of_vertices() == 0) {
			// we are adding the first landmark
			// clean the additional data in the witness triangulation first
			// so we start with clean data
			std::cout << PROGRESS_STATUS << "Initializing Witness data" << std::endl;
			std::cout << PROGRESS_MINIMUM << "0" << std::endl;
			std::cout << PROGRESS_MAXIMUM << witness_tri.number_of_vertices() << std::endl;

			Witness_iterator v_it, v_end = witness_end();
			int i = 0;
			for (v_it = witness_begin(); v_it != v_end; v_it++,i++) {
				v_it->nearest_k_landmarks().clear();
				v_it->set_landmark(false);
			}

			std::cout << PROGRESS_DONE << std::endl;
		}

		landmarks.push_back(landmark->point());
		landmark->set_landmark(true);
		
		// update witness complex
		//std::cout << PROGRESS_STATUS << "Updating Witness Complex" << std::endl;
		//std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		
		// update k nearest landmarks for the witnesses
		Witness_list witnesses_to_be_processed = update_k_nearest_landmarks(landmark);

		//std::cout << PROGRESS_MAXIMUM << witnesses_to_be_processed.size()-1 << std::endl;
		
		Point p = landmark->point();
		complex_tri.insert(p);

		Witness_list::iterator w_it, w_end = witnesses_to_be_processed.end();
		int i = 0;
		for (w_it = witnesses_to_be_processed.begin(); w_it != w_end; w_it++, i++) {

			std::vector<Point> nl = nearest_k_landmarks(*w_it,3);
			std::vector<Point> nl_old;

			int p_ind = 0;
			std::vector<Point>::iterator nl_it, nl_end = nl.end();
			for (nl_it = nl.begin(); nl_it != nl_end; nl_it++) {
				if (*nl_it == p) {
					p_ind = nl_it - nl.begin();
				} else {
					nl_old.push_back(*nl_it);
				}
			}

			// add/remove edges
			if (p_ind < N)
			{
				const int s = std::min<double>(N-1, (int) nl_old.size());
				// add new edges with p
				complex_tri.add_edge_witness(p, nl_old.begin(), nl_old.begin() + s);
					
				if (nl_old.size() >= N)
				{
					// remove edges with q
					const Point q = nl_old[N-1];
					complex_tri.remove_edge_witness(q, nl_old.begin(), nl_old.begin() + (N-1));
				}
			}
		}

		//std::cout << PROGRESS_DONE << std::endl;

	}
	
	void clear_landmarks() {
		landmarks.clear();
		complex_tri.clear();
	}

	void clear_all() {
		witness_tri.clear();
		clear_landmarks();
	}

	Witness_iterator witness_begin() {
		return witness_tri.finite_vertices_begin(); 
	}
	
	Witness_iterator witness_end() {
		return witness_tri.finite_vertices_end();
	}

	size_t number_of_witnesses() const {
		return witness_tri.number_of_vertices();
	}

	Witness_handle nearest_witness(Point p) {
		return witness_tri.nearest_vertex(p);
	}

	Witness_triangulation * witness_triangulation() {
		return &witness_tri;
	}

	Witness_complex_triangulation * witness_complex_triangulation() {
		return &complex_tri;
	}

	Landmark_list& landmark_list() {
		return landmarks;
	}

	Point nearest_landmark(Witness_handle witness) {
		return witness->nearest_k_landmarks()[0];
	}

	std::vector<Point> nearest_k_landmarks(Witness_handle witness, int k) {
		std::vector<Point> nkl = witness->nearest_k_landmarks();
		int i;
		int n = min(k, (int) nkl.size());
		std::vector<Point> res(n);
		for (i = 0; i < n; i++) {
			res[i] = nkl[i];
		}
		return res;
	}

	/**
	 * determines the next landmark in a max-min fashion.
	 * 
	 * i.e.
	 * 
	 * p = argmax_{w in W} d(w,L)
	 
	  where 
	  
	  d(w,L) = min_{v in L} d(w,v)$
	 */
	Witness_handle next_landmark_max_min() {
		if (landmarks.empty()) {
			// add an arbitrary witness as first landmark
			return witness_begin();
		}
		
//		std::cout << "Searching next landmark..." << std::endl;
//		std::cout << PROGRESS_STATUS << "Searching next landmark" << std::endl;
//		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
//		std::cout << PROGRESS_MAXIMUM << number_of_witnesses()-1 << std::endl;

		Coord_type max_dist = 0;
		Witness_handle res;
		Witness_iterator w_it, w_end = witness_end();
		int i = 0;
		for (w_it = witness_begin(); w_it != w_end; w_it++, i++) {
//			std::cout << PROGRESS_VALUE << i << std::endl;
			if (!w_it->is_landmark()) {
				const Point & nl = nearest_landmark(w_it);
				const Coord_type dist = CGAL::squared_distance(w_it->point(),nl);
				if (dist > max_dist) {
					max_dist = dist;
					res = w_it;
				}
			}
		}
//		std::cout << PROGRESS_DONE << std::endl;
//		std::cout << "epsilon = " << sqrt(max_dist) << std::endl;
		epsilon = sqrt(CGAL::To_double<Coord_type>()(max_dist));
//		std::cout << "Done." << std::endl;
		return res;
	}

	/**
	 * Return the maximal value epsilon for which the landmarks are an epsilon-sparse sample
	 */
	double get_epsilon() {
		return epsilon;
	}

	Witness_handle next_landmark_alpha(Coord_type alpha) {
		typedef typename Witness_triangulation::Vertex_circulator  Vertex_circulator;

		if (landmarks.empty()) {
			// add an arbitrary witness as first landmark
			return witness_begin();
		}
		
		std::cout << "Searching next landmark..." << std::endl;
		std::cout << PROGRESS_STATUS << "Searching next landmark" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM << number_of_witnesses()-1 << std::endl;

		Coord_type max_dist = -1;
		Witness_handle res;
		Witness_iterator w_it, w_end = witness_end();
		int i = 0;
		for (w_it = witness_begin(); w_it != w_end; w_it++, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			if (!w_it->is_landmark()) {
				Point nl = nearest_landmark(w_it);
				Coord_type dist = CGAL::squared_distance(w_it->point(),nl);
				// add penalty
				Witness_handle nn;
				Vertex_circulator nn_it, nn_begin = witness_tri.incident_vertices(w_it);
				if (nn_begin != 0) {
					Coord_type nn_dist = CGAL::squared_distance(w_it->point(), nn_begin->point());
					nn_it = nn_begin;
					nn_it++;
					for (; nn_it != nn_begin; nn_it++) {
						nn_dist = min(nn_dist, CGAL::squared_distance(w_it->point(), nn_it->point()));
					}

					dist -= alpha * nn_dist;
				}

				if (dist > max_dist || max_dist < -1) {
					max_dist = dist;
					res = w_it;
				}
			}
		}
		std::cout << PROGRESS_DONE << std::endl;
		std::cout << "Done." << std::endl;
		return res;
	}

private:
	Landmark_list landmarks;
	/*static */Witness_triangulation witness_tri;
	Witness_complex_triangulation complex_tri;
	double epsilon;

	Witness_list update_k_nearest_landmarks(Witness_handle ph) {
		typedef typename Witness_triangulation::Vertex_circulator  Vertex_circulator;

		Witness_list res;

		Point p = ph->point();

		// init marks
		witness_tri.init_vertex_marks();
		witness_tri.mark_vertex(witness_tri.infinite_vertex());
		witness_tri.mark_vertex(ph);
		ph->add_landmark(p);
		res.push_back(ph);

		std::queue<Witness_handle> process;
		process.push(ph);

		while (!process.empty()) {
			Witness_handle vh = process.front();
			process.pop();
			// iterate through all delaunay neighbours of vh
			Vertex_circulator n_begin = witness_tri.incident_vertices(vh);
			Vertex_circulator n_it = n_begin;
			if (n_it != 0) {
				do {
					Witness_handle nh = n_it.base();
					//if (! this->is_infinite(nh)) {
						if (! witness_tri.is_marked(nh)) {
							witness_tri.mark_vertex(nh);
							if (nh->add_landmark(p)) {
								// p is in the k nearest landmarks
								process.push(nh);
								res.push_back(nh);
							}
						}
					//}
					n_it++;
				} while (n_it != n_begin);
			}
		}
		return res;
	}

};

//template < class K, int N>
//CGAL::Point_set_2 <K, CGAL::Triangulation_data_structure_2 <Witness_vertex_base_2 <K, N> > > Witness_Landmark_set_2<K, N>::witness_tri = 
//	CGAL::Point_set_2 <K, CGAL::Triangulation_data_structure_2 <Witness_vertex_base_2 <K, N> > >();

#endif // MESECINA_WITNESS_LANDMARK_SET_2_H
