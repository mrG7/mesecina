#ifndef CHR_MOEBIUS_REGULAR_EUCLIDEAN_TRAITS_3_H
#define CHR_MOEBIUS_REGULAR_EUCLIDEAN_TRAITS_3_H

#include <CGAL/basic.h>
#include <CGAL/Moebius_point.h>
#include <CGAL/Regular_triangulation_euclidean_traits_3.h>
#include <CGAL/predicates/Moebius_diagram_regularftC2.h>

CGAL_BEGIN_NAMESPACE

template <class K, class W>
inline
Orientation
moebius_orientation_3 (const Moebius_point_2<K, W> &p,
		      const Moebius_point_2<K, W> &q,
		      const Moebius_point_2<K, W> &r,
		      const Moebius_point_2<K, W> &s,
		      Cartesian_tag)
{
  typedef typename K::FT FT;
   return moebius_orientationC3 (p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
				q.x(), q.y(), FT (q.lambda()), FT (q.mu()),
				r.x(), r.y(), FT (r.lambda()), FT (r.mu()),
				s.x(), s.y(), FT (s.lambda()), FT (s.mu()));
}


template <class K, class W>
inline
Orientation
moebius_orientation_3 (const Moebius_point_2<K, W> &p,
		      const Moebius_point_2<K, W> &q,
		      const Moebius_point_2<K, W> &r,
		      const Moebius_point_2<K, W> &s,
		      Homogeneous_tag)
{
  typedef typename K::FT FT;
  return moebius_orientationC3 (p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
			       q.x(), q.y(), FT (q.lambda()), FT (q.mu()),
			       r.x(), r.y(), FT (r.lambda()), FT (r.mu()),
			       s.x(), s.y(), FT (s.lambda()), FT (s.mu()));
}


template <class K, class W>
inline
Orientation
moebius_orientation_3 (const Moebius_point_2<K, W> &p,
		      const Moebius_point_2<K, W> &q,
		      const Moebius_point_2<K, W> &r,
		      const Moebius_point_2<K, W> &s)
{
  typedef typename K::Rep_tag Tag;
  return moebius_orientation_3 (p, q, r, s, Tag ());
}


template <class Point, class Weight>
class Moebius_orientation_3
{
public:
  typedef Orientation  result_type;
  typedef CGAL::Moebius_point_2 <Point, Weight>        Weighted_point;

  Orientation operator() ( const Weighted_point & p,
			   const Weighted_point & q,
			   const Weighted_point & r,
			   const Weighted_point & s) const
    {
      //std::cerr << "orientation "
      //	<< "(" << p << ")"
      //	<< "(" << q << ")"
      //	<< "(" << r << ")"
      //	<< "(" << s << ")\n";
      return moebius_orientation_3 (p, q, r, s);
    }
};


// ------------------------

template <class K, class W>
inline
Orientation
moebius_coplanar_orientation_3 (const Moebius_point_2<K, W> &p,
			       const Moebius_point_2<K, W> &q,
			       const Moebius_point_2<K, W> &r,
			       Cartesian_tag)
{
  typedef typename K::FT FT;
   return moebius_coplanar_orientationC3 (p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
					 q.x(), q.y(), FT (q.lambda()), FT (q.mu()),
					 r.x(), r.y(), FT (r.lambda()), FT (r.mu()));
}


template <class K, class W>
inline
Orientation
moebius_coplanar_orientation_3 (const Moebius_point_2<K, W> &p,
		      const Moebius_point_2<K, W> &q,
		      const Moebius_point_2<K, W> &r,
		      Homogeneous_tag)
{
  typedef typename K::FT FT;
  return moebius_coplanar_orientationC3 (p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
					q.x(), q.y(), FT (q.lambda()), FT (q.mu()),
					r.x(), r.y(), FT (r.lambda()), FT (r.mu()));
}


template <class K, class W>
inline
Orientation
moebius_coplanar_orientation_3 (const Moebius_point_2<K, W> &p,
			       const Moebius_point_2<K, W> &q,
			       const Moebius_point_2<K, W> &r)
{
  typedef typename K::Rep_tag Tag;
  return moebius_coplanar_orientation_3 (p, q, r, Tag ());
}


