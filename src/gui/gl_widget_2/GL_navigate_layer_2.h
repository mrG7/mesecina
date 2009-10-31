/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_navigate_layer_2.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef GL_NAVIGATE_LAYER_H
#define GL_NAVIGATE_LAYER_H


#include <gui/gl_widget/Visualization_layer.h>

#include <QtGui/QInputEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtCore/QPoint>
#include <QtGui/QRubberBand>

class W_API GL_navigate_layer_2 : public Visualization_layer {
public:
	enum state_type {NONE, PAN, ZOOM, ZOOM_RECT, SPRAY, MOVE};

	GL_navigate_layer_2();
	~GL_navigate_layer_2();

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void wheelEvent(QWheelEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	//virtual void keyPressEvent(QKeyEvent *);
	//virtual void keyReleaseEvent(QKeyEvent *);

private:
	state_type s;
	int first_x, first_y;
	QPoint rubberBand_origin;
	QRubberBand* rubber_band;
	QStyle* windows_style;

	void smooth_zoom(double x_min, double x_max, double y_min, double y_max);
};

#endif //GL_NAVIGATE_LAYER_H