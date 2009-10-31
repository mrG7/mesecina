/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#include "Second_voronoi_2.h"
#include "layers\Second_voronoi_layer_2.h"

#include <constants.h>

template <class K, class Bt>
Second_voronoi_2<K, Bt>::Second_voronoi_2(const std::string& _triangulation_source, const std::string& _layer_name = "2nd Order Voronoi") : Geometry() {
	triangulation_source = _triangulation_source;
	layer_name = _layer_name;

	GL_draw_layer_2* voronoi_order_2_layer = new Second_voronoi_layer_2<Second_voronoi_2<K, Bt> >(layer_name.c_str(), this, "Second order Voronoi diagam");
	add_layer(voronoi_order_2_layer);
}

template <class K, class Bt>
Second_voronoi_2<K, Bt>::~Second_voronoi_2() {
}

template <class K, class Bt>
void Second_voronoi_2<K, Bt>::receive_structure_changed(const std::string& name) {
	if (name == triangulation_source) {
		dirty = true;
		invalidate_cache();
	}
}

template <class K, class Bt>
typename Second_voronoi_2<K, Bt>::Triangulation* Second_voronoi_2<K, Bt>::get_regular_triangulation() {
	if (dirty) {
		// get triangulation structure
		void* v = request_structure(triangulation_source);
		if (v == 0) {
			std::cout << "ERROR: Could not get delaunay triangulation structure called " << triangulation_source << std::endl;
			return &regular_tri;
		}
		base_tri = static_cast<Base_triangulation *>(v);
		std::cout << PROGRESS_STATUS << "Building 2nd order voronoi diagram" << std::endl;

		// compute dual of 2nd voronoi
		regular_tri.clear();
		// forall edges e compute middle point m and weight w = -length(e)^2/4
		// and enter into the regular triangulation
		int i = 0;
		Base_triangulation::Finite_edges_iterator e_it, e_end = base_tri->finite_edges_end();
		for (e_it = base_tri->finite_edges_begin(); e_it != e_end; e_it++, i++) {
			Base_triangulation::Face_handle f = e_it->first;
			int ind = e_it->second;
			Point_2 p1,p2;
			p1 = f->vertex((ind + 1) % 3)->point();
			p2 = f->vertex((ind + 2) % 3)->point();

			Point_2 m = CGAL::midpoint(p1,p2);
			FT w = -CGAL::squared_distance(p1,p2) / 4.0;

			regular_tri.insert(Weighted_point(m,w));
		}
		dirty = false;

		std::cout << PROGRESS_DONE << std::endl;
	}
	return &regular_tri;
}



template <class K, class Bt>
void Second_voronoi_2<K, Bt>::invalidate_cache() {
	invalidate_all_layers();
}
