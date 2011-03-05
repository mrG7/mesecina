/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_draw_layer_3.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef GL_DRAW_LAYER_3_H
#define GL_DRAW_LAYER_3_H

#include <list>

#include <CGAL/basic.h> // for a certain reason it does not compile if I put it after QGLWidget !!! if you include this file before GL_widget, it does not compile!
#include <CGAL/Triangulation_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
//#include <fstream>
//#include <CGAL/IO/Polyhedron_iostream.h>

#include <gui/gl_widget_3/GL_widget_3.h>
#include <gui/app/Managable_layer.h>
#include <constants.h>

//#include "skin_surface_writer.h"


class W_API GL_draw_layer_3 : public Managable_layer {
public:
	enum Fill_flagger { RENDER_FILL, RENDER_NO_FILL };
	enum Bounding_flagger { DO_BOUNDING, NO_BOUNDING };
	enum Render_hint { NONE, SEGMENTS_START, SEGMENTS_END, TRIANGLES_START, TRIANGLES_END, POINTS_START, POINTS_END, NORMAL };
	enum Scalar_value_flagger { DO_SCALAR, NO_SCALAR, DO_INVERSE_SCALAR };

//	GL_draw_layer_3(const QString& n);
	GL_draw_layer_3(const QString& n, const QString& ttip);
	~GL_draw_layer_3();

	virtual void prepare_draw() {};
	virtual void unprepare() {};
	virtual void draw_commands() {};

	virtual bool has_valid_cache();
	virtual void invalidate_cache();
	virtual void draw();
	virtual void draw_into_cache();

	virtual QColor get_color();
	virtual void set_color(const QColor& q);

	virtual QColor get_ambient();
	virtual void set_ambient( const QColor &q);

	virtual QColor get_diffuse();
	virtual void set_diffuse( const QColor &q);

	virtual QColor get_specular();
	virtual void set_specular( const QColor &q);

	virtual QColor get_emission();
	virtual void set_emission( const QColor &q);

	virtual float  get_shininess();
	virtual void set_shininess( float i);

	virtual float get_width(); 
	virtual void set_width(float w);

	virtual bool get_skip_clipping_planes();
	virtual void set_skip_clipping_planes(bool skip);

	virtual bool get_use_display_list();
	virtual void set_use_display_list(bool use);

	virtual bool get_need_rescale_colormap();
	virtual void set_need_rescale_colormap(bool nrs);


	virtual float get_point_size();
	virtual float get_line_width();
	virtual QString get_image_file_name();
	virtual void set_image_file_name(QString s);
	virtual void set_point_size(float size);
	virtual void set_line_width(float width);
	QColor get_scalar_color(float val);

	virtual QByteArray saveState();
	virtual bool restoreState(const QByteArray &state);

	QString get_name();
	virtual QBrush symbol_brush();

	virtual bool has_property(Layer_property prop);
	Fill_flagger fill_flag;
	Bounding_flagger bounding_flag;
	Render_hint render_hint;
	Scalar_value_flagger scalar_flag;

	virtual void update_bounding_box(const Box3D& r);
	GLuint texture;

	virtual void application_settings_changed(const QString& settings_name);




//public slots:


protected:
	QString name;
	QColor color;
	QColor ambient, diffuse, specular, emission;
	QString image_name;
	float line_width;
	float point_size;
	float shininess;
	float width;
	bool skip_planes, use_display_list;
	bool need_rescale_colormap;

private:
	static const qint32 layer_token = 0xC1;
	static const qint32 version = 0x02;

	
	bool cache_has_data;

	GLuint display_list;
	bool is_prepared;
};

#define SCALAR_FILTERING_CODE 	if (l.scalar_flag == GL_draw_layer_3::DO_SCALAR || l.scalar_flag == GL_draw_layer_3::DO_INVERSE_SCALAR) {\
		if ((l.scalar_min <= l.scalar_value) && (l.scalar_value <= l.scalar_max)) {\
			QColor col = l.get_scalar_color(l.scalar_value);\
			l.widget->set_color(col.redF(), col.greenF(), col.blueF(), col.alphaF());\
		} else { l.get_scalar_color(l.scalar_value); return l; }\
	}

#ifdef MESECINA_GL_WIDGET
#define WOP_API   __declspec( dllexport )
#else
#define WOP_API 
#endif

