/* This source file is part of Mesecina, a software for visualization and studying of
* the medial axis and related computational geometry structures.
* More info: http://www.agg.ethz.ch/~miklosb/mesecina
* Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
*
* $Id: Classified_vertex_base_2.h 121 2007-06-22 15:16:16Z miklosb $
*/

#ifndef MESECINA_CLASSIFIED_VERTEX_BASE_2_H
#define MESECINA_CLASSIFIED_VERTEX_BASE_2_H

#include <CGAL/Triangulation_vertex_base_2.h>

template < class Gt, class Vb = CGAL::Triangulation_vertex_base_2<Gt>>
class Classified_vertex_base_2 : public  Vb
{
	typedef Vb                              Base;
public:

	typedef typename Vb::Face_handle        Face_handle;
	typedef typename Vb::Point              Point;

	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Vb::template Rebind_TDS<TDS2>::Other    Vb2;
		typedef Classified_vertex_base_2<Gt,Vb2>                Other;
	};

private:
	int reconstruction_edges;

public:
	Classified_vertex_base_2() : Base(), reconstruction_edges(0) {}
	Classified_vertex_base_2(const Point & p) : Base(p), reconstruction_edges(0) {}
	Classified_vertex_base_2(const Point & p, Face_handle f) : Base(f,p),  reconstruction_edges(0) {}
	Classified_vertex_base_2(Face_handle f) : Base(f),  reconstruction_edges(0) {}

	void reset_count_reconstruction_edges() { reconstruction_edges = 0; }
	void inc_count_reconstruction_edges() { reconstruction_edges++; }
	int get_count_recontruction_edges() { return reconstruction_edges; }
};

#endif //MESECINA_CLASSIFIED_VERTEX_BASE_2_H