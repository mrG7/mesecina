/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Color_map_factory.cpp 586 2009-04-03 20:01:21Z miklosb $
 */


#include <gui/app/static/Color_map_factory.h>
#include <gui/app/static/Application_settings.h>

std::vector<Color_map*> /* __declspec( dllexport )*/ Color_map_factory::color_maps;

//void Color_map_factory::load_color_maps() {
//	QDir dir(":/colormap/", "*.map", QDir::Name, QDir::Files | QDir::Hidden | QDir::NoSymLinks);
//	Application_settings::add_int_setting("random-colormap-resolution",1000);
//	color_maps.clear();
//	color_maps.push_back(new Color_map("", "Solid"));
//	color_maps.push_back(new Random_color_map(Application_settings::get_int_setting("random-colormap-resolution"), "Random"));
//
//
//	QFileInfoList list = dir.entryInfoList();
//	for (int i = 0; i < list.size(); ++i) {
//		QFileInfo fileInfo = list.at(i);
//		Color_map* cm = new Color_map(fileInfo.absoluteFilePath(), fileInfo.baseName());
//		color_maps.push_back(cm);
//	}
//}

void Color_map_factory::add_color_map(Color_map* c) {
	for (unsigned i=0; i<color_maps.size(); i++) {
		if (color_maps[i]->get_name() == c->get_name()) {
			delete color_maps[i];
			color_maps[i] = c;
			return;
		}
	}
	color_maps.push_back(c);
}
	
Color_map* Color_map_factory::get_color_map(const QString & name) {
	for (unsigned i=0; i<color_maps.size(); i++) {
		if (color_maps[i]->get_name() == name) return color_maps[i];
	}
	return 0;
}

