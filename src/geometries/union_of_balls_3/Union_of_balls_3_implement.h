#include <constants.h>
#include "layers/Alpha_triangulation_layer_3.h"
#include "layers/V_triangulation_layer_3.h"
#include "layers/Classified_v_triangulation_layer_3.h"
#include "layers/V_cell_classification_layer_3.h"
#include "layers/V_classified_triangulation_layer_3.h"
#include "layers/V_flat_vertices_layer_3.h"
#include "layers/Implicit_mesher_layer_3.h"
#include "layers/Poisson_surface_layer_3.h"
#include "layers/V_topology_filtration_debug_layer_3.h"

//#include "Medial_axis_structure_3.h"




//#define ALPHA_VALUE 0.01
#define ALPHA_VALUE 0

template <class K> 
Union_of_balls_3<K>::Union_of_balls_3() : Geometry(), has_alpha_shape(false), alpha_shape(ALPHA_VALUE, Alpha_shape_3::GENERAL), has_topology_angle_ma(false), has_v_triangulation(false), is_v_edges_classified(false),is_v_cell_flooded(false), growth_ratio(1), has_local_filtered_medial_axis(false), has_scale_filtered_medial_axis(false), topology_filtration_steps(-1) {

	// alpha triangulation
	GL_draw_layer_3* ball_centers_layer = new Alpha_triangulation_layer_3<Union_of_balls_3<K> >("Alpha centers",this, "Centers of the balls in the alpha shape", true, false, false, false, false, false);
	add_layer(ball_centers_layer);
	GL_draw_layer_3* ball_layer = new Alpha_triangulation_layer_3<Union_of_balls_3<K> >("Alpha balls",this, "Balls in alpha shapes", true, false, false, true, true, false);
	add_layer(ball_layer);
	GL_draw_layer_3* sphere_layer = new Alpha_triangulation_layer_3<Union_of_balls_3<K> >("Alpha sphere",this, "Spheres in alpha shapes", true, false, false, true, false, false);
	add_layer(sphere_layer);

	// dual simplices
	GL_draw_layer_3* dual_regular_faces_layer = new Alpha_triangulation_layer_3<Union_of_balls_3<K> >("Dual regular faces",this, "Regular faces of the dual of the balls", false, false, true, false, true, true, true, Alpha_shape_3::REGULAR);
	add_layer(dual_regular_faces_layer);
	GL_draw_layer_3* dual_regular_facese_layer = new Alpha_triangulation_layer_3<Union_of_balls_3<K> >("Dual regular faces edges",this, "Regular faces of the dual of the balls rendered only as unfilled triangles", false, false, true, false, false, true, true, Alpha_shape_3::REGULAR);
	add_layer(dual_regular_facese_layer);
	GL_draw_layer_3* dual_singular_faces_layer = new Alpha_triangulation_layer_3<Union_of_balls_3<K> >("Dual singular faces",this, "Singular faces of the dual of the balls", false, false, true, false, true, true, true, Alpha_shape_3::SINGULAR);
	add_layer(dual_singular_faces_layer);
	GL_draw_layer_3* dual_singular_facese_layer = new Alpha_triangulation_layer_3<Union_of_balls_3<K> >("Dual singular faces edges",this, "Singular faces of the dual of the balls rendered only as unfilled triangles", false, false, true, false, false, true, true, Alpha_shape_3::SINGULAR);
	add_layer(dual_singular_facese_layer);
	GL_draw_layer_3* dual_singular_edges_layer = new Alpha_triangulation_layer_3<Union_of_balls_3<K> >("Dual singular edges",this, "Singular edges of the dual of the balls", false, true, false, false, false, true, true, Alpha_shape_3::SINGULAR);
	add_layer(dual_singular_edges_layer);

	// v triangulation
	GL_draw_layer_3* v_points_layer = new V_triangulation_layer_3<Union_of_balls_3<K> >("V points",this, "Points in V triangulation", true, false);
	add_layer(v_points_layer);
	GL_draw_layer_3* v_origin_layer = new V_triangulation_layer_3<Union_of_balls_3<K> >("V origin",this, "Origin segments of points in V triangulation (connecting point with the corresponding dual face)", true, true);
	add_layer(v_origin_layer);
	GL_draw_layer_3* v_origint_layer = new V_triangulation_layer_3<Union_of_balls_3<K> >("V origin triangles",this, "Origin triangles of points in V triangulation (connecting point with the corresponding dual face)", true, true, false);
	add_layer(v_origint_layer);
	GL_draw_layer_3* v_tr_layer = new V_triangulation_layer_3<Union_of_balls_3<K> >("V triangulation",this, "V triangulation", false, false);
	add_layer(v_tr_layer);

	// v voronoi
	GL_draw_layer_3* v_vv_layer = new V_triangulation_layer_3<Union_of_balls_3<K> >("V Voronoi vertices",this, "V voronoi vertices", true, false, false, true);
	add_layer(v_vv_layer);
	GL_draw_layer_3* v_vvf_layer = new V_triangulation_layer_3<Union_of_balls_3<K> >("V Voronoi cells",this, "V voronoi cells", false, false, false, true);
	add_layer(v_vvf_layer);
	GL_draw_layer_3* v_vve_layer = new V_triangulation_layer_3<Union_of_balls_3<K> >("V Voronoi edges",this, "V Voronoi edges", false, false, true, true);
	add_layer(v_vve_layer);
	// v classified
	//GL_draw_layer_3* v_cv_layer = new Classified_v_triangulation_layer_3<Union_of_balls_3<K> >("V classified vertices",this, "V classified vertices", true);
	//add_layer(v_cv_layer);
	GL_draw_layer_3* v_ce_layer = new Classified_v_triangulation_layer_3<Union_of_balls_3<K> >("V crossing edges",this, "V crossing edges", false, C_CROSSING);
	add_layer(v_ce_layer);
	GL_draw_layer_3* v_ae_layer = new Classified_v_triangulation_layer_3<Union_of_balls_3<K> >("V alpha edges",this, "V alpha edges", false, C_ALPHA);
	add_layer(v_ae_layer);
	
	// v cell classification test
	GL_draw_layer_3* v_cce_layer = new V_cell_classification_layer_3<Union_of_balls_3<K> >("V cell exterior vertices",this, "V cell exterior vertices", M_EXTERIOR);
	add_layer(v_cce_layer);
	GL_draw_layer_3* v_cci_layer = new V_cell_classification_layer_3<Union_of_balls_3<K> >("V cell interior vertices",this, "V cell interior vertices", M_INTERIOR);
	add_layer(v_cci_layer);
	GL_draw_layer_3* v_ccdv_layer = new V_cell_classification_layer_3<Union_of_balls_3<K> >("V cell interior dual tet",this, "V cell interior dual tet", M_INTERIOR, true);
	add_layer(v_ccdv_layer);
	GL_draw_layer_3* v_mf_layer = new V_cell_classification_layer_3<Union_of_balls_3<K> >("V medial face",this, "V cell interior dual tet", M_INTERIOR, false, false, true);
	add_layer(v_mf_layer);


	GL_draw_layer_3* v_vorvi_layer = new V_classified_triangulation_layer_3<Union_of_balls_3<K> >("V interior vertices",this, "V interior vertices", true, M_INTERIOR);
	add_layer(v_vorvi_layer);
	GL_draw_layer_3* v_vorve_layer = new V_classified_triangulation_layer_3<Union_of_balls_3<K> >("V exterior vertices",this, "V exterior vertices", true, M_EXTERIOR);
	add_layer(v_vorve_layer);
	GL_draw_layer_3* v_vorvu_layer = new V_classified_triangulation_layer_3<Union_of_balls_3<K> >("V unclassified vertices",this, "V unclassified vertices", true, M_UNCLASSIFIED);
	add_layer(v_vorvu_layer);

	// v flat vertices/tets
	GL_draw_layer_3* v_fv_layer = new V_flat_vertices_layer_3<Union_of_balls_3<K> >("V flat vertices",this,"V flat vertices", true);
	add_layer(v_fv_layer);
	GL_draw_layer_3* v_ft_layer = new V_flat_vertices_layer_3<Union_of_balls_3<K> >("V flat tets",this,"V flat tets", false);
	add_layer(v_ft_layer);

	// medial axis
	GL_draw_layer_3* ma_layer = new V_classified_triangulation_layer_3<Union_of_balls_3<K> >("Medial axis",this, "Medial axis of union of balls", false, M_INTERIOR, true);
	add_layer(ma_layer);
	GL_draw_layer_3* mae_layer = new V_classified_triangulation_layer_3<Union_of_balls_3<K> >("Medial axis edges",this, "Medial axis of union of balls", false, M_INTERIOR, true, false, false);
	add_layer(mae_layer);
	GL_draw_layer_3* la_ma_layer = new V_classified_triangulation_layer_3<Union_of_balls_3<K> >("Lambda medial axis",this, "Lambda medial axis of union of balls", false, M_INTERIOR, true, false, true, true);
	add_layer(la_ma_layer);
	GL_draw_layer_3* angle_ma_layer = new V_classified_triangulation_layer_3<Union_of_balls_3<K> >("Angle medial axis",this, "Angle medial axis of union of balls", false, M_INTERIOR, true, false, true, false, true);
	add_layer(angle_ma_layer);
	GL_draw_layer_3* scale_ma_layer = new V_classified_triangulation_layer_3<Union_of_balls_3<K> >("Scale medial axis",this, "Scale medial axis of union of balls", false, M_INTERIOR, true, false, true, false, false, true);
	add_layer(scale_ma_layer);
	GL_draw_layer_3* topangle_ma_layer = new V_classified_triangulation_layer_3<Union_of_balls_3<K> >("Top angle medial axis",this, "Topology preserving angle medial axis of union of balls", false, M_INTERIOR, true, false, true, false, false, false, true);
	add_layer(topangle_ma_layer);
	GL_draw_layer_3* topangle_mae_layer = new V_classified_triangulation_layer_3<Union_of_balls_3<K> >("Top angle medial edges",this, "Topology preserving angle medial axis of union of balls", false, M_INTERIOR, true, false, false, false, false, false, true);
	add_layer(topangle_mae_layer);

	GL_draw_layer_3* scale_balls_layer = new V_classified_triangulation_layer_3<Union_of_balls_3<K> >("Scale axis balls",this, "Balls approximating the scale axis", false, M_INTERIOR, false, true, true);
	add_layer(scale_balls_layer);


	// implicite mesher
	GL_draw_layer_3* impl_mesh_layer = new Implicit_mesher_layer_3<Union_of_balls_3<K> >("Implicit mesh",this, "Implicit mesh", false);
	add_layer(impl_mesh_layer);
	GL_draw_layer_3* impl_wf_layer = new Implicit_mesher_layer_3<Union_of_balls_3<K> >("Implicit wireframe",this, "Implicit wireframe", true);
	add_layer(impl_wf_layer);

	// poisson mesher
	GL_draw_layer_3* poi_mesh_layer = new Poisson_surface_layer_3<Union_of_balls_3<K> >("Poisson mesh",this, "Poisson surface mesh reconstructed from union of balls", false);
	add_layer(poi_mesh_layer);
	GL_draw_layer_3* poi_wf_layer = new Poisson_surface_layer_3<Union_of_balls_3<K> >("Poisson wireframe",this, "Poisson surface wireframe reconstructed from union of balls", true);
	add_layer(poi_wf_layer);
	

	// V_topology_filtration_layer_3
	GL_draw_layer_3* front_layer = new V_topology_filtration_layer_3<Union_of_balls_3<K> >("Debug V front",this, "DEBUG", true, false, false);
	add_layer(front_layer);
	GL_draw_layer_3* fronte_layer = new V_topology_filtration_layer_3<Union_of_balls_3<K> >("Debug V front edges",this, "DEBUG", true, false, false, false, false);
	add_layer(fronte_layer);
	GL_draw_layer_3* not_front_layer = new V_topology_filtration_layer_3<Union_of_balls_3<K> >("Debug not V front",this, "DEBUG", true, false, false, true);
	add_layer(not_front_layer);
	GL_draw_layer_3* not_fronte_layer = new V_topology_filtration_layer_3<Union_of_balls_3<K> >("Debug not V front edges",this, "DEBUG", true, false, false, true, false);
	add_layer(not_fronte_layer);
	GL_draw_layer_3* visited_layer = new V_topology_filtration_layer_3<Union_of_balls_3<K> >("Debug V visited",this, "DEBUG", false, true, false);
	add_layer(visited_layer);

	GL_draw_layer_3* next_layer = new V_topology_filtration_layer_3<Union_of_balls_3<K> >("Debug V next edges",this, "DEBUG", false, false, true, false, false);
	add_layer(next_layer);
	GL_draw_layer_3* nexte_layer = new V_topology_filtration_layer_3<Union_of_balls_3<K> >("Debug V next",this, "DEBUG", false, false, true, false, true);
	add_layer(nexte_layer);


	add_evolution("Grow balls");

	Application_settings::add_bool_setting("debug-topology-filtration",false);
	Application_settings::add_int_setting("debug-topology-filtration-steps",0);
	topology_filtration_steps = -1;
	if (Application_settings::get_bool_setting("debug-topology-filtration"))
		topology_filtration_steps = Application_settings::get_int_setting("debug-topology-filtration-steps");


	Application_settings::add_double_setting("growing-evolution-step-percentage",  3);
	Application_settings::add_bool_setting("debug-front-propagation-instead-of-topology-filtering", false);
	Application_settings::add_double_setting("alpha-value-for-dual-of-union-of-balls",  0);
	Application_settings::add_bool_setting("v-cell-flooding-only-infinite-cells", false);
	Application_settings::add_double_setting("medial-axis-export-vertex-squared-distance-collapse", 0);
//	medial::Medial_axis_transform s;
//	s.read_from_off("D:\\work\\mesecina\\models\\scale-axis\\3d\\10_random_balls_medial_axis.off");
//	s.read_from_off("D:\\work\\mesecina\\models\\scale-axis\\3d\\3holes_medial_axis.off");
	
}

