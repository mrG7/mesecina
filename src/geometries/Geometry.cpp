/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Geometry.cpp 78 2007-03-25 03:19:20Z miklosb $
 */

#include <geometries/Geometry.h>
#include <gui/app/Managable_layer.h>
//#include <gui/app/Main_window.h>

//void Geometry::announce_structure_changed(const std::string& name) { 
//	/// TODO emit the event
////	main_window->shared_structure_changed(name); 
//}
//void* Geometry::request_structure(const std::string& name) { 
//	/// TODO emit the event
////	return main_window->get_shared_structure(name);
//	return 0;
//}

Geometry::Geometry() {
	l_layers.clear();
	l_app_variables.clear();
	l_evolutions.clear();
}

void Geometry::add_layer(Managable_layer* l) {
	l_layers.push_back(l);
}

void Geometry::add_evolution(const std::string& name) {
	l_evolutions.push_back(name);
}

void Geometry::add_variable(const char* name, float val) {
	std::vector<Application_variable>::iterator v_it, v_end = l_app_variables.end();
	for (v_it = l_app_variables.begin(); v_it!=v_end; v_it++)
		if (v_it->first == QString(name)) {
			v_it->second = QString::number(val); 
			return;
		}
	l_app_variables.push_back(Application_variable( QString(name),QString::number(val)));
}

void Geometry::add_variable(const char* name, const char* val) {
	std::vector<Application_variable>::iterator v_it, v_end = l_app_variables.end();
	for (v_it = l_app_variables.begin(); v_it!=v_end; v_it++)
		if (v_it->first == QString(name)) {
			v_it->second = val; 
			return;
		}
	l_app_variables.push_back(Application_variable( QString(name),val));
}

void Geometry::invalidate_all_layers() {
	std::vector<Managable_layer*>::iterator l_it, l_end = l_layers.end();
	for (l_it = l_layers.begin(); l_it != l_end; l_it++) {
//		std::cout << "calling invalidate on: " << *l_it << std::endl;
		(*l_it)->invalidate_cache();
	}
}