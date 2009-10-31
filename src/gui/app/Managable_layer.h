/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Managable_layer.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MANAGABLE_LAYER_H
#define MANAGABLE_LAYER_H

#include <QtGui/QColor>
#include <QtGui/QBrush>

#include <gui/app/Application_variable.h>
#include <gui/gl_widget/Visualization_layer.h>

class Managable_layer : public Visualization_layer {
public:
	enum Layer_property { COLOR_EDITABLE,
						  POINT_SIZE_EDITABLE,
						  LINE_WIDTH_EDITABLE,
						  IMAGE_EDITABLE,
						  WIDTH_EDITABLE,
						  GLUT_SPHERE,
						  SCALAR_EDITABLE,
						  USER_PROPERTY_1,
						  USER_PROPERTY_2,
						  USER_PROPERTY_3,
						  USER_PROPERTY_4,
						  USER_PROPERTY_5};

//	Managable_layer() : Visualization_layer() {scalar_min = scalar_max = scalar_value = 0; color_map = ""; has_no_value = true; has_new_range = true; tooltip = "";}
	Managable_layer(const QString& tt = "") : Visualization_layer(), tooltip(tt) {scalar_min = scalar_max = scalar_value = 0; color_map = ""; has_no_value = true; has_new_range = true;}

	virtual QString get_name() { return QString("DEFAULT-unnamed layer"); }
//	virtual bool is_active() { return false; }
//	virtual void set_active(bool a) { }
	//virtual int get_variable_count() { return 0;}
	//virtual Application_variable get_variable(int i) { return Application_variable(QString(""),0); }
	virtual bool has_valid_cache() { return false; }
	virtual void invalidate_cache() { }

	virtual bool has_property(Layer_property ) { return false; }
	virtual QColor get_color() { return Qt::black; }
	virtual void set_color( const QColor &) {}

	virtual QColor get_ambient() { return Qt::black; }
	virtual void set_ambient( const QColor &) {}

	virtual QColor get_diffuse() { return Qt::black; }
	virtual void set_diffuse( const QColor &) {}

	virtual QColor get_specular() { return Qt::white; }
	virtual void set_specular( const QColor &) {}

	virtual QColor get_emission() { return Qt::black; }
	virtual void set_emission( const QColor &) {}

	virtual float get_shininess() { return 0; }
	virtual void set_shininess( float ) {}

	virtual float get_point_size() { return 3; }
	virtual void set_point_size(float ) {}

	virtual float get_line_width() { return 1; }
	virtual void set_line_width(float ) {}

	virtual bool get_skip_clipping_planes() { return false; }
	virtual void set_skip_clipping_planes(bool ) {}

	virtual bool get_use_display_list() { return true; }
	virtual void set_use_display_list(bool ) {}

	virtual bool get_need_rescale_colormap() { return false; }
	virtual void set_need_rescale_colormap(bool ) {}

	virtual float get_width() { return 0.5; }
	virtual void set_width(float ) { }

	virtual QString get_tooltip() { return tooltip; }

	float scalar_min, scalar_max, scalar_value, scalar_range_min, scalar_range_max;
	bool has_new_range;
	bool has_no_value;
	QString color_map;

	virtual void execute_user_property(Layer_property ) { }
	virtual QString get_name_user_property(Layer_property) { return ""; }

	virtual QBrush symbol_brush() { return Qt::black; }

	virtual QString get_image_file_name() { return ""; }
	virtual void set_image_file_name(QString s) { }

	virtual QByteArray saveState() { return QByteArray(); }
	virtual bool restoreState (const QByteArray & ) { return false; }

protected:
	QString tooltip;

};

#endif //MANAGABLE_LAYER_H