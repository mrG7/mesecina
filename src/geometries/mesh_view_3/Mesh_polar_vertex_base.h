#ifndef MESECINA_POLAR_MESH_VERTEX_BASE_H
#define MESECINA_POLAR_MESH_VERTEX_BASE_H

#include <CGAL/Triangulation_vertex_base_3.h>

template < class GT, class Vb = CGAL::Triangulation_vertex_base_3<GT> >
class Mesh_polar_vertex_base
  : public Vb
{
public:
  typedef typename Vb::Cell_handle    Cell_handle;
  typedef typename Vb::Point          Point;

  template < class TDS2 >
  struct Rebind_TDS {
    typedef typename Vb::template Rebind_TDS<TDS2>::Other  Vb2;
    typedef Mesh_polar_vertex_base<GT, Vb2>                        Other;
  };

  Mesh_polar_vertex_base() { mesh_pole = 0;}

  Mesh_polar_vertex_base(const Point& p)
    : Vb(p) { mesh_pole = 0; }

  Mesh_polar_vertex_base(const Point& p, Cell_handle c)
    : Vb(p, c) { mesh_pole = 0; }

  Cell_handle     mesh_pole;
};

#endif //MESECINA_POLAR_MESH_VERTEX_BASE_H