template <class Point, class Weight>
class Moebius_coplanar_orientation_3
{
public:
  typedef Orientation  result_type;
  typedef CGAL::Moebius_point_2 <Point, Weight>        Weighted_point;

  Orientation operator() ( const Weighted_point & p,
			   const Weighted_point & q,
			   const Weighted_point & r) const
    {
      return moebius_coplanar_orientation_3 (p, q, r);
    }
};

// ------------------------

template <class K, class W>
inline
Comparison_result
moebius_compare_xyz_3 (const Moebius_point_2<K, W> &p,
		      const Moebius_point_2<K, W> &q,
		      Cartesian_tag)
{
  typedef typename K::FT FT;
   return moebius_compare_xyzC3 (p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
				q.x(), q.y(), FT (q.lambda()), FT (q.mu()));
}


template <class K, class W>
inline
Comparison_result
moebius_compare_xyz_3 (const Moebius_point_2<K, W> &p,
		      const Moebius_point_2<K, W> &q,
		      Homogeneous_tag)
{
  typedef typename K::FT FT;
  return moebius_compare_xyzC3 (p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
			       q.x(), q.y(), FT (q.lambda()), FT (q.mu()));
}


template <class K, class W>
inline
Comparison_result
moebius_compare_xyz_3 (const Moebius_point_2<K, W> &p,
		      const Moebius_point_2<K, W> &q)
{
  typedef typename K::Rep_tag Tag;
  return moebius_compare_xyz_3 (p, q, Tag ());
}


template <class Point, class Weight>
class Moebius_compare_xyz_3
{
public:
  typedef Comparison_result  result_type;
  typedef CGAL::Moebius_point_2 <Point, Weight>        Weighted_point;

  Comparison_result operator() ( const Weighted_point & p,
				 const Weighted_point & q) const
    {
      return moebius_compare_xyz_3 (p, q);
    }
};

template <class Point, class Weight>
class Moebius_less_x_3
{
public:
  typedef bool  result_type;
  typedef CGAL::Moebius_point_2 <Point, Weight>        Weighted_point;

    bool operator() ( const Weighted_point & p,
            const Weighted_point & q) const
    {
        return p.lambda() * p.x() < q.lambda() * q.x();
    }
};

template <class Point, class Weight>
class Moebius_less_y_3
{
public:
  typedef bool  result_type;
  typedef CGAL::Moebius_point_2 <Point, Weight>        Weighted_point;

    bool operator() ( const Weighted_point & p,
            const Weighted_point & q) const
    {
        return p.lambda() * p.y() < q.lambda() * q.y();
    }
};

template <class Point, class Weight>
class Moebius_less_z_3
{
public:
  typedef bool  result_type;
  typedef CGAL::Moebius_point_2 <Point, Weight>        Weighted_point;

    bool operator() ( const Weighted_point & p,
            const Weighted_point & q) const
    {
        return q.lambda() < p.lambda();
    }
};

// ------------------------


// regular triangulation predicates
// Cartesian versions.
template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K, Weight> &p,
		  const Moebius_point_2<K, Weight> &q,
		  const Moebius_point_2<K, Weight> &r,
		  const Moebius_point_2<K, Weight> &s,
		  const Moebius_point_2<K, Weight> &t,
		  Cartesian_tag)
{
  typedef typename K::FT FT;
  return moebius_power_testC3(p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
			     q.x(), q.y(), FT (q.lambda()), FT (q.mu()),
			     r.x(), r.y(), FT (r.lambda()), FT (r.mu()),
			     s.x(), s.y(), FT (s.lambda()), FT (s.mu()),
			     t.x(), t.y(), FT (t.lambda()), FT (t.mu()));
}

template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K, Weight> &p,
		  const Moebius_point_2<K, Weight> &q,
		  const Moebius_point_2<K, Weight> &r,
		  const Moebius_point_2<K, Weight> &t,
		  Cartesian_tag)
{
  typedef typename K::FT FT;
  return moebius_power_testC3(p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
			     q.x(), q.y(), FT (q.lambda()), FT (q.mu()),
			     r.x(), r.y(), FT (r.lambda()), FT (r.mu()),
			     t.x(), t.y(), FT (t.lambda()), FT (t.mu()));
}

