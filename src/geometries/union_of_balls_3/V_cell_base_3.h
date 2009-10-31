/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_cell_base_3.h 378 2008-08-03 18:50:59Z miklosb $
 */

#ifndef MESECINA_V_FACE_BASE_3_H
#define MESECINA_V_FACE_BASE_3_H

#include <CGAL/Triangulation_cell_base_with_circumcenter_3.h>

typedef	enum M_classification {M_UNCLASSIFIED, M_INTERIOR, M_EXTERIOR};
typedef enum C_classification {C_UNCLASSIFIED, C_CROSSING, C_ALPHA};

template < class Gt, class Fb, class Polar_tds >
class V_cell_base_3 : public  Fb {
private:
	typedef Fb														   Base;

public:
	typedef Gt													Geom_traits;
	typedef typename Geom_traits::FT							 Coord_type;
	typedef typename Fb::Vertex_handle						  Vertex_handle;
	typedef typename Fb::Cell_handle							Cell_handle;
	typedef typename Gt::Point_3									Point_3;
	typedef typename Polar_tds::Vertex_handle			Polar_vertex_handle;

	template < typename TDS2 >
	struct Rebind_TDS {
		typedef typename Fb::template Rebind_TDS<TDS2>::Other			Fb2;
		typedef V_cell_base_3<Gt, Fb2, Polar_tds>	Other;
	};

private:
	Polar_vertex_handle crossing_vertex[4];
	M_classification medial_class;
	Polar_vertex_handle polar_vertex;
	C_classification crossing_classification[4];
//	double angle_value[6];
	bool flat;

	void init_crossing_vertex() {
		crossing_vertex[0] = crossing_vertex[1] = crossing_vertex[2] = crossing_vertex[3] = 0;
		polar_vertex = 0;
		crossing_classification[0] = crossing_classification[1] = crossing_classification[2] = crossing_classification[3] = C_UNCLASSIFIED;
//		angle_value[0] = angle_value[1] = angle_value[2] = angle_value[3] = angle_value[4] = angle_value[5] = 0.0;
	}


public:
	V_cell_base_3()	: Base(), medial_class(M_UNCLASSIFIED), flat(false) { init_crossing_vertex(); }

	V_cell_base_3(Vertex_handle v0, Vertex_handle v1,
                          Vertex_handle v2, Vertex_handle v3)
						  : Base(v0, v1, v2, v3), medial_class(M_UNCLASSIFIED), flat(false) { init_crossing_vertex(); }

	V_cell_base_3(Vertex_handle v0, Vertex_handle v1,
                          Vertex_handle v2, Vertex_handle v3,
                          Cell_handle   n0, Cell_handle   n1,
                          Cell_handle   n2, Cell_handle   n3)
						  : Base(v0, v1, v2, v3, n0, n1, n2, n3), medial_class(M_UNCLASSIFIED), flat(false) { init_crossing_vertex(); }

	void set_crossing_vertex(int i, Polar_vertex_handle v) { crossing_vertex[i] = v; }
	Polar_vertex_handle get_crossing_vertex(int i) { return crossing_vertex[i]; }

	C_classification get_crossing_classification(int i) { return crossing_classification[i]; }
	void set_crossing_classification(int i, C_classification c) { crossing_classification[i] = c; }

	M_classification get_medial_classification() { return medial_class; }
	void set_medial_classification(M_classification c) { medial_class = c; }

	Polar_vertex_handle get_polar_vertex() { return polar_vertex; }
	void set_polar_vertex(Polar_vertex_handle p) { polar_vertex = p; }

	bool is_flat() { return flat; }
	void set_flat(bool f) { flat = f; }

	//double get_angle_value(int id) { return angle_value[id]; }
	//void set_angle_value(int id, double v) { angle_value[id] = v; }
};

#endif //MESECINA_V_FACE_BASE_3_H