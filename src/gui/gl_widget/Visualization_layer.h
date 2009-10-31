/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Visualization_layer.h 383 2008-11-03 20:12:36Z miklosb $
 */

#ifndef VISUALIZATION_LAYER_H
#define VISUALIZATION_LAYER_H

#include <QtCore/QObject>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QCursor>
#include <QtGui/QKeyEvent>
#ifdef MESECINA_3D
#include <gui/gl_widget_3/Box3D.h>
#endif

class GL_widget_3;
class GL_widget_2;

#ifdef MESECINA_GL_WIDGET
#define W_API   __declspec( dllexport )
#else
#define W_API   __declspec( dllimport )
#endif

class W_API Visualization_layer : public QObject {

	Q_OBJECT

public:
	Visualization_layer();
	~Visualization_layer();

	
#ifdef MESECINA_3D
	virtual void attach(GL_widget_3* w) { 
		widget = w;
	}
	GL_widget_3 *widget;
	virtual Box3D get_3d_bounding_box(bool *is_valid);
#else
	void attach(GL_widget_2* w) {
		widget = w; 
	}
	GL_widget_2 *widget;
#endif
	virtual void activate();
	virtual void deactivate();
	virtual void set_active(bool a);
	virtual bool is_active();
	void set_default_cursor(QCursor c);

	virtual void draw() {};
	virtual void prepare_layer() {};
	virtual QRectF get_bounding_box(bool *is_valid);

	virtual void ray_selection(double , double , bool* ){}
	virtual void disk_selection(double , double , double , bool* ) {}
	virtual void selection_at(double, double, double);
	virtual void box_selection(double , double , double , double , bool* ) {}
	virtual void selection_copied_by(double , double ) {}
	virtual void selection_moved_by(double , double ) {}
	virtual bool selection_deleted() { return false; }
	virtual void clear_selection() {}

	virtual void mousePressEvent(QMouseEvent *) {} ;
	virtual void mouseReleaseEvent(QMouseEvent *) {};
	virtual void wheelEvent(QWheelEvent *) {};
	virtual void mouseDoubleClickEvent(QMouseEvent *) {};
	virtual void mouseMoveEvent(QMouseEvent *) {};
	virtual void keyPressEvent(QKeyEvent *) {};
	virtual void keyReleaseEvent(QKeyEvent *) {};
	virtual void enterEvent(QEvent *) {};
	virtual void leaveEvent(QEvent *) {};
	virtual bool event(QEvent *e) {QObject::event(e); return true;};

	virtual void update_bounding_box(const QRectF& r);
	friend class GL_draw_layer_2;
	friend class GL_draw_layer_3;

	virtual void application_settings_changed(const QString& ) {}

signals:
	void msg(QString t, int level = 1);

protected:
	inline bool is_pure(QInputEvent *e) { return e->modifiers() == Qt::NoModifier; }
	inline bool is_shift(QInputEvent *e) { return e->modifiers() == Qt::ShiftModifier; }
	inline bool is_control(QInputEvent *e) { return e->modifiers() == Qt::ControlModifier; }
	inline bool is_alt(QInputEvent *e) { return e->modifiers() == Qt::AltModifier; }
	inline bool is_control_and_shift(QInputEvent *e) { 
		return ((e->modifiers() & Qt::ControlModifier) && (e->modifiers() & Qt::ShiftModifier));
	}
	inline bool has_control_no_shift(QInputEvent *e) { 
		return ((e->modifiers() & Qt::ControlModifier) && !(e->modifiers() & Qt::ShiftModifier));
	}
	inline bool has_control_has_alt(QInputEvent *e) { 
		return ((e->modifiers() & Qt::ControlModifier) && (e->modifiers() & Qt::AltModifier));
	}
	inline bool has_shift(QInputEvent *e) { 
		return (e->modifiers() & Qt::ShiftModifier);
	}
	inline bool has_alt(QInputEvent *e) { 
		return (e->modifiers() & Qt::AltModifier);
	}
	inline bool has_control(QInputEvent *e) { 
		return (e->modifiers() & Qt::ControlModifier);
	}

	bool active;

	double bounding_xmin, bounding_xmax, bounding_ymin, bounding_ymax, bounding_zmin, bounding_zmax;
	bool default_bounding_box;

};

#ifdef MESECINA_GL_WIDGET
#define V_API   __declspec( dllexport )
#else
#define V_API __declspec( dllimport )
#endif

V_API const char * hand_xpm[];
V_API const char * holddown_xpm[];
V_API const char * zoomin_xpm[];
V_API const char * zoomin_rect_xpm[];
V_API const char * zoomout_xpm[];

#endif //VISUALIZATION_LAYER_H