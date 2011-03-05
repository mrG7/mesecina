/* This source file is part of Mesecina, a software for visualization and studying of
* the medial axis and related computational geometry structures.
* More info: http://www.agg.ethz.ch/~miklosb/mesecina
* Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
*
* $Id: Power_crust_2.h 531 2009-02-05 22:02:06Z miklosb $
*/

#ifndef MESECINA_POWER_CRUST_2_IMPLEMENT_H
#define MESECINA_POWER_CRUST_2_IMPLEMENT_H

#include "Classified_triangulation_2.h"
#include "Polar_triangulation_2.h"

#include "layers/Points_layer_2.h"
#include "layers/Delaunay_layer_2.h"
#include "layers/Voronoi_balls_layer_2.h"
#include "layers/Filtered_medial_axis_layer_2.h"
#include "layers/Polar_triangulation_layer_2.h"
#include "layers/Polar_balls_layer_2.h"

//#include "evolutions/Smoothing_evolution_2.h"
//#include "evolutions/Noise_perturb_evolution_2.h"
//#include "evolutions/Loop_subdivision_evolution_2.h"

#include <constants.h>
#include <geometries/Geometry.h>
#include <gui/app/Managable_layer.h>

#include <CGAL/Filtered_kernel.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

#include <list>
#include <vector>