template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K, Weight> &p,
		  const Moebius_point_2<K, Weight> &q,
		  const Moebius_point_2<K, Weight> &t,
		  Cartesian_tag)
{
  typedef typename K::FT FT;
  return moebius_power_testC3(p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
			     q.x(), q.y(), FT (q.lambda()), FT (q.mu()),
			     t.x(), t.y(), FT (t.lambda()), FT (t.mu()));
}

template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K, Weight> &p,
		  const Moebius_point_2<K, Weight> &q,
		  Cartesian_tag)
{
  typedef typename K::FT FT;
  return moebius_power_testC3(p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
			     q.x(), q.y(), FT (p.lambda()), FT (q.mu()));
}


// Homogeneous versions.
template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K, Weight> &p,
		  const Moebius_point_2<K, Weight> &q,
		  const Moebius_point_2<K, Weight> &r,
		  const Moebius_point_2<K, Weight> &s,
		  const Moebius_point_2<K, Weight> &t,
		  Homogeneous_tag)
{
  typedef typename K::RT RT;
  return moebius_power_testH3(p.hx(), p.hy(), p.hw(), RT (p.lambda()), RT(p.mu()),
			     q.hx(), q.hy(), q.hw(), RT (q.lambda()), RT(q.mu()),
			     r.hx(), r.hy(), r.hw(), RT (r.lambda()), RT(r.mu()),
			     s.hx(), s.hy(), s.hw(), RT (s.lambda()), RT(s.mu()),
			     t.hx(), t.hy(), t.hw(), RT (t.lambda()), RT(t.mu()));
}

// The 2 following call the cartesian version over FT, because an homogeneous
// special version would be boring to write.

template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K, Weight> &p,
		  const Moebius_point_2<K, Weight> &q,
		  const Moebius_point_2<K, Weight> &r,
		  const Moebius_point_2<K, Weight> &t,
		  Homogeneous_tag)
{
  typedef typename K::FT FT;
  return moebius_power_testC3(p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
			     q.x(), q.y(), FT (q.lambda()), FT (q.mu()),
			     r.x(), r.y(), FT (r.lambda()), FT (r.mu()),
			     t.x(), t.y(), FT (t.lambda()), FT (t.mu()));
}

template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K, Weight> &p,
		  const Moebius_point_2<K, Weight> &q,
		  const Moebius_point_2<K, Weight> &t,
		  Homogeneous_tag)
{
  typedef typename K::FT FT;
  return moebius_power_testC3(p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
			     q.x(), q.y(), FT (q.lambda()), FT (q.mu()),
			     t.x(), t.y(), FT (t.lambda()), FT (t.mu()));
}

template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K, Weight> &p,
		  const Moebius_point_2<K, Weight> &q,
		  Homogeneous_tag)
{
  typedef typename K::FT FT;
  return moebius_power_testC3(p.x(), p.y(), FT (p.lambda()), FT (p.mu()),
			     q.x(), q.y(), FT (q.lambda()), FT (q.mu()));
}


// Kludges for M$.

template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K,Weight> &p,
		  const Moebius_point_2<K,Weight> &q,
		  const Moebius_point_2<K,Weight> &r,
		  const Moebius_point_2<K,Weight> &s,
		  const Moebius_point_2<K,Weight> &t)
{
  typedef typename K::Rep_tag Tag;
  return moebius_power_test(p,q,r,s,t, Tag());
}

template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K,Weight> &p,
		  const Moebius_point_2<K,Weight> &q,
		  const Moebius_point_2<K,Weight> &r,
		  const Moebius_point_2<K,Weight> &t)
{
  typedef typename K::Rep_tag Tag;
  return moebius_power_test(p,q,r,t, Tag());
}

template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K,Weight> &p,
		  const Moebius_point_2<K,Weight> &q,
		  const Moebius_point_2<K,Weight> &t)
{
  typedef typename K::Rep_tag Tag;
  return moebius_power_test(p,q,t, Tag());
}

template < class K, class Weight >
inline
Oriented_side
moebius_power_test(const Moebius_point_2<K,Weight> &p,
		  const Moebius_point_2<K,Weight> &q)
{
  typedef typename K::Rep_tag Tag;
  return moebius_power_test(p,q, Tag());
}

