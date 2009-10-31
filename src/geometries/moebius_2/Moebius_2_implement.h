/* This source file is part of Mesecina, a software for visualization and studying of
* the medial axis and related computational geometry structures.
* More info: http://www.agg.ethz.ch/~miklosb/mesecina
* Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
*
* $Id: Power_crust_2.h 531 2009-02-05 22:02:06Z miklosb $
*/

#ifndef MESECINA_MOEBIUS_2_IMPLEMENT_H
#define MESECINA_MOEBIUS_2_IMPLEMENT_H

#include "layers/Moebius_balls_layer_2.h"
#include "layers/Vanishing_layer_2.h"
#include "layers/Critical_layer_2.h"
#include "layers/One_vanishing_ball_layer_2.h"
#include "layers/One_critical_point_layer_2.h"
#include "layers/Moebius_dual_layer_2.h"
#include "layers/Christophe_moebius_layer_2.h"

#include <gui/app/Managable_layer.h>
#include <gui/app/static/Application_settings.h>
#include <constants.h>

#define PI 3.14159265358979323846

template <class K>
Moebius_2<K>::Moebius_2() : Geometry(), has_vanishing_points(false), has_critical_points(false), has_balls(false), has_handles_sorted_by_date(false) {

	GL_draw_layer_2* moebius_center_layer = new Moebius_balls_layer_2< Moebius_2<K> >("Moebius centers",this, "Centers of balls as input for Moebius diagram", true, false);
	add_layer(moebius_center_layer);
	GL_draw_layer_2* moebius_circle_layer = new Moebius_balls_layer_2< Moebius_2<K> >("Moebius circles",this, "Circles used as input Moebius diagram", false, false);
	add_layer(moebius_circle_layer);
	GL_draw_layer_2* moebius_disk_layer = new Moebius_balls_layer_2< Moebius_2<K> >("Moebius disks",this, "Disks used as input Moebius diagram", false, true);
	add_layer(moebius_disk_layer);

	GL_draw_layer_2* moebius_grown_circle_layer = new Moebius_balls_layer_2< Moebius_2<K> >("Moebius scaled circles",this, "Scaled versions of circles used as input Moebius diagram", false, false, true);
	add_layer(moebius_grown_circle_layer);
	scaled_balls_layers.push_back(moebius_grown_circle_layer);
	GL_draw_layer_2* moebius_grown_disk_layer = new Moebius_balls_layer_2< Moebius_2<K> >("Moebius scaled disks",this, "Scaled versions of disks used as input Moebius diagram", false, true, true);
	add_layer(moebius_grown_disk_layer);
	scaled_balls_layers.push_back(moebius_grown_disk_layer);

	GL_draw_layer_2* current_grown_disk_layer = new One_vanishing_ball_layer_2< Moebius_2<K> >("Current scaled disk",this, "Scaled version of a disk form the input Moebius diagram", false, true);
	add_layer(current_grown_disk_layer);
//	scaled_balls_layers.push_back(current_grown_disk_layer);
	GL_draw_layer_2* current_grown_circle_layer = new One_vanishing_ball_layer_2< Moebius_2<K> >("Current scaled circle",this, "Scaled version of a circle form the input Moebius diagram", false, false);
	add_layer(current_grown_circle_layer);
//	scaled_balls_layers.push_back(current_grown_circle_layer);

	GL_draw_layer_2* current_vanishing_point_layer = new One_vanishing_ball_layer_2< Moebius_2<K> >("Current vanishing point",this, "Vanishing point of a circle form the input Moebius diagram", true, false);
	add_layer(current_vanishing_point_layer);

	GL_draw_layer_2* current_critical_circle_layer = new One_critical_point_layer_2< Moebius_2<K> >("Current critical circle",this, "A critical circle from the input Moebius diagram", false, false, false, false);
	add_layer(current_critical_circle_layer);
	GL_draw_layer_2* current_critical_disk_layer = new One_critical_point_layer_2< Moebius_2<K> >("Current critical disk",this, "A critical disk from the input Moebius diagram", false, false, true, false);
	add_layer(current_critical_disk_layer);

	GL_draw_layer_2* current_scritical_circle_layer = new One_critical_point_layer_2< Moebius_2<K> >("Current scaled critical circle",this, "A scaled critical circle from the input Moebius diagram", false, false, false, true);
	add_layer(current_scritical_circle_layer);
	GL_draw_layer_2* current_scritical_disk_layer = new One_critical_point_layer_2< Moebius_2<K> >("Current scaled critical disk",this, "A scaled critical disk from the input Moebius diagram", false, false, true, true);
	add_layer(current_scritical_disk_layer);

	GL_draw_layer_2* current_critical_point_layer = new One_critical_point_layer_2< Moebius_2<K> >("Current critical point",this, "A critical point from the input Moebius diagram", true, false, false, false);
	add_layer(current_critical_point_layer);

	GL_draw_layer_2* moebius_vertices_layer = new Moebius_dual_layer_2< Moebius_2<K> >("Moebius vertices",this, "Moebius vertices from the input Moebius diagram", true, false, false);
	add_layer(moebius_vertices_layer);
	scaled_balls_layers.push_back(moebius_vertices_layer);
	GL_draw_layer_2* moebius_dual_triangles_layer = new Moebius_dual_layer_2< Moebius_2<K> >("Moebius dual triangles",this, "Dual Moebius triangles: convex hull of the centers of the closest balls to a Moebius vertex", false, false, true);
	add_layer(moebius_dual_triangles_layer);
	scaled_balls_layers.push_back(moebius_dual_triangles_layer);
	GL_draw_layer_2* moebius_dual_segments_layer = new Moebius_dual_layer_2< Moebius_2<K> >("Moebius dual segments",this, "Dual Moebius triangles' boundary: the boundary convex hull of the centers of the closest balls to a Moebius vertex", false, false, false);
	add_layer(moebius_dual_segments_layer);
	scaled_balls_layers.push_back(moebius_dual_segments_layer);
	GL_draw_layer_2* moebius_dual_vertices_layer = new Moebius_dual_layer_2< Moebius_2<K> >("Moebius dual vertices",this, "Dual Moebius triangles' vertices: the vertices convex hull of the centers of the closest balls to a Moebius vertex", false, true, false);
	add_layer(moebius_dual_vertices_layer);
	scaled_balls_layers.push_back(moebius_dual_vertices_layer);

	GL_draw_layer_2* ch_moe_layer = new Christophe_moebius_layer_2< Moebius_2<K> >("Christophe's moebius",this, "Testing Christophs moebius diagram");
	add_layer(ch_moe_layer);

	GL_draw_layer_2* vanishing_layer = new Vanishing_layer_2< Moebius_2<K> >("Vanishing points",this, "Vanishing points one-by-one for every ball ordered by the time they are reached with the multiplicative growth");
	add_layer(vanishing_layer);

	GL_draw_layer_2* critical_layer = new Critical_layer_2< Moebius_2<K> >("Critical points",this, "Critical points and their contact points");
	add_layer(critical_layer);

	// add layers 
	// add evolutions
	// add settings
	Application_settings::add_double_setting("moebius-balls-scale-factor", 1);
}

