#include <utility>
#include <fstream>
#include <queue>
#include <constants.h>

#include "Medial_axis_structure_3.h"

#include <gui/app/static/Application_settings.h>

using namespace medial;

#define NORMAL_COMP_SQUARED_EDGE_LENGTH 0.0000001
#define PI 3.14159265358979323846

/// Vertex functions

Vertex::Vertex(const Point_3& _p, double _radius_sq) : p(_p), radius_sq(_radius_sq) {	}

//Point_3& Vertex::point() { return p; }
//void Vertex::set_point(const Point_3& mp) { p=mp; }

double Vertex::radius() { return std::sqrt(radius_sq); }
//double Vertex::squared_radius() { return radius_sq; }
//void Vertex::set_squared_radius(double rsq) { radius_sq = rsq; }

/// Face functions

Face::Face(const Vertex_pointer_vector& _vertices) {
	vertices = _vertices;
	sheet_index = -1;
}
//Vector_3 Face::get_normal() { return normal; }
//void Face::set_normal(const Vector_3& v) { normal = v; }
void Face::compute_normal() {
	Point_3 p0 = vertices[0]->p;
	Point_3 p1 = vertices[1]->p;
	Point_3 p2 = vertices[2]->p;
	unsigned i = 3;
	//Vector_3 v10 = p1-p0;
	//Vector_3 v21 = p2-p1;
	normal = cross_product(p1-p0, p2-p1);
	double nl = normal.squared_length();
	if (nl > NORMAL_COMP_SQUARED_EDGE_LENGTH) i= vertices.size()+6;
	//std::cout << "p0: " << p0 << std::endl;
	//std::cout << "p1: " << p1 << std::endl;
	//std::cout << "p2: " << p2 << std::endl;
//	std::cout << "normal " << i-1 << ": " << normal << std::endl;
	while ( i<vertices.size()+1 /*&& 
			normal.squared_length() < NORMAL_COMP_SQUARED_EDGE_LENGTH*/)
/*		   (v10.squared_length() < NORMAL_COMP_SQUARED_EDGE_LENGTH || 
		    v21.squared_length() < NORMAL_COMP_SQUARED_EDGE_LENGTH)
			)*/ {
				p0 = p1;
				p1 = p2;
				p2 = vertices[i%vertices.size()]->p;
				Vector_3 new_normal = cross_product(p1-p0, p2-p1);
				double nnl = new_normal.squared_length();
//				std::cout << "normal " << i <<": " << new_normal << std::endl;
				if (nnl > nl) {
					normal = new_normal;
					nl = nnl;
					if (nl > NORMAL_COMP_SQUARED_EDGE_LENGTH) i= vertices.size()+6;
				}
				//std::cout << "p0: " << p0 << std::endl;
				//std::cout << "p1: " << p1 << std::endl;
				//std::cout << "p2: " << p2 << std::endl;

				i++;
	}
	//if (normal.squared_length() < NORMAL_COMP_SQUARED_EDGE_LENGTH) std::cout << LOG_WARNING << "We have so degenerate face we couldn't reliably compute face normal... vertices:" << vertices.size() << std::endl;
//	std::cout << "final normal before normalizing: " << normal << std::endl;
	normal = normal / std::sqrt(normal.squared_length());
}
//Face_vertex_iterator Face::vertices_begin() {
//	return vertices.begin();
//}
//Face_vertex_iterator Face::vertices_end() {
//	return vertices.end();
//}

//unsigned Face::get_sheet_index() {	
//	if (sheet_index == -1) std::cout << LOG_ERROR << "Uninitialized sheet_index" << std::endl;
//	return sheet_index;
//}
//void Face::set_sheet_index(unsigned v) {
//	sheet_index = v;
//}


/// Edge functions

Edge medial::make_edge(Vertex *v1, Vertex *v2) {
	if (v1 > v2) return std::make_pair(v2,v1);
	else return std::make_pair(v1,v2);
}


/// Medial_axis_transform functions
Medial_axis_transform_3::Medial_axis_transform_3() {

}

bool Medial_axis_transform_3::read_from_moff(std::istream& infile) {
	vertices.clear();
	faces.clear();
	triangles.clear();
	
	edges.clear();

	std::cout << PROGRESS_STATUS << "Read MOFF vertices" << std::endl;

	std::string header_string;
	infile >> header_string;
	if (header_string!="MOFF") {
		std::cout << LOG_ERROR << "File header is wrong, MOFF expected and got " << header_string << std::endl;
		return false;
	}
	std::cout << "MOFF READER: " << header_string << std::endl;
	int n_vertices, n_faces;

	infile >> n_vertices;
	vertices.reserve(n_vertices);
	infile >> n_faces;
	faces.reserve(n_faces);
	infile >> growth_ratio;
	std::cout << "MOFF READER: vertices: " << n_vertices << " faces: " << n_faces << " a growth ratio: " << growth_ratio << std::endl;


	double x,y,z, radius;  double minx, miny, minz, maxx, maxy, maxz;
	for (int i=0; i<n_vertices; i++) {
		infile >> x;
		infile >> y;
		infile >> z;
		infile >> radius;
		if (i==0) {
			minx = maxx = x;
			miny = maxy = y;
			minz = maxz = z;
		} else {
			if (x < minx) minx = x; if (x > maxx) maxx =x;
			if (y < miny) miny = y; if (y > maxy) maxy =y;
			if (z < minz) minz = z; if (z > maxz) maxz =z;
		}
		vertices.push_back(Vertex(Point_3(x,y,z),radius));
	}
	std::cout << "Read " << vertices.size() << " vertices" << std::endl;
	std::cout << "Bounding box: (" << minx << "," << miny << "," << minz << ") - (" << maxx << "," << maxy << "," << maxz << ")" << std::endl;
	double dx = maxx-minx, dy = maxy-miny, dz = maxz - minz;
	bounding_diagonal_radius = sqrt(dx*dx + dy*dy + dz*dz) / 2.0;
//	std::cout << "bounding_diagonal_radius " << bounding_diagonal_radius << std::endl;
	std::cout << PROGRESS_DONE << std::endl;


	std::cout << PROGRESS_STATUS << "Read MOFF faces" << std::endl;

	int v, index; double angle, lambda, px, py, pz;
	for (int i=0; i<n_faces; i++) {
		infile >> v;
		Vertex_pointer_vector vv;
		for (int j=0; j<v; j++) {
			infile >> index;
			vv.push_back(&vertices[index]);
		}
		infile >> angle;
		infile >> lambda;
		infile >> px;
		infile >> py;
		infile >> pz;
		faces.push_back(Face(vv));
		faces.back().angle = angle;
		faces.back().lambda = lambda;
		faces.back().intersection_point = Point_3(px,py,pz);
		Face_vertex_iterator fv_it, fv_next, fv_end = faces.back().vertices.end();
		fv_next = fv_it = faces.back().vertices.begin();
		fv_next++;
		for (; fv_next!=fv_end; ++fv_it, ++fv_next) {
			Vertex* v1 = *fv_it;
			Vertex* v2 = *fv_next;
			Edge e = make_edge(v1,v2);
//			if (edges.find(e) != edges.end()) std::cout << "edge already found" << std::endl;
			edges[e].insert(&faces.back());
			vertices_map[v1].insert(&faces.back());
		}
		// close the loop with the end -> begin
		Edge e = make_edge(*fv_it,*(faces.back().vertices.begin()));
		edges[e].insert(&faces.back());
		vertices_map[*fv_it].insert(&faces.back());

	}
	std::cout << PROGRESS_DONE << std::endl;


	std::cout << PROGRESS_STATUS << "Triangulate MOFF faces" << std::endl;

	triangle_source.clear();
	std::vector<Face*> sources;
	// triangulate the medial surface
	Face_iterator f_it, f_end = faces_end();
	for (f_it = faces_begin(); f_it!=f_end; f_it++) {
		Face* f = &(*f_it);
		// star triangulation
		for (int i=2; i<f_it->vertices.size(); i++) {
			Triangle t;
			t.p[0] = f_it->vertices[0];
			t.p[1] = f_it->vertices[i-1];
			t.p[2] = f_it->vertices[i];
			t.angle = f_it->angle;
			t.lambda = f_it->lambda;
			t.intersection_point = f_it->intersection_point;
			triangles.push_back(t);
			sources.push_back(f);
		}
	}
	std::cout << PROGRESS_DONE << std::endl;


	std::cout << PROGRESS_STATUS << "Set triangles source polygons" << std::endl;
	// map the sources to the triangles (now that the triangle locations are fixed)
	std::vector<Triangle>::iterator t_it, t_end = triangles.end();
	std::vector<Face*>::iterator sf_it = sources.begin();
	for (t_it = triangles.begin(); t_it!=t_end; t_it++, sf_it++) {
		Triangle* t = &(*t_it);
		Face* sf = *sf_it;
		triangle_source[t] = sf;
	}
	std::cout << PROGRESS_DONE << std::endl;


	std::cout << "triangle_source size: " << triangle_source.size() << std::endl;


	//Delaunay_for_unique::Finite_vertices_iterator v_it, v_end = scale_axis_balls_delaunay.finite_vertices_end();
	//for (v_it = scale_axis_balls_delaunay.finite_vertices_begin(); v_it!=v_end; ++v_it) {
	//	if (v_it->info().second.size() > 1) {
	//		std::cout << "More sources " << v_it->info().second.size() << std::endl;
	//	}
	//}

	std::cout << "Read " << faces.size() << " faces" << std::endl;
	std::cout << "Detected " << edges.size() << " face edges" << std::endl;
	std::cout << "Triangulated into " << triangles.size() << " triangles" << std::endl;
//	std::cout << "Generated " << scale_axis_balls.size() << " scale axis balls" << std::endl;


	return true;

}

