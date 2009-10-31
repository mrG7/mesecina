/* This source file is part of Mesecina, a software for visualization and studying of
* the medial axis and related computational geometry structures.
* More info: http://www.agg.ethz.ch/~miklosb/mesecina
* Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
*
* $Id: V_vertex_base_2.h 78 2007-03-25 03:19:20Z miklosb $
*/

#include <list>

#ifndef MESECINA_V_VERTEX_BASE_2_H
#define MESECINA_V_VERTEX_BASE_2_H

#include <CGAL/Triangulation_vertex_base_2.h>

template < class Gt, class Vb, class Polar_tds >
class V_vertex_base_2 : public  Vb
{
	typedef Vb                              Base;
public:
	typedef typename Polar_tds::Edge				Polar_edge;
	typedef std::list<Polar_edge>						Polar_edge_list;
	typedef typename Vb::Vertex_handle      Vertex_handle;
	typedef typename Vb::Face_handle        Face_handle;
	typedef typename Vb::Point              Point;

	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Vb::template Rebind_TDS<TDS2>::Other    Vb2;
		typedef V_vertex_base_2<Gt,Vb2,Polar_tds>                Other;
	};

private:
	Polar_edge_list origin_edges;

public:
	V_vertex_base_2() : Base(), origin_edges(Polar_edge_list()) {}
	V_vertex_base_2(const Point & p) : Base(p), origin_edges(Polar_edge_list()) {}
	V_vertex_base_2(const Point & p, Face_handle f) : Base(f,p), origin_edges(Polar_edge_list()) {}
	V_vertex_base_2(Face_handle f) : Base(f), origin_edges(Polar_edge_list()) {}

	void add_origin_edge(Polar_edge e) {
		origin_edges.push_back(e);
	}
	Polar_edge_list get_origin_edges() {
		return origin_edges;
	}
};

#endif //MESECINA_V_VERTEX_BASE_2_H