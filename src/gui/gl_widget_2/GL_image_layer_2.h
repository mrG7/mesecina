/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_image_layer_2.h 134 2007-08-02 21:50:30Z miklosb $
 */

#ifndef GL_IMAGE_LAYER_H
#define GL_IMAGE_LAYER_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

class W_API GL_image_layer_2 : public GL_draw_layer_2 {
	//Q_OBJECT
public:
	GL_image_layer_2(const QString& n);
	virtual void draw_commands();
	
	virtual void invalidate_cache();

	virtual bool has_property(Layer_property prop);

	virtual void set_active(bool a);
	virtual void application_settings_changed(const QString& settings_name);

	virtual bool restoreState (const QByteArray & state);

	virtual QBrush symbol_brush();
};

#endif //GL_IMAGE_LAYER_H