void Medial_axis_transform_3::sample_balls() {
	std::cout << PROGRESS_STATUS << "Sample medial axis with balls" << std::endl;

	// sample triangles with balls
	double max_squared_distance = Application_settings::get_double_setting("scale-axis-balls-center-max-distance")*bounding_diagonal_radius;
	//std::cout << "Application_settings::get_double_setting(scale-axis-balls-center-max-distance) " << Application_settings::get_double_setting("scale-axis-balls-center-max-distance") << std::endl;
	//std::cout << "bounding_diagonal_radius " << bounding_diagonal_radius << std::endl;
	std::cout << "max_squared_distance: " << max_squared_distance << std::endl;
	double small_angle = 180.0 - Application_settings::get_double_setting("scale-axis-balls-min-intersection-depth");
	small_angle = small_angle < 0 ? 0.0 : small_angle;
	double medial_ball_intersection_cosinus = cos(small_angle * PI / 180.0);

	scale_axis_balls_delaunay.clear();
	scale_axis_balls.clear();
	std::cout << "growth ratio: " << growth_ratio << std::endl;
	std::vector<Triangle>::iterator t_it, t_end = triangles.end();
	for (t_it = triangles.begin(); t_it!=t_end; ++t_it) {
		Point_3 p1 = t_it->p[0]->p;
		Point_3 p2 = t_it->p[1]->p;
		Point_3 p3 = t_it->p[2]->p;
		Point_3 bp = t_it->intersection_point;

		double r1 = sqrt(squared_distance(p1,bp))/growth_ratio;
		double r2 = sqrt(squared_distance(p2,bp))/growth_ratio;
		double r3 = sqrt(squared_distance(p3,bp))/growth_ratio;
//		std::cout << "r1: " << r1 << std::endl;

		Triangle* source_triangle = &(*t_it);

		add_vertex_to_unique_del(p1, r1, max_squared_distance, source_triangle);
		add_vertex_to_unique_del(p2, r2, max_squared_distance, source_triangle);
		add_vertex_to_unique_del(p3, r3, max_squared_distance, source_triangle);

		sample_triangle_with_balls(p1, r1, p2, r2, p3, r3,
			bp, max_squared_distance, medial_ball_intersection_cosinus, source_triangle);
	}
	std::cout << PROGRESS_DONE << std::endl;
}

void Medial_axis_transform_3::fill_balls_with_criteria(Filtering_citeria crit, double limit, std::vector<Ball>& balls) {
	Delaunay_for_unique::Finite_vertices_iterator v_it, v_end = scale_axis_balls_delaunay.finite_vertices_end();
	for (v_it = scale_axis_balls_delaunay.finite_vertices_begin(); v_it!=v_end; ++v_it) {
		std::set<Triangle*>& source_faces = v_it->info().second;
		std::set<Triangle*>::iterator sf_it, sf_end = source_faces.end(); bool cancome = false;
		for (sf_it=source_faces.begin(); sf_it!=sf_end && !cancome; sf_it++) {
			if (does_triangle_fulfill_criteria(*sf_it,crit,limit))
				cancome = true;
		}
		if (cancome) {
//			std::cout << "radius: " << v_it->info().first << std::endl;
			balls.push_back(std::make_pair(v_it->point(),v_it->info().first));
		}
	}
}

bool Medial_axis_transform_3::does_triangle_fulfill_criteria(Triangle* triangle, Filtering_citeria crit, double limit) {
	Face* source_face = triangle_source[triangle];
	switch (crit) {
		case CRIT_LAMBDA:
			if (source_face->lambda > limit*bounding_diagonal_radius) return true;
			return false;
			break;
		case CRIT_ANGLE:
			if (source_face->angle > limit) return true;
			return false;
			break;
		case CRIT_SHEET_TOPANGLE:
			int sheet_index = source_face->sheet_index;
			if (sheet_top_angle[sheet_index] > limit) return true;
			else return false;
			break;
	}
	return false;
}


void Medial_axis_transform_3::add_vertex_to_unique_del(Point_3 p, double r, double max_squared_distance, Triangle* source_face) {
	Unique_vertex_handle v = scale_axis_balls_delaunay.nearest_vertex(p);
	if (v == Unique_vertex_handle() || CGAL::squared_distance(v->point(),p) > max_squared_distance) {
		scale_axis_balls.push_back(std::make_pair(p,r));
		Unique_vertex_handle uv;
		if (v==Unique_vertex_handle()) uv = scale_axis_balls_delaunay.insert(p);
		else uv = scale_axis_balls_delaunay.insert(p, v->cell());
		std::set<Triangle*> fs; fs.insert(source_face);
		uv->info() = make_pair(r, fs);
	} else {
		v->info().second.insert(source_face);
	}
}

void Medial_axis_transform_3::sample_triangle_with_balls(Point_3& p1, double r1, Point_3& p2, double r2, Point_3& p3, double r3, Point_3& bp, double max_squared_distance, double inters_cos, Triangle* source_face) {
	double r1_sq, r2_sq, r3_sq;
	r1_sq = r1*r1;
	r2_sq = r2*r2;
	r3_sq = r3*r3;

	double d12_sq = CGAL::squared_distance(p1,p2);
	double d23_sq = CGAL::squared_distance(p2,p3);
	double d31_sq = CGAL::squared_distance(p3,p1);

	double cos_alpha_12 = (r1_sq + r2_sq - d12_sq)/2.0/r1/r2;
	double cos_alpha_23 = (r2_sq + r3_sq - d23_sq)/2.0/r2/r3;
	double cos_alpha_31 = (r3_sq + r1_sq - d31_sq)/2.0/r3/r1;

	if (cos_alpha_12 < inters_cos || cos_alpha_23 < inters_cos || cos_alpha_31 < inters_cos) {
		Point_3 p12 = CGAL::ORIGIN + (((p1-CGAL::ORIGIN) + (p2-CGAL::ORIGIN)) / 2.0);
		double r12 = sqrt(squared_distance(p12,bp))/growth_ratio;
		Point_3 p23 = CGAL::ORIGIN + (((p2-CGAL::ORIGIN) + (p3-CGAL::ORIGIN)) / 2.0);
		double r23 = sqrt(squared_distance(p23,bp))/growth_ratio;
		Point_3 p31 = CGAL::ORIGIN + (((p3-CGAL::ORIGIN) + (p1-CGAL::ORIGIN)) / 2.0);
		double r31 = sqrt(squared_distance(p31,bp))/growth_ratio;

		add_vertex_to_unique_del(p12, r12, max_squared_distance, source_face);
		add_vertex_to_unique_del(p23, r23, max_squared_distance, source_face);
		add_vertex_to_unique_del(p31, r31, max_squared_distance, source_face);

		sample_triangle_with_balls( p1,  r1, p12, r12, p31, r31, bp, max_squared_distance, inters_cos, source_face);
		sample_triangle_with_balls(p12, r12, p31, r31, p23, r23, bp, max_squared_distance, inters_cos, source_face);
		sample_triangle_with_balls(p12, r12,  p2,  r2, p23, r23, bp, max_squared_distance, inters_cos, source_face);
		sample_triangle_with_balls(p31, r31, p23, r23,  p3,  r3, bp, max_squared_distance, inters_cos, source_face);
	}


}

void Medial_axis_transform_3::compute_vertex_normals() {
	vertex_normals.clear();

	Vertex_iterator v_it, v_end = vertices.end(); int i=0;
	for (v_it = vertices.begin(); v_it != v_end; ++v_it, ++i) {
		Face_pointer_set fset = vertices_map[&(*v_it)];
		Neighbor_face_iterator f_it, f_end = fset.end();
		Vector_3 normal(0,0,0);
		for (f_it=fset.begin(); f_it != f_end; f_it++) {
			Vector_3 fn = (*f_it)->normal;
			if (fn*normal < 0) {
				//std::cout << "Flipping normal" << std::endl;
				fn = fn * -1;
			}
			normal = normal + fn;
		}
		normal = normal / std::sqrt(normal.squared_length());
		normal = Vector_3(1,0,0);
		vertex_normals.push_back(normal);
	}

}

void Medial_axis_transform_3::add_to_boundary_map(Vertex* source, Vertex* target, Point_3 bp, Boundary_vertex_map& bond_vertex, bool force_out) {
	Point_3 marker = Point_3(0,0,0);
	if (force_out) marker = Point_3(-1,-1,-1);

	if (bond_vertex.find(source) != bond_vertex.end()) {
		// it has already an entry
		Boundary_neighbor_info bni = bond_vertex[source].first;
		if (bni.second == (Vertex*)(0)) {
			// this is the second vertex to add
			bni.second = target;
			bond_vertex[source] = std::make_pair(bni, force_out ? marker : bond_vertex[source].second);
		} else {
			// it has more than two end vertices, don't know how to smooth these, mark with -1,-1,-1 for new point position, 
			// we will remove them later
			bond_vertex[source] = std::make_pair(bni, Point_3(-1,-1,-1));
		}

	} else {
		// has no entry, create one
		Boundary_neighbor_info bni(target,(Vertex*)(0),bp);
		bond_vertex[source] = std::make_pair(bni, marker);
	}
}

