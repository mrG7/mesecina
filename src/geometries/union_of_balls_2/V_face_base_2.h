/* This source file is part of Mesecina, a software for visualization and studying of
* the medial axis and related computational geometry structures.
* More info: http://www.agg.ethz.ch/~miklosb/mesecina
* Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
*
* $Id: V_face_base_2.h 387 2008-12-05 18:46:23Z miklosb $
*/

#ifndef MESECINA_V_FACE_BASE_2_H
#define MESECINA_V_FACE_BASE_2_H

#include <CGAL/Triangulation_face_base_2.h>

typedef	enum V_classification {V_UNCLASSIFIED, V_CROSSING, V_ALPHA, V_INTERIOR, V_EXTERIOR};

template < class Gt, class Fb, class Polar_tds>
class V_face_base_2 : public  Fb
{
private:
	typedef Fb																		Base;
public:
	typedef typename Polar_tds::Vertex_handle						 Polar_vertex_handle;
	typedef Gt														         Geom_traits;
	typedef typename Geom_traits::Point_2										Point_2;
	typedef typename Geom_traits::FT										  Coord_type;
	typedef typename Fb::Vertex_handle									   Vertex_handle;
	typedef typename Fb::Face_handle										 Face_handle;

	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Fb::template Rebind_TDS<TDS2>::Other  Fb2;
		typedef V_face_base_2<Gt, Fb2, Polar_tds>              Other;
	};
private:
	V_classification vor_edge_c[3];
	Polar_vertex_handle ball_voronoi_edge[6];
	V_classification vor_vertex;
	Polar_vertex_handle ball_voronoi_vertex;
	bool added_to_medial_balls;
	Point_2 circumcenter;

	void init() {
		vor_edge_c[0] = vor_edge_c[1] = vor_edge_c[2] = V_UNCLASSIFIED;
		vor_vertex = V_UNCLASSIFIED;
		ball_voronoi_edge[0] = ball_voronoi_edge[1] = ball_voronoi_edge[2] = ball_voronoi_edge[3] = ball_voronoi_edge[4] = ball_voronoi_edge[5] = 0;
		ball_voronoi_vertex = 0;
		added_to_medial_balls = false;
		circumcenter = Point_2(0,0);
	}

public:
	V_face_base_2() : Base() { init(); }

	V_face_base_2(Vertex_handle v0, 
		Vertex_handle v1, 
		Vertex_handle v2)
		: Base(v0,v1,v2) { init(); }

	V_face_base_2(Vertex_handle v0, 
		Vertex_handle v1, 
		Vertex_handle v2,
		Face_handle n0, 
		Face_handle n1, 
		Face_handle n2)
		: Base(v0,v1,v2,n0,n1,n2) { init(); }

	void set_classification(V_classification c) { 
		this->vor_vertex = c; 
	}
	V_classification get_classification() { return vor_vertex; }

	void set_ball_voronoi_vertex(Polar_vertex_handle &pv) { this->ball_voronoi_vertex = pv; }
	Polar_vertex_handle get_ball_voronoi_vertex() { return ball_voronoi_vertex; }

	void set_circumcenter(Point_2 p) { circumcenter = p; }
	Point_2 get_circumcenter() { return circumcenter; }

	//double get_double_radius_from_circumcenter

	void set_classification(int i, V_classification c) { this->vor_edge_c[i] = c; }
	V_classification get_classification(int i) { 
		return this->vor_edge_c[i];
	}

	void set_ball_voronoi_edge(int i, Polar_vertex_handle &pv) { this->ball_voronoi_edge[i] = pv; }
	Polar_vertex_handle get_ball_voronoi_edge(int i) { return ball_voronoi_edge[i]; }

	void set_ball_voronoi_edge_second(int i, Polar_vertex_handle &pv) { this->ball_voronoi_edge[3+i] = pv; }
	Polar_vertex_handle get_ball_voronoi_edge_second(int i) { return ball_voronoi_edge[3+i]; }

	void set_added_to_medial_balls(bool f) { added_to_medial_balls = f; }
	bool get_added_to_medial_balls() { return added_to_medial_balls; }


};

#endif //MESECINA_V_FACE_BASE_2_H