template <class K>
Power_crust_2<K>::Power_crust_2() : has_polar_triangulation(false) {

	GL_draw_layer_2* points_layer = new Points_layer_2< Power_crust_2<K> >("Points",this, "Sample points");
	add_layer(points_layer);
	GL_draw_layer_2* delaunay_layer = new Delaunay_layer_2< Power_crust_2<K> >("Delaunay",this, "Delaunay triangulation of sample points");
	add_layer(delaunay_layer);
	GL_draw_layer_2* voronoi_layer = new Delaunay_layer_2< Power_crust_2<K> >("Voronoi",this, "Voronoi diagram of sample points", true);
	add_layer(voronoi_layer);
	GL_draw_layer_2* crust_layer = new Delaunay_layer_2< Power_crust_2<K> >("Curve",this, "Reconstruction from the one-step medial axis", false, true);
	add_layer(crust_layer);
	GL_draw_layer_2* crust_normal_layer = new Delaunay_layer_2< Power_crust_2<K> >("Curve normals",this, "Normals from the reconstruction from the one-step medial axis", false, true, true);
	add_layer(crust_normal_layer);


	GL_draw_layer_2* voronoi_circles_layer = new Voronoi_balls_layer_2< Power_crust_2<K> >("Voronoi circles",this, "Voronoi circles of sample points", false, false);
	add_layer(voronoi_circles_layer);
	GL_draw_layer_2* voronoi_disks_layer = new Voronoi_balls_layer_2< Power_crust_2<K> >("Voronoi disks",this, "Voronoi disks of sample points", false, true);
	add_layer(voronoi_disks_layer);

	GL_draw_layer_2* inner_voronoi_circles_layer = new Voronoi_balls_layer_2< Power_crust_2<K> >("Interior Voronoi circles",this, "Interior Voronoi circles of sample points", false, false, true, true);
	add_layer(inner_voronoi_circles_layer);
	GL_draw_layer_2* inner_voronoi_disks_layer = new Voronoi_balls_layer_2< Power_crust_2<K> >("Interior Voronoi disks",this, "Interior Voronoi disks of sample points", false, true, true, true);
	add_layer(inner_voronoi_disks_layer);
	GL_draw_layer_2* inner_voronoi_points_layer = new Voronoi_balls_layer_2< Power_crust_2<K> >("Interior Voronoi vertices",this, "Interior Voronoi vertices of sample points", true, true, true, true);
	add_layer(inner_voronoi_points_layer);

	GL_draw_layer_2* exterior_voronoi_circles_layer = new Voronoi_balls_layer_2< Power_crust_2<K> >("Exterior Voronoi circles",this, "Exterior Voronoi circles of sample points", false, false, true, false);
	add_layer(exterior_voronoi_circles_layer);
	GL_draw_layer_2* exterior_voronoi_disks_layer = new Voronoi_balls_layer_2< Power_crust_2<K> >("Exterior Voronoi disks",this, "Exterior Voronoi diagram of sample points", false, true, true, false);
	add_layer(exterior_voronoi_disks_layer);
	GL_draw_layer_2* exterior_voronoi_points_layer = new Voronoi_balls_layer_2< Power_crust_2<K> >("Exterior Voronoi vertices",this, "Exterior Voronoi vertices of sample points", true, true, true, false);
	add_layer(exterior_voronoi_points_layer);


	GL_draw_layer_2* all_medial_axis_layer = new Filtered_medial_axis_layer_2< Power_crust_2<K> >("All medial axis",this, "Medial axis (interior and exterior) from the one-step algorithm", false, false, false);
	add_layer(all_medial_axis_layer);

	GL_draw_layer_2* medial_axis_layer = new Filtered_medial_axis_layer_2< Power_crust_2<K> >("Medial axis",this, "Interior medial axis of the one-step algorithm", false, true, false);
	add_layer(medial_axis_layer);

	GL_draw_layer_2* lamba_medial_axis_layer = new Filtered_medial_axis_layer_2< Power_crust_2<K> >("Lambda medial axis",this, "Lambda medial axis of the interior", true, true, false, false, true);
	add_layer(lamba_medial_axis_layer);
	GL_draw_layer_2* lamba_medial_disks_layer = new Filtered_medial_axis_layer_2< Power_crust_2<K> >("Lambda medial disks",this, "Lambda medial disks in the interior", true, true, true, true, true);
	add_layer(lamba_medial_disks_layer);
	GL_draw_layer_2* lamba_medial_circles_layer = new Filtered_medial_axis_layer_2< Power_crust_2<K> >("Lambda medial circles",this, "Lambda medial circles in the interior", true, true, true, false, true);
	add_layer(lamba_medial_circles_layer);

	GL_draw_layer_2* angle_medial_axis_layer = new Filtered_medial_axis_layer_2< Power_crust_2<K> >("Angle medial axis",this, "Angle medial axis of the interior", true, true, false, false, false);
	add_layer(angle_medial_axis_layer);
	GL_draw_layer_2* angle_medial_disks_layer = new Filtered_medial_axis_layer_2< Power_crust_2<K> >("Angle medial disks",this, "Angle medial disks in the interior", true, true, true, true, false);
	add_layer(angle_medial_disks_layer);
	GL_draw_layer_2* angle_medial_circles_layer = new Filtered_medial_axis_layer_2< Power_crust_2<K> >("Angle medial circles",this, "Angle medial circles in the interior", true, true, true, false, false);
	add_layer(angle_medial_circles_layer);

	GL_draw_layer_2* scale_medial_axis_layer = new Filtered_medial_axis_layer_2< Power_crust_2<K> >("Scale medial axis",this, "Scale medial axis of the interior", true, true, false, false, false, true);
	add_layer(scale_medial_axis_layer);
	GL_draw_layer_2* scale_medial_disks_layer = new Filtered_medial_axis_layer_2< Power_crust_2<K> >("Scale medial disks",this, "Scale medial disks in the interior", true, true, true, true, false, true);
	add_layer(scale_medial_disks_layer);
	GL_draw_layer_2* scale_medial_circles_layer = new Filtered_medial_axis_layer_2< Power_crust_2<K> >("Scale medial circles",this, "Scale medial circles in the interior", true, true, true, false, false, true);
	add_layer(scale_medial_circles_layer);


	GL_draw_layer_2* regular_layer = new Polar_triangulation_layer_2< Power_crust_2<K> >("Polar triangulation",this, "Regular triangulation of poles", false, false);
	add_layer(regular_layer);
	GL_draw_layer_2* power_layer = new Polar_triangulation_layer_2< Power_crust_2<K> >("Polar power diagram",this, "Power diagram of poles", true, false);
	add_layer(power_layer);
	GL_draw_layer_2* power_crust_layer = new Polar_triangulation_layer_2< Power_crust_2<K> >("Power crust",this, "Power crust, curve reconstruction using the power diagram of poles", true, true);
	add_layer(power_crust_layer);
	GL_draw_layer_2* anti_crust_layer = new Polar_triangulation_layer_2< Power_crust_2<K> >("Anti crust edges",this, "Anti crust edges, regular triangulation edges not dual to power crust edges", false, true);
	add_layer(anti_crust_layer);

	GL_draw_layer_2* polar_circles_layer = new Polar_balls_layer_2< Power_crust_2<K> >("Polar circles",this, "Polar circles of sample points", false, false);
	add_layer(polar_circles_layer);
	GL_draw_layer_2* polar_disks_layer = new Polar_balls_layer_2< Power_crust_2<K> >("Polar disks",this, "Polar disks of sample points", false, true);
	add_layer(polar_disks_layer);

	GL_draw_layer_2* inner_polar_circles_layer = new Polar_balls_layer_2< Power_crust_2<K> >("Interior polar circles",this, "Interior polar circles of sample points", false, false, true, true);
	add_layer(inner_polar_circles_layer);
	GL_draw_layer_2* inner_polar_disks_layer = new Polar_balls_layer_2< Power_crust_2<K> >("Interior polar disks",this, "Interior polar disks of sample points", false, true, true, true);
	add_layer(inner_polar_disks_layer);
	GL_draw_layer_2* inner_polar_points_layer = new Polar_balls_layer_2< Power_crust_2<K> >("Interior polar vertices",this, "Interior polar vertices of sample points", true, true, true, true);
	add_layer(inner_polar_points_layer);

	GL_draw_layer_2* exterior_polar_circles_layer = new Polar_balls_layer_2< Power_crust_2<K> >("Exterior polar circles",this, "Exterior polar circles of sample points", false, false, true, false);
	add_layer(exterior_polar_circles_layer);
	GL_draw_layer_2* exterior_polar_disks_layer = new Polar_balls_layer_2< Power_crust_2<K> >("Exterior polar disks",this, "Exterior polar diagram of sample points", false, true, true, false);
	add_layer(exterior_polar_disks_layer);
	GL_draw_layer_2* exterior_polar_points_layer = new Polar_balls_layer_2< Power_crust_2<K> >("Exterior polar vertices",this, "Exterior polar vertices of sample points", true, true, true, false);
	add_layer(exterior_polar_points_layer);

	add_evolution("Smooth");
	add_evolution("Subdivide");
	add_evolution("Noise perturb");
	Application_settings::add_double_setting("noise-perturb-factor", 2);
	
	input_triangulation.clear();
	polar_triangulation.clear();
}

