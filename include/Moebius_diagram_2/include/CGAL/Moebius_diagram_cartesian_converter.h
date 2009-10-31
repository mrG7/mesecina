#ifndef CGAL_MOEBIUS_CARTESIAN_CONVERTER_H
#define CGAL_MOEBIUS_CARTESIAN_CONVERTER_H

#include <CGAL/basic.h>

#include <CGAL/NT_converter.h>
#include <CGAL/Cartesian_converter.h>


CGAL_BEGIN_NAMESPACE

template <class GT1, class GT2,
         class Wconv = NT_converter<typename GT1::Weight, typename GT2::Weight > >
class Moebius_diagram_cartesian_converter
: public Cartesian_converter<typename GT1::Kernel, typename GT2::Kernel>
{
    typedef Cartesian_converter<typename GT1::Kernel, typename GT2::Kernel> Base;
public:
    typedef Wconv Weight_converter;

    typename GT2::Point_2
        operator() (const typename GT1::Point_2 &p) const
        {
            typename GT2::Weight l = wc (p.lambda());
            typename GT2::Weight m = wc (p.mu());
            typename GT1::Bare_point_2 bp1 = p.point();
            typename GT2::Bare_point_2 bp = Base::operator() (bp1);

            return typename GT2::Point_2 (bp, l, m);
        }
private:
    Weight_converter wc;

};

CGAL_END_NAMESPACE

#endif//CGAL_MOEBIUS_CARTESIAN_CONVERTER_H
