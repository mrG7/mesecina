/* This source file is part of Mesecina, a software for visualization and studying of
* the medial axis and related computational geometry structures.
* More info: http://www.agg.ethz.ch/~miklosb/mesecina
* Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
*
* $Id: Power_crust_2.h 531 2009-02-05 22:02:06Z miklosb $
*/

#ifndef MESECINA_UNION_OF_BALLS_2_IMPLEMENT_H
#define MESECINA_UNION_OF_BALLS_2_IMPLEMENT_H

#include "Ball_triangulation_2.h"
#include "V_triangulation_2.h"

#include "layers/Balls_layer_2.h"
#include "layers/Ball_triangulation_layer_2.h"
#include "layers/Balls_dual_layer_2.h"
#include "layers/V_points_layer_2.h"
#include "layers/V_triangulation_layer_2.h"
#include "layers/Ball_medial_axis_transform_layer_2.h"



#include <gui/app/Managable_layer.h>
#include <gui/app/static/Application_settings.h>

#define PI 3.14159265358979323846

template <class K>
Union_of_balls_2<K>::Union_of_balls_2(std::string b_source, std::string pre) : Geometry(), has_ball_triangulation(false), 
	has_v_triangulation(false), ball_source(b_source), prefix(pre), has_medial_balls(false) {

	if (ball_source != SHARED_GROWN_BALLS_FROM_MOEBIUS) {
		// balls themselves
		GL_draw_layer_2* balls_layer = new Balls_layer_2< Union_of_balls_2<K> >((prefix+" inner disks").c_str(), this, "Disks in the union of disks for Union_of_balls_2 layer", true);
		add_layer(balls_layer);
		GL_draw_layer_2* circles_layer = new Balls_layer_2< Union_of_balls_2<K> >((prefix+" inner circles").c_str(), this, "Circles in the union of disks for Union_of_balls_2 layer", false);
		add_layer(circles_layer);
		GL_draw_layer_2* centers_layer = new Balls_layer_2< Union_of_balls_2<K> >((prefix+" inner centers").c_str(), this, "Centers in the union of disks for Union_of_balls_2 layer", false, true);
		add_layer(centers_layer);



		// v points and their origins
		GL_draw_layer_2* v_points_layer = new V_points_layer_2< Union_of_balls_2<K> >((prefix+" V points").c_str(), this, "V points -- the set of intersection point of circles which are on the boundary of the union of disks", false, false);
		add_layer(v_points_layer);
		GL_draw_layer_2* v_points_edge_layer = new V_points_layer_2< Union_of_balls_2<K> >((prefix+" V origin edge").c_str(), this, "Origin of V points visualized as a segment connecting the vertex with the middle of the originating dual edge", true, false);
		add_layer(v_points_edge_layer);	
		GL_draw_layer_2* v_points_face_layer = new V_points_layer_2< Union_of_balls_2<K> >((prefix+" V origin triangle").c_str(), this, "Origin of V points visualized as a triangle formed with the vertex and the originating dual edge", false, true);
		add_layer(v_points_face_layer);	

		// v delaunay and voronoi
		GL_draw_layer_2* v_triang_layer = new V_triangulation_layer_2< Union_of_balls_2<K> >((prefix+" V Delaunay").c_str(), this, "V points delaunay triangulation", false, false, V_UNCLASSIFIED, false);
		add_layer(v_triang_layer);
		GL_draw_layer_2* v_voronoi_layer = new V_triangulation_layer_2< Union_of_balls_2<K> >((prefix+" V Voronoi").c_str(), this, "V points Voronoi diagram", true, false, V_UNCLASSIFIED, false);
		add_layer(v_voronoi_layer);

		// v classified edges and vertices
		GL_draw_layer_2* v_alph_voronoi_layer = new V_triangulation_layer_2< Union_of_balls_2<K> >((prefix+" V alpha edges").c_str(), this, "Voronoi edges of V points classified as alpha (covering a singular dual edge)", true, true, V_ALPHA, false);
		add_layer(v_alph_voronoi_layer);
		GL_draw_layer_2* v_cross_voronoi_layer = new V_triangulation_layer_2< Union_of_balls_2<K> >((prefix+" V crossing edges").c_str(), this, "Voronoi edges of V points classified as crossing (intersecting the boundary of the dual in one point)", true, true, V_CROSSING, false);
		add_layer(v_cross_voronoi_layer);
		GL_draw_layer_2* v_intv_voronoi_layer = new V_triangulation_layer_2< Union_of_balls_2<K> >((prefix+" V interior vertices").c_str(), this, "Voronoi vertice of V points classified as interior (inside the dual of the balls)", true, true, V_INTERIOR, true);
		add_layer(v_intv_voronoi_layer);
		GL_draw_layer_2* v_extv_voronoi_layer = new V_triangulation_layer_2< Union_of_balls_2<K> >((prefix+" V exterior vertices").c_str(), this, "Voronoi vertice of V points classified as interior (outside the dual of the balls)", true, true, V_EXTERIOR, true);
		add_layer(v_extv_voronoi_layer);
	}

	// ball triangulation and diagram
	GL_draw_layer_2* ball_triangulation_layer = new Ball_triangulation_layer_2< Union_of_balls_2<K> >((prefix+" ball triangulation").c_str(), this, "Regular triangulation of balls", false);
	add_layer(ball_triangulation_layer);
	GL_draw_layer_2* ball_diagram_layer = new Ball_triangulation_layer_2< Union_of_balls_2<K> >((prefix+" power diagram").c_str(), this, "Power diagram of balls", true);
	add_layer(ball_diagram_layer);

	// dual of the balls
	GL_draw_layer_2* singular_edges_layer = new Balls_dual_layer_2< Union_of_balls_2<K> >((prefix+" dual singular edges").c_str(), this, "Singular edges of the dual of the balls", false, U_SINGULAR);
	add_layer(singular_edges_layer);
	GL_draw_layer_2* regular_edges_layer = new Balls_dual_layer_2< Union_of_balls_2<K> >((prefix+" dual regular edges").c_str(), this, "Regular edges of the dual of the balls", false, U_REGULAR);
	add_layer(regular_edges_layer);	
	GL_draw_layer_2* interior_edges_layer = new Balls_dual_layer_2< Union_of_balls_2<K> >((prefix+" dual interior edges").c_str(), this, "Interior edges of the dual of the balls", false, U_INTERIOR);
	add_layer(interior_edges_layer);	
	GL_draw_layer_2* dual_faces_layer = new Balls_dual_layer_2< Union_of_balls_2<K> >((prefix+" dual faces").c_str(), this, "Faces in the dual of the balls", true);
	add_layer(dual_faces_layer);

	// medial axis of balls, centers, medial balls
	GL_draw_layer_2* ball_medial_axis_layer = new Ball_medial_axis_transform_layer_2< Union_of_balls_2<K> >((prefix+" ball medial axis").c_str(), this, "Medial axis of grown balls", true);
	add_layer(ball_medial_axis_layer);

	if (ball_source != SHARED_GROWN_BALLS_FROM_MOEBIUS) {
		GL_draw_layer_2* ball_medial_centers_layer = new Ball_medial_axis_transform_layer_2< Union_of_balls_2<K> >((prefix+" ball medial centers").c_str(), this, "Sampling of medial axis of grown balls", false, true);
		add_layer(ball_medial_centers_layer);
		GL_draw_layer_2* ball_medial_circles_layer = new Ball_medial_axis_transform_layer_2< Union_of_balls_2<K> >((prefix+" ball medial circles").c_str(), this, "Sampled circles from medial axis of grown balls", false, false, false);
		add_layer(ball_medial_circles_layer);
		GL_draw_layer_2* ball_medial_disks_layer = new Ball_medial_axis_transform_layer_2< Union_of_balls_2<K> >((prefix+" ball medial disks").c_str(), this, "Sampled disks from medial axis of grown balls", false, false, true);
		add_layer(ball_medial_disks_layer);

		// scale axis balls
		GL_draw_layer_2* ball_sa_circles_layer = new Ball_medial_axis_transform_layer_2< Union_of_balls_2<K> >((prefix+" scale axis circles").c_str(), this, "Sampled circles from the scale axis", false, false, false, true);
		add_layer(ball_sa_circles_layer);
		GL_draw_layer_2* ball_sa_disks_layer = new Ball_medial_axis_transform_layer_2< Union_of_balls_2<K> >((prefix+" scale axis disks").c_str(), this, "Sampled disks from the scale axis", false, false, true, true);
		add_layer(ball_sa_disks_layer);

		add_evolution((prefix+" growing").c_str());
		add_evolution((prefix+" reset growing").c_str());
		add_evolution((prefix+" shrinking").c_str());
		add_evolution((prefix+" scale skeleton").c_str());

		Application_settings::add_double_setting("growing-evolution-step-percentage", 3);
		Application_settings::add_bool_setting("ball-medial-axis-follow-scale-limit", false);
	}

	Application_settings::add_double_setting("medial-ball-sampling-min-angle-for-scale-axis", 175);
	double small_angle = 180.0 - Application_settings::get_double_setting("medial-ball-sampling-min-angle-for-scale-axis");
	small_angle = small_angle < 0 ? 0.0 : small_angle;
	medial_ball_intersection_cosinus = cos(small_angle * PI / 180.0);

	Application_settings::add_double_setting("alpha-value-for-dual-of-union-of-balls",  0);

}