void Medial_axis_transform_3::smooth_medial_surface_boundary(int steps, std::vector<double>* sheet_stability, double limit) {
	if (steps<1) return;

	Boundary_vertex_map bond_vertex;
//	std::cout << "limit: " << limit << std::endl;


	Edge_iterator e_it, e_end = edges_end();
	for (e_it = edges_begin(); e_it != e_end; e_it++) {
		bool is_on_boundary = (sheet_stability==0 && e_it->second.size() == 1);
		bool fix = false;
		if (sheet_stability!=0) {
			Face_pointer_set nf = e_it->second;
			Neighbor_face_iterator nf_it, nf_end = nf.end(); int neighbor_count=0;
			for (nf_it = nf.begin(); nf_it!=nf_end; ++nf_it) {
				int sheet_index = (*nf_it)->sheet_index;
				if ((*sheet_stability)[sheet_index] > limit) neighbor_count++;
			}
			if (neighbor_count==1) is_on_boundary = true;
			else is_on_boundary = false;
			if (neighbor_count>2) fix = true;
		}
		if (is_on_boundary || fix){ // we are on boundary of face of we need to exclude it
			Edge e = e_it->first;
			Vertex* v1 = e.first;
			Vertex* v2 = e.second;
			Face* f = *(e_it->second.begin());
			Point_3 bp = f->intersection_point;
			add_to_boundary_map(v1, v2, bp, bond_vertex, fix);
			add_to_boundary_map(v2, v1, bp, bond_vertex, fix);
		}
	}

	// collect marked inputs (with more than two boundary edges)
	std::list<Boundary_vertex_map::iterator> to_delete;
	Boundary_vertex_map::iterator bvm_it, bvm_end = bond_vertex.end();
	for (bvm_it = bond_vertex.begin(); bvm_it!=bvm_end; ++bvm_it) {
		if (bvm_it->second.second == Point_3(-1,-1,-1) || bvm_it->second.first.second == (Vertex*)(0))
			to_delete.push_back(bvm_it);
		else {
			Vertex* v = bvm_it->first;
			Boundary_neighbor_info bni = bvm_it->second.first;
			Vertex* n1 = bni.first;
			Vertex* n2 = bni.second;
			double distance_ratio_sq = CGAL::squared_distance(n1->p,v->p)/CGAL::squared_distance(n2->p,v->p);
			double ratio = Application_settings::get_double_setting("boundary-smoothing-neighbor-edge-max-squared-ratio");
			if (distance_ratio_sq > ratio || distance_ratio_sq < 1.0/ratio)
				to_delete.push_back(bvm_it);
		}
	}

	std::cout << "Boundary vertices before removing more then 2 boundary edge vertices: " << bond_vertex.size() << std::endl;

	// delete marked inputs
	std::list<Boundary_vertex_map::iterator>::iterator d_it, d_end = to_delete.end();
	for (d_it=to_delete.begin(); d_it!=d_end; d_it++) {
		bond_vertex.erase(*d_it);
	}

	std::cout << "Size of movable boundary vertices: " << bond_vertex.size() << std::endl;



	for (int i=0; i<steps; i++) {

		// go through the boundary vertex info and compute new location halfway between
		for (bvm_it = bond_vertex.begin(); bvm_it!=bvm_end; ++bvm_it) {
			Vertex* v = bvm_it->first;
			Boundary_neighbor_info bni = bvm_it->second.first;
			Vertex* n1 = bni.first;
			Vertex* n2 = bni.second;
			Point_3 bp = bni.third;
			Point_3 proj = Line_3(n1->p, n2->p).projection(v->p);
			Point_3 midpoint = CGAL::ORIGIN + (((n1->p-CGAL::ORIGIN) + (n2->p-CGAL::ORIGIN)) / 2.0);
			Point_3 new_point = CGAL::ORIGIN + (((midpoint-CGAL::ORIGIN) + (v->p-CGAL::ORIGIN)) / 2.0);
			bvm_it->second.second = new_point;
		}

		// go through and update the location
		for (bvm_it = bond_vertex.begin(); bvm_it!=bvm_end; ++bvm_it) {
			Vertex* v = bvm_it->first;
			Boundary_neighbor_info bni = bvm_it->second.first;
			v->p = bvm_it->second.second;
		}
	}

}

bool Medial_axis_transform_3::write_to_obj(const char* file_name, std::vector<double>* sheet_stability, double limit) {
	std::ofstream myfile(file_name);
	if (!myfile.is_open()) {
		std::cout << LOG_ERROR << "Could not open file to write off file " << file_name << std::endl;
		return false;
	}

	if (sheet_stability==0) {

		size_t number_of_points = vertices.size();
		size_t number_of_triangles = triangles.size();

		// compute vertex normals
		compute_vertex_normals();
		// collect needed normals

		// header comments
		myfile << "#" << std::endl;
		myfile << "# OBJ file generated by Mesecina 3D " << std::endl;
		myfile << "#" << std::endl;
		myfile << "# vertices: " << number_of_points << std::endl;
		myfile << "# faces: " << number_of_triangles << std::endl;
		myfile << "#" << std::endl;

		//vertices
		std::map<Vertex*,int> vertex_to_index;
		Vertex_iterator v_it, v_end = vertices.end(); int i=0;
		for (v_it = vertices.begin(); v_it != v_end; ++v_it, ++i) {
			vertex_to_index[&(*v_it)] = i+1;
			myfile << "v " << v_it->p.x() << " " << v_it->p.y() << " " <<v_it->p.z() << std::endl;
		}
		std::cout << number_of_points << " vertices written" << std::endl;

		//normals
		std::vector<Vector_3>::iterator n_it, n_end = vertex_normals.end();
		for (n_it = vertex_normals.begin(); n_it!=n_end; ++n_it) {
			myfile << "vn " << n_it->x() << " " << n_it->y() << " " << n_it->z() << std::endl;
		}

		//faces
		std::vector<Triangle>::iterator f_it, f_end = triangles.end();
		for (f_it = triangles.begin(); f_it!=f_end; f_it++) {
			myfile << "f " << vertex_to_index[f_it->p[0]] << "//" << vertex_to_index[f_it->p[0]] << " " << 
				vertex_to_index[f_it->p[1]] << "//" << vertex_to_index[f_it->p[1]] << " " << 
				vertex_to_index[f_it->p[2]] << "//" << vertex_to_index[f_it->p[2]] << std::endl;
		}
		std::cout << number_of_triangles << " triangles written" << std::endl;
	} else {

		std::set<Vertex*> unique_vertices;
		std::vector<Vertex*> needed_vertices;
		std::map<Vertex*, int> vertex_to_index;
		
		//go through faces to collect needed vertices
		std::vector<Triangle>::iterator f_it, f_end = triangles.end(); int all_triangle_count = 0; int needed_triangle_count = 0;
		for (f_it = triangles.begin(); f_it!=f_end; f_it++, all_triangle_count++) {
			Face* source_face = triangle_source[&(*f_it)];
			int sheet_index = source_face->sheet_index;
			if ((*sheet_stability)[sheet_index] > limit) {
				needed_triangle_count++;
				for (int i=0; i<3; i++) {
					Vertex* v = f_it->p[i];
					if (unique_vertices.find(v) == unique_vertices.end()) {
						unique_vertices.insert(v);
						vertex_to_index[v] = needed_vertices.size()+1;
						needed_vertices.push_back(v);
					}
				}
			}
		}


				// header comments
		myfile << "#" << std::endl;
		myfile << "# OBJ file generated by Mesecina 3D " << std::endl;
		myfile << "#" << std::endl;
		myfile << "# vertices: " << needed_vertices.size() << std::endl;
		myfile << "# faces: " << needed_triangle_count << std::endl;
		myfile << "#" << std::endl;

		//vertices
		std::vector<Vertex*>::iterator v_it, v_end = needed_vertices.end();
		for (v_it = needed_vertices.begin(); v_it != v_end; ++v_it) {
			myfile << "v " << (*v_it)->p.x() << " " << (*v_it)->p.y() << " " <<(*v_it)->p.z() << std::endl;
		}
		std::cout << needed_vertices.size() << " vertices written" << std::endl;

		//faces
		all_triangle_count = 0;
		for (f_it = triangles.begin(); f_it!=f_end; f_it++, all_triangle_count++) {
			Face* source_face = triangle_source[&(*f_it)];
			int sheet_index = source_face->sheet_index;
			if ((*sheet_stability)[sheet_index] > limit) {

				myfile << "f " << vertex_to_index[f_it->p[0]] << " " << 
					vertex_to_index[f_it->p[1]] << " " << 
					vertex_to_index[f_it->p[2]] << std::endl;
			}
		}
		std::cout << needed_triangle_count << " triangles written" << std::endl;



	}

	myfile.close();
	return true;
}



