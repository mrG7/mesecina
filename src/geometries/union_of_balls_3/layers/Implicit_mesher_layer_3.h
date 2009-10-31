/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: V_triangulation_layer_3.h 373 2008-07-11 02:20:13Z miklosb $
 */

#ifndef MESECINA_IMPLICIT_MESHER_LAYER_3_H
#define MESECINA_IMPLICIT_MESHER_LAYER_3_H

#include <gui/gl_widget_3/GL_draw_layer_3.h>
#include <gui/app/static/Application_settings.h>

#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/Complex_2_in_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>

// default triangulation for Surface_mesher
typedef CGAL::Surface_mesh_default_triangulation_3 Tr;

// c2t3
typedef CGAL::Complex_2_in_triangulation_3<Tr> C2t3;

typedef Tr::Geom_traits GT;
typedef GT::Sphere_3 Sphere_3;
typedef GT::Point_3 Point_3;
typedef GT::Triangle_3 Triangle_3;
typedef GT::Segment_3 Segment_3;

typedef GT::FT Number_type;

template <class GT, class Union_of_balls_3>
class Union_surface {
public:
	typedef typename GT Geom_traits;
	typedef typename Geom_traits::Sphere_3 Sphere_3;
	typedef typename Geom_traits::FT FT;
	typedef typename Geom_traits::Point_3 Point;
	typedef typename Union_of_balls_3::Alpha_shape_3 Alpha_shape_3;
	typedef typename Alpha_shape_3::Vertex_handle Vertex_handle;
	typedef Union_surface<Geom_traits, Union_of_balls_3> Self;

	typedef CGAL::Surface_mesher::Implicit_surface_oracle_3<
		Geom_traits,
		Self> Surface_mesher_traits_3;

	Union_surface(Alpha_shape_3* alpha_s,
		const Sphere_3 bounding_sphere,
		const FT error_bound = FT(1e-3),
		Geom_traits gt = Geom_traits())
		: 
		sphere(bounding_sphere),
		gt(gt),
		alpha_shape(alpha_s)
	{
		squared_error = error_bound * error_bound;
		squared_error = squared_error * 
			gt.compute_squared_radius_3_object()(bounding_sphere);
	}

	FT operator()(Point p) const
	{
		Vertex_handle v = alpha_shape->nearest_power_vertex(p);
		FT sqd = squared_distance(v->point().point(), p);
		double d = sqrt(CGAL::to_double(sqd));
		return d - sqrt(CGAL::to_double(v->point().weight()));
		//const FT x2=p.x()*p.x(), y2=p.y()*p.y(), z2=p.z()*p.z();
		//return x2+y2+z2-1;
	}

	const FT& squared_error_bound() const
	{
		return squared_error;
	}

	const Sphere_3& bounding_sphere() const
	{
		return sphere;
	}

	const Sphere_3& bounding_sphere_squared_radius() const
	{
		return gt.compute_squared_radius_3_object()(sphere);
	}

	template <typename Vertex_handle>
	bool vertices_not_on_same_surface_patch(const Vertex_handle& v1,
		const Vertex_handle& v2,
		const Vertex_handle& v3) const
	{
		return
			v1->point().element_index() != v2->point().element_index() ||
			v1->point().element_index() != v3->point().element_index();
	}

	//const Function& function() const
	//{
	//	return func;
	//}

private:
	Alpha_shape_3* alpha_shape;
//	Function func;
	Sphere_3 sphere;
	FT squared_error;
	Geom_traits gt;
};


template <class Union_of_balls_3>
class Implicit_mesher_layer_3 : public GL_draw_layer_3 {
	typedef typename Union_of_balls_3::Alpha_shape_3 Alpha_shape_3;
	typedef typename Alpha_shape_3::Point Point_3;


	typedef Number_type (Implicit_mesher_layer_3<Union_of_balls_3>::*Function)(Point_3);

	typedef CGAL::Implicit_surface_3<GT, Function> Surface_3;

public:

	Implicit_mesher_layer_3(const QString& name, Union_of_balls_3* m, const QString& ttip, bool wireframe) : GL_draw_layer_3(name, ttip), parent(m) {
		do_wireframe = wireframe;
		Application_settings::add_double_setting("implicit-mesher-bound-angle",30);
		Application_settings::add_double_setting("implicit-mesher-bound-radius",0.1);
		Application_settings::add_double_setting("implicit-mesher-bound-distance",0.1);
	}


	//Number_type surface_function (Point_3 p) {
	//	Alpha_shape_3::Vertex_handle ball = alpha_shape->nearest_power_vertex(p);
	//	const FT x2=p.x()*p.x(), y2=p.y()*p.y(), z2=p.z()*p.z();
	//	return x2+y2+z2-1;
	//}

	//Number_type sphere_function (Point_3 p) {
	//	const Number_type x2=p.x()*p.x(), y2=p.y()*p.y(), z2=p.z()*p.z();
	//	return x2+y2+z2-1;
	//}

	virtual void draw_commands() {
		Alpha_shape_3* alpha_shape = parent->get_alpha_shape();
		if (alpha_shape->number_of_vertices() < 4) return;

		double angle = Application_settings::get_double_setting("implicit-mesher-bound-angle"); angle = angle>30 ? 30 : angle;
		double radius = Application_settings::get_double_setting("implicit-mesher-bound-radius");
		double distance = Application_settings::get_double_setting("implicit-mesher-bound-distance");

		std::cout << "Meshing implicit with angle: " << angle << " radius: " << radius << " distance: " << distance << std::endl;

		Tr tr;            // 3D-Delaunay triangulation
		C2t3 c2t3 (tr);   // 2D-complex in 3D-Delaunay triangulation

		std::cout << PROGRESS_STATUS << "Implicit meshing" << std::endl;
		// defining the surface
		Union_surface<GT, Union_of_balls_3> surface(alpha_shape, Sphere_3(CGAL::ORIGIN, 2.));
		//Surface_3 surface(surface_function, Sphere_3(CGAL::ORIGIN, 2.));

		// defining meshing criteria
		CGAL::Surface_mesh_default_criteria_3<Tr> criteria(angle, radius, distance);

		// meshing surface
		CGAL::make_surface_mesh(c2t3, surface, criteria, CGAL::Non_manifold_tag());
		std::cout << PROGRESS_DONE << std::endl;

		*this << DO_BOUNDING;
		if (do_wireframe) {
			*this << SEGMENTS_START;
			C2t3::Edge_iterator e_it, e_end = c2t3.edges_end();
			for (e_it=c2t3.edges_begin(); e_it!=e_end; ++e_it) {
				C2t3::Cell_handle c = e_it->first;
				int id1 = e_it->second;
				int id2 = e_it->third;
				*this << Segment_3(c->vertex(id1)->point(), c->vertex(id2)->point());
			}
			*this << SEGMENTS_END;
		} else {
			*this << TRIANGLES_START;
			C2t3::Facet_iterator f_it, f_end = c2t3.facets_end();
			for (f_it=c2t3.facets_begin(); f_it!=f_end; ++f_it) {
				C2t3::Cell_handle c = f_it->first;
				int id = f_it->second;
				*this << Triangle_3(c->vertex((id+1)%4)->point(), c->vertex((id+2)%4)->point(), c->vertex((id+3)%4)->point());
			}
			*this << TRIANGLES_END;
		}



	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case LINE_WIDTH_EDITABLE:
			return do_wireframe;
			break;
		default:
			return false;
			break;
		}
	}

	//virtual QString get_name_user_property(Layer_property l) { 
	//	if (l == SCALAR_EDITABLE) return "Id";
	//	else return "";
	//}

	virtual void application_settings_changed(const QString& settings_name) {

		if (settings_name=="implicit-mesher-bound-angle" || settings_name=="implicit-mesher-bound-radius" || settings_name=="implicit-mesher-bound-distance") {
			invalidate_cache();
			widget->request_repaint();
		}
	}

private:
	Union_of_balls_3 *parent;
	bool do_wireframe;
	M_classification classification;
};


#endif //MESECINA_IMPLICIT_MESHER_LAYER_3_H