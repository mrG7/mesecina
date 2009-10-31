#ifndef CHR_MOEBIUS_POINT_H
#define CHR_MOEBIUS_POINT_H

#include <CGAL/basic.h>

CGAL_BEGIN_NAMESPACE

template <class K, class Weight = typename K::FT>
class Moebius_point_2 : public K::Point_2
{
 public:
     typedef typename K::Point_2 Point;
     typedef typename K::FT FT;

  Moebius_point_2 (const Point &p = Point (),
		  const Weight &lambda = Weight (1),
		  const Weight &mu = Weight (0))
    : Point (p), _lambda (lambda), _mu (mu) {}

  
 // const FT &x() const { return _p.x(); }
 // const FT &y() const { return _p.y(); }

  const Point &point () const { return *this; }

  Weight lambda () const {  return _lambda; }
  Weight mu () const {  return _mu; }

 private:
  Weight _lambda, _mu;
};

template <class K, class W>
std::ostream & operator<< (std::ostream & os,
			   const Moebius_point_2<K,W> &p)
{
  return os << p.point() << ' ' << p.lambda() << ' ' << p.mu();
}

template <class K, class W>
std::istream & operator>> (std::istream & is,
			   Moebius_point_2<K,W> &p)
{
  W lambda, mu;
  typename K::Point_2 _p;
  is >> _p >> lambda >> mu;
  p = Moebius_point_2<K,W> (_p, lambda, mu);
  return is;
}



CGAL_END_NAMESPACE



#endif// CHR_MOEBIUS_POINT_H
