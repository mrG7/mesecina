/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Classified_cell_base_3.h 365 2008-06-14 15:07:42Z miklosb $
 */

#ifndef MESECINA_CLASSIFIED_FACE_BASE_3_H
#define MESECINA_CLASSIFIED_FACE_BASE_3_H

#include <CGAL/Triangulation_cell_base_with_circumcenter_3.h>

typedef	enum Voronoi_classification {V_UNCLASSIFIED, V_MINUS, V_PLUS};

template < class Gt, class Fb, class Polar_vertex_handle >
class Classified_cell_base_3 : public  Fb {
private:
	typedef Fb														   Base;

public:
	typedef Gt													Geom_traits;
	typedef typename Geom_traits::FT							 Coord_type;
	typedef typename Fb::Vertex_handle						  Vertex_handle;
	typedef typename Fb::Cell_handle							Cell_handle;
	typedef typename Gt::Point_3									Point_3;

	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Fb::template Rebind_TDS<TDS2>::Other			Fb2;
		typedef Classified_cell_base_3<Gt, Fb2, Polar_vertex_handle>	Other;
	};

private:
	std::list<Vertex_handle> sample_points;
	Polar_vertex_handle polar_vertex;
	Voronoi_classification c;
	Point_3 circumc;

public:
	Classified_cell_base_3()	: Base(), c(V_UNCLASSIFIED), polar_vertex(0) { sample_points.clear(); }

	Classified_cell_base_3(Vertex_handle v0, Vertex_handle v1,
                          Vertex_handle v2, Vertex_handle v3)
    : Base(v0, v1, v2, v3) , c(V_UNCLASSIFIED), polar_vertex(0) { sample_points.clear(); }

	Classified_cell_base_3(Vertex_handle v0, Vertex_handle v1,
                          Vertex_handle v2, Vertex_handle v3,
                          Cell_handle   n0, Cell_handle   n1,
                          Cell_handle   n2, Cell_handle   n3)
    : Base(v0, v1, v2, v3, n0, n1, n2, n3), c(V_UNCLASSIFIED), polar_vertex(0) { sample_points.clear(); }

	void set_classification(Voronoi_classification c) { 
		this->c = c; 
	}
	Voronoi_classification get_classification() { return c; }
	bool is_interior() { return c==V_MINUS; }

	Voronoi_classification get_inverted_classification() {
		assert(c!=V_UNCLASSIFIED);
		if (c == V_MINUS) return V_PLUS;
		else return V_MINUS;
	}

	std::list<Vertex_handle>* get_sample_points() {
		return &sample_points;
	}

	void add_sample_point(const Vertex_handle &sp) {
		sample_points.push_back(sp);
	}


	void set_circumcenter(const Point_3 p) { circumc = p; }
	Point_3 get_circumcenter() { return circumc; }

	void set_polar_vertex(Polar_vertex_handle v) { polar_vertex = v; }
	Polar_vertex_handle get_polar_vertex() { return polar_vertex; }
};

#endif //MESECINA_CLASSIFIED_FACE_BASE_3_H