#ifndef MESECINA_POWER_CRUST_3_IMPLEMENT_H
#define MESECINA_POWER_CRUST_3_IMPLEMENT_H

#include "Power_crust_3.h"
#include "layers/Delaunay_layer_3.h"
#include "layers/Gabriel_layer_3.h"
#include "layers/Poles_layer_3.h"
#include "layers/Polar_triangulation_layer_3.h"
#include "layers/Classified_poles_layer_3.h"
#include "layers/Convex_hull_layer_3.h"

template <class K>
Power_crust_3<K>::Power_crust_3() : has_polar_triangulation(false) {
	// delaunay
	GL_draw_layer_3* points_layer = new Delaunay_layer_3<Power_crust_3<K> >("Points",this, "Points", true, false, false, false);
	add_layer(points_layer);
	GL_draw_layer_3* delaunay_edges_layer = new Delaunay_layer_3<Power_crust_3<K> >("Delaunay edges",this, "Edges of the Delaunay triangulation", false, true, false, false);
	add_layer(delaunay_edges_layer);
	GL_draw_layer_3* delaunay_faces_layer = new Delaunay_layer_3<Power_crust_3<K> >("Delaunay faces",this, "Faces of the Delaunay triangulation", false, false, true, false);
	add_layer(delaunay_faces_layer);

	// voronoi
	GL_draw_layer_3* voronoi_vertex_layer = new Delaunay_layer_3<Power_crust_3<K> >("Voronoi vertex",this, "Voronoi vertices", true, false, false, true);
	add_layer(voronoi_vertex_layer);
	GL_draw_layer_3* voronoi_edge_layer = new Delaunay_layer_3<Power_crust_3<K> >("Voronoi edges",this, "Voronoi edges of the input points", false, true, false, true);
	add_layer(voronoi_edge_layer);

	// gabriel
	GL_draw_layer_3* gabriel_edges_layer = new Gabriel_layer_3<Power_crust_3<K> >("Gabriel edges",this, "Gabriel edges", false);
	add_layer(gabriel_edges_layer);
	GL_draw_layer_3* gabriel_faces_layer = new Gabriel_layer_3<Power_crust_3<K> >("Gabriel faces",this, "Gabriel faces", true);
	add_layer(gabriel_faces_layer);
	GL_draw_layer_3* gabriel_facese_layer = new Gabriel_layer_3<Power_crust_3<K> >("Gabriel faces boundary",this, "Segments bounding the Gabriel faces", true, true);
	add_layer(gabriel_facese_layer);

	// poles
	GL_draw_layer_3* plus_poles_layer = new Poles_layer_3<Power_crust_3<K> >("Plus poles",this, "Furthest Voronoi vertices (if finite voronoi cell) for the input points", V_PLUS, false);
	add_layer(plus_poles_layer);
	GL_draw_layer_3* minus_poles_layer = new Poles_layer_3<Power_crust_3<K> >("Minus poles",this, "Furthest Voronoi vertices to the opposite side of plus poles", V_MINUS, false);
	add_layer(minus_poles_layer);
	GL_draw_layer_3* plus_poleso_layer = new Poles_layer_3<Power_crust_3<K> >("Plus poles origin",this, "Furthest Voronoi vertices (if finite voronoi cell) for the input points", V_PLUS, true);
	add_layer(plus_poleso_layer);
	GL_draw_layer_3* minus_poleso_layer = new Poles_layer_3<Power_crust_3<K> >("Minus poles origin",this, "Furthest Voronoi vertices to the opposite side of plus poles", V_MINUS, true);
	add_layer(minus_poleso_layer);

	// polar triangulation
	GL_draw_layer_3* polar_tri_layer = new Polar_triangulation_layer_3<Power_crust_3<K> >("Polar triangulation",this, "Regular triangulation of poles");
	add_layer(polar_tri_layer);

	// classified poles
	GL_draw_layer_3* interior_poles_layer = new Classified_poles_layer_3<Power_crust_3<K> >("Interior poles",this, "Interior poles", P_INTERIOR, false);
	add_layer(interior_poles_layer);
	GL_draw_layer_3* exter_poles_layer = new Classified_poles_layer_3<Power_crust_3<K> >("Exterior poles",this, "Exterior poles", P_EXTERIOR, false);
	add_layer(exter_poles_layer);
	GL_draw_layer_3* unc_poles_layer = new Classified_poles_layer_3<Power_crust_3<K> >("Unclassified poles",this, "Unclassified poles - this should be empty", P_UNCLASSIFIED, false);
	add_layer(unc_poles_layer);

	// interior polar balls and poles pairs
	GL_draw_layer_3* interior_balls_layer = new Classified_poles_layer_3<Power_crust_3<K> >("Interior balls",this, "Interior balls", P_INTERIOR, true);
	add_layer(interior_balls_layer);
	GL_draw_layer_3* ppairs_layer = new Classified_poles_layer_3<Power_crust_3<K> >("Polar pairs",this, "Polar pairs", P_INTERIOR, false, true);
	add_layer(ppairs_layer);

	// convex hull layers
	GL_draw_layer_3* ch_v_layer = new Convex_hull_layer_3<Power_crust_3<K> >("Convex hull vertices",this, "Vertices on the convex hull of input points", true);
	add_layer(ch_v_layer);
	GL_draw_layer_3* ch_layer = new Convex_hull_layer_3<Power_crust_3<K> >("Convex hull",this, "Convex hull of input points", false);
	add_layer(ch_layer);

 
}