template <class K>
void Union_of_balls_2<K>::apply_modification(const std::string& name) {
	if (name==(prefix+" growing").c_str()) {
		double current_ratio = CGAL::to_double(get_ball_triangulation()->get_growth_ratio());
		double growing_step = Application_settings::get_double_setting("growing-evolution-step-percentage");
		double ratio = current_ratio + growing_step/100.0;

		Ball_triangulation_2* t = get_ball_triangulation();
		t->clear();

		std::cout << PROGRESS_STATUS << "Inserting grown medial balls" << std::endl;
		std::list<Circle_2>* mb = get_orig_medial_balls();
		std::list<Circle_2>::iterator b_it, b_end = mb->end();
		for (b_it = mb->begin(); b_it!=b_end; b_it++) {
			double cx = CGAL::to_double(b_it->center().x());
			double cy = CGAL::to_double(b_it->center().y());
			double sqr = CGAL::to_double(b_it->squared_radius());
			// convert to double to "perturb" the input and not have such degeneracies
			t->insert(Ball_triangulation_2::Point(Point_2(cx,cy),sqr*ratio*ratio));
		}
		std::cout << PROGRESS_DONE << std::endl;

		t->multiply_growth_ratio(ratio/current_ratio);
		add_variable("Ball scaling factor", ratio);

		invalidate_cache();
		has_ball_triangulation = true;
	}
}

