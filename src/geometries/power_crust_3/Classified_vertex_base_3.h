/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Classified_vertex_base_3.h 365 2008-06-14 15:07:42Z miklosb $
 */

#ifndef MESECINA_CLASSIFIED_VERTEX_BASE_3_H
#define MESECINA_CLASSIFIED_VERTEX_BASE_3_H

#include <CGAL/Triangulation_vertex_base_3.h>

template < class Gt, class Vb = CGAL::Triangulation_vertex_base_3<Gt> >
class Classified_vertex_base_3 : public  Vb
{
	typedef Vb                              Base;
public:

	typedef typename Vb::Vertex_handle      Vertex_handle;
	typedef typename Vb::Cell_handle        Cell_handle;
	typedef typename Vb::Point              Point_3;
	typedef typename Gt::Vector_3           Vector_3;

	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Vb::template Rebind_TDS<TDS2>::Other    Vb2;
		typedef Classified_vertex_base_3<Gt,Vb2>                       Other;
	};

private:
	Cell_handle pole_plus;
	Cell_handle pole_minus;
	float poles_confidence;
	Vector_3 plus_pole_direction;
	bool convex_hull;

public:
	Classified_vertex_base_3() : Base(), pole_minus(0), pole_plus(0), poles_confidence(0), convex_hull(false), plus_pole_direction(CGAL::NULL_VECTOR) {}
	Classified_vertex_base_3(const Point_3 & p) : Base(p), pole_minus(0), pole_plus(0), poles_confidence(0), convex_hull(false), plus_pole_direction(CGAL::NULL_VECTOR)  {}
	Classified_vertex_base_3(const Point_3 & p, Cell_handle f) : Base(f,p), pole_minus(0), pole_plus(0), poles_confidence(0), convex_hull(false), plus_pole_direction(CGAL::NULL_VECTOR)  {}
//	Classified_vertex_base_3(Face_handle f) : Base(f), inner_pole(0), outer_pole(0), convex_hull(false)  {}

	void set_pole_plus(const Cell_handle f) { assert(f!=0); pole_plus = f;}
	Cell_handle get_pole_plus() {return pole_plus; }

	void set_pole_minus(const Cell_handle f) {assert(f!=0); pole_minus = f;}
	Cell_handle get_pole_minus() {return pole_minus; }

	Cell_handle get_opposite_pole(Cell_handle c) {
		if (c==pole_plus) return pole_minus;
		if (c==pole_minus) return pole_plus;
		bool we_try_to_get_opposite_pole_with_invalid_cell = false;
		assert(we_try_to_get_opposite_pole_with_invalid_cell);
		return 0;
	}

	Cell_handle get_opposite_pole(Classified_vertex_base_3* c) {
		if (c==&(*pole_plus)) return pole_minus;
		if (c==&(*pole_minus)) return pole_plus;
		bool we_try_to_get_opposite_pole_with_invalid_cell = false;
		assert(we_try_to_get_opposite_pole_with_invalid_cell);
		return 0;
	}

	Cell_handle get_pole(Voronoi_classification c) {
		if (pole_plus!=0 && pole_plus->get_classification()==c) return pole_plus;
		if (pole_minus!=0 && pole_minus->get_classification()==c) return pole_minus;
		return 0;
	}

	void set_plus_pole_direction(const Vector_3 &v) { plus_pole_direction = v; }
	Vector_3 get_plus_pole_direction() { return plus_pole_direction; }

	void set_poles_confidence(const float &v) { poles_confidence = v; }
	float get_poles_confidence() { return poles_confidence; }

	bool is_on_convex_hull() { return convex_hull; }
	void set_on_convex_hull(bool f) { convex_hull = f; }
};

#endif //MESECINA_CLASSIFIED_VERTEX_BASE_3_H