template <class K> 
Union_of_balls_3<K>::~Union_of_balls_3() {}

template <class K>
void Union_of_balls_3<K>::apply_modification(const std::string& name) {
	if (name=="Grow balls") {
		double current_ratio = growth_ratio;
		double growing_step = Application_settings::get_double_setting("growing-evolution-step-percentage");
		double ratio = current_ratio + growing_step/100.0;
		double ratio_sq = ratio * ratio;
//		std::cout << "Ratio: " << ratio << std::endl;


		// create the weighted point list for alpha shape
		std::list< Weighted_point > wps;
		std::list<Point4D>::iterator p_it, p_end = points.end();
		for (p_it = points.begin(); p_it != p_end; ++p_it) {
			wps.push_back(Weighted_point(
				Bare_point(p_it->x,p_it->y,p_it->z),p_it->w*ratio_sq));
//			std::cout << wps.back() << std::endl;
		}

		std::cout << PROGRESS_STATUS << "Building alpha shape after growing" << std::endl;
		alpha_shape.make_alpha_shape(wps.begin(), wps.end());
		std::cout << PROGRESS_DONE << std::endl;

		add_variable("Ball scaling factor", ratio);
		growth_ratio = ratio;

		invalidate_cache();
		has_alpha_shape = true;
	}
}