bool Medial_axis_transform_3::write_to_off(const char* file_name, std::vector<double>* sheet_stability, double limit) {
	std::ofstream myfile(file_name);
	if (!myfile.is_open()) {
		std::cout << LOG_ERROR << "Could not open file to write off file " << file_name << std::endl;
		return false;
	}

	if (sheet_stability==0) {

		size_t number_of_points = vertices.size();
		size_t number_of_triangles = triangles.size();

		// header
		myfile << "OFF " << number_of_points << " " << number_of_triangles << " 0" << std::endl;

		//vertices
		std::map<Vertex*,int> vertex_to_index;
		Vertex_iterator v_it, v_end = vertices.end(); int i=0;
		for (v_it = vertices.begin(); v_it != v_end; ++v_it, ++i) {
			vertex_to_index[&(*v_it)] = i;
			myfile << v_it->p.x() << " " << v_it->p.y() << " " <<v_it->p.z() << std::endl;
		}
		std::cout << number_of_points << " vertices written" << std::endl;

		//faces
		std::vector<Triangle>::iterator f_it, f_end = triangles.end();
		for (f_it = triangles.begin(); f_it!=f_end; f_it++) {
			Face* source_face = triangle_source[&(*f_it)];
			if (face_flip.find(source_face) != face_flip.end() && face_flip[source_face] == true) {
//				std::cout << "flipped written" << std::endl;
				myfile << "3  " << vertex_to_index[f_it->p[0]] << " " << 
					vertex_to_index[f_it->p[2]] << " " << 
					vertex_to_index[f_it->p[1]] << std::endl;
			} else {
				myfile << "3  " << vertex_to_index[f_it->p[0]] << " " << 
					vertex_to_index[f_it->p[1]] << " " << 
					vertex_to_index[f_it->p[2]] << std::endl;
			}

		}
		std::cout << "face_flip.size() " << face_flip.size() << std::endl;
		std::cout << number_of_triangles << " triangles written" << std::endl;
	} else {

		std::set<Vertex*> unique_vertices;
		std::vector<Vertex*> needed_vertices;
		std::map<Vertex*, int> vertex_to_index;
		
		//go through faces to collect needed vertices
		std::vector<Triangle>::iterator f_it, f_end = triangles.end(); int all_triangle_count = 0; int needed_triangle_count = 0;
		for (f_it = triangles.begin(); f_it!=f_end; f_it++, all_triangle_count++) {
			Face* source_face = triangle_source[&(*f_it)];
			int sheet_index = source_face->sheet_index;
			if ((*sheet_stability)[sheet_index] > limit) {
				needed_triangle_count++;
				for (int i=0; i<3; i++) {
					Vertex* v = f_it->p[i];
					if (unique_vertices.find(v) == unique_vertices.end()) {
						unique_vertices.insert(v);
						vertex_to_index[v] = needed_vertices.size();
						needed_vertices.push_back(v);
					}
				}
			}
		}

		// header
		myfile << "OFF " << needed_vertices.size() << " " << needed_triangle_count << " 0" << std::endl;

		//vertices
		std::vector<Vertex*>::iterator v_it, v_end = needed_vertices.end();
		for (v_it = needed_vertices.begin(); v_it != v_end; ++v_it) {
			myfile << (*v_it)->p.x() << " " << (*v_it)->p.y() << " " <<(*v_it)->p.z() << std::endl;
		}
		std::cout << needed_vertices.size() << " vertices written" << std::endl;

		//faces
		all_triangle_count = 0;
		for (f_it = triangles.begin(); f_it!=f_end; f_it++, all_triangle_count++) {
			Face* source_face = triangle_source[&(*f_it)];
			int sheet_index = source_face->sheet_index;
			if ((*sheet_stability)[sheet_index] > limit) {

				if (face_flip.find(source_face) != face_flip.end() && face_flip[source_face] == true) {
//					std::cout << "flipped written" << std::endl;
					myfile << "3  " << vertex_to_index[f_it->p[0]] << " " << 
						vertex_to_index[f_it->p[2]] << " " << 
						vertex_to_index[f_it->p[1]] << std::endl;
				} else {
//					std::cout << "not flipped written" << std::endl;
					myfile << "3  " << vertex_to_index[f_it->p[0]] << " " << 
						vertex_to_index[f_it->p[1]] << " " << 
						vertex_to_index[f_it->p[2]] << std::endl;
				}
			}
		}
		std::cout << "face_flip.size() " << face_flip.size() << std::endl;
		std::cout << needed_triangle_count << " triangles written" << std::endl;



	}

	myfile.close();
	return true;
}


bool Medial_axis_transform_3::read_from_moff(const char* file_name) {
	std::cout << PROGRESS_STATUS << "Parsing MOFF file to construct mat structure" << std::endl;
	std::ifstream infile (file_name);
	if (!infile.is_open()) {
		std::cout << LOG_ERROR << "Could not open file" << std::endl;
		return false;
	}

	if (read_from_moff(infile)) {
		infile.close();
		return true;
	} else {
		infile.close();
		return false;
	}
}


bool Medial_axis_transform_3::read_from_off(std::istream& infile) {
	vertices.clear();
	faces.clear();
	triangles.clear();
	edges.clear();

	std::string header_string;
	infile >> header_string;
	if (header_string!="OFF") {
		std::cout << LOG_ERROR << "File header is wrong, OFF expected and got " << header_string << std::endl;
		return false;
	}
	int n_vertices, n_faces;

	infile >> n_vertices;
	vertices.reserve(n_vertices);
	infile >> n_faces;
	faces.reserve(n_faces);
	int dummy; infile >> dummy;

	double x,y,z;
	for (int i=0; i<n_vertices; i++) {
		infile >> x;
		infile >> y;
		infile >> z;
		vertices.push_back(Vertex(Point_3(x,y,z),0));
	}
	std::cout << "Read " << vertices.size() << " vertices" << std::endl;

	int v, index; 
	for (int i=0; i<n_faces; i++) {
		infile >> v;
		Vertex_pointer_vector vv;
		for (int j=0; j<v; j++) {
			infile >> index;
			vv.push_back(&vertices[index]);
		}
		faces.push_back(Face(vv));
		Face_vertex_iterator fv_it, fv_next, fv_end = faces.back().vertices.end();
		fv_next = fv_it = faces.back().vertices.begin();
		fv_next++;
		for (; fv_next!=fv_end; ++fv_it, ++fv_next) {
			Vertex* v1 = *fv_it;
			Vertex* v2 = *fv_next;
			Edge e = make_edge(v1,v2);
//			if (edges.find(e) != edges.end()) std::cout << "edge already found" << std::endl;
			edges[e].insert(&faces.back());
			vertices_map[v1].insert(&faces.back());
		}
		// close the loop with the end -> begin
		Edge e = make_edge(*fv_it,*(faces.back().vertices.begin()));
		edges[e].insert(&faces.back());
		vertices_map[*fv_it].insert(&faces.back());

	}
	std::cout << "Read " << faces.size() << " faces" << std::endl;
	std::cout << "Detected " << edges.size() << " edges" << std::endl;

	std::cout << PROGRESS_DONE << std::endl;

	return true;

}

bool Medial_axis_transform_3::read_from_off(const char* file_name) {
	std::cout << PROGRESS_STATUS << "Parsing OFF file to construct mat structure" << std::endl;
	std::ifstream infile (file_name);
	if (!infile.is_open()) {
		std::cout << LOG_ERROR << "Could not open file" << std::endl;
		return false;
	}

	if (read_from_off(infile)) {
		infile.close();
		return true;
	} else {
		infile.close();
		return false;
	}
}

Face_iterator Medial_axis_transform_3::faces_begin() { return faces.begin(); }
Face_iterator Medial_axis_transform_3::faces_end() { return faces.end(); }

Edge_iterator Medial_axis_transform_3::edges_begin() { return edges.begin(); }
Edge_iterator Medial_axis_transform_3::edges_end() { return edges.end(); }

void Medial_axis_transform_3::compute_face_normals() {
	std::cout << PROGRESS_STATUS << "Computing face normals" << std::endl;
	Face_iterator f_it, f_end = faces.end();
	for (f_it = faces.begin(); f_it!=f_end; ++f_it) {
		f_it->compute_normal();
	}
	std::cout << PROGRESS_DONE << std::endl;
}

int Medial_axis_transform_3::edge_number_of_left_neighbor_faces(const Edge& e) {
	if (edges.find(e) != edges.end()) {
		Face_pointer_set& fps = edges[e];
		Neighbor_face_iterator fps_it, fps_end = fps.end(); int non_visited = 0;
		for (fps_it = fps.begin(); fps_it!=fps_end; ++fps_it) {
			if (visited.find(*fps_it) == visited.end()) non_visited++;
		}
		return non_visited;
	}
	return 0;
}


bool Medial_axis_transform_3::is_edge_interior(const Edge& e) {
	return edge_number_of_left_neighbor_faces(e)>1;
}

int Medial_axis_transform_3::edge_number_of_left_neighbor_sheets(const Edge& e) {
	if (edges.find(e) != edges.end()) {
		Face_pointer_set& fps = edges[e];
		std::set<int> non_visited_sheets;
		Neighbor_face_iterator fps_it, fps_end = fps.end();
		for (fps_it = fps.begin(); fps_it!=fps_end; ++fps_it) {
			int sh_ind = (*fps_it)->sheet_index;
			if (sheet_visited.find(sh_ind) == sheet_visited.end()) 
				non_visited_sheets.insert(sh_ind);
		}
		return non_visited_sheets.size();
	}
	return 0;
}

bool Medial_axis_transform_3::is_edge_sheet_interior(const Edge& e) {
	return edge_number_of_left_neighbor_sheets(e)>1;
}

bool Medial_axis_transform_3::is_on_sheet_boundary(const Edge& e, int sheet_index, std::set<int>& neighbor_sheet_set) {
	if (edges.find(e) != edges.end()) {
		Face_pointer_set& fps = edges[e];
		neighbor_sheet_set.clear();
		Neighbor_face_iterator fps_it, fps_end = fps.end(); int sheet_neighbor_faces = 0;
		for (fps_it = fps.begin(); fps_it!=fps_end; ++fps_it) {
			Face* f = *fps_it;
			if (sheet_index!=f->sheet_index &&
				sheet_visited.find(f->sheet_index) == sheet_visited.end()) {
				neighbor_sheet_set.insert(f->sheet_index);
			}
			if (f->sheet_index == sheet_index) sheet_neighbor_faces++;
		}
		//neighbor_sheets = neighbor_sheet_set.size();
		return sheet_neighbor_faces==1;
	}
	return false;
}

