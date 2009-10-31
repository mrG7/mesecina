/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#include "Witness_2.h"

#include "Witness_landmark_set_2.h"
#include "layers\Landmark_delaunay_layer_2.h"

#include <constants.h>
#include <gui/app/static/Application_settings.h>

template <class K>
Witness_2<K>::Witness_2() : Geometry(), has_points(false) {
	//triangulation_source = _triangulation_source;
	//layer_name = _layer_name;

	GL_draw_layer_2* landmark_point_layer = new Landmark_delaunay_layer_2<Witness_2<K> >("Landmarks", this, "Set of landmarks", true, false, false, false);
	add_layer(landmark_point_layer);
	GL_draw_layer_2* landmark_delaunay_layer = new Landmark_delaunay_layer_2<Witness_2<K> >("L Delaunay", this, "Delaunay of landmarks", false, false, false, false);
	add_layer(landmark_delaunay_layer);
	GL_draw_layer_2* landmark_voronoi_layer = new Landmark_delaunay_layer_2<Witness_2<K> >("L Voronoi", this, "Voronoi of landmarks", false, true, false, false);
	add_layer(landmark_voronoi_layer);

	GL_draw_layer_2* witness_reconstr_layer = new Landmark_delaunay_layer_2<Witness_2<K> >("Witness reconstruction", this, "Witness complex based curve reconstruction", false, false, true, false);
	add_layer(witness_reconstr_layer);

	GL_draw_layer_2* witness_ma_layer = new Landmark_delaunay_layer_2<Witness_2<K> >("Witness medial axis", this, "Witness complex based medial axis approximation", false, true, true, false);
	add_layer(witness_ma_layer);
	GL_draw_layer_2* witness_maf_layer = new Landmark_delaunay_layer_2<Witness_2<K> >("Witness full medial axis", this, "Witness complex based medial axis approximation - exterior included", false, true, true, true);
	add_layer(witness_maf_layer);

	add_evolution("Landmark maxmin");

	Application_settings::add_double_setting("witness-complex-min-landmarks-percentage",0);
}

template <class K>
Witness_2<K>::~Witness_2() {
}

template <class K>
Geometry* Witness_2<K>::clone() {
	Witness_2* new_witness_2 = new Witness_2();
	new_witness_2->witness_landmark_set = witness_landmark_set;
	//new_witness_2->points = points;
	new_witness_2->has_points = has_points;
	return new_witness_2;
}

template <class K>
void Witness_2<K>::apply_modification(const std::string& name) {
	if (name == "Landmark maxmin") {
		Witness_landmark_set* wls =  get_witness_landmark_set();
		Witness_handle ph = wls->next_landmark_max_min();
		wls->add_landmark(ph);
	}

}

template <class K>
void Witness_2<K>::application_settings_changed(const QString& settings_name) {
	if (settings_name=="witness-complex-min-landmarks-percentage") {
		Witness_landmark_set* wls =  get_witness_landmark_set();
		size_t min_landmarks = (size_t) 
			(wls->number_of_witnesses() * Application_settings::get_double_setting("witness-complex-min-landmarks-percentage")/100.0);
		if (min_landmarks > wls->number_of_witnesses()) return;
		while (wls->landmark_list().size() < min_landmarks) {
			Witness_handle ph = wls->next_landmark_max_min();
			wls->add_landmark(ph);
		}
//		std::cout << "Landmark points " << wls->landmark_list().size() << std::endl;
		invalidate_cache();
		has_points = true;
//		emit announce_structure_changed(SHARED_LANDMARK_TRIANGULATION);
		emit do_widget_repaint();
	}
}


template <class K>
void Witness_2<K>::receive_structure_changed(const std::string& name) {
	if (name == SHARED_POINTS) {
		invalidate_cache();
	}
}


template <class K>
std::list<std::string> Witness_2<K>::offer_structures() {
		std::list<std::string> res;
		res.push_back(SHARED_LANDMARK_TRIANGULATION);
//		res.push_back(WITNESS_TRIANGULATION);
//		res.push_back(WITNESS_POINT_LIST);
//		res.push_back(LANDMARK_POINT_LIST);
		return res;
	}

template <class K>
void* Witness_2<K>::give_structure(const std::string& name) {
		if (name == SHARED_LANDMARK_TRIANGULATION) {
			return get_witness_complex_triangulation();
		//} else if (name == WITNESS_TRIANGULATION) {
		//	return witness_landmark_set.witness_triangulation();
		//} else if (name == WITNESS_POINT_LIST) {
		//	return get_witness_point_list();
		//} else if (name == LANDMARK_POINT_LIST) {
		//	return &witness_landmark_set.landmark_list();
		} else {
			return 0;
		}
	}

template <class K>
typename Witness_2<K>::Witness_landmark_set* Witness_2<K>::get_witness_landmark_set() {
	if (!has_points) {
		//std::cout << LOG_GREEN << "get points from the power_crust" << std::endl;
		void* p = request_structure(SHARED_POINTS);
		if (!p) std::cout << LOG_ERROR << "Error input points from Power_crust_2, activate a power crust geometry!" << std::endl;
		else {
			std::list<Point_2>* ps = static_cast<std::list<Point_2>*>(p);
			witness_landmark_set.clear_all();
			std::list<Point_2>::iterator p_it, p_end = ps->end();
			for (p_it = ps->begin(); p_it != p_end; ++p_it) 
				witness_landmark_set.add_witness(*p_it);
			has_points = true;
		}
	}
	return &witness_landmark_set;
}

template <class K>
typename Witness_2<K>::Witness_complex_triangulation * Witness_2<K>::get_witness_complex_triangulation() {
	get_witness_landmark_set(); // make sure we have the points
	return witness_landmark_set.witness_complex_triangulation();
}




template <class K>
void Witness_2<K>::invalidate_cache() {
	invalidate_all_layers();
	has_points = false;
	emit announce_structure_changed(SHARED_LANDMARK_TRIANGULATION);
}
