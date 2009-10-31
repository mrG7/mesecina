/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_insert_point_layer_2.h 123 2007-07-01 21:43:01Z miklosb $
 */

#ifndef GL_INSER_POINT_LAYER_H
#define GL_INSER_POINT_LAYER_H

#include <gui/gl_widget/Visualization_layer.h>

#include <QtGui/QMouseEvent>
#include <QtCore/QPointF>

#include <list>

class W_API GL_insert_point_layer_2 : public Visualization_layer {
	Q_OBJECT
public:
	enum State {NONE, SPRAY};
	GL_insert_point_layer_2();
	~GL_insert_point_layer_2();

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void keyPressEvent(QKeyEvent *);
	virtual void keyReleaseEvent(QKeyEvent *);
signals:
	void point_created(double x, double y);
	void points_created(std::list<QPointF>* points);
	void add_point_to_selection(QPointF p);
	void clear_points_from_selection();

private:
	std::list<QPointF> points_buffer;
	State state;
	int last_spray_x, last_spray_y;
	int min_squared_distance;
};

#endif //GL_INSER_POINT_LAYER_H