template <class K>
Union_of_balls_2<K>::~Union_of_balls_2() {
	std::vector<Managable_layer*>::iterator l_it, l_end = l_layers.end();
	for (l_it = l_layers.begin(); l_it != l_end; l_it++)
		delete *l_it;
//	std::cout << LOG_BLUE << "freeing up layers" << std::endl;
	l_layers.clear();
}

template <class K>
std::list<std::string> Union_of_balls_2<K>::offer_structures() {
	std::list<std::string> names;
	if (ball_source != SHARED_GROWN_BALLS_FROM_MOEBIUS)
		names.push_back(SHARED_INNER_VORONOI_BALLS_FLOATS);
	return names;
}

template <class K>
void* Union_of_balls_2<K>::give_structure(const std::string& name) {
	if (ball_source != SHARED_GROWN_BALLS_FROM_MOEBIUS)
		if (name == SHARED_INNER_VORONOI_BALLS_FLOATS) {
			get_ball_triangulation();
//			std::cout << "giving balls from union of balls to moebius: " << orig_points.size() << std::endl;
			return &orig_points;
		}
	return 0;
}

template <class K>
Geometry* Union_of_balls_2<K>::clone() {
	Union_of_balls_2<K>* new_union_of_balls = new Union_of_balls_2<K>(ball_source, prefix);

	new_union_of_balls->ball_triangulation = ball_triangulation;
	new_union_of_balls->v_triangulation = v_triangulation;

	new_union_of_balls->has_ball_triangulation = has_ball_triangulation;
	new_union_of_balls->has_v_triangulation = has_v_triangulation;
	new_union_of_balls->has_medial_balls = has_medial_balls;
	new_union_of_balls->medial_balls = medial_balls;
	new_union_of_balls->orig_medial_balls = orig_medial_balls;
	new_union_of_balls->random_tokens = random_tokens;
	new_union_of_balls->orig_points = orig_points;

	return new_union_of_balls;
}

template <class K>
void Union_of_balls_2<K>::invalidate_cache(bool set_has_ball_triangulation = false) {
//	std::cout << "union invalidate cache" << std::endl;
	invalidate_all_layers();
	if (ball_source != SHARED_GROWN_BALLS_FROM_MOEBIUS) emit announce_structure_changed(SHARED_INNER_VORONOI_BALLS_FLOATS);
	has_ball_triangulation = set_has_ball_triangulation;
	has_v_triangulation = false;
	has_medial_balls = false;
	medial_balls.clear();
	ball_triangulation.set_dirty();
	v_triangulation.set_dirty();
}

template <class K>
void Union_of_balls_2<K>::application_settings_changed(const QString& settings_name) {
	if (settings_name=="medial-ball-sampling-min-angle-for-scale-axis") {
		has_medial_balls = false;

		double small_angle = 180.0 - Application_settings::get_double_setting("medial-ball-sampling-min-angle-for-scale-axis");
		small_angle = small_angle < 0 ? 0.0 : small_angle;
		medial_ball_intersection_cosinus = cos(small_angle * PI / 180.0);
		std::cout << "cosinus value: " << medial_ball_intersection_cosinus << std::endl;
		emit do_widget_repaint();
	}

	if (settings_name == "alpha-value-for-dual-of-union-of-balls") {
		ball_triangulation.set_dirty();
		invalidate_cache();
		emit do_widget_repaint();
	}
	if (settings_name == "scale-limit" && Application_settings::get_bool_setting("ball-medial-axis-follow-scale-limit")) {
		double current_ratio = CGAL::to_double(get_ball_triangulation()->get_growth_ratio());
		double ratio = Application_settings::get_double_setting("scale-limit");
		std::cout << "growing ratio with scale limit ball scaling: " << ratio << std::endl;

		Ball_triangulation_2* t = get_ball_triangulation();
		t->clear();

		std::cout << PROGRESS_STATUS << "Inserting grown medial balls" << std::endl;
		std::list<Circle_2>* mb = get_orig_medial_balls();
		std::cout << "Size of medial balls: " << mb->size() << std::endl;
		std::list<Circle_2>::iterator b_it, b_end = mb->end();
		for (b_it = mb->begin(); b_it!=b_end; b_it++) {
			double cx = CGAL::to_double(b_it->center().x());
			double cy = CGAL::to_double(b_it->center().y());
			double sqr = CGAL::to_double(b_it->squared_radius());
			// convert to double to "perturb" the input and not have such degeneracies
			t->insert(Ball_triangulation_2::Point(Point_2(cx,cy),sqr*ratio*ratio));
		}
		std::cout << PROGRESS_DONE << std::endl;

		t->multiply_growth_ratio(ratio/current_ratio);
		add_variable("Ball scaling factor", ratio);

		invalidate_cache();
		has_ball_triangulation = true;
		emit do_widget_repaint();
	}
}

template <class K>
void Union_of_balls_2<K>::receive_structure_changed(const std::string& name) {
	if (name == ball_source) {

		invalidate_cache();
	}
}

