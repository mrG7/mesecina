/* This source file is part of mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info at http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_cutting_plane_layer_3.h 93 2007-04-06 22:00:48Z miklosb $
 */


#include <gui/gl_widget_3/GL_cutting_plane_layer_3.h>

#include <QGLViewer/manipulatedFrame.h>
#include <QGLViewer/vec.h>
#include <QGLViewer/quaternion.h>


GL_cutting_plane_layer_3::GL_cutting_plane_layer_3(const QString& name) : GL_draw_layer_3(name,"Cutting plane layer to disregard a part of the layers after this layer and working with the No clipping flag of layers") {
	fr = new qglviewer::ManipulatedFrame();
}

void GL_cutting_plane_layer_3::set_active(bool a) {
	GL_draw_layer_3::set_active(a);
	if (widget && a && fr) widget->setManipulatedFrame(fr);
	else widget->setManipulatedFrame(0);
}

void GL_cutting_plane_layer_3::attach(GL_widget_3 *w) {
	GL_draw_layer_3::attach(w);
	if (is_active()) widget->setManipulatedFrame(fr);
}

void GL_cutting_plane_layer_3::draw() {

	// local coordinate system
	glPushMatrix();
	glMultMatrixd(fr->matrix());

	// draw plane
	if (fr->isManipulated()) {
		//GLfloat specular_color[4] = { shininess, shininess, shininess, color.alphaF()};
		//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  specular_color);
		//GLfloat ambient_color[4] = { color.redF(), color.greenF(), color.blueF(), color.alphaF() };
		//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,  ambient_color);

		glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
		glLineWidth(line_width);
		widget->drawGrid(widget->sceneRadius(), 2);
	}

	// add clipping plane
	GLdouble eq[4] = { 0,0, 1, 0};
	GLuint MY_CLIP_PLANE = GL_CLIP_PLANE0;
	glEnable(MY_CLIP_PLANE);
	glClipPlane(MY_CLIP_PLANE, eq);

	GLdouble eq2[4] = { 0,0, -1, width * 2 * widget->sceneRadius()};
	MY_CLIP_PLANE = GL_CLIP_PLANE1;
	glEnable(MY_CLIP_PLANE);
	glClipPlane(MY_CLIP_PLANE, eq2);

	// back to global coordinate system
	glPopMatrix();

}

bool GL_cutting_plane_layer_3::has_property(Layer_property prop) {
	switch (prop) {
		case COLOR_EDITABLE:
		case LINE_WIDTH_EDITABLE:
		case WIDTH_EDITABLE:
			return true;
			break;
		default:
			return false;
			break;
	}
}




