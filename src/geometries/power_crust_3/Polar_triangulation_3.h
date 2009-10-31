/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Polar_triangulation_3.h 370 2008-06-25 00:05:59Z miklosb $
 */

#ifndef MESECINA_POLAR_TRIANGULATION_3_H
#define MESECINA_POLAR_TRIANGULATION_3_H

#include <CGAL/Regular_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_3.h>
#include <CGAL/Regular_triangulation_cell_base_3.h>
#include <CGAL/Regular_triangulation_euclidean_traits_3.h>
#include <CGAL/Triangulation_hierarchy_3.h>
#include <CGAL/Triangulation_hierarchy_vertex_base_3.h>

#include "Polar_vertex_base_3.h"
#include "Classified_triangulation_3.h"

#include "constants.h"


template <class Gt, class Weight>
class Polar_triangulation_3 : public /*CGAL::Triangulation_hierarchy_3<*/
	CGAL::Regular_triangulation_3 < 
		CGAL::Regular_triangulation_euclidean_traits_3<Gt,Weight>, 
		CGAL::Triangulation_data_structure_3 <// CGAL::Regular_triangulation_euclidean_traits_3<Gt,Weight>, 
			/*CGAL::Triangulation_hierarchy_vertex_base_3 < */
				CGAL::Triangulation_vertex_base_3 < 
					CGAL::Regular_triangulation_euclidean_traits_3<Gt,Weight>,
					Polar_vertex_base_3 < CGAL::Regular_triangulation_euclidean_traits_3<Gt,Weight> >
				>
			/*>*/,
			CGAL::Regular_triangulation_cell_base_3 < CGAL::Regular_triangulation_euclidean_traits_3<Gt,Weight> >
		>
	> 
