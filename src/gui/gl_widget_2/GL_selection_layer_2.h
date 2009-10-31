/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_selection_layer_2.h 105 2007-05-22 14:11:04Z miklosb $
 */

#ifndef GL_SELECTION_LAYER_H
#define GL_SELECTION_LAYER_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

#include <QtCore/QPointF>
#include <QtCore/QLineF>
#include <QtGui/QRubberBand>

#include <list>

class W_API GL_selection_layer_2 : public GL_draw_layer_2 {
	Q_OBJECT
public:
	enum Status { NONE, MOVE, BOX_SELECTION };
	GL_selection_layer_2(const QString& n);
	~GL_selection_layer_2();

	virtual void draw();

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void keyPressEvent(QKeyEvent *);
	virtual void keyReleaseEvent(QKeyEvent *);

public slots:
	int add_point(QPointF p);
	void remove_point(QPointF p);
	void clear_points();
	int add_line(QLineF l);
	void remove_line(QLineF l);
	void clear_lines();
	int add_circle(QPointF c, float r);
	void remove_circle(QPointF c, float r);
	void clear_circles();
	void clear_all();

signals:
	void selection_was_deleted();
//	void ray_selection(double x, double y, bool* hit);
//	void disk_selection(double x, double y, double radius, bool* hit);
//	void box_selection(double x, double y, double width, double height, bool* hit);


private:
	std::list<QLineF> lines;
	std::list<QPointF> points;
	std::list<QPointF> centers;
	std::list<float> radii;

	int last_x;
	int last_y;
	int first_x;
	int first_y;

	Status status;

	QPoint rubber_band_origin;
	QRubberBand* rubber_band;
	QWindowsStyle* windows_style;
};

#endif //GL_SELECTION_LAYER_H