template <class K>
void Power_crust_2<K>::apply_modification(const std::string& name) {
	if (name=="Smooth") {
		Triangulation_2* t = get_crust_classified_triangulation();
		std::list< Point_2 > orig_points;
		Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
		for (v_it = t->finite_vertices_begin(); v_it != v_end; v_it++) {
			bool has;
			Point_2 p = t->smooth_position(v_it, 0.5, &has);
			if (has)
				orig_points.push_back(p);
		}
		t->clear();

		std::list< Point_2 >::iterator p_it, p_end = orig_points.end();
		for (p_it = orig_points.begin(); p_it!=p_end; p_it++) {
			t->insert(*p_it);
		}

		invalidate_cache();
	}
	if (name=="Subdivide") {
		Triangulation_2* t = get_crust_classified_triangulation();
		std::list< Point_2 > orig_points;
		Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
		for (v_it = t->finite_vertices_begin(); v_it != v_end; v_it++) {
			bool has;
			Point_2 p = t->loop_even(v_it, &has);
			if (has)
				orig_points.push_back(p);
		}

		for (Triangulation_2::Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
			Triangulation_2::Face_handle f = e_it->first;
			int id = e_it->second;
			if (f->get_edge_classification(id) == C_CRUST)
				orig_points.push_back(t->loop_uneven(f, id));
		}

		t->clear();

		std::list< Point_2 >::iterator p_it, p_end = orig_points.end();
		for (p_it = orig_points.begin(); p_it!=p_end; p_it++) {
			t->insert(*p_it);
		}

		invalidate_cache();
	}
	if (name=="Noise perturb") {
		double noise_ratio = Application_settings::get_double_setting("noise-perturb-factor");

		Triangulation_2* t = get_crust_classified_triangulation();
		std::list< Point_2 > orig_points;
		Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
		for (v_it = t->finite_vertices_begin(); v_it != v_end; v_it++) {
			bool has;
			Point_2 p = t->noise_position(v_it, noise_ratio, &has);
			if (has)
				orig_points.push_back(p);
		}
		t->clear();

		std::list< Point_2 >::iterator p_it, p_end = orig_points.end();
		for (p_it = orig_points.begin(); p_it!=p_end; p_it++) {
			t->insert(*p_it);
		}

		invalidate_cache();
	}
}


