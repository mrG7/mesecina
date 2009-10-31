/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info at http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Main_window_geometry.cpp 737 2009-05-16 15:40:46Z miklosb $
 */

#include <constants.h>
#include <geometries/Geometry.h>
#include <gui/app/Geometry_factory.h>
#include <gui/app/Main_window.h>


#include <gui/app/Evolution.h>

void Main_window::create_new_geometries() {
	std::list<QAction*>::iterator ga_it, ga_end = geometry_actions.end();
	for (ga_it = geometry_actions.begin(); ga_it!=ga_end; ga_it++) {
		if ((*ga_it)->isChecked()) {
			geometries.push_back(Geometry_factory::create_new_geometry((*ga_it)->text().toStdString()));
		}
	}

	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it!=g_end; g_it++) {
		Geometry* g= *g_it;

		// connect signals between geometry and main window
		connect(g, SIGNAL(announce_structure_changed(const std::string&)), this, SLOT(shared_structure_changed(const std::string&)));
		connect(g, SIGNAL(request_structure(const std::string&)), this, SLOT(get_shared_structure(const std::string&)));
		connect(g, SIGNAL(do_widget_repaint()), this, SLOT(do_widget_repaint()));
		connect(g, SIGNAL(do_statistics_repaint()), this, SLOT(update_stats()));

		// add evolutions to toolbar
		for (unsigned i=0; i<g->evolutions_size(); ++i)
			evolutions_toolbar->put_evolution(g->evolutions(i), &geometries);
		QSettings settings;
		QString last_evolution_index = settings.value("last-evolutions-name").toString();
		int index = evolutions_toolbar->evolutions_combo_box->findText(last_evolution_index);
		if (index!=-1) evolutions_toolbar->evolutions_combo_box->setCurrentIndex(index);
		else evolutions_toolbar->evolutions_combo_box->setCurrentIndex(0);

	}
}

void* Main_window::get_shared_structure(const std::string& name) {
	std::list<std::string>::iterator n_it, n_end = offered_structures.end();
	std::list<Geometry*>::iterator g_it, g_end = offering_geometries.end();
	for (g_it = offering_geometries.begin(), n_it = offered_structures.begin(); g_it!=g_end; g_it++, n_it++) {
		if (*n_it == name) 
			return (*g_it)->give_structure(name);
	}
	return 0;
}

void Main_window::shared_structure_changed(const std::string& name) {
//	std::cout << LOG_BLUE << "Shared structure changed: " << name << std::endl;
	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it!=g_end; g_it++) {
		(*g_it)->receive_structure_changed(name);
	}
}


void Main_window::do_widget_repaint() {
	widget->repaintGL();
}

void Main_window::set_geometries(std::list<Geometry*> *new_geometries) {
	if (Application_settings::get_bool_setting("load-off-only-replace-mesh")) return;
	// clean up old geometries
	Bundle temp_bundle("nothing");
	bool need_apply_bundle = layer_list_model->rowCount() > 0;
	if (need_apply_bundle)
		temp_bundle = layer_list_model->create_bundle_from_current("temp_bundle");

	//std::cout << "set_geometries" << std::endl;

	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it!=g_end; g_it++) {
		Geometry *g = *g_it;
		for (size_t i=0; i < g->layers_size(); i++)
			widget->detach(g->layers(i));
		g->disconnect();
		if (new_geometries==0) {
			if (!evolutions_toolbar->has_evolutions()) { // otherwise evolutions will take care of freeing up the memory
//				std::cout << "geometry deleted by set_geometries" << std::endl;
				delete g;
			}
		}
	}


	geometries.clear();
	offered_structures.clear();
	offering_geometries.clear();
	layer_list_model->clear();
#ifndef MESECINA_3D
	layer_list_model->add_layer(image_layer);
#else
	layer_list_model->add_layer(cutting_plane_layer);