template <class K>
Power_crust_3<K>::~Power_crust_3() {
}

template <class K>
Geometry* Power_crust_3<K>::clone() { return 0; }
	
template <class K>
void Power_crust_3<K>::add_points(std::list<Point3D>* pts) {
	input_points.clear();
	invalidate_cache();

	input_points.insert(input_points.begin(), pts->begin(), pts->end());

	std::vector<Point_3> point_vector;
	std::list<Point3D>::iterator p_it, p_end = pts->end(); int i=0;
	for (p_it = pts->begin(); p_it != p_end; p_it++, i++) {
		point_vector.push_back(Point_3(p_it->x, p_it->y, p_it->z));
	}
	std::cout << PROGRESS_STATUS << "Computing Delaunay of points" << std::endl;
	input_triangulation.insert(point_vector.begin(),point_vector.end());
	std::cout << PROGRESS_DONE << std::endl;

	std::cout << "Power crust got points " << input_points.size() << std::endl;
}

template <class K>
std::list<Point3D>* Power_crust_3<K>::get_points() {
	std::cout << "Power crust gives points " << input_points.size() << std::endl;
	return &input_points;
}

template <class K>
std::list<std::string> Power_crust_3<K>::offer_structures() {
	std::list<std::string> names;
	names.push_back(SHARED_INNER_POLES);
	return names;
}

template <class K>
void* Power_crust_3<K>::give_structure(const std::string& name) {
	if (name == SHARED_INNER_POLES) {
		get_classified_polar_triangulation();
		return polar_triangulation.get_inner_balls();
	}
	return 0;
}

template <class K>
typename Power_crust_3<K>::Triangulation_3* Power_crust_3<K>::get_input_triangulation() {
	return &input_triangulation;
}

template <class K>
typename Power_crust_3<K>::Triangulation_3* Power_crust_3<K>::get_pole_classified_triangulation() {
	if (input_triangulation.is_dirty()) 
		input_triangulation.compute_poles();
	return &input_triangulation;
}

template <class K>
typename Power_crust_3<K>::Polar_triangulation_3* Power_crust_3<K>::get_polar_triangulation() {
	if (!has_polar_triangulation) {
		polar_triangulation.compute_polar_triangulation(get_pole_classified_triangulation());
		add_variable("Vertices in polar triangulation", polar_triangulation.number_of_vertices());
		has_polar_triangulation = true;
	}
	return &polar_triangulation;
}

template <class K>
typename Power_crust_3<K>::Polar_triangulation_3* Power_crust_3<K>::get_classified_polar_triangulation() {
	get_polar_triangulation();
	if (polar_triangulation.is_dirty()) {
		label_poles_improved_with_heuristic();
		polar_triangulation.set_dirty(false);
	}
	return &polar_triangulation;
}