template <class K>	
std::list<std::string> Moebius_2<K>::offer_structures() {
	std::list<std::string> names;
	names.push_back(SHARED_GROWN_BALLS_FROM_MOEBIUS);
	return names;
}

template <class K>	
void* Moebius_2<K>::give_structure(const std::string& name) {
	if (name == SHARED_GROWN_BALLS_FROM_MOEBIUS) {
		return &grown_input_points;
	}
	return 0;
}

template <class K>	
void Moebius_2<K>::invalidate_scaled_balls_layer() {
	std::list<GL_draw_layer_2*>::iterator l_it, l_end = scaled_balls_layers.end();
	for (l_it = scaled_balls_layers.begin(); l_it!=l_end; ++l_it) {
		(*l_it)->invalidate_cache();
	}
	application_settings_changed("moebius-balls-scale-factor");
}

template <class K>
Moebius_2<K>::~Moebius_2() {
	std::vector<Managable_layer*>::iterator l_it, l_end = l_layers.end();
	for (l_it = l_layers.begin(); l_it != l_end; l_it++)
		delete *l_it;
	l_layers.clear();
}

template <class K>
void Moebius_2<K>::invalidate_cache() {
	invalidate_all_layers();
	// invalidate computed results, this is called when new balls have been added, etc.
	has_balls = false;
	has_vanishing_points = false;
	has_critical_points = false;
	has_handles_sorted_by_date = false;
	clear_rt_3();
	handles_sorted_by_date.clear();
	critical_points.clear();
	moebius_vertices.clear();
	emit announce_structure_changed(SHARED_GROWN_BALLS_FROM_MOEBIUS);
}

