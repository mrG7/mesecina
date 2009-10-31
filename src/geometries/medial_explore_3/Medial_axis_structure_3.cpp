#include <utility>
#include <fstream>
#include <constants.h>

#include "Medial_axis_structure_3.h"

#include <gui/app/static/Application_settings.h>

using namespace medial;

#define NORMAL_COMP_SQUARED_EDGE_LENGTH 0.0000001

/// Vertex functions

Vertex::Vertex(const Point_3& _p, double _radius_sq) : p(_p), radius_sq(_radius_sq) {	}

Point_3& Vertex::point() { return p; }
void Vertex::set_point(const Point_3& mp) { p=mp; }

double Vertex::radius() { return std::sqrt(radius_sq); }
double Vertex::squared_radius() { return radius_sq; }
void Vertex::set_squared_radius(double rsq) { radius_sq = rsq; }

/// Face functions

Face::Face(const Vertex_pointer_vector& _vertices) {
	vertices = _vertices;
}
Vector_3 Face::get_normal() { return normal; }
void Face::set_normal(const Vector_3& v) { normal = v; }
void Face::compute_normal() {
	Point_3 p0 = vertices[0]->point();
	Point_3 p1 = vertices[1]->point();
	Point_3 p2 = vertices[2]->point();
	int i = 3;
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
				p2 = vertices[i%vertices.size()]->point();
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
Face_vertex_iterator Face::vertices_begin() {
	return vertices.begin();
}
Face_vertex_iterator Face::vertices_end() {
	return vertices.end();
}


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
	edges.clear();

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
	int dummy; infile >> dummy;
	std::cout << "MOFF READER: vertices: " << n_vertices << " faces: " << n_faces << std::endl;


	double x,y,z, radius;
	for (int i=0; i<n_vertices; i++) {
		infile >> x;
		infile >> y;
		infile >> z;
		infile >> radius;
		vertices.push_back(Vertex(Point_3(x,y,z),radius));
	}
	std::cout << "Read " << vertices.size() << " vertices" << std::endl;

	int v, index; double angle, lambda;
	for (int i=0; i<n_faces; i++) {
		infile >> v;
		Vertex_pointer_vector vv;
		for (int j=0; j<v; j++) {
			infile >> index;
			vv.push_back(&vertices[index]);
		}
		infile >> angle;
		infile >> lambda;
		faces.push_back(Face(vv));
		faces.back().angle = angle;
		faces.back().lambda = lambda;
		Face_vertex_iterator fv_it, fv_next, fv_end = faces.back().vertices_end();
		fv_next = fv_it = faces.back().vertices_begin();
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
		Edge e = make_edge(*fv_it,*(faces.back().vertices_begin()));
		edges[e].insert(&faces.back());
		vertices_map[*fv_it].insert(&faces.back());

	}
	std::cout << "Read " << faces.size() << " faces" << std::endl;
	std::cout << "Detected " << edges.size() << " edges" << std::endl;

	std::cout << PROGRESS_DONE << std::endl;

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
		Face_vertex_iterator fv_it, fv_next, fv_end = faces.back().vertices_end();
		fv_next = fv_it = faces.back().vertices_begin();
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
		Edge e = make_edge(*fv_it,*(faces.back().vertices_begin()));
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
	Face_vertex_iterator fv_it, fv_next, fv_end = f->vertices_end();
	fv_next = fv_it = f->vertices_begin(); fv_next++;
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
	Edge e = make_edge(*fv_it,*(f->vertices_begin()));
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


bool Medial_axis_transform_3::find_prev_next_vertex_in_face(Vertex* v, Face* f, Vertex*&prev, Vertex*&next) {
	Face_vertex_iterator fv_it, fv_end = f->vertices_end();
	for (fv_it=f->vertices_begin(); fv_it!=fv_end; ++fv_it) {
		if (*fv_it == v) break;
	}
	if (fv_it == fv_end) return false;
	if (fv_it == f->vertices_begin()) {
		fv_it++;
		next = *fv_it;
		prev = f->vertices.back();
	} else if (*fv_it == f->vertices.back()) {
		next = *(f->vertices_begin());
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

void Medial_axis_transform_3::remove_from_front(Face* f) {
	Front_iterator f_it = find_in_front(f);
	if (f_it != front.end()) front.erase(f_it);
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
		Face_vertex_iterator fv_it, fv_next, fv_end = f->vertices_end();
		fv_next = fv_it = f->vertices_begin(); fv_next++;
		for (; fv_next!=fv_end; ++fv_it, ++fv_next) {
			Vertex* v1 = *fv_it;
			Vertex* v2 = *fv_next;
			Edge e = make_edge(v1,v2);
			update_rim_vertex_set(e);
		}
		// close the loop with the end -> begin
		Edge e = make_edge(*fv_it,*(f->vertices_begin()));
		update_rim_vertex_set(e);

		// collect all neighbor faces
		Face_vertex_iterator vfv_it, vfv_end = f->vertices_end();
		Face_pointer_set neighbor_faces;
		for (vfv_it = f->vertices_begin(); vfv_it!=vfv_end; ++vfv_it) {
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

