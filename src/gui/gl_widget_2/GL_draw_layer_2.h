/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_draw_layer_2.h 586 2009-04-03 20:01:21Z miklosb $
 */

#ifndef GL_DRAW_LAYER_2_H
#define GL_DRAW_LAYER_2_H

#include <list>

#include <CGAL/basic.h> // for a certain reason it does not compile if I put it after QGLWidget !!! if you include this file before GL_widget, it does not compile!
#include <CGAL/Triangulation_2.h>
#include <CGAL/Simple_cartesian.h>

#include <gui/gl_widget_2/GL_widget_2.h> // needed for the templated drawing opertators
#include <gui/app/Managable_layer.h>

#include <CGAL/utility.h>

template <class R>
class Arc_2 : public CGAL::Triple<
						CGAL::Circle_2<R>,
						CGAL::Point_2<R>,
						CGAL::Point_2<R> > {

	typedef CGAL::Triple<CGAL::Circle_2<R>,CGAL::Point_2<R>,CGAL::Point_2<R> > Base;
public:
	Arc_2(CGAL::Circle_2<R> c, CGAL::Point_2<R> s, CGAL::Point_2<R> e) : Base(c,s,e) {}
};


class W_API GL_draw_layer_2 : public Managable_layer {
public:
	enum Fill_flagger { RENDER_FILL, RENDER_NO_FILL };
	enum Bounding_flagger { DO_BOUNDING, NO_BOUNDING };
	enum Scalar_value_flagger { DO_SCALAR, NO_SCALAR, DO_INVERSE_SCALAR };

	GL_draw_layer_2(const QString& n, const QString& tooltip = "");
	~GL_draw_layer_2();

	virtual void draw_commands() {};
	virtual void prepare_layer();

	virtual bool has_valid_cache();
	virtual void invalidate_cache();

	virtual QColor get_color();
	virtual void set_color(const QColor& q);
	virtual float get_point_size();
	virtual float get_line_width();

	virtual bool get_need_rescale_colormap();
	virtual void set_need_rescale_colormap(bool nrs);

	virtual QString get_image_file_name();
	virtual void set_image_file_name(QString s);

	virtual QByteArray saveState();
	virtual bool restoreState(const QByteArray &state);

	QString get_name();
	virtual QBrush symbol_brush();

	virtual bool has_property(Layer_property prop);
	Fill_flagger fill_flag;
	Bounding_flagger bounding_flag;
	Scalar_value_flagger scalar_flag;

	virtual void update_bounding_box(const QRectF& r);
	GLuint texture;
	void reset_range();

//public slots:
	virtual void set_point_size(float size);
	virtual void set_line_width(float width);
	QColor get_scalar_color(float val);

protected:
	QString name;
	QColor color;
	QString image_name;
	float line_width;
	float point_size;
	bool need_rescale_colormap;


private:
	static const qint32 layer_token = 0xB1;
	static const qint32 version = 0x02;
	virtual void draw();
	virtual void draw_into_cache();
	
	bool cache_has_data;

	GLuint display_list;
};

#ifdef MESECINA_GL_WIDGET
#define WOP_API   __declspec( dllexport )
#else
#define WOP_API 
#endif

// non-templated drawing operators
WOP_API GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const GL_draw_layer_2::Fill_flagger f);
WOP_API GL_draw_layer_2&  operator<<(GL_draw_layer_2& l, const GL_draw_layer_2::Bounding_flagger f);
WOP_API GL_draw_layer_2&  operator<<(GL_draw_layer_2& l, const GL_draw_layer_2::Scalar_value_flagger f);
WOP_API GL_draw_layer_2&  operator<<(GL_draw_layer_2& l, const float scalar);
WOP_API GL_draw_layer_2&  operator<<(GL_draw_layer_2& l, const QPixmap& pixmap);
WOP_API GL_draw_layer_2&  operator<<(GL_draw_layer_2& l, const QPolygonF& polygon);
WOP_API GL_draw_layer_2&  operator<<(GL_draw_layer_2& l, const QPointF& point);
WOP_API GL_draw_layer_2&  operator<<(GL_draw_layer_2& l, const QLineF& line);

// code called before every drawing to filter away things if scalar value has to be shown
#define SCALAR_FILTERING_CODE 	if (l.scalar_flag == GL_draw_layer_2::DO_SCALAR || l.scalar_flag == GL_draw_layer_2::DO_INVERSE_SCALAR) {\
		if ((l.scalar_min <= l.scalar_value) && (l.scalar_value <= l.scalar_max)) {\
			QColor col = l.get_scalar_color(l.scalar_value);\
			l.widget->set_color(col.redF(), col.greenF(), col.blueF(), col.alphaF());\
		}\
		else { l.get_scalar_color(l.scalar_value); return l; }\
	}


// templated drawing operators
template <class R>
WOP_API GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const CGAL::Point_2<R>& p) {
	SCALAR_FILTERING_CODE
	double x = CGAL::to_double(p.x());
	double y = CGAL::to_double(p.y());
	l.update_bounding_box(l.widget->draw_point(x,y));	
	return l;
};