template <class K>
void Moebius_2<K>::receive_structure_changed(const std::string& name) {
	if (name == SHARED_INNER_VORONOI_BALLS_FLOATS) {
		rt_3.clear();
		input_points.clear();
		grown_input_points.clear();
		invalidate_cache();
	}
}

template <class K>
void Moebius_2<K>::application_settings_changed(const QString& settings_name) {
	if (settings_name=="moebius-balls-scale-factor") {
		grown_input_points.clear();
		double scale_factor = Application_settings::get_double_setting("moebius-balls-scale-factor");
		std::list<Point3D>::iterator p_it, p_end = input_points.end();
		for (p_it = input_points.begin(); p_it!=p_end; p_it++) {
			grown_input_points.push_back(Point3D(p_it->x, p_it->y, p_it->z*scale_factor));
		}
		emit announce_structure_changed(SHARED_GROWN_BALLS_FROM_MOEBIUS);
		emit do_widget_repaint();
	}
}


template <class K>
void Moebius_2<K>::add_weighted_points(const std::list<Point3D>* points) {
	invalidate_cache();

	std::cout << PROGRESS_STATUS << "Adding lifted points to regular triangulation for Moebius computation" << std::endl;

	double scale_factor = Application_settings::get_double_setting("moebius-balls-scale-factor");
	std::list<Point3D>::const_iterator p_it, p_end = points->end();
	for (p_it = points->begin(); p_it != p_end; ++p_it) {
		input_points.push_back(*p_it);
		grown_input_points.push_back(Point3D(p_it->x, p_it->y, p_it->z*scale_factor));
		rt_3.insert(lift(*p_it))->input_point = *p_it;
	}
	std::cout << PROGRESS_DONE << std::endl;
	has_balls = true;

//	std::cout << points->size() << " circles added"  << std::endl;
}

template <class K>
void Moebius_2<K>::add_weighted_point(const Point3D& point) {
	invalidate_cache();

	input_points.push_back(point);
	double scale_factor = Application_settings::get_double_setting("moebius-balls-scale-factor");
	grown_input_points.push_back(Point3D(point.x, point.y, point.z*scale_factor));
	rt_3.insert(lift(point))->input_point = point;
	has_balls = true;
}

template <class K>
std::list<Point3D>* Moebius_2<K>::get_weighted_points() {
	return &input_points;
}

template <class K>
const typename Moebius_2<K>::RT_3* Moebius_2<K>::get_lifted_triangulation() {
	if (!has_balls) {

		void* p = request_structure(SHARED_INNER_VORONOI_BALLS_FLOATS);
		if (!p) std::cout << LOG_ERROR << "Error getting the input balls for the Moebius_2!" << std::endl;
		else {
			invalidate_cache();
			std::list< Point3D >* wps =static_cast<std::list< Point3D >*>(p);
			add_weighted_points(wps);
		}
	}
	return &rt_3;
}

template <class K>
const typename Moebius_2<K>::RT_3* Moebius_2<K>::get_triangulation_with_vanishing_points() {
	get_lifted_triangulation();
	if (!has_vanishing_points) {
		std::cout << PROGRESS_STATUS << "Compute vanishing points" << std::endl;
		compute_vanishing_points();
		std::cout << PROGRESS_DONE << std::endl;
	}
	return &rt_3;
}


template <class K>
bool Moebius_2<K>::enough_spheres() const {
	return rt_3.dimension() == 3;
}


template <class K>
typename Moebius_2<K>::Weighted_point_3 Moebius_2<K>::lift(const Point3D& point) {
	double r2 = 1./(point.z*point.z);
	double p2 = point.x*point.x + point.y*point.y;
	return Weighted_point_3(Point_3(r2*point.x, r2*point.y, -r2/2.), r2*(r2 - 1.)*p2 + r2*r2/4.);
}

template <class K>
void Moebius_2<K>::compute_vanishing_points() {
	if(rt_3.dimension() < 3)
		return;
	compute_cells_values();
	compute_edges_values();
	compute_facets_and_aggregate();
	post_process();
	has_vanishing_points = true;
}