template <class K>
typename Power_crust_3<K>::Polar_vertex_vector_iterator Power_crust_3<K>::top_polar_vertex(Polar_vertex_vector* vec) {
	Polar_vertex_vector::iterator it ,top = vec->begin(), end = vec->end();
	for (it = vec->begin(); it != end; ++it) {
		if ((*top)->get_priority() < (*it)->get_priority()) top = it;
	}
	return top;
}

template <class K>
struct Compare_polar_vertex_handle
{
	bool operator()(typename Power_crust_3<K>::Polar_vertex_handle& v_one, typename Power_crust_3<K>::Polar_vertex_handle& v_two)
	{
		return v_one->get_priority() > v_two->get_priority();
	}
};


template <class K>
void Power_crust_3<K>::label_poles_improved_with_heuristic() {
	if (polar_triangulation.number_of_vertices() < 2) return;
	typedef std::back_insert_iterator<Polar_vertex_vector> Polar_insert_iterator;

	std::cout << PROGRESS_STATUS << "Classifying poles with heuristic" << std::endl;
	std::cout << PROGRESS_MINIMUM << "0" << std::endl;
	std::cout << PROGRESS_MAXIMUM << polar_triangulation.number_of_vertices() << std::endl; int i=0;

	//set infinite vertex as outside
	//Polar_vertex_handle inf = polar_triangulation.infinite_vertex();
	//inf->set_out(1); inf->update_priority();

	// init priority queue with all poles
	Polar_vertex_vector priority_queue;
	polar_triangulation.inner_balls.clear();
	Polar_triangulation_3::Finite_vertices_iterator v_it, v_end = polar_triangulation.finite_vertices_end();
	for (v_it = polar_triangulation.finite_vertices_begin (); v_it!=v_end; ++v_it) {
		priority_queue.push_back(v_it);
	}

	// init the minus poles of convex hull vertices with the in values
	typedef std::vector<Vertex_handle> Vertex_vector;
	typedef std::back_insert_iterator<Vertex_vector> Insert_iterator;
	Vertex_vector convex_hull_vertices;
	Insert_iterator ii(convex_hull_vertices);
	input_triangulation.incident_vertices( input_triangulation.infinite_vertex(), ii);
	Vertex_vector::iterator chv_it, chv_end = convex_hull_vertices.end();
	for (chv_it = convex_hull_vertices.begin(); chv_it != chv_end; ++chv_it) {
		Vertex_handle chv = *chv_it;
		if (chv->get_pole_minus() != 0) {
			Polar_vertex_handle inner_candidate_pole = chv->get_pole_minus()->get_polar_vertex();
			if (inner_candidate_pole!=0) {
				inner_candidate_pole->set_in(chv->get_poles_confidence());
				inner_candidate_pole->update_priority();
			} //else std::cout << LOG_WARNING << "in heuristic classification inner_candidate_pole is 0..." << std::endl;
		} else std::cout << LOG_WARNING << "in heuristic classification pole_minus is 0..." << std::endl;
		//			std::cout << "in confidence set to: " << chv->get_poles_confidence() << std::endl;
	}

	// make sure highest priority element is on top
	// std::sort(priority_queue.begin(), priority_queue.end(), Compare_polar_vertex_handle());

	while (!priority_queue.empty()) {
		//get top pole
		Polar_vertex_vector::iterator top_it = top_polar_vertex(&priority_queue);
		Polar_vertex_handle top = *top_it;
		priority_queue.erase(top_it);
		//			std::cout << "Top pole priority: "<< top->get_priority() << " in: " << top->get_in() << " out: " << top->get_out() << std::endl;
		//set its label
		Pole_classification label = P_UNCLASSIFIED; float tmp_confidence;
		if (top->get_in() > top->get_out()) {
			//label interior
			label = P_INTERIOR;
			tmp_confidence = top->get_in();
			top->set_classification(P_INTERIOR);
			polar_triangulation.inner_balls.push_back(top->point());
		} else {
			//label exterior
			tmp_confidence = top->get_out();
			top->set_classification(P_EXTERIOR);
		}
		i++; std::cout << PROGRESS_VALUE << i << std::endl;
		// get all samples to which this is a pole
		Classified_cell* voronoi_vertex = polar_triangulation.get_voronoi_vertex_for_polar(top);
		std::list<Vertex_handle>* samples_for_poles = voronoi_vertex->get_sample_points();
		std::list<Vertex_handle>::iterator s_it, s_end = samples_for_poles->end();
		for (s_it = samples_for_poles->begin(); s_it != s_end; ++s_it) {
			Vertex_handle s = *s_it;
			if (s->get_pole_plus()->get_polar_vertex() != polar_triangulation.infinite_vertex()) {
				// propagate information to opposite pole
				Classified_cell* opp_voronoi = &(*(s->get_pole_plus()));
				if (opp_voronoi == voronoi_vertex) {
					opp_voronoi = &(*(s->get_pole_minus()));
				} else {
					if (&(*(s->get_pole_minus())) != voronoi_vertex )
						std::cout << LOG_ERROR << "Pole pairs information inconsistent at labeling stage!" << std::endl;
				}
				Polar_vertex_handle opp_pole = opp_voronoi->get_polar_vertex();
				if (opp_pole!=0) {
					if (label == P_INTERIOR) {
						opp_pole->set_out( 
							(tmp_confidence*s->get_poles_confidence() > opp_pole->get_out() )? 
							tmp_confidence*s->get_poles_confidence() : 
						opp_pole->get_out()
							);
					} else {
						opp_pole->set_in( 
							(tmp_confidence*s->get_poles_confidence() > opp_pole->get_in() )? 
							tmp_confidence*s->get_poles_confidence() : 
						opp_pole->get_in()
							);
					}
					// update opposite pole's priority
					opp_pole->update_priority();
				} else std::cout << LOG_WARNING << "in heuristic classification opp_pole is 0..." << std::endl;
			}
		}
		// go through all adjacent poles and propagate information to deep intersections
		if (!polar_triangulation.is_infinite(top)) {
			Polar_vertex_vector adjacent_vertices;
			Polar_insert_iterator ii(adjacent_vertices);
			polar_triangulation.incident_vertices( top, ii);
			for ( Polar_vertex_vector::iterator vvv_it = adjacent_vertices.begin(); vvv_it != adjacent_vertices.end(); vvv_it++) {
				Polar_vertex_handle vv_it = *vvv_it;
				// compute intersection depth
				if (!polar_triangulation.is_infinite(vv_it) && !vv_it->is_classified()) {
					double d_sq = CGAL::to_double(squared_distance(top->point().point(), vv_it->point().point()));
					double r1_sq = CGAL::to_double(top->point().weight());
					double r2_sq = CGAL::to_double(vv_it->point().weight());
					float minus_cos_intersection = (r1_sq + r2_sq - d_sq) / (2 * sqrt(r1_sq * r2_sq));
					if (minus_cos_intersection > 0.2)
					{
						//the balls intersect deeply
						if (label == P_INTERIOR) {
							vv_it->set_in( 
								(tmp_confidence*minus_cos_intersection > vv_it->get_in() )? 
								tmp_confidence*minus_cos_intersection : 
							vv_it->get_in()
								);
						} else {
							vv_it->set_out( 
								(tmp_confidence*minus_cos_intersection > vv_it->get_out() )? 
								tmp_confidence*minus_cos_intersection : 
							vv_it->get_out()
								);
						}
						vv_it->update_priority();
					} // end if deep intersection
				} // end if infinite
			} // end for adjacent vertices
		} // end if infinite vertex test
		//std::sort(priority_queue.begin(), priority_queue.end(), Compare_polar_vertex_handle());
	} // end while priority queue
	std::cout << PROGRESS_DONE << std::endl;
} // end function

template <class K>
void Power_crust_3<K>::invalidate_cache() {
	invalidate_all_layers();
	has_polar_triangulation = false;
	input_triangulation.set_dirty();
	polar_triangulation.set_dirty();
	emit announce_structure_changed(SHARED_INNER_POLES);
}

#endif //MESECINA_POWER_CRUST_3_IMPLEMENT_H