template <class K>
Power_crust_2<K>::~Power_crust_2() {
	std::vector<Managable_layer*>::iterator l_it, l_end = l_layers.end();
	for (l_it = l_layers.begin(); l_it != l_end; l_it++)
		delete *l_it;
//	std::cout << LOG_BLUE << "freeing up layers" << std::endl;
	l_layers.clear();
}

template <class K>
Geometry* Power_crust_2<K>::clone() {
	std::cout << PROGRESS_STATUS << "Copying Power crust data structures" << std::endl;
	std::cout << PROGRESS_MINIMUM << "0" << std::endl;
	std::cout << PROGRESS_MAXIMUM << "2" << std::endl;
	Power_crust_2<K>* new_power_crust = new Power_crust_2<K>();
	new_power_crust->input_triangulation = input_triangulation;
	//		new_power_crust->invalidate_cache();
	std::cout << PROGRESS_VALUE << "1" << std::endl;
	if (has_polar_triangulation) new_power_crust->polar_triangulation = polar_triangulation;
	std::cout << PROGRESS_VALUE << "2" << std::endl;
	new_power_crust->has_polar_triangulation = has_polar_triangulation;
	std::cout << PROGRESS_DONE << std::endl;

	new_power_crust->l_app_variables.insert(new_power_crust->l_app_variables.begin(),
		l_app_variables.begin(), l_app_variables.end());
	return new_power_crust;
}

template <class K>
std::list<std::string> Power_crust_2<K>::offer_structures() {
	std::list<std::string> names;
	names.push_back(SHARED_INNER_VORONOI_BALLS);
	names.push_back(SHARED_INNER_POLES);
	names.push_back(SHARED_ONESTEP_CURVE);
	names.push_back(SHARED_INPUT_TRIANGULATION);
	names.push_back(SHARED_POINTS);
	return names;
}

template <class K>
void* Power_crust_2<K>::give_structure(const std::string& name) {
	if (name == SHARED_INNER_VORONOI_BALLS)
		return input_triangulation.get_inner_balls();
	if (name == SHARED_INNER_POLES) {
		get_polar_triangulation();
		return polar_triangulation.get_inner_balls();
	}
	if (name == SHARED_ONESTEP_CURVE) {
		get_crust_classified_triangulation();
		return input_triangulation.get_ordered_onestep_points();
	}
	if (name == SHARED_INPUT_TRIANGULATION) {
		return &input_triangulation;
	}
	if (name == SHARED_POINTS) {
		return get_point_list();
	}
	return 0;
}


template <class K>
void Power_crust_2<K>::add_point(double x, double y) {
	input_triangulation.insert(Point_2(x,y));
	invalidate_cache();
}

template <class K>
void Power_crust_2<K>::add_points(std::list<QPointF>* points) {
	std::cout << "Starting to build delaunay triangulation..." << std::endl;
	std::cout << PROGRESS_STATUS << "Building delaunay triangulation" << std::endl;
	std::cout << PROGRESS_MINIMUM << "0" << std::endl;
	std::cout << PROGRESS_MAXIMUM <<  points->size()-1 << std::endl;
	std::list<QPointF>::iterator p_it, p_end = points->end(); int i=0;
	for (p_it = points->begin(); p_it != p_end; p_it++, i++) {
		std::cout << PROGRESS_VALUE << i << std::endl;
		input_triangulation.insert(Point_2(p_it->x(), p_it->y()));
	}
	std::cout << PROGRESS_DONE << std::endl;
	invalidate_cache();
}

template <class K>
std::list<QPointF>* Power_crust_2<K>::get_points() {
	points.clear();
	Finite_vertices_iterator v_it, v_end = input_triangulation.finite_vertices_end();
	for (v_it = input_triangulation.finite_vertices_begin(); v_it!=v_end; v_it++) {
		points.push_back(QPointF(CGAL::to_double(v_it->point().x()),
			CGAL::to_double(v_it->point().y())));
	}
	return &points;
}