template <class K>
const std::vector<typename Moebius_2<K>::Vertex_handle>* Moebius_2<K>::get_handles_sorted_by_date() {
	get_triangulation_with_vanishing_points();
	if (!has_handles_sorted_by_date) {
		std::cout << PROGRESS_STATUS << "Sort vanishing points" << std::endl;
		compute_handles_sorted_by_date();
		std::cout << PROGRESS_DONE << std::endl;
	}
	return &handles_sorted_by_date;
}

template <class K>
void Moebius_2<K>::compute_handles_sorted_by_date() {
	if(rt_3.dimension() < 3)
		return;

	handles_sorted_by_date.clear();

	handles_sorted_by_date.resize(rt_3.number_of_vertices());
	std::copy(Iterator_wrapper<Vertex_handle>(rt_3.finite_vertices_begin()), 
						Iterator_wrapper<Vertex_handle>(rt_3.finite_vertices_end()), handles_sorted_by_date.begin());
	std::sort(handles_sorted_by_date.begin(), handles_sorted_by_date.end(), Date_comparison());

	has_handles_sorted_by_date = true;
}

template <class K>
const std::vector<typename Moebius_2<K>::Critical_point_and_hull>* Moebius_2<K>::get_critical_points() {
	get_triangulation_with_vanishing_points();
	if (!has_critical_points) {
		std::cout << PROGRESS_STATUS << "Compute Moebius vertices and critical points" << std::endl;
		compute_critical_points();
		std::cout << PROGRESS_DONE << std::endl;
	}
	return &critical_points;
}

template <class K>
const std::vector<typename Moebius_2<K>::Critical_point_and_hull>* Moebius_2<K>::get_moebius_vertices() {
	get_triangulation_with_vanishing_points();
	if (!has_critical_points) {
		std::cout << PROGRESS_STATUS << "Compute Moebius vertices and critical points" << std::endl;
		compute_critical_points();
		std::cout << PROGRESS_DONE << std::endl;
	}
	return &moebius_vertices;
}

template <class K>
void Moebius_2<K>::compute_critical_points() {
	if(rt_3.dimension() < 3)
		return;

//	critical_points.clear(); DO NOT CLEAR, it already contains some of them

	RT_3::Finite_cells_iterator a, b;
	b = rt_3.finite_cells_end();

	for(a = rt_3.finite_cells_begin(); a != b; ++a) {
		check_moebius_vertices(a);
	}
	sort_critical_points();
	has_critical_points = true;
}

template <class K>
void Moebius_2<K>::sort_critical_points() {
	std::sort(critical_points.begin(), critical_points.end(), Critical_date_comparison());
	std::vector<Critical_point_and_hull>::iterator a = critical_points.begin();

	while(a != critical_points.end() && a->second.get<0>()->multiplicative_distance_to(a->first) < 1.)
		++a;
	critical_points.erase(critical_points.begin(), a);
}

template <class K>
void Moebius_2<K>::check_moebius_vertices(Cell_handle a) {
	std::map<Rtc*, std::vector<Point_3> >::const_iterator x,y;
	y = a->edge_intersection_points.end();
	for(x = a->edge_intersection_points.begin(); x != y; ++x) {
		Rtc* b = x->first;
		V_triple facet = common_facet(&(*a), b);
		const std::vector<Point_3>& moebius_v = x->second;
		for(size_t i = 0; i < moebius_v.size(); ++i) { // at most two...
			Point_2 mv(moebius_v[i].x(), moebius_v[i].y());
			if(is_inside_triangle(mv, facet))
				critical_points.push_back(std::make_pair(mv, facet));
			moebius_vertices.push_back(std::make_pair(mv, facet));
		}
	}
}

template <class K>
typename Moebius_2<K>::V_triple Moebius_2<K>::common_facet(Rtc* a, Rtc* b) {
	int i;
	for(i = 0; i < 4; ++i) {
		if(&(*(a->neighbor(i))) == b)
			break;
	}
	return boost::make_tuple(a->vertex((i+1)%4), a->vertex((i+2)%4), a->vertex((i+3)%4));
}

template <class K>
bool Moebius_2<K>::is_inside_triangle(const Point_2& mv, const V_triple& facet) {
	Point_2 p = facet.get<0>()->input_center();
	Point_2 q = facet.get<1>()->input_center();
	Point_2 r = facet.get<2>()->input_center();
	CGAL::Triangle_2<K> t(p,q,r);
	if(t.is_degenerate())
		return false;
	return t.bounded_side(mv) != CGAL::ON_UNBOUNDED_SIDE;
}