bool Medial_axis_transform_3::is_sheet_on_front(unsigned sheet_index) {
	if (sheet_index >= sheets.size() || sheet_index < 0) return false;
	std::set<Face*>& sheet = sheets[sheet_index];

	std::list<Edge> face_edges;
	std::set<Face*>::iterator f_it, f_end = sheet.end();
	for (f_it = sheet.begin(); f_it!=f_end; ++f_it) {
		// go through and collect edges of the face
		Face* f = *f_it;
		Face_vertex_iterator fv_it, fv_next, fv_end = f->vertices.end();
		fv_next = fv_it = f->vertices.begin();
		fv_next++;
		for (; fv_next!=fv_end; ++fv_it, ++fv_next) {
			Vertex* v1 = *fv_it;
			Vertex* v2 = *fv_next;
			Edge e = make_edge(v1,v2);
			std::set<int> neighbor_sheets;
			if (is_on_sheet_boundary(e, sheet_index, neighbor_sheets)) 
				face_edges.push_back(e); 
			else if (neighbor_sheets.size() > 0) return false;
			// else if has other sheet return false
		}
		// close the loop with the end -> begin
		Edge e = make_edge(*fv_it,*(f->vertices.begin()));
		std::set<int> neighbor_sheets;
		if (is_on_sheet_boundary(e, sheet_index, neighbor_sheets)) 
			face_edges.push_back(e);
		else if (neighbor_sheets.size() > 0) return false;
		// else if has other sheet return false
	}

	// construct the data structure of the vertex neighborhoods
	Sheet_boundary sheet_boundary; //std::set<Edge> boundary_not_visited_edge;
	std::list<Edge>::iterator e_it, e_end = face_edges.end();
	for (e_it = face_edges.begin(); e_it!=e_end; ++e_it) {
		if (sheet_boundary.find(e_it->first) == sheet_boundary.end()) {
			std::list<Vertex*> vl; vl.push_back(e_it->second);
			sheet_boundary[e_it->first] = vl;
		} else sheet_boundary[e_it->first].push_back(e_it->second);
		if (sheet_boundary.find(e_it->second) == sheet_boundary.end()) {
			std::list<Vertex*> vl; vl.push_back(e_it->first);
			sheet_boundary[e_it->second] = vl;
		} else sheet_boundary[e_it->second].push_back(e_it->first);
		//			boundary_not_visited_edge.insert(*e_it);
	}

	//std::cout << "number of edges in face boundary: " << face_edges.size() << std::endl;
	//if (face_edges.size() == 0) {
	//	std::cout << "face " << std::endl;
	//	for (f_it = sheet.begin(); f_it!=f_end; ++f_it) {
	//		// go through and collect edges of the face
	//		Face* f = *f_it;
	//		Face_vertex_iterator fv_it, fv_end = f->vertices_end();
	//		fv_it = f->vertices_begin();
	//		for (; fv_it!=fv_end; ++fv_it) {
	//			Vertex* v1 = *fv_it;
	//			std::cout << v1->point() << std::endl;
	//			// else if has other sheet return false
	//		}
	//	}
	//}
	//std::cout << "number of faces: " << sheet.size() << std::endl;

	Sheet_boundary::iterator sh_it, sh_end = sheet_boundary.end();
	for (sh_it=sheet_boundary.begin(); sh_it!=sh_end; ++sh_it) {
		if (sh_it->second.size()>2) {
//			std::cout << LOG_WARNING << "Vertex neighbor sizes which should be <= 2 but it is " << sh_it->second.size() << std::endl;
			//std::cout << "face " << std::endl;
			//for (f_it = sheet.begin(); f_it!=f_end; ++f_it) {
			//	// go through and collect edges of the face
			//	Face* f = *f_it;
			//	Face_vertex_iterator fv_it, fv_end = f->vertices_end();
			//	fv_it = f->vertices_begin();
			//	for (; fv_it!=fv_end; ++fv_it) {
			//		Vertex* v1 = *fv_it;
			//		std::cout << v1->point() << std::endl;
			//		// else if has other sheet return false
			//	}
			//}
		}
	}

	

	// walk around boundary and check if it is safe to remoe topologically
	// we have two connected components of sheet boundary in one loop:
	// one at the boundary of the medial axis and one 
	if (face_edges.empty()) return false;
	std::queue<Edge> eq; eq.push(*(face_edges.begin()));
	int status = is_edge_sheet_interior(eq.front()) ? 1 : 0;
	std::set<Edge> visited_edges;
	Edge other_kind_edge_in_loop = make_edge(0,0);
	// 1 means that more sheets meet here, 0 means we are at the boundary of the medial axis
	// flood through the boundary using this 
	Edge ne; bool iesi;
	while (!eq.empty()) {
		// pop and mark as visited 
		Edge e = eq.front(); eq.pop();
		visited_edges.insert(e);
		Vertex* v1 = e.first;
		Vertex* v2 = e.second;
		std::list<Vertex*> v_n; std::list<Vertex*>::iterator v_it;
		v_n = sheet_boundary[v1];
		v_it  = v_n.begin();
		if (*v_it == v2) {
			if (v_n.size() > 1)
				++v_it;
			else v_it = v_n.end();
		}
		if (v_it!=v_n.end()) {
			Vertex* v0 = *v_it;
			// edge neighboring from v1
			ne = make_edge(v0,v1);
			iesi = is_edge_sheet_interior(ne);
			if ((status == 1 && iesi) || (status == 0 && !iesi)) {
				if (visited_edges.find(ne)==visited_edges.end()) eq.push(ne);
			}
			else other_kind_edge_in_loop = ne;
		}
		
		v_n = sheet_boundary[v2];
		v_it  = v_n.begin();
		if (*v_it == v1) {
			if (v_n.size() > 1)
				++v_it;
			else v_it = v_n.end();
		}
		if (v_it!=v_n.end()) {
			Vertex* v3 = *v_it;
			// edge neighboring from v2
			ne = make_edge(v2,v3);
			iesi = is_edge_sheet_interior(ne);
			if ((status == 1 && iesi) || (status == 0 && !iesi)) {
				if (visited_edges.find(ne)==visited_edges.end()) eq.push(ne);
			}
			else other_kind_edge_in_loop = ne;		
		}
	}

	if (other_kind_edge_in_loop == make_edge(0,0)) return false;

	// switch status
	status = (status==1) ? 0 : 1; 
	eq.push(other_kind_edge_in_loop);
	while (!eq.empty()) {
		// pop and mark as visited 
		Edge e = eq.front(); eq.pop();
		visited_edges.insert(e);
		Vertex* v1 = e.first;
		Vertex* v2 = e.second;
		std::list<Vertex*> v_n; std::list<Vertex*>::iterator v_it;
		v_n = sheet_boundary[v1];
		v_it  = v_n.begin();
		if (*v_it == v2) {
			if (v_n.size() > 1)
				++v_it;
			else v_it = v_n.end();
		}
		if (v_it!=v_n.end()) {
			Vertex* v0 = *v_it;
			// edge neighboring from v1
			Edge ne; bool iesi;
			ne = make_edge(v0,v1);
			iesi = is_edge_sheet_interior(ne);
			if ((status == 1 && iesi) || (status == 0 && !iesi)) {
				if (visited_edges.find(ne)==visited_edges.end()) eq.push(ne);
			}
			else other_kind_edge_in_loop = ne;
		}
		
		v_n = sheet_boundary[v2];
		v_it  = v_n.begin();
		if (*v_it == v1) {
			if (v_n.size() > 1)
				++v_it;
			else v_it = v_n.end();
		}
		if (v_it!=v_n.end()) {
			Vertex* v3 = *v_it;
			// edge neighboring from v2
			ne = make_edge(v2,v3);
			iesi = is_edge_sheet_interior(ne);
			if ((status == 1 && iesi) || (status == 0 && !iesi)) {
				if (visited_edges.find(ne)==visited_edges.end()) eq.push(ne);
			}
			else other_kind_edge_in_loop = ne;		
		}
	}

	if (visited_edges.size() == face_edges.size()) return true;

    
	return false;

}

//bool Medial_axis_transform_3::is_sheet_medial_boundary_edge(Edge& e) {
//	int non_visited = 0;
//	if (edges.find(e) != edges.end()) {
//		Face_pointer_set& fps = edges[e];
//		Neighbor_face_iterator fps_it, fps_end = fps.end();
//		for (fps_it = fps.begin(); fps_it!=fps_end; ++fps_it) {
//			non_visited++;
//		}
//	}
//	return non_visited==1;
//
//}

bool Medial_axis_transform_3::is_edge_boundary(const Edge& e) {
	return edge_number_of_left_neighbor_faces(e)==1;

}

void Medial_axis_transform_3::update_rim_vertex_set(const Edge& e) {
	if (is_edge_boundary(e)) {
		// this edge is medial axis rim, we have to keep the vertices in the rim set
		rim_vertices[e.first].insert(e);
		rim_vertices[e.second].insert(e);
	} else {
		// this edge is not medial axis rim, so we have to remove it from the rim edges set of 
		// first vertex
		Edge_set::iterator es_it; Vertex *v;
		v = e.first;
		Edge_set& es1 = rim_vertices[v]; es_it = es1.find(e);
		if (es_it != es1.end()) es1.erase(es_it);
		if (es1.empty()) {
//			std::cout << "remove rim vertex" << std::endl;
			rim_vertices.erase(rim_vertices.find(v));
		}

		// this edge is not medial axis rim, so we have to remove it from the rim edges set of 
		// second vertex
		v = e.second;
		Edge_set& es2 = rim_vertices[v]; es_it = es2.find(e);
		if (es_it != es2.end()) es2.erase(es_it);
		if (es2.empty()) {
//			std::cout << "remove rim vertex" << std::endl;
			rim_vertices.erase(rim_vertices.find(v));
		}
	}
}

