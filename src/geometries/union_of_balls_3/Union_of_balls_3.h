/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_UNION_OF_BALLS_3_H
#define MESECINA_UNION_OF_BALLS_3_H

#include <geometries/Geometry.h>

#include <CGAL/Regular_triangulation_3.h>
#include <CGAL/Weighted_alpha_shape_euclidean_traits_3.h>
#include <CGAL/Alpha_shape_3.h>
#include <CGAL/Polyhedron_3.h>

#include "V_triangulation_3.h"

#include <list>
#include <vector>
#include <string>


template <class K> // Kernel used in all computations
class Union_of_balls_3 : public Geometry {

public:
	typedef typename K													GT;
	typedef typename K::FT												NT;
	typedef typename K::Point_3									   Point_3;
	typedef typename K::Vector_3                                  Vector_3;
	typedef typename K::Triangle_3                              Triangle_3;
	typedef typename K::Segment_3                                Segment_3;
	typedef typename K::Line_3										Line_3;
	typedef typename K::Ray_3										 Ray_3;
	typedef typename K::Plane_3									   Plane_3;
	typedef typename K::Sphere_3								  Sphere_3;

	typedef CGAL::Weighted_alpha_shape_euclidean_traits_3<K> WGt;

	// alpha shapes
	typedef CGAL::Alpha_shape_vertex_base_3<WGt>         WVb;
	typedef CGAL::Alpha_shape_cell_base_3<WGt>           WFb;
	typedef CGAL::Triangulation_data_structure_3<WVb,WFb> WTds;
	typedef CGAL::Regular_triangulation_3<WGt,WTds>       Alpha_triangulation_3;
	typedef CGAL::Alpha_shape_3<Alpha_triangulation_3>        Alpha_shape_3;
	typedef typename Alpha_shape_3::Facet						Alpha_facet;
	typedef typename Alpha_shape_3::Cell						Alpha_cell;
	typedef typename Alpha_shape_3::Cell_handle						Alpha_cell_handle;
	typedef typename Alpha_shape_3::Vertex						Alpha_vertex;
	typedef typename Alpha_shape_3::Vertex_handle				Alpha_vertex_handle;
	typedef typename Alpha_shape_3::Weighted_point						Weighted_point;
	typedef typename Alpha_shape_3::Weighted_point::Point				Bare_point;

	typedef V_triangulation_3<K,WTds>				V_triangulation_3;
	typedef typename V_triangulation_3::Vertex_handle			V_vertex_handle;
	typedef typename V_triangulation_3::Edges_with_doubles Edges_with_doubles;
	typedef typename V_triangulation_3::Edges_set Edges_set;

	typedef CGAL::Polyhedron_3<K>								Polyhedron;

	Union_of_balls_3();
	virtual ~Union_of_balls_3();
	virtual Geometry* clone();
	
	//// methods to communicate with other geometries
	virtual std::list<std::string> offer_structures();
	virtual void* give_structure(const std::string& name);
	virtual void receive_structure_changed(const std::string& name);
	
	//// points communication with user interface
	//virtual void add_points(std::list<Point3D>*);
	//virtual std::list<Point3D>* get_points();

	// ball communication with user interface
	virtual void add_weighted_points(std::list<Point4D>*);
	virtual std::list<Point4D>* get_weighted_points();

	// directly handle file io
	virtual void load_generic_file(const std::string&);
	virtual void save_generic_file(const std::string&);

	// receive application settings changes
	virtual void application_settings_changed(const QString&);
	// modification for evolutions
	virtual void apply_modification(const std::string& );

	void invalidate_cache();

	Alpha_shape_3* get_alpha_shape();
	V_triangulation_3* get_v_triangulation();
	V_triangulation_3* get_classified_v_triangulation();
	V_triangulation_3* get_flooded_v_triangulation();
//	V_triangulation_3* get_local_filtered_medial_axis();
	Edges_with_doubles* get_angle_filtered_medial_axis();
	Edges_with_doubles* get_topology_angle_filtered_medial_axis();
	Edges_with_doubles* get_scale_filtered_medial_axis();
	Edges_set* get_medial_axis();
	std::string* get_moff_string();

	Polyhedron* get_boundary_polyhedron();
	void mesh_union_boundary_implicit();

	void write_medial_axis_to_off(const std::string& file_name);
	void write_simplified_medial_axis_to_off(const std::string& file_name);
	void write_medial_axis_to_moff(const std::string& file_name);
	void write_less_medial_balls(const std::string& file_name);

	void update_ball_scaling();


//protected:
	static bool intersect_ball_line(const Weighted_point &wp, const Line_3 &l, Point_3 &ip1, Point_3 &ip2);

public:
	Alpha_shape_3* alpha_shape;
	Polyhedron surface_polyhedron;
	std::list<Point4D> points, scale_balls;
	V_triangulation_3 v_triangulation;
	bool has_moff_string, has_boundary_surface, has_alpha_shape, has_v_triangulation, is_v_edges_classified, is_v_cell_flooded, has_local_filtered_medial_axis, has_topology_angle_ma, has_scale_filtered_medial_axis, has_medial_edges;
	double growth_ratio;
	std::string moff_string;
	int topology_filtration_steps;
};

#endif //MESECINA_UNION_OF_BALLS_3_H