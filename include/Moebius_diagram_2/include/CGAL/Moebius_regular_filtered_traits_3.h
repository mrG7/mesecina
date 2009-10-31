#ifndef CGAL_MOEBIUS_REGULAR_FILTERED_TRAITS_3_H
#define CGAL_MOEBIUS_REGULAR_FILTERED_TRAITS_3_H

#include <CGAL/basic.h>
#include <CGAL/Filtered_predicate.h>



CGAL_BEGIN_NAMESPACE

template <typename MGT >
class Moebius_regular_filtered_traits_3
: public MGT::C_traits::Regular_traits
{
public:
    typedef typename MGT::C2F C2F;
    typedef typename MGT::C2E C2E;

private:
    typedef typename MGT::C_traits ACT;
    typedef typename MGT::F_traits AFT;
    typedef typename MGT::E_traits AET;

public:
    typedef typename MGT::C_traits::Regular_traits C_traits;
    typedef typename MGT::F_traits::Regular_traits F_traits;
    typedef typename MGT::E_traits::Regular_traits E_traits;

    typedef typename F_traits::Power_test_3 F_Power_test_3;
    typedef typename F_traits::Compare_power_distance_3 F_Compare_power_distance_3;
    typedef typename F_traits::Orientation_3 F_Orientation_3;
    typedef typename F_traits::Coplanar_orientation_3 F_Coplanar_orientation_3;
    typedef typename F_traits::Compare_xyz_3 F_Compare_xyz_3;

    typedef typename E_traits::Power_test_3 E_Power_test_3;
    typedef typename E_traits::Compare_power_distance_3 E_Compare_power_distance_3;
    typedef typename E_traits::Orientation_3 E_Orientation_3;
    typedef typename E_traits::Coplanar_orientation_3 E_Coplanar_orientation_3;
    typedef typename E_traits::Compare_xyz_3 E_Compare_xyz_3;

typedef Filtered_predicate<E_Power_test_3, F_Power_test_3, C2E, C2F>
Power_test_3;
typedef Filtered_predicate<E_Compare_power_distance_3, F_Compare_power_distance_3, C2E, C2F>
Compare_power_distance_3;
typedef Filtered_predicate<E_Orientation_3, F_Orientation_3, C2E, C2F>
Orientation_3;
typedef Filtered_predicate<E_Coplanar_orientation_3, F_Coplanar_orientation_3, C2E, C2F>
Coplanar_orientation_3;
typedef Filtered_predicate<E_Compare_xyz_3, F_Compare_xyz_3, C2E, C2F>
Compare_xyz_3;

    Power_test_3 power_test_3_object () const { return Power_test_3 (); }
    Orientation_3 orientation_3_object () const { return Orientation_3 (); }
    Coplanar_orientation_3 coplanar_orientation_3_object () const { return Coplanar_orientation_3 (); }
    Compare_xyz_3 compare_xyz_3_object () const { return Compare_xyz_3 (); }


};


CGAL_END_NAMESPACE

#endif//CGAL_MOEBIUS_REGULAR_FILTERED_TRAITS_3_H