template <class K> 
Geometry* Union_of_balls_3<K>::clone() {
	Union_of_balls_3<K>* new_union_of_balls = new Union_of_balls_3<K>();

	// create the weighted point list for alpha shape
	std::list< Weighted_point > wps;
	std::list<Point4D>::iterator p_it, p_end = points.end();
	for (p_it = points.begin(); p_it != p_end; ++p_it) {
		wps.push_back(Weighted_point(
			Bare_point(p_it->x,p_it->y,p_it->z),p_it->w));
		new_union_of_balls->points.push_back(*p_it);
	}

	// todo NO ALPHA SHAPE AT THIS POINT
	//std::cout << PROGRESS_STATUS << "Building alpha shape in cloning" << std::endl;
	//new_union_of_balls->alpha_shape.make_alpha_shape(wps.begin(), wps.end());
	//std::cout << PROGRESS_DONE << std::endl;

	new_union_of_balls->has_alpha_shape = true;
	new_union_of_balls->growth_ratio = growth_ratio;

	return new_union_of_balls;
}




template <class K> 
void Union_of_balls_3<K>::add_weighted_points(std::list<Point4D>* pts) {
	points.clear();
	std::list< Weighted_point > wps;
	std::list<Point4D>::iterator p_it, p_end = pts->end();
	for (p_it = pts->begin(); p_it != p_end; ++p_it) {
		wps.push_back(Weighted_point(
			Bare_point(p_it->x,p_it->y,p_it->z),p_it->w));
		points.push_back(*p_it);
	}
	std::cout << LOG_RED << "add_weighted_points point size: " << points.size() << std::endl;

	std::cout << PROGRESS_STATUS << "Building alpha shape" << std::endl;
	alpha_shape.make_alpha_shape(wps.begin(), wps.end());
	std::cout << PROGRESS_DONE << std::endl;

	// alpha values
	//int n = alpha_shape.number_of_alphas();
	//std::cout << "alphas: [";
	//for (int i=1; i<n; i++) {
	//	double a = CGAL::to_double(alpha_shape.get_nth_alpha(i));
	//	if (a>0 && a<10)
	//		std::cout << a << " ";
	//}
	//std::cout << "]" << std::endl;

	invalidate_cache();
	has_alpha_shape = true;
	add_variable("Balls",alpha_shape.number_of_vertices());
	std::cout << "balls number in variables: " << alpha_shape.number_of_vertices() << std::endl;
	emit announce_structure_changed(SHARED_INNER_DOUBLE_BALLS);
	emit announce_structure_changed(SHARED_SCALE_DOUBLE_BALLS);
//	emit do_widget_repaint();
}

