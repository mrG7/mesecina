/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Polar_vertex_base_2.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef MESECINA_POLAR_VERTEX_BASE_H
#define MESECINA_POLAR_VERTEX_BASE_H

#include <CGAL/Triangulation_vertex_base_2.h>

typedef	enum Pole_classification {P_UNCLASSIFIED, P_INTERIOR, P_EXTERIOR};

template < class Gt, class Vb = CGAL::Triangulation_vertex_base_2<Gt> >
class Polar_vertex_base_2 : public  Vb
{
	typedef Vb                              Base;
public:

	typedef typename Vb::Vertex_handle      Vertex_handle;
	typedef typename Vb::Face_handle        Face_handle;
	typedef typename Vb::Point              Point_2;

	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Vb::template Rebind_TDS<TDS2>::Other    Vb2;
		typedef Polar_vertex_base_2<Gt,Vb2>                       Other;
	};

private:
	Vertex_handle  opposite_pole;
	Pole_classification c;

public:
	Polar_vertex_base_2() : Base(), c(P_UNCLASSIFIED), opposite_pole(0) {}
	Polar_vertex_base_2(const Point_2 & p) : Base(p), c(p.c), opposite_pole(p.opposite_pole) {}
	Polar_vertex_base_2(const Point_2 & p, Face_handle f) : Base(f,p), c(p.c), opposite_pole(p.opposite_pole) {}
	Polar_vertex_base_2(Face_handle f) : Base(f), c(P_UNCLASSIFIED), opposite_pole(0) {}

	void set_opposite_pole(Vertex_handle v) { opposite_pole = v;}
	Vertex_handle get_opposite_pole() {return opposite_pole; }

	Pole_classification get_classification() { return c; }
	bool is_interior() { return c == P_INTERIOR; }
	Pole_classification get_opposite_classification() {
		assert(c != P_UNCLASSIFIED);
		if (c == P_INTERIOR) return P_EXTERIOR;
		else return P_INTERIOR;
	}
	void set_classification(Pole_classification c) { this->c = c; }
	bool is_classified() { return c!=P_UNCLASSIFIED; }
};

#endif //MESECINA_POLAR_VERTEX_BASE_H