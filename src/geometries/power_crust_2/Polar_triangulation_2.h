/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Polar_triangulation_2.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef MESECINA_POLAR_TRIANGULATION_2_H
#define MESECINA_POLAR_TRIANGULATION_2_H

#include <CGAL/Regular_triangulation_2.h>
#include <CGAL/Regular_triangulation_vertex_base_2.h>
#include <CGAL/Regular_triangulation_face_base_2.h>
#include <CGAL/Regular_triangulation_euclidean_traits_2.h>

#include "Polar_vertex_base_2.h"
#include "Classified_triangulation_2.h"

#include "constants.h"


template <class Gt, class Weight>
class Polar_triangulation_2 : public CGAL::Regular_triangulation_2 < 
		CGAL::Regular_triangulation_euclidean_traits_2<Gt,Weight>, 
		CGAL::Triangulation_default_data_structure_2 < CGAL::Regular_triangulation_euclidean_traits_2<Gt,Weight>, 
			CGAL::Regular_triangulation_vertex_base_2 < 
				CGAL::Regular_triangulation_euclidean_traits_2<Gt,Weight>,
				Polar_vertex_base_2 < CGAL::Regular_triangulation_euclidean_traits_2<Gt,Weight> >
			>,
			CGAL::Regular_triangulation_face_base_2 < CGAL::Regular_triangulation_euclidean_traits_2<Gt,Weight> >
		>
	>
{
public:
	typedef Classified_triangulation_2<Gt>														 Triangulation_2;
	typedef CGAL::Regular_triangulation_euclidean_traits_2<Gt,Weight>								 Geom_traits;
	typedef typename
		CGAL::Triangulation_default_data_structure_2 < CGAL::Regular_triangulation_euclidean_traits_2<Gt,Weight>, 
			CGAL::Regular_triangulation_vertex_base_2 < 
				CGAL::Regular_triangulation_euclidean_traits_2<Gt,Weight>,
				Polar_vertex_base_2 < CGAL::Regular_triangulation_euclidean_traits_2<Gt,Weight> >
			>,
			CGAL::Regular_triangulation_face_base_2 < CGAL::Regular_triangulation_euclidean_traits_2<Gt,Weight> >
		>																							Rtds;
	typedef CGAL::Triangulation_2<Geom_traits>         Traingulation_base;
	typedef typename Geom_traits::FT				  Coord_type;
	typedef typename CGAL::Regular_triangulation_2<Geom_traits,Rtds>::Point  Weighted_point;
	typedef typename Geom_traits::Point_2				Point_2;
	typedef typename Geom_traits::Vector_2				Vector_2;
	typedef typename Geom_traits::Segment_2			   Segment_2;
	typedef typename Geom_traits::Triangle_2		  Triangle_2;
	typedef typename CGAL::Regular_triangulation_2<Geom_traits,Rtds>::Vertex_handle Vertex_handle;
	typedef typename CGAL::Regular_triangulation_2<Geom_traits,Rtds>::Vertex_circulator Vertex_circulator;
	typedef typename CGAL::Regular_triangulation_2<Geom_traits,Rtds>::Finite_vertices_iterator Finite_vertices_iterator;

	Polar_triangulation_2(const Geom_traits& gt = Geom_traits())
		: CGAL::Regular_triangulation_2<Geom_traits,Rtds>(gt) { 
			classified = false; inside_pole_seed = 0; 
//			std::cout << "Polar_triangulation_2(const Geom_traits& gt = Geom_traits()) constructor" << std::endl;
	}

	Polar_triangulation_2(const Polar_triangulation_2<Gt, Weight> &tr) {
		std::cout << "Polar_triangulation_2(const Polar_triangulation_2<Gt, Weight> &tr)" << std::endl;
		classified = tr.classified;
		inside_pole_seed = tr.inside_pole_seed; 

		CGAL::Regular_triangulation_2<Geom_traits,Rtds>::copy_triangulation(tr);

		//Traingulation_base::copy_triangulation(tr);
		//_hidden_vertices = tr._hidden_vertices;
		//// the list of vertices have been copied member for member and are
		//// not good
		//// clear them and next
		//// scan the hidden vertices to retablish the list in faces
		//typename Rtds::Face_iterator 
		//	baseit= this->_tds.face_iterator_base_begin();
		//for( ; baseit !=  this->_tds.face_iterator_base_end(); baseit++){
		//	baseit->vertex_list().clear();
		//}
		//Hidden_vertices_iterator hvit = hidden_vertices_begin();
		//for( ; hvit !=  hidden_vertices_end() ; ++hvit){
		//	hvit->face()->vertex_list().push_back(hvit);
		//}
		//CGAL_triangulation_postcondition(is_valid());


//		CGAL::Regular_triangulation_2<Geom_traits,Rtds>::copy_triangulation(tr);
//		CGAL::CGAL_triangulation_postcondition( is_valid() ); /// \todo removed because of mac

		//CGAL::CGAL_triangulation_postcondition(CGAL::is_valid());
	}

	Polar_triangulation_2<Gt, Weight>& operator=(const Polar_triangulation_2<Gt, Weight> &tr)
	{
		Finite_vertices_iterator v_it, v_end = tr.finite_vertices_end();
		for (v_it = tr.finite_vertices_begin(); v_it != v_end; ++v_it) {
			Vertex_handle new_v = insert(v_it->point());
			new_v->set_classification(v_it->get_classification());
		}
		classified = tr.classified;
		inside_pole_seed = tr.inside_pole_seed; 

		return *this;
	}

	void compute_polar_triangulation(Triangulation_2* t) {
		if (t->number_of_faces () < 2) return;
		std::cout << "Starting to build regular triangulation of poles..." << std::endl;
		std::cout << PROGRESS_STATUS << "Building regular triangulations of poles" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM <<  t->number_of_vertices()-1 << std::endl;
		clear(); //initialize polar triangulation
		inside_pole_seed = 0;  // this is the seed to separate inside/outside poles
		// iterate through vertices
		int i=0;
		typedef typename Triangulation_2::Finite_vertices_iterator Finite_vertices_iterator;
		typedef typename Triangulation_2::Face_circulator Face_circulator;
		typedef typename Triangulation_2::Vertex_circulator Vertex_circulator;
		Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
		for (v_it = t->finite_vertices_begin(); v_it != v_end; ++v_it, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			Point_2 vertex = v_it->point();
			Point_2 pole = vertex;
			Vector_2 first_pole_direction;
			bool first_pole_is_infinite = false;
			Vertex_handle first_pole;
			Face_circulator f_it, f_begin;
			f_it = f_begin = t->incident_faces(v_it);
			// iterate through faces incident to v_it and find pole
			do {
				// if current voronoi vertex is the farthest, overwrite pole
				if (!t->is_infinite(f_it)) {
					Point_2 vor = t->dual(f_it);
					if (pole == vertex || has_larger_distance_to_point(vertex, vor, pole))
						pole = vor;
					++f_it;
				} else first_pole_is_infinite = true;
			} while (f_it != f_begin && !first_pole_is_infinite);
			// add to regular triangulation the current pole
			if (first_pole_is_infinite) {
				Vertex_circulator vv_it = t->incident_vertices(v_it,f_it);
				while (!t->is_infinite(vv_it)) { 
					vv_it++; 
				}
				vv_it--; Point_2 ne1 = vv_it->point();
				Vector_2 n1(ne1 - vertex); // first convex hull neighbor vector
				vv_it++;
				assert(t->is_infinite(vv_it));
				vv_it++; Point_2 ne2 = vv_it->point();
				Vector_2 n2(vertex - ne2); // second convex hull neighbor vector
				n1 = Vector_2(-1*n1.y(), n1.x()); // rotate 90 degrees
				Coord_type norm_sq = n1.x()*n1.x() + n1.y()*n1.y();
				n1 = n1 / CGAL::sqrt(CGAL::to_double(norm_sq)); // normalize
				n2 = Vector_2(-1*n2.y(), n2.x()); // rotate 90 degrees
				n2 = n2 / CGAL::sqrt(CGAL::to_double( n2.x()*n2.x() + n2.y()*n2.y())); // normalize
				first_pole_direction = n1 + n2; //add them together
			} else if (pole != vertex) {
				first_pole = insert(Weighted_point(pole, squared_distance(pole, vertex)));
				first_pole_direction = pole - vertex;
			}

			pole = vertex;
			f_it = f_begin = t->incident_faces(v_it);
			do {
				// if current voronoi vertex is the farthest, overwrite pole
				if (!t->is_infinite(f_it)) {
					Point_2 vor = t->dual(f_it);
					if ( ((vor - vertex) * first_pole_direction < 0) &&
						(pole == vertex || has_larger_distance_to_point(vertex, vor, pole)) )
						pole = vor;
				}
				++f_it;
			} while (f_it != f_begin);
			// add to regular triangulation the current pole
			if (pole != vertex) {
				Vertex_handle inserted = insert(Weighted_point(pole, squared_distance(pole, vertex)));
				if (inside_pole_seed == 0 && first_pole_is_infinite) {
					inside_pole_seed = inserted;
				}
				if (!first_pole_is_infinite && !first_pole->is_hidden()) {
					first_pole->set_opposite_pole(inserted);
					inserted->set_opposite_pole(first_pole);
				}
			}
		}
		std::cout << PROGRESS_DONE << std::endl;
	}

	void compute_voronoi_triangulation(Triangulation_2* t) {
		if (t->number_of_faces () < 2) return;
		std::cout << "Starting to build regular triangulation of voronoi vertices..." << std::endl;
		std::cout << PROGRESS_STATUS << "Building regular triangulations of voronoi vertices" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM <<  t->number_of_faces()-1 << std::endl;
		clear(); //initialize polar triangulation

		typedef typename Triangulation_2::Finite_faces_iterator Finite_faces_iterator;
		Finite_faces_iterator f_it, f_end = t->finite_faces_end(); int i = 0;
		for (f_it = t->finite_faces_begin(); f_it != f_end; f_it++, i++) {
			Point_2 p = t->dual(f_it);
			std::cout << PROGRESS_VALUE << i << std::endl;
			insert(Weighted_point(p, squared_distance(p, f_it->vertex(0)->point())));
		}
		std::cout << PROGRESS_DONE << std::endl;
	}

	void compute_classification() {
		if (number_of_vertices() < 2) return;
		std::cout << "Starting to classify poles..." << std::endl;
		std::queue<Vertex_handle> Q;
		inside_pole_seed->set_classification(P_INTERIOR);
		inner_balls.clear();
		inner_balls.push_back(inside_pole_seed->point());
		if (!inside_pole_seed->is_hidden()) Q.push(inside_pole_seed);
		Vertex_handle inf_vertex = infinite_vertex();
		inf_vertex->set_classification(P_EXTERIOR); 
		Q.push(inf_vertex);
		while (!Q.empty()) {
			Vertex_handle pole = Q.front(); Q.pop();
			Vertex_circulator vv_start = incident_vertices(pole);
			Vertex_circulator vv_it = vv_start;
			do {
				if (!is_infinite(vv_it) && !vv_it->is_classified() && 
					(squared_distance(pole->point().point(), vv_it->point().point()) <
					(
					pole->point().weight() + vv_it->point().weight() -
					CGAL::sqrt(CGAL::to_double(2.0 * pole->point().weight() * vv_it->point().weight())) /// \todo see why does it crash for MP_float
					)
				 ))
				{
					//the balls intersect deeply 
					vv_it->set_classification(pole->get_classification());
					if (vv_it->is_interior()) inner_balls.push_back(vv_it->point());
					Vertex_handle vv_h = vv_it; Q.push(vv_h);
					Vertex_handle opp = vv_it->get_opposite_pole();
					if (opp != 0 && !opp->is_classified() && !opp->is_hidden()) {
						opp->set_classification(pole->get_opposite_classification());
						if (opp->is_interior()) inner_balls.push_back(opp->point());
						Q.push(opp);
					}
				}
				vv_it++;
			} while (vv_it!=vv_start);
		}
		classified = true;
	}

	std::list< Weighted_point >* get_inner_balls() {
		if (is_dirty()) compute_classification();
		return &inner_balls;
	}

	void set_dirty() { classified = false; }
	bool is_dirty() { return !classified; }
private:
	bool classified;
	Vertex_handle inside_pole_seed;
	std::list< Weighted_point > inner_balls;

};

#endif //MESECINA_POLAR_TRIANGULATION_2_H