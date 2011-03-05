/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_widget_3.h 556 2009-03-24 12:35:12Z miklosb $
 */

#ifndef GL_WIDGET_3_H
#define GL_WIDGET_3_H

//#include <CGAL/basic.h> // for a certain reson it does not compile if I put it after QGLWidget
//#include <CGAL/Simple_cartesian.h>

#include <QtGui/QtGui>
//#include <QtOpenGL/QGLWidget>
#include <vector>
#include <gui/gl_widget/Visualization_widget.h>
#include <QGLViewer/qglviewer.h>
#include <gui/gl_widget_3/Box3D.h>
#include <gui/app/Point3D.h>

class Visualization_layer;

typedef QVector<Point3D> QPolygon3;

class W_API GL_widget_3 : public QGLViewer {

	Q_OBJECT

public:
	enum Renderer_type { OPEN_GL, QT_PAINTER };

	GL_widget_3(QWidget * parent = 0);
//	~GL_widget_3();
	GLuint p;

	void attach(Visualization_layer *l);
	void detach(Visualization_layer *l);

	virtual void export_to_image(QString filename);
	virtual void export_to_vector(QString file_name);
	virtual void print_to_printer();
	virtual void postSelection(const QPoint& point);

	bool has_boundingbox() { 
		return !default_bounding_box; 
	}
	void update_bounding_box(const Box3D& r);

	virtual void init();
	virtual void draw();
	void create_sphere_display_list();

	friend class Visualization_layer;
	friend class GL_draw_layer_3;

	virtual bool get_use_display_lists() {
		return 
			//false;
			use_display_lists;
	}
	void set_use_display_lists(bool v) { use_display_lists = v; }

	virtual void set_window_to_boundingbox();


	//QRectF draw_disk(double orig_x, double orig_y, double radius);
	Box3D draw_sphere(double orig_x, double orig_y, double orig_z, double radius);
	Box3D draw_sphere_wireframe(double orig_x, double orig_y, double orig_z, double radius);	
	Box3D draw_point(double x, double y, double z, bool with_begin_end = true);
	void set_normal(double x, double y, double z);
	void start_points();
	void end_points();
	Box3D draw_segment(double x1, double y1, double z1, double x2, double y2, double z2, bool with_begin_end = true);
	//QRectF draw_line(double px, double py, double dirx, double diry, bool with_begin_end = true);
	//QRectF draw_ray(double p1dx, double p1dy, double p2dx, double p2dy, bool with_begin_end = true);
	void start_segments();
	void end_segments();
	//QRectF draw_pixmap(const QPixmap& pixmap, GLuint &texture);
	Box3D draw_triangle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3,  double xn, double yn, double zn,  bool with_begin_end = true);
	Box3D draw_polygon(const QVector<Point3D>& polygon);
	Box3D draw_polygon_lines(const QVector<Point3D>& polygon);
	void start_triangles();
	void end_triangles();
	void set_color(float red, float green, float blue, float alpha);
//	inline GL_widget_3 operator this() { return QGLViewer::this; }

	void application_settings_changed(const QString& settings_name);

	void set_camera_projection();

	void add_point_to_selection(double x, double y, double z);
	void remove_point_from_selection(double x, double y, double z);
	void clear_selection();

public slots:
	void repaintGL();
	virtual void resizeGL( int w, int h );
	void save_view(bool);
	void load_view(bool);
	// the selection slots below retransmit everything to the layers

signals:

	void s_add_point_to_selection(double x, double y, double z);
	void s_remove_point_from_selection(double x, double y, double z);
	void s_clear_selection();

	//void s_mousePressEvent(QMouseEvent *e);
	//void s_mouseReleaseEvent(QMouseEvent *e);
	//void s_mouseMoveEvent(QMouseEvent *e);
	//void s_paintEvent(QPaintEvent *e);
	//void s_resizeEvent(QResizeEvent *e);
	//void s_wheelEvent(QWheelEvent *e);
	//void s_mouseDoubleClickEvent(QMouseEvent *e);
	//void s_keyPressEvent(QKeyEvent *e);
	//void s_keyReleaseEvent(QKeyEvent *e);
	//void s_enterEvent(QEvent *e);
	//void s_leaveEvent(QEvent *e);
	//void s_event(QEvent *e);
	void widget_resized_to(const QRect &size);
	//void msg(QString a);
	virtual void repaitedGL();
	void settings_changed();

public:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	bool event(QEvent *e);
	void request_repaint();


private:
	std::vector<Visualization_layer*> layers;
	QCursor default_cursor;
	bool want_repaint;

	int sphere_slices, sphere_stacks;


	bool default_bounding_box;
	double bounding_xmin, bounding_xmax, bounding_ymin, bounding_ymax, bounding_zmin, bounding_zmax;

	bool use_display_lists;
	GLint sphere_dl;
	GLint sphere_wire_dl;

};

#endif //GL_WIDGET_3_H