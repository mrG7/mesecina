/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Color_map_factory.h 255 2007-12-03 23:01:56Z miklosb $
 */

#ifndef COLOR_MAP_FACTORY_H
#define COLOR_MAP_FACTORY_H

#include <vector>
#include <QtCore/QString>
#include <QtCore/QDir>

#include "Color_map.h"

class API Color_map_factory {
public:
	static std::vector<Color_map*> color_maps;

	static void add_color_map(Color_map* c);
//	static void load_color_maps();

	static Color_map* get_color_map(const QString & name);

};


#endif //COLOR_MAP_FACTORY_H