template <class R>
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const CGAL::Point_3<R>& p) {
	SCALAR_FILTERING_CODE
	double x = CGAL::to_double(p.x());
	double y = CGAL::to_double(p.y());
	double z = CGAL::to_double(p.z());
	if (l.render_hint == GL_draw_layer_3::NORMAL) {
		l.widget->set_normal(x,y,z);
		l.render_hint = GL_draw_layer_3::NONE;
	}
	else {
		l.update_bounding_box(l.widget->draw_point(x,y,z,l.render_hint != GL_draw_layer_3::POINTS_START));	
	}
	return l;
};
//
//template <class R>
//GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const CGAL::Circle_2<R>& c) {
//	SCALAR_FILTERING_CODE
//	double orig_x = CGAL::to_double(c.center().x()),
//		orig_y = CGAL::to_double(c.center().y()),
//		radius = std::sqrt(CGAL::to_double(c.squared_radius()));
//	if (l.fill_flag == GL_draw_layer_2::RENDER_FILL)
//		l.update_bounding_box(l.widget->draw_disk(orig_x, orig_y, radius));		
//	else
//		l.update_bounding_box(l.widget->draw_circle(orig_x, orig_y, radius));		
//
//	return l;
//};
//
template <class R>
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const CGAL::Segment_3<R>& s) {
	SCALAR_FILTERING_CODE
	double x1, y1, z1, x2, y2, z2;
	x1 = CGAL::to_double(s.source().x());
	y1 = CGAL::to_double(s.source().y());
	z1 = CGAL::to_double(s.source().z());
	x2 = CGAL::to_double(s.target().x());
	y2 = CGAL::to_double(s.target().y());
	z2 = CGAL::to_double(s.target().z());
	l.update_bounding_box(l.widget->draw_segment(x1,y1,z1,x2,y2,z2, l.render_hint != GL_draw_layer_3::SEGMENTS_START));
	return l;
};

template <class R>
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const CGAL::Line_3<R>& line)
{
	//typedef CGAL::Simple_cartesian<double> Rep;
	//typedef CGAL::Point_2<Rep> Point;
	SCALAR_FILTERING_CODE
	const CGAL::Point_3<R> p1=line.point(), p2=CGAL::Point_3<R>(0,0)+line.direction().vector();
	double px, py, pz, dirx, diry, dirz;
	px = CGAL::to_double(p1.x());
	py = CGAL::to_double(p1.y());
	dirx = CGAL::to_double(p2.x());
	diry = CGAL::to_double(p2.y());
//	l.widget->draw_line(px,py,pz,dirx,diry,dirz);
	return l;
};

template <class R>
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const CGAL::Ray_3<R>& r)
{
	SCALAR_FILTERING_CODE
	const CGAL::Point_3<R> p1=r.point(0), p2=r.point(1);
	double p1dx = CGAL::to_double(p1.x());
	double p1dy = CGAL::to_double(p1.y());
	double p2dx = CGAL::to_double(p2.x());
	double p2dy = CGAL::to_double(p2.y());

//	l.update_bounding_box(l.widget->draw_ray(p1dx,p1dy,p2dx,p2dy));
	return l;
};


WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const GL_draw_layer_3::Fill_flagger f);
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const GL_draw_layer_3::Bounding_flagger f);
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const GL_draw_layer_3::Render_hint f);
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const GL_draw_layer_3::Scalar_value_flagger f);
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const float scalar);

WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const QVector<Point3D>& polygon);

template <class R>
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const std::list<CGAL::Point_3<R> >& s) {
	SCALAR_FILTERING_CODE

	QVector<Point3D> pol;
	std::list<CGAL::Point_3<R> >::const_iterator p_it, p_end = s.end();
	for (p_it = s.begin(); p_it!=p_end; ++p_it) {
		pol.append(Point3D(CGAL::to_double(p_it->x()), CGAL::to_double(p_it->y()), CGAL::to_double(p_it->z())));
	}
	if (l.fill_flag == GL_draw_layer_3::RENDER_NO_FILL) l.update_bounding_box(l.widget->draw_polygon_lines(pol));
	else l.update_bounding_box(l.widget->draw_polygon(pol));
	return l;
}