template <class K>
void Moebius_2<K>::post_process() {
	RT_3::Finite_vertices_iterator a,b;
	b = rt_3.finite_vertices_end();
	double max_date = 0.;
	for(a = rt_3.finite_vertices_begin(); a != b; ++a) {
		a->set_vanishing_date();
		if(a->vanishing_date > max_date)
			max_date = a->vanishing_date;
	}
	for(a = rt_3.finite_vertices_begin(); a != b; ++a) {
		if(a->extremal_points.empty())
			a->vanishing_date = max_date;
	}
}

template <class K>
void Moebius_2<K>::clear_rt_3() {
	RT_3::Finite_vertices_iterator a,b;
	b = rt_3.finite_vertices_end();
	for(a = rt_3.finite_vertices_begin(); a != b; ++a) {
		a->extremal_points.clear();
	}
	RT_3::Finite_cells_iterator c,d;
	d = rt_3.finite_cells_end();
	for(c = rt_3.finite_cells_begin(); c != d; ++c) {
		c->edge_intersection_points.clear();
	}
}

template <class K>
void Moebius_2<K>::compute_facets_and_aggregate() {
	RT_3::Finite_vertices_iterator a,b;
	b = rt_3.finite_vertices_end();
	for(a = rt_3.finite_vertices_begin(); a != b; ++a) {
		if(!compute_facets_around(a))
			aggregate(a);
	}
}

template <class K>
bool Moebius_2<K>::compute_facets_around(typename RT_3::Finite_vertices_iterator a) {
	std::vector<RT_3::Edge> neighbors;
	rt_3.finite_incident_edges(a, std::back_inserter(neighbors));
	std::vector<RT_3::Edge>::const_iterator n,m;
	m = neighbors.end();

	bool maximal_weight = true;
	std::vector<Point3D> max_neighbors;
	for(n = neighbors.begin(); n != m; ++n) {
		Point_2 extremal, contact;
		typename RT_3::Vertex_handle b = n->first->vertex(n->third);
		if(a->input_point.z < n->first->vertex(n->third)->input_point.z) {
			maximal_weight = false;
			extremal = extrem_on_bisector(a->input_point, n->first->vertex(n->third)->input_point);
			if(closer_to_edge(extremal, *n)) {
				a->extremal_points.push_back(extremal);
			}
			contact = contact_on_bisector(a->input_point, n->first->vertex(n->third)->input_point);
			if(closer_to_edge(contact, *n)) {
				critical_points.push_back(std::make_pair(contact, V_triple(a, a, n->first->vertex(n->third))));
			}
		}
		else {
			if(a->input_point.z == n->first->vertex(n->third)->input_point.z)
				max_neighbors.push_back(n->first->vertex(n->third)->input_point);
		}
	}
	if(maximal_weight && !is_inside_convex_hull(a->input_point, max_neighbors)) {
		a->extremal_points.clear();
		return true;
	}
	return false;
}

template <class K>
bool Moebius_2<K>::is_inside_convex_hull(const Point3D& p, const std::vector<Point3D>& hull) {
	typedef typename CGAL::Delaunay_triangulation_2<K> DT_2;

	DT_2 dt;
	for(std::vector<Point3D>::const_iterator a = hull.begin(); a != hull.end(); ++a) {
		dt.insert(Point_2(a->x, a->y));
	}
	if(dt.dimension() < 2)
		return false;

	DT_2::Locate_type ltype;
	int index;
	DT_2::Face_handle location = dt.locate(Point_2(p.x, p.y), ltype, index);

	if(ltype == DT_2::OUTSIDE_CONVEX_HULL)
		return false;
	if(ltype == DT_2::EDGE)
		return (!dt.is_infinite(location) && !dt.is_infinite(location->neighbor(index)));
	if(ltype == DT_2::VERTEX) {
		DT_2::Vertex_circulator nbx = dt.incident_vertices(location->vertex(index));
		DT_2::Vertex_circulator nby(nbx);
		do {
			++nbx;
			if(dt.is_infinite(nbx))
				return false;
		} while (nbx != nby);
		return true;
	}
	return true; // should not happen
}