bool Medial_axis_transform_3::is_vertex_interior_alone(Vertex* v) {
	return (rim_vertices.find(v) == rim_vertices.end());
}

bool Medial_axis_transform_3::is_vertex_interior_for_face(Vertex* v, Face *f) {
	if (!is_vertex_interior_alone(v)) {
		// it is bondary in general, should return false
		bool more_components;
		// but if it hase cycle or more components, it means that it is still interior, return true
		if (vertex_has_cycle_with_face(v,f,more_components) || more_components) {
			return true;
		} else return false;
	} else {
		// it is interior in general
		return true;
	}
}


bool Medial_axis_transform_3::is_front(Face* f) {
	if (visited.find(f) != visited.end()) return false;
	int status = 0, first_status = 0; // 0 unintialized, 1 interior medial axis, 2 boundary medial axis, 
	int change = 0;
	// go through edges
	Face_vertex_iterator fv_it, fv_next, fv_end = f->vertices.end();
	fv_next = fv_it = f->vertices.begin(); fv_next++;
	bool has_boundary_edge = false;
	for (; fv_next!=fv_end; ++fv_it, ++fv_next) {
		Vertex* v1 = *fv_it;
		if (is_vertex_interior_for_face(v1, f)) {
			if (status!=0 && status!=1) change++;
			if (status==0) first_status = 1;
			status = 1;
		} else {
			if (status!=0 && status!=2) change++;
			if (status==0) first_status = 2;
			status = 2;
		}
		Vertex* v2 = *fv_next;
		Edge e = make_edge(v1,v2);
		if (is_edge_interior(e)) {
			if (status!=0 && status!=1) change++;
			if (status==0) first_status = 1;
			status = 1;
		} else {
			if (status!=0 && status!=2) change++;
			if (status==0) first_status = 2;
			status = 2;
			has_boundary_edge = true;
		}
	}
	// close the loop with the end -> begin
	if (is_vertex_interior_for_face(*fv_it, f)) {
		if (status!=0 && status!=1) change++;
		if (status==0) first_status = 1;
		status = 1;
	} else {
		if (status!=0 && status!=2) change++;
		if (status==0) first_status = 2;
		status = 2;
	}
	Edge e = make_edge(*fv_it,*(f->vertices.begin()));
	if (is_edge_interior(e)) {
		if (status!=0 && status!=1) change++;
		if (status==0) first_status = 1;
		status = 1;
	} else {
		if (status!=0 && status!=2) change++;
		if (status==0) first_status = 2;
		status = 2;
		has_boundary_edge = true;
	}
	if (first_status!=status) change++;

	if (change%2==1) std::cout << LOG_ERROR << "Change number is not even in Medial_axis_transform_3::is_front(Face* f)" << std::endl;
	if (has_boundary_edge && change == 2) return true;
	else return false;
}

Front_iterator Medial_axis_transform_3::find_in_front(Face* f) {
	std::pair<Front_iterator, Front_iterator> range = front.equal_range(f->angle);
	Front_iterator f_it, f_end = range.second; 
	for (f_it = range.first; f_it!=f_end; ++f_it) {
		if (f_it->second == f) {
			return f_it;
		}
	}
	return front.end();
}

Sheet_front_iterator Medial_axis_transform_3::find_in_sheet_front(int i, double stability) {
	std::pair<Sheet_front_iterator, Sheet_front_iterator> range = sheet_front.equal_range(stability);
	Sheet_front_iterator f_it, f_end = range.second; 
	for (f_it = range.first; f_it!=f_end; ++f_it) {
		if (f_it->second == i) {
			return f_it;
		}
	}
	return sheet_front.end();
}


bool Medial_axis_transform_3::find_prev_next_vertex_in_face(Vertex* v, Face* f, Vertex*&prev, Vertex*&next) {
	Face_vertex_iterator fv_it, fv_end = f->vertices.end();
	for (fv_it=f->vertices.begin(); fv_it!=fv_end; ++fv_it) {
		if (*fv_it == v) break;
	}
	if (fv_it == fv_end) return false;
	if (fv_it == f->vertices.begin()) {
		fv_it++;
		next = *fv_it;
		prev = f->vertices.back();
	} else if (*fv_it == f->vertices.back()) {
		next = *(f->vertices.begin());
		fv_it--;
		prev = *fv_it;
	} else {
		fv_it++;
		next = *fv_it;
		fv_it--; fv_it--;
		prev = *fv_it;
	}
	return true;
}

bool Medial_axis_transform_3::vertex_has_cycle_with_face(Vertex *v, Face *f, bool& more_components) {
	// collect unvisited neighbor faces
	Face_pointer_set unvisited_neighbor_faces;
	//Face_vertex_iterator vfv_it, vfv_end = f->vertices_end();
	//for (vfv_it = f->vertices_begin(); vfv_it!=vfv_end; ++vfv_it) {
		Neighbor_face_iterator unf_it, unf_end = vertices_map[v].end();
		for (unf_it = vertices_map[v].begin(); unf_it!=unf_end; ++unf_it) {
			if (visited.find(*unf_it)==visited.end())
				unvisited_neighbor_faces.insert(*unf_it);
		}
	//}

	Face_connectivity_map face_connections;
	Neighbor_face_iterator nf_it, nf_end = unvisited_neighbor_faces.end();
	for (nf_it=unvisited_neighbor_faces.begin(); nf_it!=nf_end; nf_it++) {
		// for every face neighboring our vertex v we find the two edges incident to v
		// and go through the neighbors of the edges to build up the adjacency list
		// in the forrm of a map of faces to a set of faces
		Face* nf = *nf_it;
		Vertex *prev, *next; 
		if (!find_prev_next_vertex_in_face(v, nf, prev, next)) {
			std::cout << LOG_ERROR << "Medial_axis_transform_3::vertex_has_cycle_with_face() tests a v vertex which is not part of its neighbor face" << std::endl;
			continue;
		}
		Neighbor_face_iterator enf_it, enf_end; Edge e;
		e = make_edge(prev,v);
		face_connections[nf].insert(e);
		//for (enf_it = edges[e].begin(), enf_end = edges[e].end(); enf_it!=enf_end; ++enf_it) {
		//	Face* nnf = *enf_it;
		//	if (nnf!=nf) {
		//		face_connections[nf].insert(nnf);
		//		face_connections[nnf].insert(nf);
		//	}
		//}
		e = make_edge(v,next);
		face_connections[nf].insert(e);
		//for (enf_it = edges[e].begin(), enf_end = edges[e].end(); enf_it!=enf_end; ++enf_it) {
		//	Face* nnf = *enf_it;
		//	if (nnf!=nf) {
		//		face_connections[nf].insert(nnf);
		//		face_connections[nnf].insert(nf);
		//	}
		//}
	}

	std::map<Face*,int> face_index;
	Face_connectivity_map::iterator fcm_it, fcm_end; int i = 0;
	for (fcm_it=face_connections.begin(), fcm_end=face_connections.end(); fcm_it!=fcm_end; ++fcm_it, ++i) {
		face_index[fcm_it->first] = i;
	}

	// depth first search from f to find a cycle 
	Edge_set cycle_visited;
	Face* current = f;
//	std::cout << "target: " << face_index[current] << std::endl;
//	print_connectivity_map(face_connections, cycle_visited);

	bool has_cycle = found_cycle_to(face_connections,f,f,cycle_visited);
	more_components = false;
	for (fcm_it=face_connections.begin(), fcm_end=face_connections.end(); fcm_it!=fcm_end; ++fcm_it) {
		// go though edges and collect neighbors
		Face_pointer_set neighbor_faces;
		Edge_set& fps = fcm_it->second;
		Edge_set_iterator e_it, e_end = fps.end();
		for (e_it = fps.begin(); e_it!=e_end; ++e_it) {
			// only if cycle didn't visit this already
			if (cycle_visited.find(*e_it) == cycle_visited.end()) {
				more_components = true;
			}
		}
	}

	//Neighbor_face_iterator lnf_it, lnf_end = unvisited_neighbor_faces.end();
	//for (lnf_it=unvisited_neighbor_faces.begin(); lnf_it!=lnf_end; lnf_it++) {
	//	if (cycle_visited.find(*lnf_it) == cycle_visited.end() && face_connections[*lnf_it].size()>0) {
	//		more_components = true;
	//		break;
	//	}
	//}

	return has_cycle;
}

