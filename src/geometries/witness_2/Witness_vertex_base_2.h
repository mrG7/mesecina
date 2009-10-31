/* This source file is part of Mesecina, a software for visualization and studying of
* the medial axis and related computational geometry structures.
* More info: http://www.agg.ethz.ch/~miklosb/mesecina
* Copyright Armin Häberling, ETH Zurich
*
* $Id: Witness_vertex_base_2.h 342 2008-02-29 10:51:56Z arminha $
*/

#ifndef MESECINA_WITNESS_VERTEX_BASE_2_H
#define MESECINA_WITNESS_VERTEX_BASE_2_H

#include <CGAL/Triangulation_vertex_base_2.h>
#include <CGAL/squared_distance_2.h>

/**
 * Extension of the CGAL::Triangulation_vertex_base_2.
 * Allows to store a list of N-nearest landmarks for each witness.
 * 
 * The ::add_landmark function updates the N-nearest landmark list.
 *
 */
template <class K, int N = 2, class Vb = CGAL::Triangulation_vertex_base_2<K>>
class Witness_vertex_base_2 : public  Vb
{
	typedef Vb                              Base;
public:

	typedef typename Vb::Face_handle        Face_handle;
	typedef typename Vb::Point              Point;
	typedef typename std::vector<Point>		Neighbour_Point_list;


	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Vb::template Rebind_TDS<TDS2>::Other    Vb2;
		typedef Witness_vertex_base_2<K,N,Vb2>                    Other;
	};

private:
	Neighbour_Point_list _nearest_k_landmarks;
	bool _is_landmark;

public:
	Witness_vertex_base_2() : Base(), _nearest_k_landmarks(), _is_landmark(false) {}
	Witness_vertex_base_2(const Point & p) : Base(p), _nearest_k_landmarks(), _is_landmark(false) {}
	Witness_vertex_base_2(const Point & p, Face_handle f) : Base(f,p), _nearest_k_landmarks(), _is_landmark(false) {}
	Witness_vertex_base_2(Face_handle f) : Base(f), _nearest_k_landmarks(), _is_landmark(false) {}

	const Neighbour_Point_list & nearest_k_landmarks() const {
		return _nearest_k_landmarks;
	}

	Neighbour_Point_list & nearest_k_landmarks() {
		return _nearest_k_landmarks;
	}

	/**
	 * Updates the list of the k-nearest landmarks with the new landmark p.
	 * Returns true if the list has changed.
	 */
	bool add_landmark(Point& p)
		// return true if p is in the nearest N landmarks of this vertex
	{
		K::FT dist_to_p = CGAL::squared_distance(p, point());
		Neighbour_Point_list::iterator n_it, n_end = _nearest_k_landmarks.end();
		int i = 0;
		for (n_it = _nearest_k_landmarks.begin(); n_it != n_end; n_it++) {
			K::FT dist = CGAL::squared_distance(*n_it, point());
			if (dist_to_p < dist) {
				break;
			}
			i++;
		}
		if (i < N) {
			_nearest_k_landmarks.insert(n_it, p);
			if (_nearest_k_landmarks.size() > (N+1)) {
				_nearest_k_landmarks.pop_back();
			}
			return true;
		}
		return false;
	}

	bool is_landmark() {
		return _is_landmark;
	}

	void set_landmark(bool landmark) {
		_is_landmark = landmark;
	}
};

#endif //MESECINA_WITNESS_VERTEX_BASE_2_H