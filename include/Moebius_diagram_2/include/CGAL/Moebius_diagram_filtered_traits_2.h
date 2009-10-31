#ifndef CGAL_MOEBIUS_DIAGRAM_FILTERED_TRAITS_2_H
#define CGAL_MOEBIUS_DIAGRAM_FILTERED_TRAITS_2_H

#include <CGAL/basic.h>

#include <CGAL/MP_Float.h>
#include <CGAL/Simple_cartesian.h>

#include <CGAL/Moebius_diagram_cartesian_converter.h>
#include <CGAL/Filtered_predicate.h>
#include <CGAL/Moebius_regular_filtered_traits_3.h>
#include <CGAL/Moebius_diagram_euclidean_traits_2.h>


CGAL_BEGIN_NAMESPACE

template < typename CK, typename CW = typename CK::FT,
         typename FK = Simple_cartesian<Interval_nt<false> >,
         typename FW = typename FK::FT,
         typename EK = Simple_cartesian<MP_Float>,
         typename EW = typename EK::FT >
class Moebius_diagram_filtered_traits_2
: public Moebius_diagram_euclidean_traits_2<CK, CW>
{
public:
    typedef Moebius_diagram_euclidean_traits_2<CK, CW> C_t;
    typedef Moebius_diagram_euclidean_traits_2<FK, FW> F_t;
    typedef Moebius_diagram_euclidean_traits_2<EK, EW> E_t;

    typedef C_t C_traits;
    typedef F_t F_traits;
    typedef E_t E_traits;

    typedef Moebius_diagram_cartesian_converter<C_t, F_t> C2F;
    typedef Moebius_diagram_cartesian_converter<C_t, E_t> C2E;

    typedef Moebius_diagram_filtered_traits_2<CK,CW,FK,FW,EK,EW> Traits;

    typedef Moebius_regular_filtered_traits_3<Traits> Regular_traits;

private:

    typedef typename F_t::Compare_on_oriented_line_2 F_Compare_on_oriented_line_2;
    typedef typename F_t::Moebius_orientation_2 F_Moebius_orientation_2;
    typedef typename F_t::Moebius_has_circle_2 F_Moebius_has_circle_2;
    typedef typename F_t::Moebius_circle_cross_line_2 F_Moebius_circle_cross_line_2;
    typedef typename F_t::Moebius_circle_side_of_center_2 F_Moebius_circle_side_of_center_2;
    typedef typename F_t::Moebius_side_of_vertex_2 F_Moebius_side_of_vertex_2;
    typedef typename F_t::Moebius_circle_side_of_vertex_2 F_Moebius_circle_side_of_vertex_2;

    typedef typename E_t::Compare_on_oriented_line_2 E_Compare_on_oriented_line_2;
    typedef typename E_t::Moebius_orientation_2 E_Moebius_orientation_2;
    typedef typename E_t::Moebius_has_circle_2 E_Moebius_has_circle_2;
    typedef typename E_t::Moebius_circle_cross_line_2 E_Moebius_circle_cross_line_2;
    typedef typename E_t::Moebius_circle_side_of_center_2 E_Moebius_circle_side_of_center_2;
    typedef typename E_t::Moebius_side_of_vertex_2 E_Moebius_side_of_vertex_2;
    typedef typename E_t::Moebius_circle_side_of_vertex_2 E_Moebius_circle_side_of_vertex_2;



public:
    typedef Filtered_predicate<E_Compare_on_oriented_line_2, F_Compare_on_oriented_line_2, C2E, C2F>
        Compare_on_oriented_line_2;
    typedef Filtered_predicate<E_Moebius_orientation_2, F_Moebius_orientation_2, C2E, C2F>
        Moebius_orientation_2;
    typedef Filtered_predicate<E_Moebius_has_circle_2, F_Moebius_has_circle_2, C2E, C2F>
        Moebius_has_circle_2;
    typedef Filtered_predicate<E_Moebius_circle_cross_line_2, F_Moebius_circle_cross_line_2, C2E, C2F> 
        Moebius_circle_cross_line_2;
    typedef Filtered_predicate<E_Moebius_circle_side_of_center_2, F_Moebius_circle_side_of_center_2, C2E, C2F>
        Moebius_circle_side_of_center_2;
    typedef Filtered_predicate<E_Moebius_side_of_vertex_2, F_Moebius_side_of_vertex_2, C2E, C2F>
        Moebius_side_of_vertex_2;
    typedef Filtered_predicate<E_Moebius_circle_side_of_vertex_2, F_Moebius_circle_side_of_vertex_2, C2E, C2F>
        Moebius_circle_side_of_vertex_2;




    Compare_on_oriented_line_2 compare_on_oriented_line_2_object () const { return Compare_on_oriented_line_2(); }
    Moebius_orientation_2 moebius_orientation_2_object () const { return Moebius_orientation_2(); }
    Moebius_has_circle_2 has_circle_2_object () const { return Moebius_has_circle_2(); }
    Moebius_circle_cross_line_2 circle_cross_line_2_object () const { return Moebius_circle_cross_line_2(); }
    Moebius_circle_side_of_center_2 circle_side_of_center_2_object () const { return Moebius_circle_side_of_center_2(); }
    Moebius_side_of_vertex_2 side_of_vertex_2_object () const { return Moebius_side_of_vertex_2(); }
    Moebius_circle_side_of_vertex_2 circle_side_of_vertex_2_object () const { return Moebius_circle_side_of_vertex_2(); }
};



CGAL_END_NAMESPACE


#endif//CGAL_MOEBIUS_DIAGRAM_FILTERED_TRAITS_2_H
