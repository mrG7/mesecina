/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Ball_face_base_2.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef MESECINA_BALL_FACE_BASE_2_H
#define MESECINA_BALL_FACE_BASE_2_H

typedef	enum Union_classification {U_UNCLASSIFIED, U_INTERIOR, U_EXTERIOR, U_REGULAR, U_SINGULAR};

#include <CGAL/Triangulation_face_base_2.h>

template < class Gt, class Fb = CGAL::Triangulation_face_base_2<Gt> >
class Ball_face_base_2 : public  Fb
{
private:
	typedef Fb													Base;
public:
	typedef Gt                                           Geom_traits;
	typedef typename Geom_traits::FT					  Coord_type;
	typedef typename Fb::Vertex_handle                 Vertex_handle;
	typedef typename Fb::Face_handle                     Face_handle;

	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Fb::template Rebind_TDS<TDS2>::Other  Fb2;
		typedef Ball_face_base_2<Gt, Fb2>             Other;
	};
private:
	Coord_type sqa;
	Coord_type edge_sqa[3];
	bool computed;
	Union_classification c;
	Union_classification edge_c[3];

	void compute_sqa_if_needed() {
		if (computed || this->vertex(0) == 0 || this->vertex(1) == 0 || this->vertex(2) == 0 ) return;
		sqa = Gt().compute_squared_radius_2_object()(this->vertex(0)->point(),
			this->vertex(1)->point(),
			this->vertex(2)->point());
		for (int i=0; i<3; i++) 
			edge_sqa[i] = Gt().compute_squared_radius_2_object()(
			this->vertex((i+1)%3)->point(), this->vertex((i+2)%3)->point());
		computed = true;
	}

public:
	Ball_face_base_2()
		: Base(), computed(false) { init_sqa(); }

	void init_sqa() {
		sqa = edge_sqa[0] = edge_sqa[1] = edge_sqa[2] = 1;
		c = edge_c[0] = edge_c[1] = edge_c[2] = U_UNCLASSIFIED;
		computed = false;
	}

	Ball_face_base_2(Vertex_handle v0, 
		Vertex_handle v1, 
		Vertex_handle v2)
		: Base(v0,v1,v2), computed(false) { init_sqa(); }

	Ball_face_base_2(Vertex_handle v0, 
		Vertex_handle v1, 
		Vertex_handle v2,
		Face_handle n0, 
		Face_handle n1, 
		Face_handle n2)
		: Base(v0,v1,v2,n0,n1,n2), computed(false) { init_sqa(); }

	Coord_type get_squared_alpha() { 
		compute_sqa_if_needed();
		return sqa;
	}
	Coord_type get_squared_alpha(int i) { 
		compute_sqa_if_needed();
		return edge_sqa[i]; 
	}
	bool is_attached(int i) {
		CGAL::Bounded_side b = Gt().side_of_bounded_circle_2_object()(
			this->vertex((i+1)%3)->point(),
			this->vertex((i+2)%3)->point(),
			this->vertex(i)->point());

		return (b == CGAL::ON_BOUNDED_SIDE) ? true : false;
	}
	void set_classification(Union_classification c) { this->c = c; }
	Union_classification get_classification() { return c; }
	void set_classification(int i, Union_classification c) { this->edge_c[i] = c; }
	Union_classification get_classification(int i) { return this->edge_c[i]; }

};

#endif //MESECINA_BALL_FACE_BASE_2_H