template <class Traits, class Tds>
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const CGAL::Triangulation_3<Traits,Tds>& t) {
	double x1, y1, z1, x2, y2, z2;
	l.widget->start_segments();
	typedef typename CGAL::Triangulation_3<Traits,Tds>::Finite_edges_iterator Finite_edges_iterator;
	typedef typename CGAL::Triangulation_3<Traits,Tds>::Cell_handle Cell_handle;
	Finite_edges_iterator e_it = t.finite_edges_begin(), e_end = t.finite_edges_end();
	for (; e_it != e_end; ++e_it) {
		Cell_handle f = e_it->first;
		int id = e_it->second;
		int id2 = e_it->third;
		x1 = CGAL::to_double(f->vertex((id))->point().x());
		y1 = CGAL::to_double(f->vertex((id))->point().y());
		z1 = CGAL::to_double(f->vertex((id))->point().z());
		x2 = CGAL::to_double(f->vertex((id2))->point().x());
		y2 = CGAL::to_double(f->vertex((id2))->point().y());
		z2 = CGAL::to_double(f->vertex((id2))->point().z());
		l.update_bounding_box(l.widget->draw_segment(x1,y1,z1,x2,y2,z2,false));
	}
	l.widget->end_segments();
	return l;
};

template <class Traits, class DS>
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, CGAL::Polyhedron_3<Traits, DS>& p) {
	double x[4], y[4], z[4];
	if (!p.is_pure_triangle()) {
		std::cout << LOG_ERROR << "Polyhedron to be rendered is not triangular" << std::endl;
		return l;
	}
	typedef typename CGAL::Polyhedron_3<Traits, DS> Polyhedron;
	typedef typename Polyhedron::Vertex_iterator                  Vertex_iterator;
	typedef typename Polyhedron::Facet_iterator                   Facet_iterator;
	typedef typename Polyhedron::Edge_iterator                    Edge_iterator;
	typedef typename Polyhedron::Halfedge						  Halfedge;
	typedef typename Polyhedron::Halfedge_around_facet_circulator HFC;
	typedef typename Polyhedron::Vertex_handle                    Vertex_handle;
	typedef typename Polyhedron::Traits::Vector_3                 Vector;
	typedef typename Polyhedron::Traits::Point_3                  Point;

	//typedef typename CGAL::Polyhedron_3<Traits, DS>::Facet_iterator  Facet_iterator ;
	//typedef typename CGAL::Polyhedron_3<Traits, DS>::Facet_handle  Facet_handle;
	//typedef typename CGAL::Polyhedron_3<Traits, DS>::Halfedge_around_facet_circulator   Halfedge_around_facet_circulator ;
//	typedef typename CGAL::Polyhedron_3<Traits>::Point   Point;
	if (l.fill_flag == GL_draw_layer_3::RENDER_FILL) {
//		std::cout << PROGRESS_STATUS << "Render polyhedron with this loop function" << std::endl;
		l.widget->start_triangles();
		int ind = 0, i;	Point pts[3];
		for(Facet_iterator fi = p.facets_begin();
			fi != p.facets_end(); ++fi, ++ind) {
				//std::cout << PROGRESS_VALUE << ind << std::endl;
				HFC hc = fi->facet_begin();
				HFC hc_end = hc;
//				std::size_t n = circulator_size( hc);
				i = 0;
				//std::cout << n;
				do {
					pts[i] = (*hc).vertex()->point();
					x[i] = CGAL::to_double(pts[i].x());
					y[i] = CGAL::to_double(pts[i].y());
					z[i] = CGAL::to_double(pts[i].z());
					i++;
					//out << " " << index[vh];
				} while (++hc != hc_end);
				Vector v12 = Vector(pts[1], pts[2]);
				Vector v10 = Vector(pts[1], pts[0]);
				Vector normal = CGAL::cross_product(v12, v10);
				double length = sqrt(CGAL::to_double(normal*normal));
				x[3] = CGAL::to_double(normal.x())/length;
				y[3] = CGAL::to_double(normal.y())/length;
				z[3] = CGAL::to_double(normal.z())/length;
				l.update_bounding_box(l.widget->draw_triangle(x[0],y[0],z[0],x[1],y[1],z[1],x[2],y[2],z[2],x[3],y[3],z[3],false));
		}
		l.widget->end_triangles();
//		std::cout << PROGRESS_DONE  << std::endl;
	} else {
//		std::cout << PROGRESS_STATUS << "Render polyhedron with this loop function" << std::endl;
		l.widget->start_segments();
		//int ind = 0, i;	
		Point pts[3];
		for(Edge_iterator fi = p.edges_begin (); fi != p.edges_end(); ++fi) {
			pts[0] = fi->vertex()->point();
			x[0] = CGAL::to_double(pts[0].x());
			y[0] = CGAL::to_double(pts[0].y());
			z[0] = CGAL::to_double(pts[0].z());
			pts[1] = fi->opposite()->vertex()->point();
			x[1] = CGAL::to_double(pts[1].x());
			y[1] = CGAL::to_double(pts[1].y());
			z[1] = CGAL::to_double(pts[1].z());
			l.update_bounding_box(l.widget->draw_segment(x[0],y[0],z[0],x[1],y[1],z[1],false));
		}
		l.widget->end_segments();
//		std::cout << PROGRESS_DONE  << std::endl;
	}


	//CGAL::Point_3<Traits> pts[3];
	//Facet_iterator f_it = p.facets_begin(), f_end = p.facets_end(); int ind = 0;
	//for (; f_it != f_end; ++f_it, ++ind) {
	//	//CGAL::Vector_3<Traits> normal = f_it->plane().orthogonal_vector();
	//	//x[3] = CGAL::to_double(normal.x());
	//	//y[3] = CGAL::to_double(normal.y());
	//	//z[3] = CGAL::to_double(normal.z());
	//	std::cout << PROGRESS_VALUE << ind << std::endl;

	//	Halfedge_around_facet_circulator he_it = f_it->facet_begin();
	//	Halfedge_around_facet_circulator he_end = he_it; int i=0;
 //       do {
	//		pts[i] = he_it->vertex()->point();
	//		x[i] = CGAL::to_double(pts[i].x());
	//		y[i] = CGAL::to_double(pts[i].y());
	//		z[i] = CGAL::to_double(pts[i].z());
 //           ++he_it; ++i;
 //       } while( he_it != he_end /*&& i < 3*/);
	//	
	//	CGAL::Vector_3<Traits> v12 = CGAL::Vector_3<Traits>(pts[1], pts[2]);
	//	CGAL::Vector_3<Traits> v10 = CGAL::Vector_3<Traits>(pts[1], pts[0]);
	//	CGAL::Vector_3<Traits> normal = CGAL::cross_product(v12, v10);
	//	x[3] = CGAL::to_double(normal.x());
	//	y[3] = CGAL::to_double(normal.y());
	//	z[3] = CGAL::to_double(normal.z());
	//	
	//	std::cout << "draw triangle" << std::endl;
	//	l.update_bounding_box(l.widget->draw_triangle(x[0],y[0],z[0],x[1],y[1],z[1],x[2],y[2],z[2],x[3],y[3],z[3],false));
	//}

	return l;
};