template <class Point, class Weight>
class Moebius_power_test_3
{
public:
  typedef Oriented_side  result_type;
  typedef CGAL::Moebius_point_2 <Point, Weight>        Weighted_point;

  Oriented_side operator() ( const Weighted_point & p,
			     const Weighted_point & q,
			     const Weighted_point & r,
			     const Weighted_point & s,
			     const Weighted_point & t) const
    {
      return CGAL::moebius_power_test(p,q,r,s,t);
    }
  Oriented_side operator() ( const Weighted_point & p,
			     const Weighted_point & q,
			     const Weighted_point & r,
			     const Weighted_point & s) const
    {
      return CGAL::moebius_power_test(p,q,r,s);
    }

  Oriented_side operator() ( const Weighted_point & p,
			     const Weighted_point & q,
			     const Weighted_point & r) const
    {
      return CGAL::moebius_power_test(p,q,r);
    }

  Oriented_side operator() ( const Weighted_point & p,
			     const Weighted_point & q) const
    {
      return CGAL::moebius_power_test(p,q);
    }
};

template <class K, class Weight>
class Moebius_compare_power_distance_3
{
public:
    typedef Comparison_result result_type;
    typedef CGAL::Moebius_point_2 <K, Weight>        Weighted_point;

    result_type operator() (const typename K::Point_2 &p, const Weighted_point &q, const Weighted_point &r) const
    {
	return _ (p.x(), p.y(),
		  q.x(), q.y(), q.lambda(), q.mu(),
		  r.x(), r.y(), r.lambda(), r.mu());
    }

private:
    template <class FT>
    static inline 
    result_type _ (const FT &x, const FT &y,
	    const FT &x1, const FT &y1, const FT &l1, const FT &m1,
	    const FT &x2, const FT &y2, const FT &l2, const FT &m2)
    {
	FT d1 = l1 * (square (x - x1) + square (y - y1)) - m1;
	FT d2 = l2 * (square (x - x2) + square (y - y2)) - m2;
	return compare (d1, d2);
    }
};


template <class K, class Weight = typename K::FT>
class Moebius_regular_triangulation_euclidean_traits_3 : public K
{
 public:
  typedef typename K::Point_2                        Bare_point;
  typedef CGAL::Moebius_point_2<K, Weight>     Weighted_point_3;
  typedef Weighted_point_3                             Point_3;

  // The next typedef is there for backward compatibility
  // Some users take their point type from the traits class.
  // Before this type was Point
  typedef Point_3 Point;

  typedef CGAL::Moebius_power_test_3<K, Weight> Power_test_3;
  typedef CGAL::Moebius_compare_power_distance_3<K, Weight> Compare_power_distance_3;
  typedef CGAL::Moebius_orientation_3<K, Weight> Orientation_3;
  typedef CGAL::Moebius_coplanar_orientation_3<K, Weight> Coplanar_orientation_3;
  typedef CGAL::Moebius_less_x_3<K, Weight> Less_x_3;
  typedef CGAL::Moebius_less_y_3<K, Weight> Less_y_3;
  typedef CGAL::Moebius_less_z_3<K, Weight> Less_z_3;
  typedef CGAL::Moebius_compare_xyz_3<K, Weight> Compare_xyz_3;

  Compare_power_distance_3 
  compare_power_distance_3_object() const
    {  return Compare_power_distance_3(); }

  Power_test_3 
  power_test_3_object() const
    {  return Power_test_3(); }

  Orientation_3
    orientation_3_object() const
    { return Orientation_3 (); }

  Coplanar_orientation_3
    coplanar_orientation_3_object() const
    { return Coplanar_orientation_3 (); }

  Less_x_3
    less_x_3_object() const
    { return Less_x_3 (); }

  Less_y_3
    less_y_3_object() const
    { return Less_y_3 (); }

  Less_z_3
    less_z_3_object() const
    { return Less_z_3 (); }

  Compare_xyz_3
    compare_xyz_3_object() const
    { return Compare_xyz_3 (); }
};



CGAL_END_NAMESPACE
#endif //  CHR_MOEBIUS_REGULAR_EUCLIDEAN_TRAITS_3_H