template <class K>
void Union_of_balls_2<K>::add_weighted_points(const std::list<Point3D>* points) {
	if (ball_source == SHARED_GROWN_BALLS_FROM_MOEBIUS) return;
	invalidate_cache();
	std::list< Weighted_point > wps;
	std::list<Point3D>::const_iterator p_it, p_end = points->end();
	for (p_it = points->begin(); p_it != p_end; ++p_it) {
		wps.push_back(Weighted_point(
			Point_2(p_it->x,p_it->y),p_it->z*p_it->z));
	}
//	std::cout << "add_weighted_points point size: " << wps.size() << std::endl;
	orig_points.insert(orig_points.end(), points->begin(), points->end());

	ball_triangulation.import_weighted_points(&wps);
	has_ball_triangulation = true;
	add_variable("Balls",ball_triangulation.number_of_vertices()+ball_triangulation.number_of_hidden_vertices ());

	int number_of_balls = ball_triangulation.number_of_vertices () +ball_triangulation.number_of_hidden_vertices ();
	random_tokens.clear();
	random_tokens.resize(number_of_balls);
	// create vector and fill it with increasing indeces
	for (int i=0; i<number_of_balls; i++) {
		random_tokens[i] = i;
	}

	// suffle the values
	for (int i=0; i<number_of_balls; i++) {
		int j = static_cast<int>(((double)rand()/RAND_MAX) * (number_of_balls-1));
		int temp = random_tokens[j];
		random_tokens[j] = random_tokens[i];
		random_tokens[i] = temp;
	}

	emit do_widget_repaint();

//	std::cout << "balls number in variables: " << ball_triangulation.number_of_vertices() << std::endl;
}

template <class K>
void Union_of_balls_2<K>::add_weighted_point(const Point3D& point) {
	if (ball_source == SHARED_GROWN_BALLS_FROM_MOEBIUS) return;
	invalidate_cache();

	ball_triangulation.insert(Weighted_point(Point_2(point.x,point.y),point.z*point.z));
	has_ball_triangulation = true;
	add_variable("Balls",ball_triangulation.number_of_vertices()+ball_triangulation.number_of_hidden_vertices ());
	orig_points.push_back(point);

	int number_of_balls = ball_triangulation.number_of_vertices () +ball_triangulation.number_of_hidden_vertices ();
	random_tokens.clear();
	random_tokens.resize(number_of_balls);
	// create vector and fill it with increasing indeces
	for (int i=0; i<number_of_balls; i++) {
		random_tokens[i] = i;
	}

	// suffle the values
	for (int i=0; i<number_of_balls; i++) {
		int j = static_cast<int>(((double)rand()/RAND_MAX) * (number_of_balls-1));
		int temp = random_tokens[j];
		random_tokens[j] = random_tokens[i];
		random_tokens[i] = temp;
	}

	emit do_widget_repaint();


//	std::cout << "balls number in variables: " << ball_triangulation.number_of_vertices() << std::endl;
}

template <class K>
std::list<Point3D>* Union_of_balls_2<K>::get_weighted_points() {
	return &orig_points;
}


template <class K>
typename Union_of_balls_2<K>::Ball_triangulation_2* Union_of_balls_2<K>::get_ball_triangulation() {
	if (!has_ball_triangulation) {
		void* p = request_structure(ball_source);
		if (!p) std::cout << LOG_ERROR << "Error getting the input balls for the Union_of_balls_2!" << std::endl;
		else {
			invalidate_cache();
			ball_triangulation.clear();
			std::list< Weighted_point >* wps = 0;
			if (ball_source == SHARED_GROWN_BALLS_FROM_MOEBIUS) {
				wps = new std::list< Weighted_point >();
				std::list< Point3D >* points = static_cast<std::list< Point3D >*>(p);
				std::list<Point3D>::iterator p_it, p_end = points->end();
				for (p_it = points->begin(); p_it != p_end; ++p_it) {
					wps->push_back(Weighted_point(Point_2(p_it->x,p_it->y),p_it->z*p_it->z));
//					std::cout << "point: " << p_it->x << ", " << p_it->y << ", " << p_it->z << std::endl;
				}
			} else {
				try {
					wps =static_cast<std::list< Weighted_point >*>(p);
				} catch (const std::bad_cast& e) {
					std::cout << LOG_ERROR << "Union of balls could not convert balls from power crust. Make sure you use the same kernel in both geometries."
						" The error message is: " << e.what() << std::endl;
				}

			}
			
//			std::cout << "got " << wps->size() << " balls from the power crust" << std::endl;
			ball_triangulation.import_weighted_points(wps);
			if (ball_source == SHARED_GROWN_BALLS_FROM_MOEBIUS) {
				delete wps;
			}

			int number_of_balls = ball_triangulation.number_of_vertices () +ball_triangulation.number_of_hidden_vertices ();
			random_tokens.clear();
			random_tokens.resize(number_of_balls);
			// create vector and fill it with increasing indeces
			for (int i=0; i<number_of_balls; i++) {
				random_tokens[i] = i;
			}

			// suffle the values
			for (int i=0; i<number_of_balls; i++) {
				int j = static_cast<int>(((double)rand()/RAND_MAX) * (number_of_balls-1));
				int temp = random_tokens[j];
				random_tokens[j] = random_tokens[i];
				random_tokens[i] = temp;
			}

			add_variable("Balls",number_of_balls);

		}
		has_ball_triangulation = true;
	}
	return &ball_triangulation;
}

template <class K>
std::vector<int>*  Union_of_balls_2<K>::get_ball_random_tokens() {
	get_ball_triangulation();
	return &random_tokens;
}

