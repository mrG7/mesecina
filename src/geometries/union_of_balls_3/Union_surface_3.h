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
class Union_surface_3 {
public:
	typedef typename GT Geom_traits;
	typedef typename Geom_traits::Sphere_3 Sphere_3;
	typedef typename Geom_traits::FT FT;
	typedef typename Geom_traits::Point_3 Point;
	typedef typename Union_of_balls_3::Alpha_shape_3 Alpha_shape_3;
	typedef typename Alpha_shape_3::Vertex_handle Vertex_handle;
	typedef Union_surface_3<Geom_traits, Union_of_balls_3> Self;

	typedef CGAL::Surface_mesher::Implicit_surface_oracle_3<
		Geom_traits,
		Self> Surface_mesher_traits_3;

	Union_surface_3(Alpha_shape_3* alpha_s,
//		const Sphere_3 bounding_sphere,
		const FT error_bound,
		Geom_traits gt = Geom_traits())
		: 
//		sphere(bounding_sphere),
		gt(gt),
		alpha_shape(alpha_s)
	{
		typedef typename Alpha_shape_3::Finite_vertices_iterator	Finite_vertices_iterator;

		Finite_vertices_iterator v_it, v_end = alpha_shape->finite_vertices_end();

		double current_radius; double minx, miny, minz, maxx, maxy, maxz;
		for (v_it=alpha_shape->finite_vertices_begin(); v_it!=v_end; ++v_it) {
			if (v_it == alpha_shape->finite_vertices_begin()) {
				sphere = Sphere_3(v_it->point().point(), v_it->point().weight());
				current_radius = sqrt(CGAL::to_double(sphere.squared_radius()));
				double x = v_it->point().x();
				double y = v_it->point().y();
				double z = v_it->point().z();
				minx = x - current_radius;
				maxx = x + current_radius;
				miny = y - current_radius;
				maxy = y + current_radius;
				minz = z - current_radius;
				maxz = z + current_radius;
			} else {
				double dist = sqrt(CGAL::to_double(CGAL::squared_distance(sphere.center(), v_it->point().point())));
				double new_radius_candidate = dist + sqrt(CGAL::to_double(v_it->point().weight()));
				if (new_radius_candidate > current_radius) {
					//if (sphere.squared_radius() > v_it->point().weight()) {
						sphere= Sphere_3(sphere.center(), new_radius_candidate*new_radius_candidate);
						current_radius = new_radius_candidate;
					//}
				}
				double r = sqrt(CGAL::to_double(sphere.squared_radius()));
				double x = v_it->point().x();
				double y = v_it->point().y();
				double z = v_it->point().z();
				if (x-r < minx)	minx = x - r;
				if (maxx < x+r)	maxx = x + r;
				if (y-r < miny)	miny = y - r;
				if (maxy < y+r)	maxy = y + r;
				if (z-r < minz)	minz = z - r;
				if (maxz < z-r)	maxz = z + r;
			}
		}
		double dx = maxx-minx, dy = maxy-miny, dz = maxz - minz;
		bounding_radius = sqrt(dx*dx + dy*dy + dz*dz) / 2.0;

		squared_error = error_bound * error_bound;
		squared_error = squared_error * bounding_radius;
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

	//const Sphere_3& bounding_sphere_squared_radius() const
	//{
	//	return gt.compute_squared_radius_3_object()(sphere);
	//}

	template <typename Vertex_handle>
	bool vertices_not_on_same_surface_patch(const Vertex_handle& v1,
		const Vertex_handle& v2,
		const Vertex_handle& v3) const
	{
		return
			v1->point().element_index() != v2->point().element_index() ||
			v1->point().element_index() != v3->point().element_index();
	}

	double bounding_radius;

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