template <class K> 
std::list<std::string>  Union_of_balls_3<K>::offer_structures() {
	std::list<std::string> names;
	names.push_back(SHARED_INNER_DOUBLE_BALLS);
	names.push_back(SHARED_SCALE_DOUBLE_BALLS);
	return names;
}

template <class K> 
void*  Union_of_balls_3<K>::give_structure(const std::string& name) {
	if (name==SHARED_INNER_DOUBLE_BALLS) {
		get_alpha_shape();
		return &points;
	}
	if (name==SHARED_SCALE_DOUBLE_BALLS) {
		get_flooded_v_triangulation();
		return &scale_balls;
	}
}

template <class K> 
typename Union_of_balls_3<K>::Alpha_shape_3* Union_of_balls_3<K>::get_alpha_shape() {
	if (!has_alpha_shape) {
		//		std::cout << LOG_GREEN << "get alpha shape" << std::endl;
		void* p = request_structure(SHARED_INNER_POLES);
		if (!p) std::cout << LOG_ERROR << "Error getting the input balls from the Union_of_balls_3!" << std::endl;
		else {
			std::list< Weighted_point >* wps = static_cast<std::list< Weighted_point >*>(p);
			std::list< Weighted_point >::iterator wp_it, wp_end = wps->end();
			points.clear();
			for (wp_it = wps->begin(); wp_it != wp_end; ++wp_it) {
				points.push_back(Point4D(
					CGAL::to_double(wp_it->point().x()),
					CGAL::to_double(wp_it->point().y()),
					CGAL::to_double(wp_it->point().z()),
					CGAL::to_double(wp_it->weight())
					));
			}
			std::cout << PROGRESS_STATUS << "Building alpha shape" << std::endl;
			alpha_shape.make_alpha_shape(wps->begin(), wps->end());
			std::cout << PROGRESS_DONE << std::endl;
			has_alpha_shape = true;
		}
	}
	return &alpha_shape;
}