template <class R>
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const CGAL::Triangle_3<R>& t) {
	SCALAR_FILTERING_CODE
	double x1, y1, z1, x2, y2, z2, x3, y3, z3, xn, yn, zn;
	x1 = CGAL::to_double(t.vertex(0).x());
	y1 = CGAL::to_double(t.vertex(0).y());
	z1 = CGAL::to_double(t.vertex(0).z());
	x2 = CGAL::to_double(t.vertex(1).x());
	y2 = CGAL::to_double(t.vertex(1).y());
	z2 = CGAL::to_double(t.vertex(1).z());
	x3 = CGAL::to_double(t.vertex(2).x());
	y3 = CGAL::to_double(t.vertex(2).y());
	z3 = CGAL::to_double(t.vertex(2).z());
	CGAL::Vector_3<R> normal = t.supporting_plane().orthogonal_vector();
	xn = CGAL::to_double(normal.x());
	yn = CGAL::to_double(normal.y());
	zn = CGAL::to_double(normal.z());
	l.update_bounding_box(l.widget->draw_triangle(x1,y1,z1,x2,y2,z2,x3,y3,z3,xn,yn,zn,l.render_hint != GL_draw_layer_3::TRIANGLES_START));
	return l;
};

template <class R>
WOP_API GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const CGAL::Sphere_3<R>& s) {
	SCALAR_FILTERING_CODE
	double x, y, z, radius;
	x = CGAL::to_double(s.center().x());
	y = CGAL::to_double(s.center().y());
	z = CGAL::to_double(s.center().z());
	radius = sqrt(CGAL::to_double(s.squared_radius()));
	if (l.fill_flag == GL_draw_layer_3::RENDER_NO_FILL) l.update_bounding_box(l.widget->draw_sphere_wireframe(x,y,z,radius));
	else l.update_bounding_box(l.widget->draw_sphere(x,y,z,radius));
	return l;
};

#endif //GL_DRAW_LAYER_3_H