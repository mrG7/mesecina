#include <CGAL/basic.h>

#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>
#include <cassert>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Moebius_diagram_filtered_traits_2.h>
#include <CGAL/Moebius_diagram_2.h>
#include <CGAL/spatial_sort.h>

#include <CGAL/Random.h>
#include <CGAL/point_generators_2.h>


#include <CGAL/Timer.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Moebius_diagram_filtered_traits_2<K> GT;
typedef CGAL::Moebius_diagram_2<GT> MD;

typedef K::Point_2 Point;
typedef GT::Point_2 MPoint;

typedef CGAL::Creator_uniform_2<double,Point>  Creator;


struct Moebius_spatial_sort_traits : public GT::Regular_traits
{
    typedef MPoint Point_3;
};

int main (int argc, char **argv)
{
    std::ios_base::sync_with_stdio(false);

    int nb_locate = 1;
    if (argc > 1) { std::istringstream is (argv[1]); is >> nb_locate; }

    CGAL::Timer tread, tinit, tbuild, tmoebius, tlocate;

    std::cout << "Reading... " << std::flush;
    tread.start();
    std::vector<MPoint> points;
    {
        std::istream_iterator<MPoint> is (std::cin), end;
        std::copy (is, end, std::back_inserter (points));
    }
    tread.stop(); std::cout << tread.time() << std::endl;

//    std::cout << "spatial_sort()... " << std::flush;
//    tmoebius.start(); tsort.start();
    //CGAL::spatial_sort (points.begin(), points.end(), Moebius_spatial_sort_traits());
//    CGAL::spatial_sort (points.begin(), points.end());
//    tsort.stop(); std::cout << tsort.time() << std::endl;

    std::cout << "init()... " << std::flush;
    tinit.start();

    MD md;
    md.init (points.begin(), points.end());
    tinit.stop(); std::cout << tinit.time() << std::endl;

    std::cout << "build()... " << std::flush;
    tbuild.start();
    md.build();
    tbuild.stop(); std::cout << tbuild.time() << std::endl;
    std::cout << "init() + build()... " << std::flush;
    tmoebius.stop(); std::cout << tmoebius.time() << std::endl;

    std::cout << nb_locate << " locate()... " << std::flush;
    tlocate.start();
    MD::Vertex_handle v;
    for (MD::Finite_vertices_iterator i = md.finite_vertices_begin();
	    i != md.finite_vertices_end(); ++i)
    {
	if (! i->present()) continue;
	
	MD::Vertex_handle w = md.locate (i->point().point(), v);
#if 0	
	if (! w->present())
	    std::cerr << "\nOUPS! " << w->point() << " is not present " << std::flush;
	if (w != MD::Vertex_handle (i))
	    std::cerr << "\nOUPS! " << i->point()
		<< " is in " << w->point() << " " << std::flush;
#endif
	v = w;
    }
    tlocate.stop(); std::cout << tlocate.time() << std::endl;
    
    assert (md.built());
    std::cout << "ok" << std::endl;
    return 0;
}