template <class K> 
typename Union_of_balls_3<K>::V_triangulation_3* Union_of_balls_3<K>::get_v_triangulation() {
	if (!has_v_triangulation) {
		v_triangulation.clear();
		get_alpha_shape();
		std::cout << "Alpha value: " << CGAL::to_double(alpha_shape.get_alpha()) << std::endl;
		std::list<Alpha_facet>       facets;
		if (alpha_shape.number_of_vertices() > 3) {
			Point_3 p1, p2;

			// singular facets
			alpha_shape.get_alpha_shape_facets(std::back_inserter(facets), Alpha_shape_3::SINGULAR);
			std::list<Alpha_facet>::iterator f_it, f_end = facets.end(); int i = 0;
			std::cout << PROGRESS_STATUS << "Adding V points from singular facets" << std::endl;
			std::cout << PROGRESS_MINIMUM << "0" << std::endl;
			std::cout << PROGRESS_MAXIMUM <<  facets.size () << std::endl;
			for (f_it = facets.begin() ;f_it!=f_end; f_it++,i++) {
				std::cout << PROGRESS_VALUE << i << std::endl;
				CGAL::Object o = alpha_shape.dual(*f_it);
				Line_3 l; Ray_3 r; Segment_3 s;
				if (CGAL::assign(s,o)) l = s.supporting_line ();
				if (CGAL::assign(r,o)) l = r.supporting_line ();
				Weighted_point ball = f_it->first->vertex((f_it->second+1)%4)->point();
				ball = Weighted_point(ball.point(), ball.weight() + alpha_shape.get_alpha());
				if (intersect_ball_line(ball, l, p1, p2)) {
					v_triangulation.insert(p1, *f_it);
					v_triangulation.insert(p2, *f_it);
				} else {
					std::cout << LOG_WARNING << "Balls for singular face do not intersect!!" << std::endl;
				}
			}
			std::cout << PROGRESS_DONE << std::endl;

			facets.clear();

			// regular facets
			alpha_shape.get_alpha_shape_facets(std::back_inserter(facets), Alpha_shape_3::REGULAR);
			std::cout << PROGRESS_STATUS << "Adding V points from regular facets" << std::endl;
			std::cout << PROGRESS_MINIMUM << "0" << std::endl;
			std::cout << PROGRESS_MAXIMUM <<  facets.size () << std::endl;
			f_end = facets.end(); i = 0;
			for (f_it = facets.begin() ;f_it!=f_end; f_it++,i++) {
				std::cout << PROGRESS_VALUE << i << std::endl;
				CGAL::Object o = alpha_shape.dual(*f_it);
				Line_3 l; Ray_3 r; Segment_3 s;
				if (CGAL::assign(s,o)) l = s.supporting_line ();
				if (CGAL::assign(r,o)) l = r.supporting_line ();
				Weighted_point ball = f_it->first->vertex((f_it->second+1)%4)->point();
				ball = Weighted_point(ball.point(), ball.weight() + alpha_shape.get_alpha());
				if (intersect_ball_line(ball, l, p1, p2)) {
					Alpha_cell_handle c = f_it->first; int id = f_it->second;
					if (alpha_shape.classify(c) != Alpha_shape_3::INTERIOR) {
						Alpha_cell_handle nc = c->neighbor(id);
						int nid = nc->index(c);
						c = nc; id = nid;
						if (alpha_shape.classify(c) != Alpha_shape_3::INTERIOR) std::cout << LOG_ERROR << "regular face in dual has no interior cell neigbor?? error in V-point computation" << std::endl;
					}
					Plane_3 plane = Plane_3(c->vertex((id+1)%4)->point(),c->vertex((id+2)%4)->point(),c->vertex((id+3)%4)->point());
					if (plane.oriented_side(p1) != plane.oriented_side(c->vertex(id)->point()))
						v_triangulation.insert(p1, *f_it);
					else
						v_triangulation.insert(p2, *f_it);
				} else {
					std::cout << LOG_WARNING << "Balls for regular face do not intersect!!" << std::endl;
				}
			}
			std::cout << PROGRESS_DONE << std::endl;


		}
		has_v_triangulation = true;
		v_triangulation.set_dirty();
	}
	return &v_triangulation;
}

