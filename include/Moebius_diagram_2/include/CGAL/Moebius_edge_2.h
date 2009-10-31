#ifndef CHR_MOEBIUS_EDGE_2_H
#define CHR_MOEBIUS_EDGE_2_H

#include <CGAL/Moebius_utils.h>
#include <cmath>
#include <cassert>
CGAL_BEGIN_NAMESPACE


template <class Gt>
class Moebius_edge_2
{
 public:
  typedef typename Gt::Bare_point_2 Point;
  typedef typename Gt::Line_2 Line;
  typedef typename Gt::Vector_2 Vector;
  typedef typename Gt::Circle_2 Circle;
  typedef typename Gt::Segment_2 Segment;
  typedef typename Gt::Ray_2 Ray;
  typedef typename Gt::Direction_2 Direction;
  
  typedef std::list<Point *> List;
  typedef typename List::iterator Iterator;
  typedef typename List::const_iterator Const_iterator;

  //  typedef Gt::Segment_Circle_2 Arc;

  bool are_ordered (const Point &p, const Point &q) const {
    CGAL_precondition (is_line ());
    return _gt.compare_on_oriented_line_2_object () (line (), p, q);
  }

 public:
  Moebius_edge_2 () { CGAL_assertion (false); }

  Moebius_edge_2 (const Line &bisector) : 
    _is_line (true), _start (), _stop (), _gt () {
    _bisec = (void *) new Line (bisector);
  }

  Moebius_edge_2 (const Circle &bisector) : 
    _is_line (false), _start (), _stop (), _gt () {
    _bisec = (void *) new Circle (bisector);
  }

  void start (const Point &p) {
  
    if (_start == NULL) {
   
      _start = new Point (p);
    } else {
      CGAL_assertion (_stop != NULL);
      
      _points.push_front (_stop);
    
      _stop = NULL;
     
      _points.push_front (new Point (p));
    }
  }

  void stop (const Point &p) {
   
    if (_stop == NULL) {
      
      _stop = new Point (p);
    } else {
      CGAL_assertion (_start != NULL);
    
      _points.push_front (new Point (p));
     
      _points.push_front (_start);
     
      _start = NULL;
    }
  }
  
  bool is_line () const { return _is_line; }
  bool is_circle () const { return ! _is_line; }

  const Circle &circle () const { CGAL_assertion (is_circle ()); return *((Circle *) _bisec); }
  const Line &line () const { CGAL_assertion (is_line ()); return *((Line *) _bisec); }


  List points () const { return _points; }
  Const_iterator begin () const { return _points.begin (); }
  Const_iterator end () const { return _points.end (); }

  template <class OutputIterator>
  OutputIterator sample (const double &epsilon, OutputIterator o) const
  {
     
      if (_is_line) {
		//std::cout << "SAMPLE LINE" << std::endl;
		Direction dir (line ());
		Vector v =dir.vector();
		double lon=CGAL::sqrt(v.x()*v.x()+v.y()*v.y());
		v=(2.0/lon)*v;//d/|v| *v => normalized
		for (Const_iterator i = begin(); i != end(); ++i) {
			Point *start = *(i);
    		CGAL_assertion ((++i) != end());
    		Point *stop = *(i);
    		Point a;
			double s=2.0/epsilon; //  d/0.05=40;
			double ss=1.0 / s; //0.025
			if (start == NULL && stop == NULL ) {std::cout << "LINE NULL NULL" << std::endl;return o;}
    		if (start == NULL) {
				//v=-dir.vector();
				//v=5*v;
				a = *stop;
				for (int j = 1; j < s ; ++j) {
					*o++ = a - v * (j * ss);
				}
			}	
			else if (stop == NULL) {
			//v=5*v;
			a = *start;
			for (int j = 1; j < s ; ++j) {
				*o++ = a + v * (j * ss);
			}
			}
			else  {
			a = *start;
			Point b = *stop;
			//std::cout << " d/epsilon = "  << (d / epsilon)  <<" and its floor s = "  << (s)  <<" ." <<std::endl;
			//std::cout << "GOOD LINE" << std::endl;
			Direction dir2= ((b - a).direction());
			Vector v2=dir2.vector();
			double sin1=v.x()/v.y();
			double sin2=v2.x()/v2.y();
			v= b - a;
			if(sin1*sin2>0){//they are ordered
				double d = typename Gt::Kernel().compute_squared_distance_2_object() (a, b);
				d = CGAL::sqrt (d);
				s= (d / epsilon);
				ss= (epsilon / d);
				for (int j = 1; j < s ; ++j) {
					*o++ = a + v * (j * ss);
				}
			}
			else{//not ordered
				//std::cout << "NOT ORDERED LINE" << std::endl;
 				//v=5*v;
				for (int j = 1; j < s ; ++j) {
					*o++ = a - v * (j * ss);
				}
			 	for (int j = 1; j < s ; ++j) {
					*o++ = b + v * (j * ss);
				}
			}
			
			} 

	  }
      } else {
	  //std::cout << "SAMPLE CIRCLE" << std::endl;
	  for (Const_iterator i = begin(); i != end(); ++i) {
	      
		Point *start = *(i);
    	CGAL_assertion ((++i) != end());
    	Point *stop = *(i);
		if (start == NULL || stop == NULL ) {std::cout << "CIRCLE NULL or NULL" << std::endl;return o;}
		Point a = *start;
		Point b = *stop;
// 		if (*i == NULL  ) return o;
// 			const Point &a = **i;
// 	      ++i;
// 		if (*i == NULL  ) return o;
// 	      const Point &b = **i;

	      double xa = a.x() - circle().center().x();
	      double ya = a.y() - circle().center().y();

	      double angle_a = atan2 (ya, xa);

	      double xb = b.x() - circle().center().x();
	      double yb = b.y() - circle().center().y();

	      double angle_b = atan2 (yb, xb);
	      if (angle_b < angle_a) angle_b += 2 * 3.1416;

	      double r = sqrt (circle().squared_radius());

	      int jmax = (r * (angle_b - angle_a) / epsilon) ;//std::floor (r * (angle_b - angle_a) / epsilon) ;
		// std::cout << "jmax= " << jmax << " . "<< std::endl;
		jmax=20;
	      for (int j = 1; j < jmax; ++j) {
//		  std::cout << "SAMPLE POINT CIRCLE" << std::endl;
			
		  //double angle = angle_a + epsilon / (r * (angle_b - angle_a));
			double angle = angle_a + (j* (angle_b - angle_a)/ jmax);
		  double x = r * cos (angle);
		  double y = r * sin (angle);

		  *o++ = circle().center() + Vector (x, y);
	      }
		 //std::cout << "FINISH SAMPLE CIRCLE" << std::endl;

	  }

	  
      }
      return o;
  }

#ifdef CGAL_USE_QT
  void draw (Qt_widget *widget) const {
    *widget << *this;
  }
#endif

