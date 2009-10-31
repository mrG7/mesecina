/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Ball_face_base_2.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef MESECINA_GEOMETRY_FACTORY_H
#define MESECINA_GEOMETRY_FACTORY_H

#include <list>
#include <string>

class Geometry;

class Geometry_factory {
public:
	// these have to be changed when a new geometry is implemented
	static void fill_available_geometries();
	static Geometry* create_new_geometry(const std::string& name);

	static std::list<std::string> available_geometries;

	static std::list<std::string> depends_on(const std::string& name);
	static std::list<std::string> conflicts_with(const std::string& name);

};

#endif // MESECINA_GEOMETRY_FACTORY_H