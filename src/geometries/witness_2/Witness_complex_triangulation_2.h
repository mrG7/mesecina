/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Armin Häberling, ETH Zurich
 *
 * $Id: Witness_complex_triangulation_2.h 344 2008-02-29 12:50:20Z arminha $
 */

#ifndef MESECINA_WITNESS_COMPLEX_TRIANGULATION_2_H
#define MESECINA_WITNESS_COMPLEX_TRIANGULATION_2_H

#include "Witness_complex_face_base_2.h"

#include <CGAL/Triangulation_data_structure_2.h>
#include <CGAL/Triangulation_vertex_base_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <queue>

/**
 * Delaunay triangulation of the landmarks. Contains the witness complex as a subcomplex
 */
template <class Gt>
class Witness_complex_triangulation_2 : public CGAL::Delaunay_triangulation_2 <
	Gt, CGAL::Triangulation_data_structure_2 < CGAL::Triangulation_vertex_base_2<Gt> , Witness_complex_face_base_2<Gt> > 
	>
{
public:
	typedef Gt Geom_traits;
	typedef typename CGAL::Triangulation_data_structure_2 <
		CGAL::Triangulation_vertex_base_2<Gt> , Witness_complex_face_base_2<Gt>
	>																				Tds;
	typedef typename CGAL::Delaunay_triangulation_2 <Gt, Tds >						Base;
	typedef typename Geom_traits::FT												Coord_type;
	typedef typename Geom_traits::Point_2											Point_2;
	typedef typename Geom_traits::Vector_2      Vector_2;
	typedef typename Geom_traits::Segment_2     Segment_2;
	typedef typename Geom_traits::Triangle_2    Triangle_2;
	typedef typename Base::Vertex_handle											Vertex_handle;
	typedef typename Base::Edge														Edge;
	typedef typename Base::Face_handle												Face_handle;
	typedef typename Base::All_faces_iterator All_faces_iterator;
	typedef typename Base::Finite_vertices_iterator Finite_vertices_iterator;
	typedef typename Base::Finite_edges_iterator Finite_edges_iterator;
	typedef typename Base::Face_circulator Face_circulator;
	typedef typename Base::Vertex_circulator Vertex_circulator;
	typedef typename Base::Edge_circulator Edge_circulator;
private:
	typedef typename std::pair<Point, Point> Edge_pair;
	typedef typename std::map<Edge_pair, int> Edge_map;

public:
	Witness_complex_triangulation_2(const Gt& gt = Gt())
		: Base(gt), classification_dirty(true) { }

	bool is_witness_complex_edge(const Edge & e) {
		const Face_handle & f = e.first;
		const int & ind = e.second;
		const Point & p = f->vertex((ind + 1) % 3)->point();
		const Point & q = f->vertex((ind + 2) % 3)->point();
		return edge_witnesses(p,q) > 0;
	}

	bool classification_is_dirty() {
		return classification_dirty;
	}

	/**
	 * Classify all faces as inside or outside the shape reconstruted with the witness complex.
	 */
	void compute_witness_complex_classification() {
		if (number_of_vertices() <= 2) return;

		All_faces_iterator af_it, af_end = all_faces_end();
		for (af_it = all_faces_begin(); af_it != af_end; af_it++) {
			if (is_infinite(af_it)) {
				af_it->set_witness_complex_classification(WC_EXTERIOR);
			} else {
				af_it->set_witness_complex_classification(WC_UNCLASSIFIED);
			}
		}

		std::queue<Face_handle> q;
		q.push(infinite_face());

		while(!q.empty()) {
			Face_handle f = q.front();
			q.pop();

			int i;
			for (i = 0; i < 3; i++) {
				Face_handle nF = f->neighbor(i);
				if (nF->get_witness_complex_classification() == WC_UNCLASSIFIED) {
					if (is_witness_complex_edge(std::make_pair(f,i))) {
						if (f->get_witness_complex_classification() == WC_EXTERIOR) {
							nF->set_witness_complex_classification(WC_INTERIOR);
						} else {
							nF->set_witness_complex_classification(WC_EXTERIOR);
						}
					} else {
						nF->set_witness_complex_classification(f->get_witness_complex_classification());
					}
					q.push(nF);
				}
			}
		}
		classification_dirty = false;
	}

	/**
	 * Returns the number of witnesses for the edge between p and q
	 */
	int edge_witnesses(const Point & p, const Point & q) {
		Edge_map::iterator it = edge_map.find(make_edge(p,q));
		if (it != edge_map.end()) {
			return it->second;
		}
		return -1;
	}

	/**
	 * Increase the number of witnesses for the edge between p and q
	 */
	void add_edge_witness(const Point & p, const Point & q) {
		const Edge_pair e = make_edge(p,q);
		Edge_map::iterator it = edge_map.find(e);
		if (it != edge_map.end()) {
			it->second++;
		}
		edge_map.insert(make_pair(e,1));
		classification_dirty = true;
	}

	/**
	 * Increase the number of witnesses for the edges with vertex p and a vertex in [begin,end).
	 */
	template <class InputIterator>
	void add_edge_witness(const Point & p, InputIterator begin, InputIterator end) {
		InputIterator it;
		for (it = begin; it != end; it++) {
			add_edge_witness(p, *it);
		}
	}

	/**
	 * Decrease the number of witnesses for the edge between p and q
	 */
	void remove_edge_witness(const Point & p, const Point & q) {
		const Edge_pair e = make_edge(p,q);
		Edge_map::iterator it = edge_map.find(e);
		if (it != edge_map.end()) {
			it->second--;
			if (it->second <= 0) {
				edge_map.erase(it);
			}
		}
		classification_dirty = true;
	}

	/**
	 * Decrease the number of witnesses for the edges with vertex p and a vertex in [begin,end).
	 */
	template <class InputIterator>
	void remove_edge_witness(const Point & p,  InputIterator begin, InputIterator end) {
		InputIterator it;
		for (it = begin; it != end; it++) {
			remove_edge_witness(p, *it);
		}
	}

	//overwrite clear
	void clear() {
		((Base*) this)->clear();
		edge_map.clear();
	}

private:
	bool classification_dirty;

	Edge_map edge_map;

	Edge_pair make_edge(Point p,Point q) {
		return std::make_pair(std::min(p,q), std::max(p,q));
	}

};

#endif // MESECINA_WITNESS_COMPLEX_TRIANGULATION_2_H