template <class K>
void Power_crust_2<K>::invalidate_cache() {
	invalidate_all_layers();
	emit announce_structure_changed(SHARED_INNER_VORONOI_BALLS);
	emit announce_structure_changed(SHARED_INNER_POLES);
	emit announce_structure_changed(SHARED_ONESTEP_CURVE);
	emit announce_structure_changed(SHARED_INPUT_TRIANGULATION);
	emit announce_structure_changed(SHARED_POINTS);
	has_polar_triangulation = false;
	polar_triangulation.set_dirty();
	input_triangulation.set_dirty();

	add_variable("Points", input_triangulation.number_of_vertices());
	add_variable("Delaunay/Voronoi edges (infinite edges included)", input_triangulation.tds().number_of_edges());
	add_variable("Delaunay triangles -- Voronoi vertices", input_triangulation.number_of_faces());
}

template <class K>
typename Power_crust_2<K>::Triangulation_2* Power_crust_2<K>::get_input_triangulation() {
	return &input_triangulation;
}

template <class K>
typename Power_crust_2<K>::Polar_triangulation_2* Power_crust_2<K>::get_polar_triangulation() {
	if (!has_polar_triangulation) {
		polar_triangulation.compute_polar_triangulation(&input_triangulation);
		has_polar_triangulation = true;
	}
	return &polar_triangulation;
}


template <class K>
typename Power_crust_2<K>::Polar_triangulation_2* Power_crust_2<K>::get_classified_polar_triangulation() {
	get_polar_triangulation();
	if (polar_triangulation.is_dirty()) {
		polar_triangulation.compute_classification();
	}
	return &polar_triangulation;
}


template <class K>
typename Power_crust_2<K>::Triangulation_2* Power_crust_2<K>::get_crust_classified_triangulation() {
	if (input_triangulation.is_crust_dirty()) {
		input_triangulation.compute_crust_classification();

		// Add classified variables !!!
		Triangulation_2::Finite_faces_iterator f_it, f_end = input_triangulation.finite_faces_end();
		int int_count, ext_count, un_count;
		int_count = ext_count = un_count = 0;

		for (f_it = input_triangulation.finite_faces_begin(); f_it!=f_end; f_it++) {
			if (f_it->get_onestep_classification() == O_INTERIOR) int_count++;
			if (f_it->get_onestep_classification() == O_EXTERIOR) ext_count++;
			if (f_it->get_onestep_classification() == O_UNCLASSIFIED) un_count++;
		}
		add_variable("Interior Voronoi vertices", int_count);
		add_variable("Exterior Voronoi vertices", ext_count);
		add_variable("Unclassified Voronoi vertices", un_count);

		int crust_count, medial_count;
		crust_count = medial_count = 0;
		for (Triangulation_2::Finite_edges_iterator e_it = input_triangulation.finite_edges_begin(), e_end = input_triangulation.finite_edges_end();
			e_it != e_end; ++e_it) {
				int id = e_it->second;
				if (e_it->first->get_edge_classification(id) == C_CRUST) crust_count++;
				else medial_count++;
		}
		add_variable("Reconstruction segments", crust_count);
		add_variable("All medial axis segments", medial_count);


	}
	return &input_triangulation;
}


template <class K>
typename Power_crust_2<K>::Triangulation_2* Power_crust_2<K>::get_lambda_angle_classified_triangulation() {
	get_crust_classified_triangulation();
	if (input_triangulation.is_lambda_dirty()) {
		input_triangulation.compute_lambda_angle_classification();
	}
	return &input_triangulation;
}

template <class K>
QVector< QPointF >* Power_crust_2<K>::get_ordered_normals() {
	get_crust_classified_triangulation();
	return input_triangulation.get_ordered_onestep_normals();
}

template <class K>
QVector< QPointF >* Power_crust_2<K>::get_ordered_points() {
	get_crust_classified_triangulation();
	return static_cast<QVector< QPointF >*>(input_triangulation.get_ordered_onestep_points());
}


template <class K>
std::list<typename Power_crust_2<K>::Point_2>* Power_crust_2<K>::get_point_list() {
	point_list.clear();
	Finite_vertices_iterator v_it, v_end = input_triangulation.finite_vertices_end();
	for (v_it = input_triangulation.finite_vertices_begin(); v_it!=v_end; v_it++) {
		point_list.push_back(v_it->point());
	}
	return &point_list;
}

#endif //MESECINA_POWER_CRUST_2_IMPLEMENT_H