void Medial_axis_transform_3::print_connectivity_map(Face_connectivity_map& face_connections, Edge_set& cycle_visited) {
	std::map<Face*,int> face_index;
	Face_connectivity_map::iterator fcm_it, fcm_end; int i = 0;
	for (fcm_it=face_connections.begin(), fcm_end=face_connections.end(); fcm_it!=fcm_end; ++fcm_it, ++i) {
		face_index[fcm_it->first] = i;
	}


	//for (fcm_it=face_connections.begin(), fcm_end=face_connections.end(); fcm_it!=fcm_end; ++fcm_it) {
	//	std::cout << fcm_it->first << ": ";
	//	Face_pointer_set& fps = fcm_it->second;
	//	Neighbor_face_iterator nf_it, nf_end = fps.end();
	//	for (nf_it = fps.begin(); nf_it!=nf_end; ++nf_it) {
	//		std::cout << *nf_it << " ";
	//	}
	//	std::cout << std::endl;
	//}
	//std::cout << "--- and now with indices" << std::endl;


	for (fcm_it=face_connections.begin(), fcm_end=face_connections.end(); fcm_it!=fcm_end; ++fcm_it) {
		// print out source
		int source = face_index[fcm_it->first];
		std::cout << source << ": ";

		// go though edges and collect neighbors
		Face_pointer_set neighbor_faces;
		Edge_set& fps = fcm_it->second;
		Edge_set_iterator e_it, e_end = fps.end();
		for (e_it = fps.begin(); e_it!=e_end; ++e_it) {
			// only if cycle didn't visit this already
			if (cycle_visited.find(*e_it) == cycle_visited.end()) {
				Neighbor_face_iterator nf_it, nf_end = edges[*e_it].end();
				for (nf_it=edges[*e_it].begin(); nf_it!=nf_end; ++nf_it) {
					// only if the face wasn't visited in the topology flooding before
					if (*nf_it != fcm_it->first && visited.find(*nf_it)==visited.end()) neighbor_faces.insert(*nf_it);
				}
			}
		}

		// print out neighbor faces
		Neighbor_face_iterator nf_it, nf_end = neighbor_faces.end();
		for (nf_it=neighbor_faces.begin(); nf_it!=nf_end; ++nf_it) {
			std::cout << face_index[*nf_it] << " ";
		}
		std::cout << std::endl;

	}
	std::cout << "---" << std::endl;

}

bool Medial_axis_transform_3::found_cycle_to(Face_connectivity_map& face_connections, Face* current, Face* target, Edge_set& cycle_visited) {
	Edge_set& neighbor_edges = face_connections[current];
	Edge_set_iterator e_it, e_end = neighbor_edges.end(); bool has_cycle = false;
	for (e_it = neighbor_edges.begin(); e_it!=e_end; ++e_it) {
		// only if cycle didn't visit this already
		if (cycle_visited.find(*e_it) == cycle_visited.end()) {
			cycle_visited.insert(*e_it);
			Neighbor_face_iterator nf_it, nf_end = edges[*e_it].end();
			for (nf_it=edges[*e_it].begin(); nf_it!=nf_end; ++nf_it) {
				if (*nf_it != current) {
					if (*nf_it == target) return true;
					if (visited.find(*nf_it)==visited.end()) {
//						print_connectivity_map(face_connections, cycle_visited);
						has_cycle = has_cycle || found_cycle_to(face_connections, *nf_it, target, cycle_visited);
					}
				}
			}
		}
	}
	return has_cycle;

	
	
	//	Neighbor_face_iterator nf_it, nf_end = face_connections[current].end(); bool has_cycle = false;
//	for (nf_it=face_connections[current].begin(); nf_it!=nf_end; nf_it++) {
//		if (*nf_it == target) return true;
//		if (cycle_visited.find(*nf_it) == cycle_visited.end()) {
//			cycle_visited.insert(current);
//			face_connections[*nf_it].erase(current);
////			face_connections[current].erase(*nf_it);
//			print_connectivity_map(face_connections, cycle_visited);
//			has_cycle = has_cycle || found_cycle_to(face_connections, *nf_it, target, cycle_visited);
//		}
//	}
//	return has_cycle;
//	return false;
}

void Medial_axis_transform_3::add_to_front(Face* f) {
	if (find_in_front(f) == front.end()) front.insert(std::make_pair(f->angle,f));
}

void Medial_axis_transform_3::add_to_sheet_front(int i, double stability) {
	if (find_in_sheet_front(i, stability) == sheet_front.end()) 
		sheet_front.insert(std::make_pair(stability,i));
}

void Medial_axis_transform_3::remove_from_sheet_front(int i, double stability) {
	Sheet_front_iterator sh_it = find_in_sheet_front(i, stability);
	if (sh_it != sheet_front.end()) 
		sheet_front.erase(sh_it);
}



void Medial_axis_transform_3::remove_from_front(Face* f) {
	Front_iterator f_it = find_in_front(f);
	if (f_it != front.end()) front.erase(f_it);
}

void Medial_axis_transform_3::compute_topology_angle_sheet_filtration() {
	sheet_visited.clear();
	sheet_front.clear();

	std::cout << PROGRESS_STATUS << "Compute sheet topology filtration" << std::endl;

	// get the angle stability of all sheets
	std::vector<double> sheet_angle;
	std::vector<std::set<Face*> >::iterator sh_it, sh_end = sheets.end(); int i=0;
	for (sh_it=sheets.begin(); sh_it!=sh_end; ++sh_it, i++) {
		std::set<Face*>::iterator f_it, f_end = sh_it->end();
		double max_angle = -1;
		for (f_it = sh_it->begin(); f_it!=f_end; ++f_it) {
			if (max_angle < (*f_it)->angle) max_angle = (*f_it)->angle;
		}
		sheet_angle.push_back(max_angle);
	}


	// add front sheets to the front
	i=0;
	for (sh_it=sheets.begin(); sh_it!=sh_end; ++sh_it, i++) {
		std::set<Face*>::iterator f_it, f_end = sh_it->end();
		if (is_sheet_on_front(i)) add_to_sheet_front(i, sheet_angle[i]);
	}

	// do the flooding of the sheets
	double front_value = -1; int counter = 0; sheet_top_angle.resize(sheets.size());
	int steps = Application_settings::get_int_setting("medial-explore-sheet-front-steps");
	bool no_debug = !Application_settings::get_bool_setting("debug-medial-explore-sheet-front");
	while (!sheet_front.empty() && (no_debug || counter < steps)) {
		// get the smallest angle in the front
		Sheet_front_iterator fr_it = sheet_front.begin(); counter++;
		front_value = std::max(fr_it->first, front_value);
		int sheet_index = fr_it->second;
		sheet_front.erase(fr_it);
		if (!is_sheet_on_front(sheet_index)) {
			std::cout << LOG_ERROR << "The highest sheet in front returns false for is_sheet_on_front()" << std::endl;
			std::cout << "Sheet index: " << sheet_index << std::endl;
			continue;
		}
		if (sheet_visited.find(sheet_index)!=sheet_visited.end()) {
			std::cout << LOG_ERROR << "The highest sheet in front is already visited" << std::endl;
			continue;
		}
		// mark sheet visited
		sheet_visited.insert(sheet_index);
		// set stability of this sheet
		sheet_top_angle[sheet_index] = front_value;


		// update front in with the neighbor sheets
		std::set<int>& neighbor_sheets = sheet_connectivity[sheet_index];
//		std::cout << "neighbors of sheet " << sheet_index << std::endl;
		std::set<int>::iterator nsh_it, nsh_end = neighbor_sheets.end();
		for (nsh_it = neighbor_sheets.begin(); nsh_it != nsh_end; nsh_it++) {
//			std::cout << *nsh_it;
			if (sheet_visited.find(*nsh_it) == sheet_visited.end()) {
//				std::cout << " not visited";
				if (is_sheet_on_front(*nsh_it)) {
//					std::cout << " and on front so we add" << std::endl;
					//	std::cout << "adding a neighbor sheet to the front" << std::endl;
					add_to_sheet_front(*nsh_it, sheet_angle[*nsh_it]);
				}
				else {
//					std::cout << " but not on front so we remove" << std::endl;
					remove_from_sheet_front(*nsh_it, sheet_angle[*nsh_it]);
				}
			} //else std::cout << " already visited" << std::endl;
		}
	}

	// set the unvisited sheets to 91
	for (unsigned i = 0; i<sheets.size(); i++ ) {
		if (sheet_visited.find(i) == sheet_visited.end()) 
			sheet_top_angle[i] = 91;
	}

	std::cout << PROGRESS_DONE << std::endl;

	std::cout << PROGRESS_STATUS << "Smooth boundary of scale axis" << std::endl;

	smooth_medial_surface_boundary(
		Application_settings::get_int_setting("scale-surface-boundary-smoothing-steps"),
		&sheet_top_angle, 
		Application_settings::get_double_setting("topology-filtration-angle-lower-limit"));

	std::cout << PROGRESS_DONE << std::endl;

}

