/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Ball_triangulation_2.h 388 2008-12-12 18:17:06Z miklosb $
 */

#ifndef MESECINA_BALL_TRIANGULATION_2_H
#define MESECINA_BALL_TRIANGULATION_2_H

#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Regular_triangulation_2.h>
#include <CGAL/Regular_triangulation_vertex_base_2.h>
#include <CGAL/Regular_triangulation_face_base_2.h>
#include <CGAL/Weighted_alpha_shape_euclidean_traits_2.h>

#include "Ball_face_base_2.h"
#include "Ball_vertex_base_2.h"

#include <constants.h>


template <class Gt, class Weight>
class Ball_triangulation_2 : public CGAL::Regular_triangulation_2 < 
		CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt>, 
		CGAL::Triangulation_default_data_structure_2 < CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt>, 
			CGAL::Regular_triangulation_vertex_base_2 < 
				CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt>,
				Ball_vertex_base_2 < CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt> >
			>,
			CGAL::Regular_triangulation_face_base_2 < 
				CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt>,
				Ball_face_base_2 < CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt> >
			>
		>
	>
{
public:
	typedef CGAL::Delaunay_triangulation_2<Gt>							Triangulation_2;
	typedef CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt>								 Geom_traits;
	typedef typename 
		CGAL::Triangulation_default_data_structure_2 < CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt>, 
			CGAL::Regular_triangulation_vertex_base_2 < 
				CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt>,
				Ball_vertex_base_2 < CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt> >
			>,
			CGAL::Regular_triangulation_face_base_2 < 
				CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt>,
				Ball_face_base_2 < CGAL::Weighted_alpha_shape_euclidean_traits_2<Gt> >
			>
		>																							Rtds;
	typedef typename Geom_traits::FT				  Coord_type;
	typedef typename CGAL::Regular_triangulation_2<Geom_traits,Rtds>::Point  Weighted_point;
	typedef typename Weighted_point::Point				Point_base_for_weighted_point;
	typedef typename Geom_traits::Vector_2				Vector_2;
	typedef typename Geom_traits::Segment_2			   Segment_2;
	typedef typename Geom_traits::Triangle_2		  Triangle_2;
	
	typedef typename CGAL::Regular_triangulation_2<Geom_traits,Rtds>::Vertex_handle Vertex_handle;
	typedef typename CGAL::Regular_triangulation_2<Geom_traits,Rtds>::All_faces_iterator All_faces_iterator;
	typedef typename CGAL::Regular_triangulation_2<Geom_traits,Rtds>::Finite_edges_iterator Finite_edges_iterator;
	typedef typename CGAL::Regular_triangulation_2<Geom_traits,Rtds>::Edge Edge;	
	typedef typename CGAL::Regular_triangulation_2<Geom_traits,Rtds>::Face_handle Face_handle;
	typedef typename CGAL::Regular_triangulation_2<Geom_traits,Rtds>::Finite_vertices_iterator Finite_vertices_iterator;

	
	typedef typename  std::list< Weighted_point > Weighted_point_list;
	typedef typename Weighted_point_list::iterator Weighted_point_list_iterator;


	Ball_triangulation_2(const Geom_traits& gt = Geom_traits())
		: CGAL::Regular_triangulation_2<Geom_traits,Rtds>(gt) { classified = false; inside_pole_seed = 0; growth = 1; }

	Ball_triangulation_2(const Ball_triangulation_2<Gt, Weight> &tr)
		: CGAL::Regular_triangulation_2<Geom_traits,Rtds>(tr)
	{   
//	CGAL::CGAL_triangulation_postcondition( is_valid() );  /// \todo removed this for mac version
	classified = tr.classified; inside_pole_seed = 0;  growth = 1; 
	}

	Ball_triangulation_2<Gt, Weight>& operator=(const Ball_triangulation_2<Gt, Weight> &tr)
	{
		Finite_vertices_iterator v_it, v_end = tr.finite_vertices_end();
		for (v_it = tr.finite_vertices_begin(); v_it != v_end; ++v_it) {
			Vertex_handle new_v = insert(v_it->point());
		}
		Hidden_vertices_iterator hv_it, hv_end = tr.hidden_vertices_end();
		for (hv_it = tr.hidden_vertices_begin(); hv_it!=hv_end; hv_it++) {
			Vertex_handle new_v = insert(hv_it->point());
		}		
		classified = false; /// \todo check how to copy face classification at cloning
		inside_pole_seed = tr.inside_pole_seed; 
		growth = tr.growth;

		return *this;
	}

	void set_dirty() { classified = false; }
	bool is_dirty() { return !classified; }

	void import_weighted_points(std::list< Weighted_point >* wps) {
		if (wps==0 || wps->size() < 1) return;
//		clear();
		std::cout << PROGRESS_STATUS << "Building regular triangulations of poles" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM <<  wps->size()-1 << std::endl;
		int i = 0;
		Weighted_point_list_iterator p_it, p_end = wps->end();
		for (p_it = wps->begin(); p_it!=p_end; p_it++, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			//Weighted_point p = Weighted_point(
			//	Point_base_for_weighted_point(CGAL::to_double(p_it->point().x()),
			//		  CGAL::to_double(p_it->point().y())
			//	),
			//	CGAL::to_double(p_it->weight())
			//);
			//insert(p);
			insert(*p_it);
		}
		std::cout << PROGRESS_DONE << std::endl;
	}

	void compute_dual(double alpha = 0) {
		Finite_vertices_iterator v_it, v_end = finite_vertices_end();
		for (v_it = finite_vertices_begin(); v_it != v_end; ++v_it) {
			v_it->set_classification(D_NOT);
		}

		Finite_faces_iterator ff_it, ff_end = finite_faces_end();
		for (ff_it = finite_faces_begin(); ff_it != ff_end; ++ff_it) {
			ff_it->init_sqa();
		}
		

		//std::cout << "Starting to classify faces of regular triangulations..." << std::endl;
		std::cout << PROGRESS_STATUS << "Classifying faces of regular triangulations" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM <<  number_of_faces()-1 << std::endl; int i = 0;
		for (All_faces_iterator f_it = all_faces_begin(), f_end = all_faces_end(); f_it != f_end; f_it++, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			if (!is_infinite(f_it) && f_it->get_squared_alpha() <= alpha)
				f_it->set_classification(U_INTERIOR);
			else f_it->set_classification(U_EXTERIOR);
		}
		std::cout << PROGRESS_DONE << std::endl; //std::cout << "Done." << std::endl;

		//std::cout << "Starting to classify edges of regular triangulations..." << std::endl;
		std::cout << PROGRESS_STATUS << "Classifying edges of regular triangulations" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM <<  tds().number_of_edges()-1 << std::endl; i = 0;
		for (Finite_edges_iterator e_it = finite_edges_begin(), e_end = finite_edges_end(); e_it != e_end; e_it++, i++) {
			std::cout << PROGRESS_VALUE << i << std::endl;
			Edge e = *e_it;
			Face_handle f = e.first;
			int id = e.second; int a_f = 0;
			Face_handle nF = f->neighbor(id);
			if (nF==0) return; // for the case of one edge and one face.
			int nId = nF->index(f);

			Coord_type f_alpha, nF_alpha;
			if (is_infinite(f)) f_alpha = alpha - 1;
			else f_alpha = f->is_attached(id) ? f->get_squared_alpha() : f->get_squared_alpha(id);
			if (is_infinite(nF)) nF_alpha = alpha - 1;
			else nF_alpha = nF->is_attached(nId) ? nF->get_squared_alpha() : nF->get_squared_alpha(nId);

			//Coord_type f_alpha = !is_infinite(f) && f->is_attached(id) ? f->get_squared_alpha() : f->get_squared_alpha(id);
			//Coord_type nF_alpha = !is_infinite(nF) && nF->is_attached(nId) ? nF->get_squared_alpha() : nF->get_squared_alpha(nId);
			if (f_alpha <= alpha && nF_alpha <= alpha) {
				if (f->get_classification() == U_INTERIOR) a_f++;
				if (nF->get_classification() == U_INTERIOR) a_f++;
				if (a_f == 0) { 
					f->set_classification(id, U_SINGULAR);
					nF->set_classification(nId, U_SINGULAR);
				}
				if (a_f == 1) { 
					f->set_classification(id, U_REGULAR); 
					nF->set_classification(nId, U_REGULAR);
				}
				if (a_f == 2) { 
					f->set_classification(id, U_INTERIOR); nF->set_classification(nId, U_INTERIOR);
				}
				if (a_f < 2) {
					f->vertex((id+1)%3)->set_classification(D_IS);
					f->vertex((id+2)%3)->set_classification(D_IS);
				}
			} else {
				f->set_classification(id, U_EXTERIOR); nF->set_classification(nId, U_EXTERIOR);
			}
		}
		std::cout << PROGRESS_DONE << std::endl; //std::cout << "Done." << std::endl;
		classified = true;
	}

	void multiply_growth_ratio(Coord_type ratio) {
		growth *= ratio;
	}

	Coord_type get_shrink_ratio() {
		return 1/growth;
	}

	Coord_type get_growth_ratio() {
		return growth;
	}

private:
	bool classified;
	Vertex_handle inside_pole_seed;
	Coord_type growth;

};

#endif //MESECINA_BALL_TRIANGULATION_2_H