template <class R>
WOP_API GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const Arc_2<R>& a) {
	SCALAR_FILTERING_CODE
	double orig_x = CGAL::to_double(a.first.center().x()),
		orig_y = CGAL::to_double(a.first.center().y()),
		radius = std::sqrt(CGAL::to_double(a.first.squared_radius()));
	double sx = CGAL::to_double(a.second.x());
	double sy = CGAL::to_double(a.second.y());
	double tx = CGAL::to_double(a.third.x());
	double ty = CGAL::to_double(a.third.y());

	double x1 = sx - orig_x;
	double y1 = sy - orig_y;
	double x2 = tx - orig_x;
	double y2 = ty - orig_y;
	static const double raddeg = 180 / 3.14159265358979;
	double da1 = /*raddeg **/ atan2 (y1, x1);
	double da2 = /*raddeg **/ atan2 (y2, x2);
//	std::cout << "ORIG da1: " << da1 << " da2: " << da2 << std::endl;

	double twopi = 2*3.14159265358979;
	double da = da2 - da1;
	while (da < 0) da = da + twopi;
	while (da > twopi) da = da - twopi;


	if (a.first.orientation () == CGAL::CLOCKWISE) { 
		da = twopi - da;
		da1 = da2;
	}

//	if (da2<da1) da2 = da2 + 3.14159265358979;
//	std::cout << "da1: " << da1 << " da: " << da << std::endl;

	l.update_bounding_box(l.widget->draw_arc(orig_x, orig_y, radius, da1, da));	
	return l;
};

template <class R>
WOP_API  GL_draw_layer_2&  operator<<(GL_draw_layer_2& l, const CGAL::Circle_2<R>& c) {
	SCALAR_FILTERING_CODE
	double orig_x = CGAL::to_double(c.center().x()),
		orig_y = CGAL::to_double(c.center().y()),
		radius = std::sqrt(CGAL::to_double(c.squared_radius()));
	if (l.fill_flag == GL_draw_layer_2::RENDER_FILL)
		l.update_bounding_box(l.widget->draw_disk(orig_x, orig_y, radius));		
	else
		l.update_bounding_box(l.widget->draw_circle(orig_x, orig_y, radius));		

	return l;
};

template <class R>
WOP_API GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const CGAL::Segment_2<R>& s) {
	SCALAR_FILTERING_CODE
	double x1, y1, x2, y2;
	x1 = CGAL::to_double(s.source().x());
	y1 = CGAL::to_double(s.source().y());
	x2 = CGAL::to_double(s.target().x());
	y2 = CGAL::to_double(s.target().y());
	l.update_bounding_box(l.widget->draw_segment(x1,y1,x2,y2));
	return l;
};

template <class R>
WOP_API GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const CGAL::Line_2<R>& line)
{
	//typedef CGAL::Simple_cartesian<double> Rep;
	//typedef CGAL::Point_2<Rep> Point;
	SCALAR_FILTERING_CODE
	const CGAL::Point_2<R> p1=line.point(), p2=CGAL::Point_2<R>(0,0)+line.direction().vector();
	double px, py, dirx, diry;
	px = CGAL::to_double(p1.x());
	py = CGAL::to_double(p1.y());
	dirx = CGAL::to_double(p2.x());
	diry = CGAL::to_double(p2.y());
	l.widget->draw_line(px,py,dirx,diry);
	return l;
};

template <class R>
WOP_API GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const CGAL::Ray_2<R>& r)
{
	SCALAR_FILTERING_CODE
	const CGAL::Point_2<R> p1=r.point(0), p2=r.point(1);
	double p1dx = CGAL::to_double(p1.x());
	double p1dy = CGAL::to_double(p1.y());
	double p2dx = CGAL::to_double(p2.x());
	double p2dy = CGAL::to_double(p2.y());

	l.update_bounding_box(l.widget->draw_ray(p1dx,p1dy,p2dx,p2dy));
	return l;
};

template <class Traits, class Tds>
WOP_API GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const CGAL::Triangulation_2<Traits,Tds>& t) {
	double x1, y1, x2, y2;
	l.widget->start_segments();
	typedef typename CGAL::Triangulation_2<Traits,Tds>::Finite_edges_iterator Finite_edges_iterator;
	typedef typename CGAL::Triangulation_2<Traits,Tds>::Face_handle Face_handle;
	Finite_edges_iterator e_it = t.finite_edges_begin(), e_end = t.finite_edges_end();
	for (; e_it != e_end; ++e_it) {
		Face_handle f = e_it->first;
		int id = e_it->second;
		x1 = CGAL::to_double(f->vertex((id+1)%3)->point().x());
		y1 = CGAL::to_double(f->vertex((id+1)%3)->point().y());
		x2 = CGAL::to_double(f->vertex((id+2)%3)->point().x());
		y2 = CGAL::to_double(f->vertex((id+2)%3)->point().y());
		l.update_bounding_box(l.widget->draw_segment(x1,y1,x2,y2,false));
	}
	l.widget->end_segments();
	return l;
};

template <class R>
WOP_API GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const CGAL::Triangle_2<R>& t) {
	SCALAR_FILTERING_CODE
	double x1, y1, x2, y2, x3, y3;
	x1 = CGAL::to_double(t.vertex(0).x());
	y1 = CGAL::to_double(t.vertex(0).y());
	x2 = CGAL::to_double(t.vertex(1).x());
	y2 = CGAL::to_double(t.vertex(1).y());
	x3 = CGAL::to_double(t.vertex(2).x());
	y3 = CGAL::to_double(t.vertex(2).y());
	l.update_bounding_box(l.widget->draw_triangle(x1,y1,x2,y2,x3,y3,true));
	return l;
};


#endif //GL_DRAW_LAYER_2_H