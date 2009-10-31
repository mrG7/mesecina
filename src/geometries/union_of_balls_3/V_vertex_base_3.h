/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_vertex_base_3.h 378 2008-08-03 18:50:59Z miklosb $
 */

#ifndef MESECINA_V_VERTEX_BASE_3_H
#define MESECINA_V_VERTEX_BASE_3_H

#include <CGAL/Triangulation_vertex_base_3.h>

template < class Gt, class Vb, class Polar_tds>
class V_vertex_base_3 : public  Vb
{
	typedef Vb                              Base;
public:

	typedef typename Vb::Vertex_handle      Vertex_handle;
	typedef typename Vb::Cell_handle        Cell_handle;
	typedef typename Vb::Point              Point_3;
	typedef typename Gt::Vector_3           Vector_3;
	typedef typename Polar_tds::Facet		Polar_facet;
	typedef std::set<Polar_facet>			Polar_facet_set;


	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Vb::template Rebind_TDS<TDS2>::Other    Vb2;
		typedef V_vertex_base_3<Gt,Vb2,Polar_tds>     Other;
	};

private:
	Polar_facet_set dual_source_facets;

public:
	V_vertex_base_3() : Base()/*, dual_source_facet(0,0)*/ {}
	V_vertex_base_3(const Point_3 & p) : Base(p)/*, dual_source_facet(0,0)*/ {}
	V_vertex_base_3(const Point_3 & p, Cell_handle f) : Base(f,p)/*, dual_source_facet(0,0)*/ {}
//	Classified_vertex_base_3(Face_handle f) : Base(f), inner_pole(0), outer_pole(0), convex_hull(false)  {}

	void add_dual_source_facet(Polar_facet f) { dual_source_facets.insert(f); }
	Polar_facet_set& get_dual_source_facets() { return dual_source_facets;}


};

#endif //MESECINA_V_VERTEX_BASE_3_H