//>
{
public:
	typedef CGAL::Regular_triangulation_euclidean_traits_3<Gt,Weight>								 Geom_traits;
	typedef typename
		CGAL::Triangulation_data_structure_3 <// CGAL::Regular_triangulation_euclidean_traits_3<Gt,Weight>, 
			/*CGAL::Triangulation_hierarchy_vertex_base_3 < */
				CGAL::Triangulation_vertex_base_3 < 
					CGAL::Regular_triangulation_euclidean_traits_3<Gt,Weight>,
					Polar_vertex_base_3 < CGAL::Regular_triangulation_euclidean_traits_3<Gt,Weight> >
				>
			/*>*/,
			CGAL::Regular_triangulation_cell_base_3 < CGAL::Regular_triangulation_euclidean_traits_3<Gt,Weight> >
		>																					Rtds;
	typedef typename Rtds::Vertex_handle				Vertex_handle;
	typedef typename Rtds::Cell_handle				Cell_handle;
	typedef typename Classified_triangulation_3<Gt, Vertex_handle>	 Classified_triangulation_3;
	typedef typename Classified_triangulation_3::Cell_handle			Classified_cell_handle;
	typedef typename Classified_triangulation_3::Cell			Classified_cell;
//	typedef CGAL::Triangulation_3<Geom_traits>         Traingulation_base;
	typedef typename Geom_traits::FT				  Coord_type;
//	typedef typename Geom_traits::Point  Weighted_point;
	typedef typename Geom_traits::Point_3				Point_3;
	typedef typename Geom_traits::Vector_3				Vector_3;
	typedef typename Geom_traits::Segment_3			   Segment_3;
	typedef typename Geom_traits::Triangle_3		  Triangle_3;
//	typedef typename CGAL::Regular_triangulation_3<Geom_traits,Rtds>::Vertex_handle Vertex_handle;
//	typedef typename CGAL::Regular_triangulation_3<Geom_traits,Rtds>::Vertex_circulator Vertex_circulator;
	typedef typename CGAL::Regular_triangulation_3<Geom_traits,Rtds>::Finite_vertices_iterator Finite_vertices_iterator;
	
	Polar_triangulation_3(const Geom_traits& gt = Geom_traits())
		: /*CGAL::Triangulation_hierarchy_3<*/ CGAL::Regular_triangulation_3<Geom_traits,Rtds> /*>*/(gt) { 
			classified = false; inside_pole_seed = 0; 
//			std::cout << "Polar_triangulation_3(const Geom_traits& gt = Geom_traits()) constructor" << std::endl;
	}

	Polar_triangulation_3(const Polar_triangulation_3<Gt, Weight> &tr) {
		std::cout << "Polar_triangulation_3(const Polar_triangulation_3<Gt, Weight> &tr)" << std::endl;
		classified = tr.classified;
		inside_pole_seed = tr.inside_pole_seed; 

		CGAL::Regular_triangulation_3<Geom_traits,Rtds>::copy_triangulation(tr);

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


//		CGAL::Regular_triangulation_3<Geom_traits,Rtds>::copy_triangulation(tr);
//		CGAL::CGAL_triangulation_postcondition( is_valid() ); /// \todo removed because of mac

		//CGAL::CGAL_triangulation_postcondition(CGAL::is_valid());
	}

	Polar_triangulation_3<Gt, Weight>& operator=(const Polar_triangulation_3<Gt, Weight> &tr)
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

	void compute_polar_triangulation(Classified_triangulation_3* t) {
		if (t->number_of_cells () < 2) return;
		//std::cout << "Starting to collect poles weighted points in a vector..." << std::endl;
		//std::cout << PROGRESS_STATUS << "Collect poles weighted points in a vector" << std::endl;
		//std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		//std::cout << PROGRESS_MAXIMUM <<  t->number_of_finite_cells() << std::endl; int i=0;
		clear(); //initialize polar triangulation
		typedef typename Classified_triangulation_3::Finite_cells_iterator Finite_cells_iterator;
		typedef typename Classified_triangulation_3::Finite_vertices_iterator Finite_vertices_iterator;
		typedef typename Classified_triangulation_3::Vertex_handle Triangulation_vertex_handle;
		typedef typename Classified_triangulation_3::Cell_handle Triangulation_cell_handle;

		//Finite_cells_iterator c_it, c_end = t->finite_cells_end();
		//for (c_it = t->finite_cells_begin(); c_it != c_end; c_it++) 
		//	c_it->set_added(false);

		//Finite_vertices_iterator v_it, v_end = t->finite_vertices_end();
		//for (v_it = t->finite_vertices_begin(); v_it != v_end; v_it++, i++) {
		//	std::cout << PROGRESS_VALUE << i << std::endl;
		//	Triangulation_cell_handle pole_plus = v_it->get_pole_plus();
		//	Triangulation_cell_handle pole_minus = v_it->get_pole_minus();
		//	Vertex_handle pv_minus = pole_minus->get_polar_vertex();
		//	if (pv_minus==0) {
		//		pv_minus = insert(Weighted_point(pole_minus->get_circumcenter(), squared_distance(pole_minus->get_circumcenter(), pole_minus->vertex(0)->point())));
		//		pole_minus->set_polar_vertex(pv_minus);
		//	}
		//	Vertex_handle pv_plus = pole_plus->get_polar_vertex();
		//	if (pv_plus==0) {
		//		if (!t->is_infinite(pole_plus)) {
		//			pv_plus = insert(Weighted_point(pole_plus->get_circumcenter(), squared_distance(pole_plus->get_circumcenter(), pole_plus->vertex(0)->point())));
		//			pole_plus->set_polar_vertex(pv_plus);
		//		} else if (inside_pole_seed==0) inside_pole_seed = pv_minus;
		//	}
		//	if (pv_plus!=0 && pv_minus!=0) {
		//		pv_plus->set_opposite_pole(pv_minus);
		//		pv_minus->set_opposite_pole(pv_plus);
		//	}
		//}




		std::vector<Weighted_point> point_vector;

		std::cout << PROGRESS_STATUS << "Build regular triangulation of all poles" << std::endl;
		Finite_cells_iterator c_it, c_end = t->finite_cells_end();
		// iterate through the cells
		for (c_it = t->finite_cells_begin(); c_it != c_end; c_it++) {
//			std::cout << PROGRESS_VALUE << i << std::endl;
			if (c_it->get_classification() != V_UNCLASSIFIED ) {
				Weight weight = CGAL::squared_distance(c_it->get_circumcenter(), c_it->vertex(0)->point());
//				Point_3 circcent = c_it->get_circumcenter();
				Vertex_handle s = 
					insert(Weighted_point(c_it->get_circumcenter(), weight));
				if (s!=Vertex_handle()) {
					c_it->set_polar_vertex(s);
					set_voronoi_vertex_for_polar(&(*c_it), s);
				} else {
					c_it->set_polar_vertex(s);
//					set_voronoi_vertex_for_polar(&(*c_it), s);
					//std::cout << log_warning << "insert polar vertex in regular triangulation gives a 0-pointer... " << i << std::endl;
					//std::cout << log_warning << "point was " << c_it->get_circumcenter() << ", " << " weight " << weight << std::endl;
				}
				//Classified_cell* cch = get_voronoi_vertex_for_polar(s);
				//if (cch->vertex(0) != c_it->vertex(0))
				//	std::cout << LOG_ERROR << "backlink to voronoi vertex wrong" << std::endl;
				//else std::cout << LOG_GREEN << "backlink to voronoi vertex is not wrong" << std::endl;

				//if (inside_pole_seed==0) {
				//	Triangulation_cell_handle opp_pole = (*(c_it->get_sample_points()->begin()))->get_opposite_pole(c_it);
				//	if (t->is_infinite(opp_pole)) {
				//		inside_pole_seed = s;
				//	}
				//}
			}
		}
		std::cout << PROGRESS_DONE << std::endl;

			
		std::cout << PROGRESS_STATUS << "Updating links between the regular triangulation and input triangulation" << std::endl;
		// set infinite faces to infinite pole
		typedef std::vector<Classified_cell_handle> Cell_vector;
		typedef std::back_insert_iterator<Cell_vector> Insert_iterator;
		
		Cell_vector infinite_cells;
		Insert_iterator ii(infinite_cells);
		t->incident_cells( t->infinite_vertex(), ii);
		for ( Cell_vector::iterator i = infinite_cells.begin(); i != infinite_cells.end(); ++i) {
			Classified_cell_handle ch = *i;
			ch->set_polar_vertex(infinite_vertex());
			set_voronoi_vertex_for_polar(&(*ch), infinite_vertex());
		}

		// setting polar pairs which have hidden polar vertices to 0 confidence
		Finite_vertices_iterator v_it, v_end = t->finite_vertices_end(); 
		for (v_it=t->finite_vertices_begin(); v_it!=v_end; ++v_it) {
			if (v_it->get_pole_minus()->get_polar_vertex() == Vertex_handle() || 
				v_it->get_pole_plus()->get_polar_vertex() == Vertex_handle())
					v_it->set_poles_confidence(0);
		}
		std::cout << PROGRESS_DONE << std::endl;
	}

	void set_voronoi_vertex_for_polar(Classified_cell* ch, Vertex_handle& v) {
		v->voronoi_vertex() = ch;
	}

	Classified_cell* get_voronoi_vertex_for_polar(Vertex_handle& v) {
		return static_cast<Classified_cell*>(v->voronoi_vertex());
	}

	void compute_classification() {
		typedef std::vector<Vertex_handle> Vertex_vector;
		typedef std::back_insert_iterator<Vertex_vector> Insert_iterator;

		if (number_of_vertices() < 2) return;
		std::cout << "Starting to classify poles..." << std::endl;
		std::cout << PROGRESS_STATUS << "Classifying poles" << std::endl;
		std::cout << PROGRESS_MINIMUM << "0" << std::endl;
		std::cout << PROGRESS_MAXIMUM << number_of_vertices()-1 << std::endl; int i=0;
		std::queue<Vertex_handle> Q;
		inside_pole_seed->set_classification(P_INTERIOR);
		inner_balls.clear();
		inner_balls.push_back(inside_pole_seed->point());
		/*if (!inside_pole_seed->is_hidden())*/ Q.push(inside_pole_seed);
		Vertex_handle inf_vertex = infinite_vertex();
		inf_vertex->set_classification(P_EXTERIOR); 
		Q.push(inf_vertex);
		while (!Q.empty()) {
			Vertex_handle pole = Q.front(); Q.pop();
			Vertex_vector adjacent_vertices;
			Insert_iterator ii(adjacent_vertices);
			incident_vertices( pole, ii);
			for ( Vertex_vector::iterator vvv_it = adjacent_vertices.begin(); vvv_it != adjacent_vertices.end(); vvv_it++) {
				Vertex_handle vv_it = *vvv_it;
				if (!is_infinite(vv_it) && !vv_it->is_classified() && 
					(squared_distance(pole->point().point(), vv_it->point().point()) <
					(
					pole->point().weight() + vv_it->point().weight() -
					CGAL::sqrt(2.0 * pole->point().weight() * vv_it->point().weight()) /// \todo see why does it crash for MP_float
					)
				 ))
				{
					//the balls intersect deeply 
					vv_it->set_classification(pole->get_classification()); i++; std::cout << PROGRESS_VALUE << i << std::endl;
					if (vv_it->is_interior()) inner_balls.push_back(vv_it->point());
					Vertex_handle vv_h = vv_it; Q.push(vv_h);
					/// \todo let this go in when we have the info
					//Vertex_handle opp = vv_it->get_opposite_pole();
					//if (opp != 0 && !opp->is_classified() /*&& !opp->is_hidden()*/) {
					//	opp->set_classification(pole->get_opposite_classification()); i++; std::cout << PROGRESS_VALUE << i << std::endl;
					//	if (opp->is_interior()) inner_balls.push_back(opp->point());
					//	Q.push(opp);
					//}
				}
			} 
		}
		std::cout << PROGRESS_DONE << std::endl;
		classified = true;
		std::cout << "Done." << std::endl;
	}

	std::list< Weighted_point >* get_inner_balls() {
//		if (is_dirty()) compute_classification();
		return &inner_balls;
	}

	void set_dirty(bool t = true) { classified = !t; }
	bool is_dirty() { return !classified; }
	std::list< Weighted_point > inner_balls;

private:
	bool classified;
	Vertex_handle inside_pole_seed;


};

#endif //MESECINA_POLAR_TRIANGULATION_3_H