template <class K>
typename Union_of_balls_2<K>::Ball_triangulation_2* Union_of_balls_2<K>::get_dual_ball_triangulation() {
	get_ball_triangulation();
	if (ball_triangulation.is_dirty()) {
		double alpha = Application_settings::get_double_setting("alpha-value-for-dual-of-union-of-balls");
		std::cout << "recomputing dual of union of balls with alpha " << alpha << std::endl;
		ball_triangulation.compute_dual(alpha);
	}
	return &ball_triangulation;
}

template <class K>
typename Union_of_balls_2<K>::V_triangulation_2* Union_of_balls_2<K>::get_v_triangulation() {
	get_dual_ball_triangulation();
	if (!has_v_triangulation) fill_v_triangulation();
	return &v_triangulation;
}

template <class K>
void Union_of_balls_2<K>::fill_v_triangulation() {
	std::cout << PROGRESS_STATUS << "Computing v-vertex positions" << std::endl;
	std::cout << PROGRESS_MINIMUM << "0" << std::endl;
	std::cout << PROGRESS_MAXIMUM <<  ball_triangulation.tds().number_of_edges()-1 << std::endl; int i=0;
	std::list< std::pair<Point_2,Edge> > v_points;
	v_triangulation.clear();
	for (Finite_edges_iterator e_it = ball_triangulation.finite_edges_begin(),
		e_end = ball_triangulation.finite_edges_end(); e_it != e_end; e_it++, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			Face_handle f = e_it->first;
			int id = e_it->second; 
			if (f->get_classification(id) == U_SINGULAR || f->get_classification(id) == U_REGULAR) {
				Weighted_point wp0 = f->vertex((id+1)%3)->point();
				Weighted_point wp1 = f->vertex((id+2)%3)->point();
				Point_2 p1, p2;
				bool has_intersection = intersect_ball_ball(wp0, wp1, p1, p2);
				if (has_intersection) {
					//for singular add both points
					if (f->get_classification(id) == U_SINGULAR) {
						v_points.push_back(std::make_pair(p1,*e_it));
						v_points.push_back(std::make_pair(p2,*e_it));
					}
					//for regular add only the "other side point"
					if (f->get_classification(id) == U_REGULAR) {
						Face_handle nF = f->neighbor(id);
						int nid = nF->index(f);;
						if (f->get_classification()!=U_INTERIOR) {
							f = nF;
							id = nid;
						}
						assert(f->get_classification()==U_INTERIOR);
						Line_2 edge = Line_2(f->vertex((id+1)%3)->point(), f->vertex((id+2)%3)->point());
						if (edge.oriented_side(p1) != edge.oriented_side(f->vertex((id)%3)->point())) {
							v_points.push_back(std::make_pair(p1,*e_it));
						} else {
							v_points.push_back(std::make_pair(p2,*e_it));
						}
					}
				} else {
					std::cout << LOG_ERROR << "ERROR: COULD NOT COMPUTE V VERTEX FOR A REGULAR OR SINGULAR EDGE" << std::endl;
				}
			}
	}
	std::cout << PROGRESS_DONE << std::endl;

	std::cout << PROGRESS_STATUS << "Computing v triangulation" << std::endl;
	std::cout << PROGRESS_MINIMUM << "0" << std::endl;
	std::cout << PROGRESS_MAXIMUM <<  v_points.size() << std::endl; i=0;
	// the spacial sorting takes much more and does not improve on the speed of delaunay triangulation
	// with spacial sorting takes 17 sec, without directly one by one 10 sec for the giraffe

	std::list< std::pair<Point_2,Edge> >::iterator vp_it, vp_end = v_points.end(); 
	for (vp_it = v_points.begin(); vp_it!=vp_end; vp_it++, i++) {
		std::cout << PROGRESS_VALUE << i << std::endl;
		v_triangulation.safe_insert(vp_it->first, vp_it->second);
	}
	std::cout << PROGRESS_DONE << std::endl;
	has_v_triangulation = true;
}

template <class K>
bool Union_of_balls_2<K>::intersect_ball_ball(const typename Union_of_balls_2<K>::Weighted_point &wp0, 
											  const typename Union_of_balls_2<K>::Weighted_point &wp1, 
											  typename Union_of_balls_2<K>::Point_2& ip1, 
											  typename Union_of_balls_2<K>::Point_2& ip2) {
	NT x0 = wp0.point().x();
	NT y0 = wp0.point().y();
	Point_2 p0 = wp0.point();
	NT w0 = wp0.weight();

	NT x1 = wp1.point().x();
	NT y1 = wp1.point().y();
	Point_2 p1 = wp1.point();
	NT w1 = wp1.weight();

	NT d_sq = squared_distance(p0, p1);
	NT d = CGAL::sqrt(CGAL::to_double(d_sq)); /// \todo why does it freeze for CORE?

	NT a = (w0-w1 + d_sq) / (NT(2)*d);

	Vector_2 v0 = p0 - CGAL::ORIGIN;
	Vector_2 v1 = p1 - CGAL::ORIGIN;
	Point_2 p2 = CGAL::ORIGIN + (v0 + (a*(v1-v0)/d));

	NT x2 = p2.x();
	NT y2 = p2.y();

	NT h2 = w0-a*a;
	if (h2<0) h2 = 0;
	NT h = CGAL::sqrt(h2);

	NT ix1 = x2 + h * (y0-y1) / d;
	NT iy1 = y2 + h * (x1-x0) / d;

	NT ix2 = x2 - h * (y0-y1) / d;
	NT iy2 = y2 - h * (x1-x0) / d;

	ip1 = Point_2(ix1, iy1);
	ip2 = Point_2(ix2, iy2);

	return true;
}

