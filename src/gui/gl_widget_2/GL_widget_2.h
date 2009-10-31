/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_widget_2.h 533 2009-02-14 23:24:47Z miklosb $
 */

#ifndef GL_WIDGET_2_H
#define GL_WIDGET_2_H

//#include <CGAL/basic.h> // for a certain reson it does not compile if I put it after QGLWidget
//#include <CGAL/Simple_cartesian.h>

#include <QtGui/QtGui>
#include <vector>
#include <gui/gl_widget/Visualization_widget.h>

//class Computation_thread;

class Visualization_layer;

class W_API GL_widget_2 : public Visualization_widget {

	Q_OBJECT

public:
	enum Renderer_type { OPEN_GL, QT_PAINTER };
	enum Rendering_effect { RENDERING_NORMAL, RENDERING_PENCIL };

	GL_widget_2(QWidget * parent = 0);
	~GL_widget_2();

	void attach(Visualization_layer *l);
	void detach(Visualization_layer *l);

//	Computation_thread* computation_thread;

	virtual void export_to_image(QString filename);
	virtual void export_to_vector(QString file_name);
	virtual void print_to_printer();

	void paintGL ();
	void resizeGL ( int width, int height );
	void initializeGL ();

	void set_window(double x_min,
		double x_max, 
		double y_min, 
		double y_max);
	virtual void set_window_to_boundingbox();
	void smooth_zoom(double x_min, double x_max, double y_min, double y_max);
	void zoom(double ratio);
	void zoom(double ratio, double xc, double yc);

	void move_center(const double distx, const double disty);
	void set_center(const double x, const double y);

	void set_scales();
	void set_default_cursor(QCursor c);
	QCursor get_default_cursor();

	double x_pixel_d(double x);
	double y_pixel_d(double y);
	double x_pixel_dist_d(double d);
	double y_pixel_dist_d(double d);
	double x_real(int x) const;
	double y_real(int y) const;
	inline double x_min() const { return xmin; };
	inline double y_min() const { return ymin; };
	inline double x_max() const { return xmax; };
	inline double y_max() const { return ymax; };

	double get_bounding_xmin() { return bounding_xmin; }
	double get_bounding_xmax() { return bounding_xmax; }
	double get_bounding_ymin() { return bounding_ymin; }
	double get_bounding_ymax() { return bounding_ymax; }
	bool has_boundingbox() { 
		return !default_bounding_box; 
	}
	void update_bounding_box(const QRectF& r);

	friend class Visualization_layer;
	friend class GL_draw_layer_2;

	Renderer_type get_renderer_type();
	void set_renderer_type(Renderer_type r);

	QPainter* get_painter();
	void set_painter(QPainter* p);
	void paint_to_painter();
	void set_point_pen(QPen pen);
	void set_line_pen(QPen pen);

	virtual bool get_use_display_lists() { return use_display_lists; }
	void set_use_display_lists(bool v) { use_display_lists = v; }

	QRectF draw_disk(double orig_x, double orig_y, double radius);
	QRectF draw_circle(double orig_x, double orig_y, double radius);
	QRectF draw_arc(double orig_x, double orig_y, double radius, double start, double span);
	QRectF draw_point(double x, double y);
	QRectF draw_segment(double x1, double y1, double x2, double y2, bool with_begin_end = true);
	QRectF draw_line(double px, double py, double dirx, double diry, bool with_begin_end = true);
	QRectF draw_ray(double p1dx, double p1dy, double p2dx, double p2dy, bool with_begin_end = true);
	QRectF draw_polygon(const QPolygonF& polygon, bool with_begin_end = true);
	void start_segments();
	void end_segments();
	QRectF draw_pixmap(const QPixmap& pixmap, GLuint &texture);
	QRectF draw_triangle(double x1, double y1, double x2, double y2, double x3, double y3, bool with_begin_end = true);
	void start_triangles();
	void end_triangles();
	void set_color(float red, float green, float blue, float alpha);


	void add_point_to_selection(double x, double y);
	void add_line_to_selection(double x1, double y1, double x2, double y2);
	void add_circle_to_selection(double x, double y, double r);
	void remove_point_from_selection(double x, double y);
	void remove_line_from_selection(double x1, double y1, double x2, double y2);
	void remove_circle_from_selection(double x, double y, double r);

	void application_settings_changed(const QString& settings_name);
	void request_repaint();

public slots:
	virtual void repaintGL();
	// the selection slots below retransmit everything to the layers
	void ray_selection(double x, double y, bool* hit);
	void disk_selection(double x, double y, double radius, bool* hit);
	void box_selection(double x, double y, double width, double height, bool* hit);
	void selection_copied_by(double x, double y);
	void selection_moved_by(double x, double y);
	bool selection_deleted();
	void clear_selection();
	void save_view(bool);
	void load_view(bool);

signals:
	void s_mousePressEvent(QMouseEvent *e);
	void s_mouseReleaseEvent(QMouseEvent *e);
	void s_mouseMoveEvent(QMouseEvent *e);
	void s_paintEvent(QPaintEvent *e);
	void s_resizeEvent(QResizeEvent *e);
	void s_wheelEvent(QWheelEvent *e);
	void s_mouseDoubleClickEvent(QMouseEvent *e);
	void s_keyPressEvent(QKeyEvent *e);
	void s_keyReleaseEvent(QKeyEvent *e);
	void s_enterEvent(QEvent *e);
	void s_leaveEvent(QEvent *e);
	void s_event(QEvent *e);
	void mouse_at(const QPointF& pos);
	virtual void widget_resized_to(const QRect &size);
	void msg(QString a);
	virtual void repaitedGL();

	void add_point_to_selection(QPointF p);
	void add_line_to_selection(QLineF l);
	void add_circle_to_selection(QPointF c, float r);

	void points_modified();

	void remove_point_from_selection(QPointF p);
	void remove_line_from_selection(QLineF l);
	void remove_circle_from_selection(QPointF c, float r);

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

	void switch_render_effect();
	std::vector<Visualization_layer*> layers;

private:
	QCursor default_cursor;
	bool want_repaint;

	double xscal, yscal;
	double xmin, xmax, ymin, ymax;					// real dimensions
	double xmin_old, xmax_old, ymin_old, ymax_old; 	//backup ranges for resize
	double depth;

	bool default_bounding_box;
	double bounding_xmin, bounding_xmax, bounding_ymin, bounding_ymax;
	Renderer_type renderer_type;
	Rendering_effect rendering_effect;

	bool use_display_lists;

	QPainter* painter;
	QPen point_pen;
	QPen line_pen;

	QGLPixelBuffer* pbuffer; //pixelbuffer where the rendering should happen for the scene so that we can apply later some npr on the texture
	GLuint dynamic_texture, noise_texture, paper_texture; //the texture generated after the scene is rendered
	GLuint npr_shader_program;
	void init_common();

	QWidget* old_parent;
	QPoint prev_pos;
	void switch_full_screen();

};

#endif //GL_WIDGET_2_H