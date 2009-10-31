/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Ball_vertex_base_2.h 127 2007-07-05 16:50:24Z miklosb $
 */

#ifndef MESECINA_BALL_VERTEX_BASE_2_H
#define MESECINA_BALL_VERTEX_BASE_2_H

#include <CGAL/Triangulation_vertex_base_2.h>

typedef	enum Ball_dual_classification {D_IS, D_NOT};

template < class Gt, class Vb = CGAL::Triangulation_vertex_base_2<Gt> >
class Ball_vertex_base_2 : public  Vb
{
	typedef Vb                              Base;
public:

	typedef typename Vb::Vertex_handle      Vertex_handle;
	typedef typename Vb::Face_handle        Face_handle;
	typedef typename Vb::Point              Point_2;

	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Vb::template Rebind_TDS<TDS2>::Other    Vb2;
		typedef Ball_vertex_base_2<Gt,Vb2>                       Other;
	};

private:
	Ball_dual_classification c;

public:
	Ball_vertex_base_2() : Base(), c(D_NOT){}
	Ball_vertex_base_2(const Point_2 & p) : Base(p), c(p.c) {}
	Ball_vertex_base_2(const Point_2 & p, Face_handle f) : Base(f,p), c(p.c) {}
	Ball_vertex_base_2(Face_handle f) : Base(f), c(D_NOT) {}

	Ball_dual_classification get_classification() { return c; }
	void set_classification(Ball_dual_classification c) { this->c = c; }
};

#endif //MESECINA_BALL_VERTEX_BASE_2_H