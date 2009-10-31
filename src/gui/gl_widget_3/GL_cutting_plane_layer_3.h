/* This source file is part of mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info at http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_cutting_plane_layer_3.h 93 2007-04-06 22:00:48Z miklosb $
 */

#ifndef MESECINA_CUTTING_PLANE_LAYER_3_H
#define MESECINA_CUTTING_PLANE_LAYER_3_H

#include <gui/gl_widget_3/GL_draw_layer_3.h>

class qglviewer::ManipulatedFrame;

class W_API GL_cutting_plane_layer_3 : public GL_draw_layer_3 {
public:

	GL_cutting_plane_layer_3(const QString& name);

	virtual void set_active(bool a);
	virtual void attach(GL_widget_3 *w);
	virtual void draw();

	virtual bool has_property(Layer_property prop);
	qglviewer::ManipulatedFrame* fr;
};
#endif //MESECINA_CUTTING_PLANE_LAYER_3_H


