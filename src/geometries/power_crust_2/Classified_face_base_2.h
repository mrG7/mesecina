/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Classified_face_base_2.h 385 2008-11-26 22:53:58Z miklosb $
 */

#ifndef MESECINA_CLASSIFIED_FACE_BASE_H
#define MESECINA_CLASSIFIED_FACE_BASE_H

#include <CGAL/Triangulation_face_base_2.h>

typedef	enum Medial_classification {M_UNCLASSIFIED, M_INTERIOR, M_EXTERIOR};
typedef	enum Crust_classification {C_UNCLASSIFIED, C_CRUST};
typedef enum Onestep_classification { O_UNCLASSIFIED, O_INTERIOR, O_EXTERIOR};

template < class Gt, class Fb = CGAL::Triangulation_face_base_2<Gt> >
class Classified_face_base_2 : public  Fb {
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
		typedef Classified_face_base_2<Gt, Fb2>							  Other;
	};

private:
	Face_handle opposite_pole;
	Medial_classification c;
	Crust_classification cc[3];
	Point_2 circumcenter;
	Onestep_classification o;
	double vertex_lambda;
	double vertex_angle;
	double vertex_scale;
	double scale[3];
	bool flood_visited[3];


public:
	Classified_face_base_2()	: Base(), o(O_UNCLASSIFIED), c(M_UNCLASSIFIED), opposite_pole(0) { cc[0] = cc[1] = cc[2] = C_UNCLASSIFIED; 
	vertex_scale = vertex_angle = vertex_lambda = 0; scale[0] = scale[1] = scale[2] = 0; flood_visited[0] = flood_visited[1] = flood_visited[2] = false;}

	Classified_face_base_2(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2)
		: Base(v0,v1,v2), o(O_UNCLASSIFIED), c(M_UNCLASSIFIED), opposite_pole(0) { cc[0] = cc[1] = cc[2] = C_UNCLASSIFIED; 
	vertex_scale = vertex_angle = vertex_lambda = 0; scale[0] = scale[1] = scale[2] = 0; flood_visited[0] = flood_visited[1] = flood_visited[2] = false;}

	Classified_face_base_2(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2, Face_handle n0, Face_handle n1, Face_handle n2) : 
				Base(v0,v1,v2,n0,n1,n2), o(O_UNCLASSIFIED), c(M_UNCLASSIFIED), opposite_pole(0)  { cc[0] = cc[1] = cc[2] = C_UNCLASSIFIED; 
				vertex_scale = vertex_angle = vertex_lambda = 0; scale[0] = scale[1] = scale[2] = 0; flood_visited[0] = flood_visited[1] = flood_visited[2] = false;}

	void set_classification(Medial_classification c) { 
		this->c = c; 
	}
	Medial_classification get_classification() { return c; }
	bool is_interior() { return c==M_INTERIOR; }

	void set_onestep_classification(Onestep_classification oc) { o = oc; }
	Onestep_classification get_onestep_classification() { return o; }

	Medial_classification get_inverted_classification() {
		assert(c!=M_UNCLASSIFIED);
		if (c == M_INTERIOR) return M_EXTERIOR;
		else return M_INTERIOR;
	}

	Face_handle get_opposite_pole() {
		return opposite_pole;
	}

	void set_opposite_pole(const Face_handle &op) {
		opposite_pole = op;
	}

	Point_2 get_circumcenter() { return circumcenter; }
	void set_circumcenter(Point_2 c) { circumcenter = c; }

	Crust_classification get_edge_classification(int i) { return cc[i]; }
	void set_edge_classification(int i, Crust_classification ccl) { cc[i] = ccl; }

	double get_vertex_lambda() { return vertex_lambda; }
	void set_vertex_lambda(double v) { vertex_lambda = v; }

	double get_vertex_angle() { return vertex_angle; }
	void set_vertex_angle(double v) { vertex_angle = v; }

	double get_vertex_scale() { return vertex_scale; }
	void set_vertex_scale(double v) { vertex_scale = v; }

	double get_edge_scale(int i) { return scale[i]; }
	void set_edge_scale(int i, double v) { scale[i] = v; }

	bool is_flood_visited(int i) { return flood_visited[i]; }
	void set_flood_visited(int i, bool b) { flood_visited[i] = b; }

};

#endif //MESECINA_CLASSIFIED_FACE_BASE_H