template <class K> 
typename Union_of_balls_3<K>::V_triangulation_3* Union_of_balls_3<K>::get_classified_v_triangulation() {
	if (!is_v_edges_classified) {
		get_v_triangulation();
		v_triangulation.compute_classification();
		is_v_edges_classified = true;
	}
	return &v_triangulation;
}


template <class K> 
typename Union_of_balls_3<K>::V_triangulation_3* Union_of_balls_3<K>::get_flooded_v_triangulation() {
	if (!is_v_cell_flooded) {
		get_classified_v_triangulation();
		v_triangulation.flood_cell_based_voronoi_classification();

		// extract medial balls which are shrunk
		scale_balls.clear();
		double scale_back = growth_ratio * growth_ratio;
		V_triangulation_3::Finite_cells_iterator c_it, c_end = v_triangulation.finite_cells_end();
		for (c_it = v_triangulation.finite_cells_begin(); c_it!=c_end; ++c_it) {
			if (c_it->get_medial_classification()==M_INTERIOR) {
				Point_3 c = v_triangulation.voronoi_vertex(c_it);
				double sqradius = CGAL::to_double(squared_distance(c, c_it->vertex(0)->point()));
				scale_balls.push_back(Point4D(
					CGAL::to_double(c.x()),
					CGAL::to_double(c.y()),
					CGAL::to_double(c.z()),
					sqradius/scale_back));
			}
		}
		add_variable("Interior V voronoi vertices (size of scale balls", scale_balls.size());

		//count voronoi faces in medial axis
		int voroni_faces_in_medial_axis = 0;
		V_triangulation_3::All_edges_iterator e_it, e_end = v_triangulation.all_edges_end();
		for (e_it=v_triangulation.all_edges_begin(); e_it!=e_end; ++e_it) {
			V_triangulation_3::Cell_circulator c_start, c_circ = v_triangulation.incident_cells(*e_it);
			c_start = c_circ;
			int vertices = 0;
			bool all_interior = true;
			do {
				if (!v_triangulation.is_infinite(c_circ) && c_circ->get_medial_classification() == M_INTERIOR) { 
					vertices++;
					//						std::cout << t->dual(c_circ) << std::endl;
				} else all_interior = false;
				c_circ++;
			} while (c_circ!=c_start);
			if (vertices>2 && (all_interior==true || !Application_settings::get_bool_setting("ignore-incomplete-v-voronoi-faces-for-medial-axis"))) {
				voroni_faces_in_medial_axis++;
			}
		}
		add_variable("V voronoi faces in medial axis", voroni_faces_in_medial_axis);



		is_v_cell_flooded = true;
	}
	return &v_triangulation;
}