  void finalize () {
    if (_start == NULL && _stop == NULL) return;
    //TRACE ("    finalize, ");

    //if (_stop == NULL) TRACE ("pushing NULL");
    //else TRACE ("pushing ("<<*_stop<<")");
    _points.push_front (_stop);

    //if (_start == NULL) TRACE (" and NULL\n");
    //else TRACE (" and ("<<*_start<<")\n");
    _points.push_front (_start);

    _start = _stop = NULL;
  }

 private:
  bool _is_line;
  Point *_start;
  Point *_stop;
  Gt _gt;
  void *_bisec;
  List _points;
};

#ifdef CGAL_USE_QT

CGAL_END_NAMESPACE
#include <CGAL/basic.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Point_2.h>
#include <CGAL/Segment_2.h>
CGAL_BEGIN_NAMESPACE

// workaround : in Qt_widget << Segment, ambiguous call to to_double...
template <class Point>
CGAL::Segment_2<CGAL::Cartesian<double> >
my_segment (const Point &source, const Point &target)
{
  typedef CGAL::Segment_2<CGAL::Cartesian<double> > S;
  typedef CGAL::Point_2<CGAL::Cartesian<double> > P;
  double x1 = CGAL::to_double (source.x());
  double y1 = CGAL::to_double (source.y());
  double x2 = CGAL::to_double (target.x());
  double y2 = CGAL::to_double (target.y());
  return S (P (x1, y1), P (x2, y2));
}


template <class Gt>
Qt_widget& operator<<(Qt_widget& widget, const Moebius_edge_2<Gt>& e)
{
  typedef Moebius_edge_2<Gt> Edge;
  typedef typename Edge::Point Point;
  typedef typename Edge::Ray Ray;
  typedef typename Edge::Direction Direction;

  typedef typename Edge::Const_iterator Iterator;
  Iterator it = e.begin ();
  Iterator end = e.end ();

  //TRACE ("Moebius_edge << Qt, ");
  //if (e.is_line ()) TRACE ("line: " << e.line ());
  //else TRACE ("circle: " << e.circle ());
  //TRACE ("\n");
  if (it == end) {
    //TRACE ("  whole bisector\n");
    if (e.is_line ()) {
      widget << e.line ();
    } else {
      widget << e.circle ();
    }
  }
  
  while (it != end) {
    Point *start = *(it++);
    CGAL_assertion (it != end);
    Point *stop = *(it++);
    
    if (e.is_line ()) {
      Direction dir (e.line ());
      if (start == NULL) {
	CGAL_assertion (stop != NULL);
	//TRACE ("  ray ("<<*stop<<") ("<<-dir<<")\n");
	widget << Ray (*stop, -dir);
      } else if (stop == NULL) {
	//TRACE ("  ray ("<<*start<<") ("<<dir<<")\n");
	widget << Ray (*start, dir);
      } else if (e.are_ordered (*start, *stop)) {
	//TRACE ("  segment ("<<*start<<") ("<<*stop<<")\n");
	widget << my_segment<Point> (*start, *stop);
      } else {
	//TRACE ("  two half lines ("<<*start<<") ("<<*stop<<")\n");
	widget
	  << Ray (*start, dir)
	  << Ray (*stop, -dir);
      }
    } else {
      CGAL_assertion (start != NULL);
      CGAL_assertion (stop != NULL);
      //TRACE ("  arc ("<<*start<<") -> ("<<*stop<<")\n");
      qt_draw_arc (widget, e.circle (), *start, *stop);
    }
  }
  return widget;
}
#endif

CGAL_END_NAMESPACE
#endif//CHR_MOEBIUS_EDGE_2_H