template <class K>
typename Union_of_balls_2<K>::V_triangulation_2* Union_of_balls_2<K>::get_classified_v_triangulation() {
	get_v_triangulation();
	if (v_triangulation.is_dirty()) {
		v_triangulation.compute_classification();
	}
	return &v_triangulation;
}

template <class K>
typename Union_of_balls_2<K>::V_triangulation_2* Union_of_balls_2<K>::get_flooded_v_triangulation() {
	get_classified_v_triangulation();
	if (!v_triangulation.is_flooded()) {
		v_triangulation.flood_voronoi_classification();
	}
	return &v_triangulation;
}

template <class K>
void Union_of_balls_2<K>::add_medial_balls_between(typename Union_of_balls_2<K>::Point_2& c1, 
												   typename Union_of_balls_2<K>::NT& w1, 
												   typename Union_of_balls_2<K>::Point_2& c2, 
												   typename Union_of_balls_2<K>::NT& w2, 
												   typename Union_of_balls_2<K>::Point_2& intersection,
												   bool debug_this = false) {
	double c1x = CGAL::to_double(c1.x());
	double c1y = CGAL::to_double(c1.y());				
	double c2x = CGAL::to_double(c2.x());
	double c2y = CGAL::to_double(c2.y());
	//if ((-4.50 < c1x && c1x < -4.4) || (-4.50 < c2x && c2x < -4.4))
	//	if ((5.1 < c1x && c1x < 5.2) || (5.1 < c2x && c2x < 5.2))
	//		if ((-10.4 < c1y && c1y < -10.3) || (-10.4 < c2y && c2y < -10.3))
	//			if ((-69.6 < c1y && c1y < -69.5) || (-69.6 < c2y && c2y < -69.5)) {
	//				debug_this = true; 
	//				std::cout << LOG_GREEN << "found an edge to debug" << std::endl;
	//			}
	double d_sq = (c1x-c2x)*(c1x-c2x) + (c1y-c2y)*(c1y-c2y);
	double d = sqrt(d_sq);
	double sr_sq = CGAL::to_double(ball_triangulation.get_shrink_ratio())*CGAL::to_double(ball_triangulation.get_shrink_ratio());
	double r1_sq = CGAL::to_double(w1)* sr_sq;
	double r1 = sqrt(r1_sq);
	double r2_sq = CGAL::to_double(w2)* sr_sq;
	double r2 = sqrt(r2_sq);
	double cos_alpha = (r1_sq + r2_sq - d_sq)/2.0/r1/r2;
	//		if (r1>r2) { double temp = r2; r2 = r1; r1 = temp; } // r1 is the smaller radius

	if (debug_this) {
		std::cout << "c1: " << c1 << "   -   w1: " << w1 << std::endl;
		std::cout << "c2: " << c2 << "   -   w2: " << w2 <<std::endl;
		std::cout << "intersection: " << intersection << " sr_sq: " << sr_sq << std::endl;
		std::cout << LOG_BLUE << "d: " << d << " r1: " << r1 << " r2: " << r2 << " cos_alpha: " << cos_alpha <<std::endl;
	}
	if ( cos_alpha < medial_ball_intersection_cosinus) {
		Point_2 midcenter = CGAL::ORIGIN + (((c1-CGAL::ORIGIN) + (c2-CGAL::ORIGIN)) / 2.0);
		double vx = CGAL::to_double(midcenter.x());
		double vy = CGAL::to_double(midcenter.y());
		double fx = CGAL::to_double(c1.x());
		double fy = CGAL::to_double(c1.y());
		if (debug_this) std::cout <<  LOG_BLUE << "want to split" << std::endl;
		if ((vx-fx)*(vx-fx) + (vy-fy)*(vy-fy) > MAX_CROSSING_V_VORONOI_SQUARED) {
			NT w = squared_distance(midcenter, intersection);
			if (debug_this) std::cout <<  LOG_BLUE << "split with: " << midcenter << "    -w: " << CGAL::to_double(w) << std::endl;
			medial_balls.push_back(Circle_2(midcenter, w));
			add_medial_balls_between(c1, w1, midcenter, w, intersection, debug_this);
			add_medial_balls_between(midcenter, w, c2, w2, intersection, debug_this);
		}

	}
}

template <class K>
void Union_of_balls_2<K>::add_medial_balls_between(typename Union_of_balls_2<K>::Point_2& c1, 
												   typename Union_of_balls_2<K>::NT& w1, 
												   typename Union_of_balls_2<K>::Point_2& c2, 
												   typename Union_of_balls_2<K>::NT& w2) {
	double vx = CGAL::to_double(c1.x());
	double vy = CGAL::to_double(c1.y());
	double fx = CGAL::to_double(c2.x());
	double fy = CGAL::to_double(c2.y());
	if ((vx-fx)*(vx-fx) + (vy-fy)*(vy-fy) > MAX_CROSSING_V_VORONOI_SQUARED) {
		Point_2 int1, int2;
		if (intersect_ball_ball(Weighted_point(c1,w1), Weighted_point(c2,w2), int1, int2)) {
			add_medial_balls_between(c1, w1, c2, w2, int1);
		}

	}
}

