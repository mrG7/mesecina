/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Classified_triangulation_3.h 369 2008-06-23 22:49:40Z miklosb $
 */

#ifndef MESECINA_CLASSIFIED_TRIANGULATION_3_H
#define MESECINA_CLASSIFIED_TRIANGULATION_3_H

#include "Classified_cell_base_3.h"
#include "Classified_vertex_base_3.h"
#include <CGAL/Weighted_point.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <constants.h>

#include <queue>

template <class Gt, class Polar_vertex_handle>
class Classified_triangulation_3 : public CGAL::Delaunay_triangulation_3 < 
	Gt,
	CGAL::Triangulation_data_structure_3 < 
		Classified_vertex_base_3<Gt> ,
		Classified_cell_base_3<Gt,
			CGAL::Triangulation_cell_base_with_circumcenter_3<Gt>,
			Polar_vertex_handle 
		> 
	> 
>
{
public:
	typedef typename CGAL::Triangulation_data_structure_3 < Classified_vertex_base_3<Gt> , Classified_cell_base_3<Gt, CGAL::Triangulation_cell_base_with_circumcenter_3<Gt>, Polar_vertex_handle>  > Tds;
	typedef typename Geom_traits::FT			Coord_type;
	typedef typename Geom_traits::Point_3       Point_3;
	typedef typename CGAL::Weighted_point<Point_3,Coord_type> Weighted_point;
	typedef typename Geom_traits::Vector_3       Vector_3;
	typedef typename Geom_traits::Segment_3     Segment_3;
	typedef typename Geom_traits::Triangle_3    Triangle_3;
	typedef typename Tds::Vertex_handle				Vertex_handle;
	typedef typename Tds::Cell_handle				Cell_handle;
//	typedef typename Tds::All_faces_iterator		All_faces_iterator;
//	typedef typename Tds::Finite_vertices_iterator	Finite_vertices_iterator;
//	typedef typename Tds::Face_circulator			Face_circulator;
//	typedef typename Tds::Vertex_circulator			Vertex_circulator;
//	typedef typename Tds::Edge_circulator			Edge_circulator;


	Classified_triangulation_3(const Gt& gt = Gt())
		: CGAL::Delaunay_triangulation_3<Gt,Tds>(gt) { classified = false; }

	Classified_triangulation_3(const Classified_triangulation_3<Gt, Polar_vertex_handle> &tr) : CGAL::Delaunay_triangulation_3<Gt,Tds>(tr)
	{   
	//CGAL::CGAL_triangulation_postcondition( is_valid() ); /// \todo check what to do, removed because of mac version
	classified = tr.classified; }

	void set_dirty() { classified = false; } /// \todo check if we could make it so that it is done by the insert...

	bool is_dirty() { return !classified; }

	void compute_poles() {
		typedef std::vector<Vertex_handle> Vertex_vector;
		typedef std::back_insert_iterator<Vertex_vector> Insert_iterator;
		if (number_of_vertices() < 4) return;

		// Mark all vertices on the boundary of the convex hull. 
		// Such vertices are incident to the vertex at infinity.
		Vertex_vector on_convex_hull;
		Insert_iterator ii(on_convex_hull);
		incident_vertices( infinite_vertex(), ii);
		for ( Vertex_vector::iterator i = on_convex_hull.begin(); i != on_convex_hull.end(); ++i)
			(*i)->set_on_convex_hull(true); 

		// Compute the center of mass of the sample points.
		Vector_3 center_of_mass = CGAL::NULL_VECTOR;
		Finite_vertices_iterator vit = finite_vertices_begin();
		Finite_vertices_iterator vdone = finite_vertices_end(); 
		for (; vit != vdone; vit++) 
			center_of_mass = center_of_mass + (vit->point() - CGAL::ORIGIN);
		double n = number_of_vertices();
		center_of_mass = center_of_mass / n;

		std::cout << PROGRESS_STATUS << "Storing cell circumcenters" << std::endl;
		for ( Finite_cells_iterator cit = finite_cells_begin(); cit != finite_cells_end(); ++cit) {
			cit->set_circumcenter(dual(cit));
		}	
		std::cout << PROGRESS_DONE << std::endl;

		std::cout << PROGRESS_STATUS << "Identifying plus poles" << std::endl;
		for ( Finite_cells_iterator cit = finite_cells_begin(); cit != finite_cells_end(); ++cit) {
			for ( int id = 0; id < 4; ++id) {
				// If the neighboring cell of *cit opposite to the vertex with
				// index id is infinite then all vertices of *cit different from 
				// the vertex with index id are on the boundary of the convex hull. 
				// For such vertices the normal is computed as the average of the 
				// normals of the incident facets on the convex hull.
				if ( is_infinite( cit->neighbor(id))) {
					Vector_3 facet_normal = CGAL::cross_product(
						  cit->vertex((id+2)%4)->point() - 	cit->vertex((id+1)%4)->point(), 
							cit->vertex((id+3)%4)->point() -  cit->vertex((id+1)%4)->point() );

					// Normalize and choose a consistent orientation. This is 
					// important for computing the average of facet normals of 
					// facets incident to one vertex in a meaningful way.
					facet_normal = facet_normal / 
							sqrt(CGAL::to_double( facet_normal.x() * facet_normal.x() + 
							facet_normal.y() * facet_normal.y() + 
							facet_normal.z() * facet_normal.z() ) );
					if ( facet_normal * (cit->vertex((id+1)%4)->point() - 
							(CGAL::ORIGIN + center_of_mass)) < 0)
						facet_normal = -facet_normal;
			
					// Add facet normal to all vertices of the facet opposite to
					// the vertex with index i.
					for ( int i = 1; i < 4; ++i) {
						Vertex_handle vh = cit->vertex((id+i)%4);
						Vector_3 new_normal = vh->get_plus_pole_direction() + facet_normal;
						new_normal = new_normal / 
								sqrt(CGAL::to_double( new_normal.squared_length()));
						vh->set_plus_pole_direction( new_normal);
						vh->set_pole_plus(cit->neighbor(id));
					}
				}

				// Update the pole if it is further away than the current pole
				Vertex_handle vh = cit->vertex(id);
				Vector_3 cp = cit->get_circumcenter() - vh->point();
				if ( (! vh->is_on_convex_hull()) && 
					 (vh->get_plus_pole_direction().squared_length() < cp.squared_length())) {
					vh->set_plus_pole_direction( cp);
					vh->set_pole_plus(cit);
				}
			}
		}
		std::cout << PROGRESS_DONE << std::endl;



		std::cout << PROGRESS_STATUS << "Identifying minus poles" << std::endl;
		for ( Finite_cells_iterator cit = finite_cells_begin(); cit != finite_cells_end(); ++cit) {
			for ( int id = 0; id < 4; ++id) {
				Vertex_handle vh = cit->vertex(id);
				Vector_3 cp = cit->get_circumcenter() - vh->point();
				if (cp * vh->get_plus_pole_direction() < 0) {
					if (vh->get_pole_minus()==0)
						vh->set_pole_minus(cit);
					else {
						Vector_3 old_cp = vh->get_pole_minus()->get_circumcenter() - vh->point();
						if (old_cp.squared_length() < cp.squared_length())
							vh->set_pole_minus(cit);
					}
				}
			}
		}
		std::cout << PROGRESS_DONE << std::endl;

		std::cout << PROGRESS_STATUS << "Computing poles confidence" << std::endl;
		for ( Finite_vertices_iterator vit = finite_vertices_begin(); vit != finite_vertices_end(); ++vit) {
			Vector_3 ppd = vit->get_plus_pole_direction();
			ppd = ppd / sqrt(CGAL::to_double(ppd.squared_length()));
			if (vit->get_pole_minus()!=0) {
				Vector_3 pmd = vit->get_pole_minus()->get_circumcenter() - vit->point();
				pmd = pmd / sqrt(CGAL::to_double(pmd.squared_length()));
				float confidence = CGAL::to_double(-1*(ppd*pmd));
				//std::cout << "Poles confidence: " << confidence << std::endl;
				vit->set_poles_confidence(confidence);
			} else {
				vit->set_poles_confidence(0);
				std::cout << LOG_WARNING << "in computing poles confidence minus_pole is 0..." << std::endl;
			}
		}
		std::cout << PROGRESS_DONE << std::endl;

		Finite_vertices_iterator v_it, v_end = finite_vertices_end(); int i=0;
		std::cout << PROGRESS_STATUS << "Storing plus poles and minus poles" << std::endl;
		for (v_it = finite_vertices_begin(); v_it != v_end; v_it++, i++) {
			v_it->get_pole_plus()->set_classification(V_PLUS);
			v_it->get_pole_plus()->add_sample_point(v_it);
			if (v_it->get_pole_minus()==0) 
				std::cout << LOG_ERROR << "oups, no minus pole found for vertex " << i << std::endl;
			else { 
				v_it->get_pole_minus()->set_classification(V_MINUS);
				v_it->get_pole_minus()->add_sample_point(v_it);
			}
//			if (is_infinite(v_it->get_pole_plus())) std::cout << "an infinite cell has been linked to its vertex" << std::endl;

		}
		std::cout << PROGRESS_DONE << std::endl;


		classified = true;
	}

private:
	bool classified;
	std::list< Weighted_point > inner_balls;
};

#endif //MESECINA_CLASSIFIED_TRIANGULATION_3_H