/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Polar_vertex_base_3.h 365 2008-06-14 15:07:42Z miklosb $
 */

#ifndef MESECINA_POLAR_VERTEX_BASE_3_H
#define MESECINA_POLAR_VERTEX_BASE_3_H

#include <CGAL/Triangulation_vertex_base_3.h>

typedef	enum Pole_classification {P_UNCLASSIFIED, P_INTERIOR, P_EXTERIOR};

template < class Gt, class Vb = CGAL::Triangulation_vertex_base_3<Gt> >
class Polar_vertex_base_3 : public  Vb
{
	typedef Vb                              Base;
public:

//	typedef typename Triangulation_3::Cell_handle Delaunay_cell_handle;
	typedef typename Vb::Vertex_handle      Vertex_handle;
	typedef typename Vb::Cell_handle        Cell_handle;
	typedef typename Vb::Point              Point_3;

	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Vb::template Rebind_TDS<TDS2>::Other    Vb2;
		typedef Polar_vertex_base_3<Gt,Vb2>  Other;
	};


private:
	void*  voronoi_vert;
	float in, out, pri;
	Pole_classification c;

public:
	Polar_vertex_base_3() : Base(), c(P_UNCLASSIFIED) , voronoi_vert(0), in(0), out(0) { update_priority(); }
	Polar_vertex_base_3(const Point_3 & p) : Base(p), c(p.c) , voronoi_vert(0), in(0), out(0) { update_priority(); }
	Polar_vertex_base_3(const Point_3 & p, Cell_handle f) : Base(f,p), c(p.c) , voronoi_vert(0), in(0), out(0) { update_priority(); }
//	Polar_vertex_base_3(Face_handle f) : Base(f), c(P_UNCLASSIFIED), voronoi_vertex(0) {}

	void*& voronoi_vertex() { return voronoi_vert; } 

	Pole_classification get_classification() { return c; }
	bool is_interior() { return c == P_INTERIOR; }
	Pole_classification get_opposite_classification() {
		assert(c != P_UNCLASSIFIED);
		if (c == P_INTERIOR) return P_EXTERIOR;
		else return P_INTERIOR;
	}
	void set_classification(Pole_classification c) { this->c = c; }
	bool is_classified() { return c!=P_UNCLASSIFIED; }

	void update_priority() {
		if (in>0 && out>0) pri = fabs(in - out) - 1;
		else pri = (in > out) ? in : out;
	}

	float get_priority() { return pri; }
	float get_out() { return out; }
	void set_out(float _out) { out = _out; }
	float get_in() { return in; }
	void set_in(float _in) { in = _in; }
};

#endif //MESECINA_POLAR_VERTEX_BASE_3_H