template <class K>
std::list<typename Union_of_balls_2<K>::Circle_2>* Union_of_balls_2<K>::get_medial_balls() {
	typedef typename Ball_triangulation_2::Vertex_handle							Ball_vertex_handle;
	typedef typename Ball_triangulation_2::Face_handle									Ball_face_handle;
	typedef typename Ball_triangulation_2::Finite_edges_iterator			Ball_finite_edges_iterator;
	typedef typename V_triangulation_2::All_edges_iterator					All_edges_iterator;
	typedef typename V_triangulation_2::Finite_edges_iterator					Finite_edges_iterator;
	typedef typename V_triangulation_2::Finite_faces_iterator					Finite_faces_iterator;

	V_triangulation_2* t = get_flooded_v_triangulation();
	if (!has_medial_balls) {
		medial_balls.clear();
		std::cout << "Storing circumcenters of v voronoi" << std::endl;
		std::cout << PROGRESS_STATUS << "Storing circumcenters of v voronoi v voronoi" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM <<  t->number_of_faces()  << std::endl; int i = 0;
		for (Finite_faces_iterator f_it = t->finite_faces_begin(), f_end = t->finite_faces_end(); f_it != f_end; ++f_it, ++i) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			f_it->set_added_to_medial_balls(false);
			f_it->set_circumcenter(t->dual(f_it));
		}
		std::cout << PROGRESS_DONE << std::endl;

		std::cout << "Extracting medial disks from v voronoi" << std::endl;
		std::cout << PROGRESS_STATUS << "Extracting medial disks from v voronoi" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM <<  t->tds().number_of_edges()-1  << std::endl; i = 0;
		for (All_edges_iterator e_it = t->all_edges_begin(), e_end = t->all_edges_end();	e_it != e_end; ++e_it, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			V_face_handle f = e_it->first;
			int id = e_it->second;
			V_face_handle nF = f->neighbor(id);
			//std::cout << LOG_GREEN << "testing edge "<< f->get_circumcenter()<<" - " << nF->get_circumcenter() << std::endl;
			if (f->get_classification() == V_INTERIOR && nF->get_classification() == V_INTERIOR) {
				Point_2 vorf = f->get_circumcenter(); NT wf, wnF;
				Point_2 vornF = nF->get_circumcenter();
				wf = squared_distance(vorf, f->vertex(0)->point());
				wnF = squared_distance(vornF, nF->vertex(0)->point());
				if (!f->get_added_to_medial_balls()) {
					medial_balls.push_back(Circle_2(vorf,  wf));
					t->set_voronoi_vertex_is_added(f);
				}
				if (!nF->get_added_to_medial_balls()) {
					medial_balls.push_back(Circle_2(vornF,  wnF));
					t->set_voronoi_vertex_is_added(nF);
				}

				add_medial_balls_between(vorf, wf, vornF, wnF, f->vertex((e_it->second+1)%3)->point());
			} else if (f->get_classification(id) == V_CROSSING) {
				Ball_vertex_handle mid = f->get_ball_voronoi_edge(id);
				// this is a crossing edge
				Point_2 dual_v_vertex = f->vertex((id+1)%3)->point();
				if (f->get_classification() != V_INTERIOR) {
					V_face_handle temp = f;
					f = nF;
					nF = temp;
				}
				if (f->get_classification() == V_INTERIOR && nF->get_classification() == V_EXTERIOR) {
					// f has to be interior end vertex, and nF exterior
					// if the associated ball point where this edge has to be split is not an endvertex
					// add it to the medial axis
					if (mid != f->get_ball_voronoi_vertex() && mid != nF->get_ball_voronoi_vertex()) {
						//std::cout << "crossing edge processing "<< f->get_circumcenter()<<" - " << nF->get_circumcenter() << std::endl;
						bool added = false;
						Point_2 vorf = f->get_circumcenter();
						NT wf, wmid;
						wf = squared_distance(vorf, f->vertex(0)->point());
						if (!f->get_added_to_medial_balls()) {
							//std::cout << "f added: " << vor << std::endl;
							//							wf = squared_distance(vorf, f->vertex(0)->point());
							medial_balls.push_back(Circle_2(vorf, wf));
							t->set_voronoi_vertex_is_added(f);
						}

						// test if a crossing edge is not "degenerate.. sucht that it is basically touching...
						// I didn't f
						Point_2 vormid = mid->point();//.point();
						double vx = CGAL::to_double(vormid.x());
						double vy = CGAL::to_double(vormid.y());
						double fx = CGAL::to_double(f->get_circumcenter().x());
						double fy = CGAL::to_double(f->get_circumcenter().y());
						if ((vx-fx)*(vx-fx) + (vy-fy)*(vy-fy) > MAX_CROSSING_V_VORONOI_SQUARED) {
							//std::cout << "mid added: " << vor << std::endl;
							wmid = squared_distance(vormid, dual_v_vertex);
							medial_balls.push_back(Circle_2(vormid, wmid));
							added = true;

						}
						if (added) add_medial_balls_between(vorf, wf, vormid, wmid);
						//std::cout << "crossing edge processing done"<< std::endl;
					}
				} else {
					//std::cout << LOG_ERROR << "V_CROSSING edge is not really consistent" << std::endl;
				}
			} else if (f->get_classification(id) == V_ALPHA) {
				Ball_vertex_handle mid = f->get_ball_voronoi_edge(id);
				Ball_vertex_handle mid2 = f->get_ball_voronoi_edge_second(id);

				if (mid == mid2) std::cout << LOG_ERROR << " mid == mid2 " << std::endl;
				//					else std::cout << LOG_GREEN << " mid is different than mid2 " << std::endl;

				Point_2 vormid; NT wmid; bool mid_added = false;

				if (mid==f->get_ball_voronoi_vertex()) {
					vormid = f->get_circumcenter();
					wmid = squared_distance(vormid, f->vertex(0)->point());
					mid_added = true;
					if (!f->get_added_to_medial_balls()) {
						//std::cout << "f added: " << vor << std::endl;
						medial_balls.push_back(Circle_2(vormid, wmid));
						t->set_voronoi_vertex_is_added(f);
					}
				} else if (mid==nF->get_ball_voronoi_vertex()) {
					vormid = nF->get_circumcenter();
					wmid = squared_distance(vormid, nF->vertex(0)->point());
					mid_added = true;
					if (!nF->get_added_to_medial_balls()) {
						medial_balls.push_back(Circle_2(vormid,  wmid));
						t->set_voronoi_vertex_is_added(nF);
					}
				} else {
					vormid = mid->point().point();
					wmid = mid->point().weight();
					double vx = CGAL::to_double(vormid.x());
					double vy = CGAL::to_double(vormid.y());
					double fx = CGAL::to_double(f->get_circumcenter().x());
					double fy = CGAL::to_double(f->get_circumcenter().y());
					double nfx = CGAL::to_double(nF->get_circumcenter().x());
					double nfy = CGAL::to_double(nF->get_circumcenter().y());
					if ((vx-fx)*(vx-fx) + (vy-fy)*(vy-fy) > MAX_CROSSING_V_VORONOI_SQUARED &&
						(vx-nfx)*(vx-nfx) + (vy-nfy)*(vy-nfy) > MAX_CROSSING_V_VORONOI_SQUARED) {
							//std::cout << "mid added: " << vor << std::endl;
							medial_balls.push_back(Circle_2(mid->point().point(),  wmid));
							mid_added = true;
					}
				}

				Point_2 vormid2; NT wmid2; bool mid2_added = false;
				if (mid2==f->get_ball_voronoi_vertex()) {
					vormid2 = f->get_circumcenter();
					wmid2 = squared_distance(vormid2, f->vertex(0)->point());
					mid2_added = true;
					if (!f->get_added_to_medial_balls()) {
						//std::cout << "f added: " << vor << std::endl;
						medial_balls.push_back(Circle_2(vormid2,  wmid2));
						t->set_voronoi_vertex_is_added(f);
					}
				} else if (mid2==nF->get_ball_voronoi_vertex()) {
					vormid2 = nF->get_circumcenter();
					wmid2 = squared_distance(vormid2, nF->vertex(0)->point());
					mid2_added = true;
					if (!nF->get_added_to_medial_balls()) {
						medial_balls.push_back(Circle_2(vormid2,  wmid2));
						t->set_voronoi_vertex_is_added(nF);
					}
				} else {
					vormid2 = mid2->point().point();
					wmid2 = mid2->point().weight();
					double vx = CGAL::to_double(vormid2.x());
					double vy = CGAL::to_double(vormid2.y());
					double fx = CGAL::to_double(f->get_circumcenter().x());
					double fy = CGAL::to_double(f->get_circumcenter().y());
					double nfx = CGAL::to_double(nF->get_circumcenter().x());
					double nfy = CGAL::to_double(nF->get_circumcenter().y());
					if ((vx-fx)*(vx-fx) + (vy-fy)*(vy-fy) > MAX_CROSSING_V_VORONOI_SQUARED &&
						(vx-nfx)*(vx-nfx) + (vy-nfy)*(vy-nfy) > MAX_CROSSING_V_VORONOI_SQUARED) {
							//std::cout << "mid added: " << vor << std::endl;
							medial_balls.push_back(Circle_2(mid2->point().point(),  wmid2));
							mid2_added = true;
					}

				}

				//					if (mid_added && mid2_added) {
				//						std::cout << LOG_BLUE << "singular middle balls to be added" << std::endl;
				add_medial_balls_between(vormid, wmid, vormid2, wmid2);
				//					} else {

				//					}

			}
		}
		std::cout << PROGRESS_DONE << std::endl;

		std::list<Circle_2>::iterator b_it, b_end = medial_balls.end();
		for (b_it = medial_balls.begin(); b_it!=b_end; b_it++)
			medial_points.push_back(Point3D(CGAL::to_double(b_it->center().x()),
			CGAL::to_double(b_it->center().y()),
			CGAL::to_double(b_it->squared_radius())));

		has_medial_balls = true;
	}
	return &medial_balls;
}

template <class K>
std::list<typename Union_of_balls_2<K>::Circle_2>* Union_of_balls_2<K>::get_orig_medial_balls(bool not_compute = false) {
	if (ball_triangulation.get_shrink_ratio()==1 && !not_compute) {
		get_medial_balls();
		orig_medial_balls = medial_balls;
	}
	return &orig_medial_balls;
}



#endif //MESECINA_UNION_OF_BALLS_2_IMPLEMENT_H