void Medial_axis_transform_3::compute_topology_angle_filtration() {
	visited.clear();
	front.clear();
	rim_vertices.clear();

	std::cout << PROGRESS_STATUS << "Compute topology filtration" << std::endl;
	
	// mark vertices which are on the boundary or outside the medial axis
	Edge_iterator e_it, e_end = edges_end();
	for (e_it = edges_begin(); e_it!=e_end; ++e_it) {
		update_rim_vertex_set(e_it->first);
	}

	// add faces of the medial axis to the front
	Face_iterator f_it, f_end = faces.end();
	for (f_it = faces.begin(); f_it!=f_end; ++f_it) {
		if (is_front(&(*f_it))) add_to_front(&(*f_it));
	}


	double front_value = -1; int counter = 0;
	int steps = Application_settings::get_int_setting("medial-explore-front-steps");
	bool no_debug = !Application_settings::get_bool_setting("debug-medial-explore-front");
	while (!front.empty() && (no_debug || counter < steps)) {
		// get the smallest angle in the front
		Front_iterator fr_it = front.begin(); counter++;
		front_value = std::max(fr_it->first, front_value);
		Face* f = fr_it->second;
		front.erase(fr_it);
		if (!is_front(f)) {
			std::cout << LOG_ERROR << "The highest face in front returns false for is_front()" << std::endl;
			continue;
		}
		if (visited.find(f)!=visited.end()) {
			std::cout << LOG_ERROR << "The highest face in front is already visited" << std::endl;
			continue;
		}
		// mark face visited
		visited.insert(f);
		// set stability of this face
		f->top_angle = front_value;

		// Go through edges and update the correspoinding vertices' rim entries
		Face_vertex_iterator fv_it, fv_next, fv_end = f->vertices.end();
		fv_next = fv_it = f->vertices.begin(); fv_next++;
		for (; fv_next!=fv_end; ++fv_it, ++fv_next) {
			Vertex* v1 = *fv_it;
			Vertex* v2 = *fv_next;
			Edge e = make_edge(v1,v2);
			update_rim_vertex_set(e);
		}
		// close the loop with the end -> begin
		Edge e = make_edge(*fv_it,*(f->vertices.begin()));
		update_rim_vertex_set(e);

		// collect all neighbor faces
		Face_vertex_iterator vfv_it, vfv_end = f->vertices.end();
		Face_pointer_set neighbor_faces;
		for (vfv_it = f->vertices.begin(); vfv_it!=vfv_end; ++vfv_it) {
			Neighbor_face_iterator nf_it, nf_end = vertices_map[*vfv_it].end();
			for (nf_it = vertices_map[*vfv_it].begin(); nf_it!=nf_end; ++nf_it) {
				neighbor_faces.insert(*nf_it);
			}
		}

		// update front in with the neighbor faces
		Neighbor_face_iterator nf_it, nf_end = neighbor_faces.end();
		for (nf_it = neighbor_faces.begin(); nf_it!=nf_end; ++nf_it) {
			if (is_front(*nf_it)) add_to_front(*nf_it);
			else remove_from_front(*nf_it);
		}

/*
		// go through edges
		Face_vertex_iterator fv_it, fv_next, fv_end = f->vertices_end();
		fv_next = fv_it = f->vertices_begin(); fv_next++;
		for (; fv_next!=fv_end; ++fv_it, ++fv_next) {
			Vertex* v1 = *fv_it;
			Vertex* v2 = *fv_next;
			Edge e = make_edge(v1,v2);
			Neighbor_face_iterator nf_it, nf_end = edges[e].end();
			for (nf_it = edges[e].begin(); nf_it!=nf_end; ++nf_it) {
				if (is_front(*nf_it)) add_to_front(*nf_it);
				else remove_from_front(*nf_it);
			}
		}
		// close the loop with the end -> begin
		Edge e = make_edge(*fv_it,*(f->vertices_begin()));
		Neighbor_face_iterator nf_it, nf_end = edges[e].end();
		for (nf_it = edges[e].begin(); nf_it!=nf_end; ++nf_it) {
			if (is_front(*nf_it)) add_to_front(*nf_it);
			else remove_from_front(*nf_it);
		}
		*/
	}

	// set the unvisited faces to 91
	for (f_it = faces_begin(); f_it!=f_end; ++f_it) {
		if (visited.find(&(*f_it)) == visited.end()) 
			f_it->top_angle = 91;
	}


	std::cout << PROGRESS_DONE << std::endl;

}


void Medial_axis_transform_3::identify_sheets() {
	std::cout << PROGRESS_STATUS << "Identifying sheets" << std::endl;
	sheets.clear();
	face_flip.clear();
	Face_set sheet_unvisited;
	Face_iterator f_it, f_end = faces.end(); 
	for (f_it = faces.begin(); f_it!=f_end; ++f_it) {
		sheet_unvisited.insert(&(*f_it));
	}

	unsigned i=0;
	while (!sheet_unvisited.empty()) {
//		std::cout << "get seed for " << i <<std::endl;
		//create sheet
		sheets.push_back(std::set<Face*>());
		// find a seed face for a sheet
		Face* seed = *(sheet_unvisited.begin());
		sheet_unvisited.erase(sheet_unvisited.begin());
//		int sheet_unvisited_size = sheet_unvisited.size();
	
		// set index and init the flooding data structure
		std::queue<Face*> last_sheet_faces;
		last_sheet_faces.push(seed);
		face_flip[seed]=false;
		seed->sheet_index = i;
		sheets.back().insert(seed);
		Face_set last_sheet_flooded;

		// go to neighbors and add them if they do belong to sheet
		while (!last_sheet_faces.empty()) {
//			std::cout << "get a face for the sheet " << std::endl;
			// get a recent sheet face
			Face* f = last_sheet_faces.front();
			last_sheet_faces.pop();
			last_sheet_flooded.insert(f);
			Vector_3 seed_normal = f->normal;
			bool seed_flip = face_flip[f];
//			std::cout << "seed_normal: " << seed_normal << std::endl;


			// collect edges
			Edge_set face_edges;
			Face_vertex_iterator fv_it, fv_next, fv_end =f->vertices.end();
			fv_next = fv_it = f->vertices.begin();
			fv_next++;
			for (; fv_next!=fv_end; ++fv_it, ++fv_next) {
				Vertex* v1 = *fv_it;
				Vertex* v2 = *fv_next;
				Edge e = make_edge(v1,v2);
				face_edges.insert(e);
			}
			// close the loop with the end -> begin
			Edge e = make_edge(*fv_it,*(f->vertices.begin()));
			face_edges.insert(e);

			// go through edges
			Edge_set_iterator e_it, e_end = face_edges.end();
			for (e_it = face_edges.begin(); e_it!=e_end; ++e_it) {
				// consider neighbor only if edge has two neighbor faces
				if (edges[*e_it].size()==2) {
					Neighbor_face_iterator nf_it, nf_end = edges[*e_it].end();
					for (nf_it=edges[*e_it].begin(); nf_it!=nf_end; ++nf_it) {
						Face* neighbor_face = *nf_it;
						if (last_sheet_flooded.find(neighbor_face)==last_sheet_flooded.end()) {
							Vector_3 neighbor_normal = neighbor_face->normal;
							//						std::cout << "neighbor_normal: " << neighbor_normal << std::endl;
							bool want_flip;
							if ((seed_normal * neighbor_normal) > 0) // same orientation like seed
								face_flip[neighbor_face] = seed_flip;
							else 
								face_flip[neighbor_face] = !seed_flip;
							neighbor_face->sheet_index = i; // set sheet index
							sheets.back().insert(neighbor_face);
							last_sheet_flooded.insert(neighbor_face); // add to flooded so that we don't visit it again
							last_sheet_faces.push(neighbor_face); // add him to the queue to visit neighbors
							sheet_unvisited.erase(neighbor_face); // make sure we don't choose this as seed for a new sheet
						}
					}



				}

			}
		}
		i++;
	}
	std::cout << "Flip information for " << face_flip.size() << " faces " << std::endl;

	// sort sheets
	struct Sheet_size_descend_comp {
		bool operator()(std::set<Face*>& sh_1, std::set<Face*>& sh_2)
		{
			return sh_1.size() > sh_2.size();
		}
	};

	std::sort(sheets.begin(), sheets.end(), Sheet_size_descend_comp());

	// renumber sheets such that it matches descreasing order
	std::vector<std::set<Face*> >::iterator sh_it, sh_end = sheets.end(); i=0;
	for (sh_it=sheets.begin(); sh_it!=sh_end; ++sh_it, i++) {
		std::set<Face*>::iterator f_it, f_end = sh_it->end();
		for (f_it = sh_it->begin(); f_it!=f_end; ++f_it) {
			(*f_it)->sheet_index = i;
		}
		
	}

	// create the connectivity information for sheets
	sheet_visited.clear();
	sheet_connectivity.clear();
	for (unsigned sheet_index=0; sheet_index<sheets.size(); sheet_index++) {
		std::set<int> n_sheets;
		std::set<Face*>& sheet = sheets[sheet_index];

		std::list<Edge> face_edges;
		std::set<Face*>::iterator f_it, f_end = sheet.end();
		for (f_it = sheet.begin(); f_it!=f_end; ++f_it) {
			// go through and collect edges of the face
			Face* f = *f_it;
			Face_vertex_iterator fv_it, fv_next, fv_end = f->vertices.end();
			fv_next = fv_it = f->vertices.begin();
			fv_next++;
			for (; fv_next!=fv_end; ++fv_it, ++fv_next) {
				Vertex* v1 = *fv_it;
				Vertex* v2 = *fv_next;
				Edge e = make_edge(v1,v2);
				std::set<int> neighbor_sheets;
				if (is_on_sheet_boundary(e, sheet_index, neighbor_sheets)) {
					face_edges.push_back(e); 
				}
				n_sheets.insert(neighbor_sheets.begin(), neighbor_sheets.end());

				// else if has other sheet return false
			}
			// close the loop with the end -> begin
			Edge e = make_edge(*fv_it,*(f->vertices.begin()));
			std::set<int> neighbor_sheets;
			if (is_on_sheet_boundary(e, sheet_index, neighbor_sheets)) {
				face_edges.push_back(e);
			}
			n_sheets.insert(neighbor_sheets.begin(), neighbor_sheets.end());
		}

		std::set<int>::iterator self = n_sheets.find(sheet_index);
		if (self!=n_sheets.end()) {
			std::cout << LOG_ERROR << "identify_sheets(): Found itself in the sheet neighbor list..." << std::endl;
			n_sheets.erase(self);
		}

		if (n_sheets.size() == 0) 
			std::cout << LOG_WARNING << "sheet connectivity entry has " << n_sheets.size() << " neighbors for sheet " << sheet_index << std::endl;
		sheet_connectivity.insert(std::make_pair(sheet_index, n_sheets));


	}

	std::cout << PROGRESS_DONE << std::endl;
 }