#endif
	
	// add new ones
	if (new_geometries == 0) {
		create_new_geometries();
		
	} else {
		geometries.insert(geometries.begin(), new_geometries->begin(), new_geometries->end());
		std::list<Geometry*>::iterator g_it, g_end = geometries.end();
		// connect the current set of geometries to the slots
		for (g_it = geometries.begin(); g_it!=g_end; g_it++) {
			Geometry *g = *g_it;
			connect(g, SIGNAL(announce_structure_changed(const std::string&)), this, SLOT(shared_structure_changed(const std::string&)));
			connect(g, SIGNAL(request_structure(const std::string&)), this, SLOT(get_shared_structure(const std::string&)));
			connect(g, SIGNAL(do_widget_repaint()), this, SLOT(do_widget_repaint()));
			connect(g, SIGNAL(do_statistics_repaint()), this, SLOT(update_stats()));
		}

//		reset_evolutions_to_current_geometries();
	}
	
	//std::cout << "geometries added" << std::endl;


	//add their layers
	for (g_it = geometries.begin(); g_it != g_end; g_it++) {
		Geometry *g = *g_it;
		for (size_t i=0; i<g->layers_size(); i++) {
			widget->attach(g->layers(i));
			layer_list_model->add_layer(g->layers(i));
		}
		std::list<std::string> offered_names = g->offer_structures();
		std::list<std::string>::iterator on_it, on_end = offered_names.end();
		for (on_it=offered_names.begin(); on_it != on_end; on_it++) {
			offered_structures.push_back(*on_it);
			offering_geometries.push_back(g);
		}
	}
	//std::cout << "layers added" << std::endl;


	if (need_apply_bundle) {
		// image layer removed before, add it back
		//widget->attach(image_layer);
#ifdef MESECINA_3D
		layer_list_model->add_layer(cutting_plane_layer);
#else
		layer_list_model->add_layer(image_layer);
#endif
		// layer states have to be reloaded
		//std::cout << "before bundle" << std::endl;
		layer_list_model->import_from_bundle(temp_bundle);
		//std::cout << "after bundle" << std::endl;

		//invalidate layers cache
		//std::vector<Visualization_layer*>::iterator l_it, l_end = widget->layers.end();
		//for (l_it = widget->layers.begin(); l_it!=l_end; l_it++) {
		//	(*l_it)->invalidate_cache();
		//}

#ifdef MESECINA_3D
#else
		selection_layer->clear_all();
#endif
	}

	//std::cout << "apply bundles" << std::endl;

	// show new stats
	update_stats();
	//std::cout << "set_geometries done" << std::endl;

}

std::list<Geometry*>* Main_window::clone_geometries() {
	std::list<Geometry*>* new_gl = new std::list<Geometry*>();
	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it != g_end; g_it++) {
		Geometry* g = *g_it;
		Geometry* clone_geometry = g->clone();
		new_gl->push_back(clone_geometry);
	}
	return new_gl;
}

std::list<Geometry*>* Main_window::clone_and_bind_geometries() {
	std::list<Geometry*>* new_gl = new std::list<Geometry*>();
	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it != g_end; g_it++) {
		Geometry* g = *g_it;
		//for (size_t i=0; i < g->layers_size(); i++)
		//	widget->detach(g->layers(i));
		Geometry* clone_geometry = g->clone();
		if (clone_geometry) new_gl->push_back(clone_geometry);
	}

	//geometries.clear();
	//offered_structures.clear();
	//offering_geometries.clear();

	//geometries.insert(geometries.begin(), new_gl->begin(), new_gl->end());

	////add their offered structures
	//for (g_it = geometries.begin(); g_it != g_end; g_it++) {
	//	Geometry *g = *g_it;
	//	std::list<std::string> offered_names = g->offer_structures();
	//	std::list<std::string>::iterator on_it, on_end = offered_names.end();
	//	for (on_it=offered_names.begin(); on_it != on_end; on_it++) {
	//		offered_structures.push_back(*on_it);
	//		offering_geometries.push_back(g);
	//	}
	//}
	return new_gl;
}

void Main_window::reset_evolutions_to_current_geometries() {
	evolutions_toolbar->init_evolutions(&geometries);

	//int last_evolution_selected = evolutions_toolbar->evolutions_combo_box->currentIndex();
	//evolutions_toolbar->clear_evolutions();
	//if (!keep_position) 
	//	evolutions_toolbar->slider->setValue(0);

	//evolutions_toolbar->connect_evolution_change(false);
	//std::list<Geometry*>::iterator g_it, g_end = geometries.end(); int g_index = 0;
	//for (g_it = geometries.begin(); g_it!=g_end; g_it++, g_index++) {
	//	Geometry *g = *g_it;
	//	//for (size_t i=0; i < g->evolutions_size(); i++) {
	//	//	std::list<Geometry*>* new_gl = new std::list<Geometry*>(geometries.begin(), geometries.end());
	//	//	Evolution* e = g->evolutions(i);
	//	//	e->init_geometries(new_gl);

	//	//	evolutions_toolbar->add_evolution(e);
	//	//}
	//}
	//evolutions_toolbar->connect_evolution_change(true);
	//evolutions_toolbar->move_evolutions_to_zero();
	//if (last_evolution_selected != -1) 
	//	evolutions_toolbar->evolutions_combo_box->setCurrentIndex(last_evolution_selected);
	//else
	//	evolutions_toolbar->evolutions_combo_box->setCurrentIndex(0);
}