template <class K>
void Moebius_2<K>::aggregate(typename RT_3::Finite_vertices_iterator a) {
	std::vector<typename RT_3::Cell_handle> incident_cells;
	rt_3.finite_incident_cells(a, std::back_inserter(incident_cells));
	std::vector<typename RT_3::Cell_handle>::const_iterator u,v;
	v = incident_cells.end();

	for(u = incident_cells.begin(); u != v; ++u) {
		std::map<Rtc*, std::vector<Point_3> >::const_iterator x,y;
		y = (*u)->edge_intersection_points.end();
		for(x = (*u)->edge_intersection_points.begin(); x != y; ++x) {
			if(x->first->has_vertex(a))
				transfer_points(x->second, a->extremal_points);
		}
	}

	a->vanishing_point = furthest_point(Point_2(a->input_point.x, a->input_point.y), a->extremal_points);
}

template <class K>
typename Moebius_2<K>::Point_2 Moebius_2<K>::furthest_point(const Point_2& source, const std::vector<Point_2>& targets) {
	if(targets.empty())
		return source;
	Point_2 furthest = targets.front();

	for(std::vector<Point_2>::const_iterator a = ++(targets.begin()); a != targets.end(); ++a) {
		if(CGAL::compare_distance_to_point(source, *a, furthest) == CGAL::LARGER)
			furthest = *a;
	}
	return furthest;
}

template <class K>
void Moebius_2<K>::transfer_points(const std::vector<Point_3>& points3, std::vector<Point_2>& points2) {
	for(std::vector<Point_3>::const_iterator a = points3.begin(); a != points3.end(); ++a) {
		points2.push_back(Point_2(a->x(), a->y()));
	}
}

//template <class K>
//bool Moebius_2<K>::closer_to_edge(const Point_2& p, const typename RT_3::Edge& e) const {
//	Point3D source = e.first->vertex(e.second)->input_point;
//	Point_2 s(source.x, source.y);
//	FT sq_mult_distance = CGAL::squared_distance(s, p)/(source.z*source.z);
//
//	std::vector<typename RT_3::Vertex_handle> neighbors;
//	rt_3.incident_vertices(e.first->vertex(e.second), std::back_inserter(neighbors));
//
//	std::vector<typename RT_3::Vertex_handle>::const_iterator f,g;
//	g = neighbors.end();
//	for(f = neighbors.begin(); f != g; ++f) {
//		if(*f == e.first->vertex(e.third))
//			continue;
//		Point3D opponent = (*f)->input_point;
//		Point_2 op(opponent.x, opponent.y);
//		FT sq_mult_d = CGAL::squared_distance(op, p)/(opponent.z*opponent.z);
//		if(sq_mult_d < sq_mult_distance)
//			return false;
//	}
//	return true;
//}

template <class K>
bool Moebius_2<K>::closer_to_edge(const Point_2& p, const typename RT_3::Edge& e) const {
	Point3D source = e.first->vertex(e.second)->input_point;
	Point_2 s(source.x, source.y);
	FT sq_mult_distance = CGAL::squared_distance(s, p)/(source.z*source.z);

	typename RT_3::Facet_circulator f = rt_3.incident_facets(e);
	typename RT_3::Facet_circulator g(f);

//	std::cout << "ball of radius " << source.z << std::endl;
//	std::cout << "sqdist base = " << sq_mult_distance << std::endl;
	do {
		++f;
		RT_3::Vertex_handle o = third_vertex(*f, e);
		if(rt_3.is_infinite(o))
			continue;
		Point3D opponent = o->input_point;
		Point_2 op(opponent.x, opponent.y);
		FT sq_mult_d = CGAL::squared_distance(op, p)/(opponent.z*opponent.z);
//		std::cout << "sqdist = " << sq_mult_d << std::endl;

		if(sq_mult_d < sq_mult_distance)
			return false;
	} while (f != g);
	return true;
}

template <class K>
typename Moebius_2<K>::RT_3::Vertex_handle Moebius_2<K>::third_vertex(const typename RT_3::Facet& f, const typename RT_3::Edge& e) {
	typename RT_3::Vertex_handle a = e.first->vertex(e.second);
	typename RT_3::Vertex_handle b = e.first->vertex(e.third);

	for(int i = 0; i < 3; ++i) {
		typename RT_3::Vertex_handle c = f.first->vertex((f.second + i)% 4);
		if(c != a && c != b)
			return c;
	}
	return typename RT_3::Vertex_handle(); // should bever happen
}

template <class K>
typename Moebius_2<K>::Point_2 Moebius_2<K>::extrem_on_bisector(const Point3D& a, const Point3D& n) {
	Point_2 pa(a.x, a.y);
	Point_2 pn(n.x, n.y);

	return pa + (a.z/(n.z - a.z))*(pa - pn);
}