//template <class K> 
//typename Union_of_balls_3<K>::V_triangulation_3* Union_of_balls_3<K>::get_local_filtered_medial_axis() {
//	if (!has_local_filtered_medial_axis) {
//		get_flooded_v_triangulation();
//		v_triangulation.compute_local_filtration_values();
//		has_local_filtered_medial_axis = true;
//	}
//	return &v_triangulation;
//}

template <class K> 
void  Union_of_balls_3<K>::write_medial_axis_to_off(const std::string& file_name) {
	get_flooded_v_triangulation();
	v_triangulation.write_medial_axis_to_off(file_name);
}

template <class K> 
void  Union_of_balls_3<K>::write_simplified_medial_axis_to_off(const std::string& file_name) {
	get_flooded_v_triangulation();
	v_triangulation.write_simplified_medial_axis_to_off(file_name);
}

template <class K> 
void  Union_of_balls_3<K>::write_medial_axis_to_moff(const std::string& file_name) {
	get_angle_filtered_medial_axis();
	v_triangulation.write_medial_axis_to_moff(file_name);
}


template <class K> 
typename Union_of_balls_3<K>::Edges_with_doubles* Union_of_balls_3<K>::get_angle_filtered_medial_axis() {
	if (!has_local_filtered_medial_axis) {
		get_flooded_v_triangulation();
		v_triangulation.compute_local_filtration_values();
		has_local_filtered_medial_axis = true;
	}
	return &(v_triangulation.angle_values);
}

template <class K> 
typename Union_of_balls_3<K>::Edges_with_doubles* Union_of_balls_3<K>::get_topology_angle_filtered_medial_axis() {
	if (!has_topology_angle_ma) {
		get_angle_filtered_medial_axis();
		std::cout << "Computing topology filtration with limit on steps: " << topology_filtration_steps << std::endl;
		v_triangulation.compute_topology_angle_filtration(topology_filtration_steps);
		has_topology_angle_ma = true;
		v_triangulation.check_edges_with_doubles(v_triangulation.topology_angle_values);
	}
	return &(v_triangulation.topology_angle_values);
}

template <class K> 
typename Union_of_balls_3<K>::Edges_with_doubles* Union_of_balls_3<K>::get_scale_filtered_medial_axis() {
	if (!has_scale_filtered_medial_axis) {
		get_flooded_v_triangulation();
		v_triangulation.compute_scale_filtration_values();
		has_scale_filtered_medial_axis = true;
	}
	return &(v_triangulation.scale_values);
}

