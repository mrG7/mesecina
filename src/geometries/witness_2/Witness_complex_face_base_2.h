/* This source file is part of Mesecina, a software for visualization and studying of
* the medial axis and related computational geometry structures.
* More info: http://www.agg.ethz.ch/~miklosb/mesecina
* Copyright Armin Häberling, ETH Zurich
*
* $Id: Witness_complex_face_base_2.h 344 2008-02-29 12:50:20Z arminha $
*/

#ifndef MESECINA_WITNESS_COMPLEX_FACE_BASE_2_H
#define MESECINA_WITNESS_COMPLEX_FACE_BASE_2_H

#include <CGAL/Triangulation_face_base_2.h>

typedef enum Witness_complex_classification { WC_UNCLASSIFIED, WC_INTERIOR, WC_EXTERIOR };

/**
 * Extension of CGAL::Triangulation_face_base_2 with additional attributes used in the
 * Delaunay triangulation of the landmarks, resp. for the class Witness_complex_triangulation_2.
 */
template < class Gt, class Fb = CGAL::Triangulation_face_base_2<Gt> >
class Witness_complex_face_base_2 : public  Fb {
private:
	typedef Fb														   Base;

public:
	typedef Gt													Geom_traits;
	typedef typename Geom_traits::FT							 Coord_type;
	typedef typename Fb::Vertex_handle						  Vertex_handle;
	typedef typename Fb::Face_handle							Face_handle;
	typedef typename Gt::Point_2									Point_2;

	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Fb::template Rebind_TDS<TDS2>::Other			Fb2;
		typedef Witness_complex_face_base_2<Gt, Fb2>				  Other;
	};

private:
	Witness_complex_classification w;

public:
	Witness_complex_face_base_2()	: Base(), w(WC_UNCLASSIFIED) {}

	Witness_complex_face_base_2(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2)
		: Base(v0,v1,v2), w(WC_UNCLASSIFIED) {}

	Witness_complex_face_base_2(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2, Face_handle n0, Face_handle n1, Face_handle n2)
		: Base(v0,v1,v2,n0,n1,n2), w(WC_UNCLASSIFIED) {}

	void set_witness_complex_classification(Witness_complex_classification wc_class) {
		w = wc_class;
	}

	const Witness_complex_classification & get_witness_complex_classification() {
		return w;
	}
};

#endif // MESECINA_WITNESS_COMPLEX_FACE_BASE_2_H