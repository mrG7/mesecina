#include "Scale_analysis_2.h"
#include <constants.h>

#include "layers\Scale_analysis_balls_layer_2.h"


Scale_analysis_2::Scale_analysis_2() : Geometry() {
	has_ordered_balls = has_balls = has_points = false;

	GL_draw_layer_2* balls_center_layer = new Scale_analysis_balls_layer_2("SA balls center", this, "Balls used for scale analysis");
	add_layer(balls_center_layer);
	GL_draw_layer_2* circles_layer = new Scale_analysis_balls_layer_2("SA circles", this, "Balls used for scale analysis", true);
	add_layer(circles_layer);
	GL_draw_layer_2* disks_center_layer = new Scale_analysis_balls_layer_2("SA disks", this, "Balls used for scale analysis", true, true);
	add_layer(disks_center_layer);

	GL_draw_layer_2* scaled_circles_layer = new Scale_analysis_balls_layer_2("SA scaled circles", this, "Balls used for scale analysis, scaled as the application parameter says", true, false, true);
	add_layer(scaled_circles_layer);
	GL_draw_layer_2* scaled_disks_center_layer = new Scale_analysis_balls_layer_2("SA scaled disks", this, "Balls used for scale analysis, scaled as the application parameter says", true, true, true);
	add_layer(scaled_disks_center_layer);

}

Scale_analysis_2::~Scale_analysis_2() {
}

void Scale_analysis_2::receive_structure_changed(const std::string& name) {
	if (name == SHARED_POINTS) {
		invalidate_cache();
	}
}

Scale_analysis_2::Delaunay_with_scale* Scale_analysis_2::get_delaunay() {
	if (!has_points) {
		std::cout << LOG_GREEN << "Scale_analysis: get points from the power_crust" << std::endl;
		void* p = request_structure(SHARED_POINTS);
		if (!p) std::cout << LOG_ERROR << "Error input points from Power_crust_2, activate a power crust geometry!" << std::endl;
		else {
			std::list<Point_2>* ps = static_cast<std::list<Point_2>*>(p);
			delaunay.clear();
			std::list<Point_2>::iterator p_it, p_end = ps->end();
			for (p_it = ps->begin(); p_it != p_end; ++p_it) 
				delaunay.insert(*p_it);
			has_points = true;
		}
	}
	return &delaunay;
}

Scale_analysis_2::Delaunay_with_scale* Scale_analysis_2::get_sa_balls_delaunay() {
	if (!has_balls) {
		Delaunay_with_scale* t = get_delaunay();
		std::cout << PROGRESS_STATUS << "Identifying balls for the scale analysis" << std::endl;
		Finite_faces_iterator f_it, f_end = t->finite_faces_end();
		for (f_it = t->finite_faces_begin(); f_it!=f_end; f_it++) {
			Point_2 center = t->dual(f_it);
			if (t->is_infinite(t->locate(center)))
				f_it->info() = -1; // outside convex hull, we don't care
			else f_it->info() = 0; // this is a scale analysis ball
		}
		std::cout << PROGRESS_DONE << std::endl;
		has_balls = true;
	}
	return &delaunay;
}

Scale_analysis_2::Delaunay_with_scale* Scale_analysis_2::get_sa_ordered_balls_delaunay() {
	if (!has_ordered_balls) {
		Delaunay_with_scale* t = get_sa_balls_delaunay();
		std::cout << PROGRESS_STATUS << "Ordering balls for the scale analysis ... ugly n squared" << std::endl;

		Finite_faces_iterator f_it, f_end = t->finite_faces_end();
		for (Finite_faces_iterator f_it = t->finite_faces_begin(), f_end = t->finite_faces_end();	f_it != f_end; ++f_it) {
			if (f_it->info() >= 0) {
				Point_2 c1 = t->dual(f_it);
				double r1_sq = CGAL::to_double(squared_distance(f_it->vertex(0)->point(), c1));
				f_it->info() = 100000;
				for (Finite_faces_iterator f2_it = t->finite_faces_begin(), f2_end = t->finite_faces_end(); f2_it != f2_end; ++f2_it) {
					if (f2_it->info() >= 0) {
						Point_2 c2 = t->dual(f2_it);
						double r2_sq = CGAL::to_double(squared_distance(f2_it->vertex(0)->point(), c2));

						if (r2_sq > r1_sq) {
							double d = sqrt(CGAL::to_double(squared_distance(c1,c2)));
							double s = d / (sqrt(r2_sq) - sqrt(r1_sq));
							//std::cout << "circumcenter 1: " << c1 << " radius 1 " << sqrt(r1_sq) << std::endl;
							//std::cout << "circumcenter 3: " << c2 << " radius 2 " << sqrt(r2_sq) << std::endl;
							//std::cout << "d: " << d << std::endl;
							//std::cout << LOG_BLUE << "s: " << s << std::endl;


							if (s < f_it->info()) {
								//						std::cout << " updating scale value of a ball" << std::endl;
								f_it->info() = s;
							}
						}
					}
				}
			}
		}

		std::cout << PROGRESS_DONE << std::endl;
		has_ordered_balls = true;
	}
	return &delaunay;
}

//void Scale_analysis_2::add_point(double x, double y) {
//	std::cout << "Scale_analysis_2: point added " << x << " " << x << std::endl;
//	input_triangulation.insert(Point_2(x,y));
//	invalidate_cache();
//}
//
//void Scale_analysis_2::add_points(std::list<QPointF>* points) {
//	std::cout << "Scale_analysis_2: " << points->size() << " points added " << std::endl;
//	std::cout << PROGRESS_STATUS << "Building delaunay triangulation" << std::endl;
//	std::list<QPointF>::iterator p_it, p_end = points->end(); int i=0;
//	for (p_it = points->begin(); p_it != p_end; p_it++, i++) {
//		input_triangulation.insert(Point_2(p_it->x(), p_it->y()));
//	}
//	std::cout << PROGRESS_DONE << std::endl;
//	invalidate_cache();
//}
//std::list<QPointF>* Scale_analysis_2::get_points() {
//	return 0;
//}

void Scale_analysis_2::invalidate_cache() {
	invalidate_all_layers();
	has_ordered_balls = has_balls = has_points = false;
}