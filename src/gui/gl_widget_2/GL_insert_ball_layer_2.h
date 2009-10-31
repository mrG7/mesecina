/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_insert_point_layer_2.h 123 2007-07-01 21:43:01Z miklosb $
 */

#ifndef GL_INSER_BALL_LAYER_H
#define GL_INSER_BALL_LAYER_H

#include <gui/gl_widget/Visualization_layer.h>
#include <gui/app/Point3D.h>

#include <QtGui/QMouseEvent>
#include <QtCore/QPointF>

#include <list>

class W_API GL_insert_ball_layer_2 : public Visualization_layer {
	Q_OBJECT
public:
	enum State {NONE, SPRAY, CIRCLE};
	GL_insert_ball_layer_2();
	~GL_insert_ball_layer_2();

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void keyPressEvent(QKeyEvent *);
	virtual void keyReleaseEvent(QKeyEvent *);
signals:
	void ball_created(Point3D ball);
	void balls_created(std::list<Point3D>* balls);
	void add_ball_to_selection(QPointF, float);
	void add_point_to_selection(QPointF);
	void clear_balls_from_selection();
	void clear_points_from_selection();

private:
	std::list<Point3D> balls_buffer;
	State state;
	int last_spray_x, last_spray_y;
	int min_squared_distance;
	double last_x, last_y;
	double last_radius;
};

#endif //GL_INSER_BALL_LAYER_H