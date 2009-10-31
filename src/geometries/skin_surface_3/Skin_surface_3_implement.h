#include <gui/app/static/Application_settings.h>
#include <constants.h>

#include "layers/Balls_union_boundary_layer_3.h"

template <class K> 
Skin_surface_3<K>::Skin_surface_3(std::string b_source) : Geometry(), has_skin_surface(false) {
	ball_source = b_source;

	QString mesh_name = "Boundary mesh";
	QString wireframe_name = "Boundary wireframe";
	if (b_source==SHARED_SCALE_DOUBLE_BALLS) {
		mesh_name = "Scale boundary mesh";
		wireframe_name = "Scale boundary wireframe";
	}

	GL_draw_layer_3* mesh_layer = new Balls_union_boundary_layer_3<Skin_surface_3<K> >(mesh_name,this, "Mesh of the boundary of the union of balls",false);
	add_layer(mesh_layer);
	GL_draw_layer_3* wireframe_layer = new Balls_union_boundary_layer_3<Skin_surface_3<K> >(wireframe_name,this, "Wireframe of the boundary of the union of balls", true);
	add_layer(wireframe_layer);

	Application_settings::add_int_setting("skin-surface-subdivision-steps", 0);
	Application_settings::add_double_setting("skin-surface-simplification-percentage", 100);
	
}

template <class K> 
Skin_surface_3<K>::~Skin_surface_3() {}

template <class K> 
Geometry* Skin_surface_3<K>::clone() {
	Skin_surface_3<K>* new_skin_surface = new Skin_surface_3<K>(ball_source);
	return new_skin_surface;
}

template <class K> 
void Skin_surface_3<K>::receive_structure_changed(const std::string& name) {
	if (name == ball_source)
		invalidate_cache();
}

template <class K> 
typename Skin_surface_3<K>::Polyhedron* Skin_surface_3<K>::get_skin_surface() {
	if (!has_skin_surface) {

		void* p = request_structure(ball_source);
		if (!p) std::cout << LOG_ERROR << "Error getting the input balls for the Union_of_balls_3!" << std::endl;
		else {
			std::list< Point4D >* pts = static_cast<std::list< Point4D >*>(p);

			std::list<Point4D>::iterator p_it, p_end = pts->end();
			std::list<Skin_weighted_point> sp;
//			std::cout << "get_skin_surface point size: " << points.size() << std::endl;
			for (p_it = pts->begin(); p_it!=p_end; p_it++) {
//				std::cout << LOG_GREEN << "push sp" << std::endl;
				sp.push_back(Skin_weighted_point(Skin_bare_point(p_it->x, p_it->y, p_it->z),p_it->w));
			}



			if (sp.size() >0) {
				skin_surface.clear();
				std::cout << PROGRESS_STATUS << "Building skin surface data structure..." << std::endl;
				Skin_union_of_balls_3 ss(sp.begin(), sp.end()/*, 0.99*/);
				std::cout << PROGRESS_DONE << std::endl;
				std::cout << PROGRESS_STATUS << "Building skin surface mesh..." << std::endl;
				ss.mesh_skin_surface_3(skin_surface);
				std::cout << PROGRESS_DONE << std::endl;

				std::cout << PROGRESS_STATUS << "Subdividing skin surface mesh..." << std::endl;
				for (int steps=0; steps < Application_settings::get_int_setting("skin-surface-subdivision-steps"); ++steps) {
					ss.subdivide_skin_surface_mesh_3(skin_surface);
				}
				std::cout << PROGRESS_DONE << std::endl;


				std::cout << PROGRESS_STATUS << "Simplifying skin surface mesh..." << std::endl;
				double percentage = Application_settings::get_double_setting("skin-surface-simplification-percentage")/100.0;
				if (percentage > 0 && percentage < 1) {
					// This is a stop predicate (defines when the algorithm terminates).
					// In this example, the simplification stops when the number of undirected edges
					// left in the surface drops below the specified number (1000)
					CGAL::Surface_mesh_simplification::Count_stop_predicate<Polyhedron> stop((int)(percentage*skin_surface.size_of_vertices ()));
					// This the actual call to the simplification algorithm.
					// The surface and stop conditions are mandatory arguments.
					// The index maps are needed because the vertices and edges
					// of this surface lack an "id()" field.
					/*int r = */CGAL::Surface_mesh_simplification::edge_collapse
						(skin_surface
						,stop
						,CGAL::vertex_index_map(boost::get(CGAL::vertex_external_index,skin_surface)) 
						.edge_index_map  (boost::get(CGAL::edge_external_index  ,skin_surface)) 
						);
				}                 
				std::cout << PROGRESS_DONE << std::endl;
			}
			has_skin_surface = true;
		}

	}
	return &skin_surface;
}

template <class K> 
void Skin_surface_3<K>::application_settings_changed(const QString& name) {
	if (name == "skin-surface-simplification-percentage" || name=="skin-surface-subdivision-steps") {
		invalidate_cache();
	}
}

template <class K> 
void Skin_surface_3<K>::invalidate_cache() {
	invalidate_all_layers();
	has_skin_surface = false;
	emit do_widget_repaint();
}