template <class K>
typename Moebius_2<K>::Point_2 Moebius_2<K>::contact_on_bisector(const Point3D& a, const Point3D& n) {
	Point_2 pa(a.x, a.y);
	Point_2 pn(n.x, n.y);

	return pa + (a.z/(n.z + a.z))*(pn - pa);
}

template <class K>
void Moebius_2<K>::compute_cells_values() {
	RT_3::Finite_cells_iterator a,b;
	b = rt_3.finite_cells_end();
	for(a = rt_3.finite_cells_begin(); a != b; ++a) 
		compute_cell_value(a);
}

template <class K>
void Moebius_2<K>::compute_cell_value(typename RT_3::Finite_cells_iterator a) {
	Point_3 dv = rt_3.dual(a);
	a->dual_vertex = dv;
	a->dual_value = dv.z() - dv.x()*dv.x() - dv.y()*dv.y();
}

template <class K>
void Moebius_2<K>::compute_edges_values() {
	RT_3::Finite_cells_iterator a,b;
	b = rt_3.finite_cells_end();
	for(a = rt_3.finite_cells_begin(); a != b; ++a) 
		compute_edges_around(a);
}

template <class K>
void Moebius_2<K>::compute_edges_around(typename RT_3::Finite_cells_iterator a) {
	for(int i = 0; i < 4; ++i) {
		if(rt_3.is_infinite(a->neighbor(i))) {
			CGAL::Ray_3<K> ray;
			CGAL::assign(ray, rt_3.dual(std::make_pair(a, i)));
			a->edge_intersection_points[&(*(a->neighbor(i)))] = 
				solve_ray_intersection(ray.point(1), ray.source());
			continue;
		}
		if(&(*a) < &(*(a->neighbor(i))))
			a->edge_intersection_points[&(*(a->neighbor(i)))] = 
			intersection_points(a, a->neighbor(i));
	}
}


template <class K>
std::vector<typename Moebius_2<K>::Point_3> Moebius_2<K>::intersection_points(typename RT_3::Cell_handle a, typename RT_3::Cell_handle b) const {
	if(a->dual_value > 0. && b->dual_value > 0.)
		return std::vector<Point_3>();
	return solve_segment_intersection(a->dual_vertex, b->dual_vertex);
}

template <class K>
std::vector<typename Moebius_2<K>::Point_3> Moebius_2<K>::solve_segment_intersection(const Point_3& a, const Point_3& b) {
	Vector_3 ba = a - b;
	FT u = ba.x()*ba.x() + ba.y()*ba.y();
	FT v = 2.*(ba.x()*b.x() + ba.y()*b.y()) - ba.z();
	FT w = b.x()*b.x() + b.y()*b.y() - b.z();

	FT delta = v*v - 4.*u*w;
	if(delta <= 0.)
		return std::vector<Point_3>();

	std::vector<Point_3> result;
	FT s1 = (-v + CGAL::sqrt(delta))/(2. * u);
	FT s2 = (-v - CGAL::sqrt(delta))/(2. * u);

	if(s1 > -0.005 && s1 < 1.005)
		result.push_back(b + s1*ba);
	if(s2 > -0.005 && s2 < 1.005)
		result.push_back(b + s2*ba);

	return result;
}

// source is the second argument 
template <class K>
std::vector<typename Moebius_2<K>::Point_3> Moebius_2<K>::solve_ray_intersection(const Point_3& a, const Point_3& b) {
	Vector_3 ba = a - b;
	FT u = ba.x()*ba.x() + ba.y()*ba.y();
	FT v = 2.*(ba.x()*b.x() + ba.y()*b.y()) - ba.z();
	FT w = b.x()*b.x() + b.y()*b.y() - b.z();

	FT delta = v*v - 4.*u*w;
	if(delta <= 0.)
		return std::vector<Point_3>();

	std::vector<Point_3> result;
	FT s1 = (-v + CGAL::sqrt(delta))/(2. * u);
	FT s2 = (-v - CGAL::sqrt(delta))/(2. * u);

	if(s1 > 0.)
		result.push_back(b + s1*ba);
	if(s2 > 0.)
		result.push_back(b + s2*ba);

	return result;
}

#endif //MESECINA_MOEBIUS_2_IMPLEMENT_H