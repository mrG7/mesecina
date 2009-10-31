/* This source file is part of Mesecina, a software for visualization and studying of
* the medial axis and related computational geometry structures.
* More info: http://www.agg.ethz.ch/~miklosb/mesecina
* Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
*
* $Id: Evolution.cpp 78 2007-03-25 03:19:20Z miklosb $
*/
#include <cassert>

#include <gui/app/Evolution.h>
#include <geometries/Geometry.h>


Evolution::Evolution(const std::string& name, Geometry_list* gl) {
	this->name = name;
	show_max = 3;
	init_geometries(gl);
}

int Evolution::get_max_computed() { return static_cast<int>(gll.size()-1); }

std::string Evolution::get_name() { return name; }

Evolution::Geometry_list* Evolution::get_geometries(unsigned index) {
	assert(index < gll.size());
	current = index+1;
//	protect_geometry = gll[index];
	return &gll[index];
}

void Evolution::init_geometries(Geometry_list* gl) {
	current = 0;
	gll.clear();

	gll.push_back(Geometry_list());
	Geometry_list::iterator g_it, g_end = gl->end();
	for (g_it=gl->begin(); g_it!=g_end; ++g_it) {
		gll.back().push_back(*g_it);
	}

}


void Evolution::add_geometries(Geometry_list* gl) {
	gll.push_back(Geometry_list());
	Geometry_list::iterator g_it, g_end = gl->end();
	for (g_it=gl->begin(); g_it!=g_end; ++g_it) {
		(*g_it)->apply_modification(name);
		gll.back().push_back(*g_it);
	}
}