template <class K> 
bool Union_of_balls_3<K>::intersect_ball_line(const Weighted_point &wp, const Line_3 &l, Point_3 &ip1, Point_3 &ip2) {
	Point_3 bp = wp.point();
	NT r_sq = wp.weight();

	Point_3 lp1 = l.projection(wp.point());

	Point_3 lp2 = CGAL::ORIGIN + (l.to_vector() + (lp1 - CGAL::ORIGIN));
	lp1 = CGAL::ORIGIN + ((lp1 - CGAL::ORIGIN) - l.to_vector()); // to make it more stable??


	NT lp1_x = lp1.x(); NT lp1_y = lp1.y(); NT lp1_z = lp1.z();
	NT lp2_x = lp2.x(); NT lp2_y = lp2.y(); NT lp2_z = lp2.z();
	NT bp_x = bp.x(); NT bp_y = bp.y(); NT bp_z = bp.z();

	NT a = squared_distance(lp1, lp2);
	NT b = (lp2_x-lp1_x)*(lp1_x - bp_x) + (lp2_y-lp1_y)*(lp1_y - bp_y) + (lp2_z-lp1_z)*(lp1_z - bp_z);
	b = 2*b;
	NT c = bp_x*bp_x + bp_y*bp_y + bp_z*bp_z + lp1_x*lp1_x + lp1_y*lp1_y + lp1_z*lp1_z -
		2*(bp_x*lp1_x + bp_y*lp1_y +bp_z*lp1_z) - r_sq;

	NT delta = b*b - 4*a*c;

	NT x,y,z;
	if (delta<0) return false;
	double sqr_delta = sqrt(CGAL::to_double(delta));
	NT mu = (-1 * b + sqr_delta)/2/a;
	x = lp1_x + mu*(lp2_x - lp1_x);
	y = lp1_y + mu*(lp2_y - lp1_y);
	z = lp1_z + mu*(lp2_z - lp1_z);
	ip1 = Point_3(x,y,z);

	mu = (-1* b - sqr_delta)/2/a;
	x = lp1_x + mu*(lp2_x - lp1_x);
	y = lp1_y + mu*(lp2_y - lp1_y);
	z = lp1_z + mu*(lp2_z - lp1_z);
	ip2 = Point_3(x,y,z);

	return true;
}

template <class K> 
void Union_of_balls_3<K>::receive_structure_changed(const std::string& name) {
	if (name == SHARED_INNER_POLES) {
//		std::cout << LOG_GREEN << "SHARED_INNER_POLES changed, invalidating alpha shape" << std::endl;
		invalidate_cache();
	}
}

template <class K>
void Union_of_balls_3<K>::application_settings_changed(const QString& settings_name) {
	if (settings_name == "alpha-value-for-dual-of-union-of-balls") {
		alpha_shape.set_alpha(Application_settings::get_double_setting("alpha-value-for-dual-of-union-of-balls"));
		invalidate_cache();
		has_alpha_shape = true;
		emit do_widget_repaint();
	}
	if (settings_name == "debug-front-propagation-instead-of-topology-filtering") {
		invalidate_all_layers();
		has_topology_angle_ma = false;
		emit do_widget_repaint();
	}

	if (settings_name.startsWith("debug-topology-filtration")) {
		topology_filtration_steps = -1;
		if (Application_settings::get_bool_setting("debug-topology-filtration"))
			topology_filtration_steps = Application_settings::get_int_setting("debug-topology-filtration-steps");
		invalidate_all_layers();
		has_topology_angle_ma = false;
		emit do_widget_repaint();
	}
}

template <class K> 
std::list<Point4D>* Union_of_balls_3<K>::get_weighted_points() {
	get_alpha_shape();
	std::cout << LOG_RED << "Getting weighted points from union of balls" << std::endl;
	return &points;
}

template <class K> 
void Union_of_balls_3<K>::load_generic_file(const std::string&) {
}

template <class K> 
void Union_of_balls_3<K>::save_generic_file(const std::string&) {
}

template <class K> 
void Union_of_balls_3<K>::invalidate_cache() {
//	std::cout << LOG_RED << "invalidate union of balls cache" << std::endl;
	invalidate_all_layers();
	has_alpha_shape = false;
	has_v_triangulation = false;
	is_v_edges_classified = false;
	is_v_cell_flooded = false;
	has_local_filtered_medial_axis = false;
	has_scale_filtered_medial_axis = false;
	has_topology_angle_ma = false;
//	emit announce_structure_changed(SHARED_INNER_DOUBLE_BALLS);
}