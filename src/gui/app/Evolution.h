/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Evolution.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef MESECINA_EVOLUTION_H
#define MESECINA_EVOLUTION_H

#include <list>
#include <vector>
#include <string>

#include <geometries/Geometry.h>


class Evolution {
public:
	typedef std::list<Geometry*> Geometry_list;
	typedef std::vector<Geometry_list> Geometry_list_vector;
	Geometry_list_vector gll;

	Geometry_list* get_geometries(unsigned index);
	Evolution(const std::string& name, Geometry_list* gl);

	std::string name;
	std::string get_name();

	int show_max;
	int current;
	int parent_index;

	int get_max_computed();

	void init_geometries(Geometry_list* gl);
	void add_geometries(Geometry_list* gl);
//	void step_up_operation();

	//virtual void update_parent() {}
//	~Evolution();

//	static Geometry_list* protect_geometry;


};

#endif //MESECINA_EVOLUTION_H