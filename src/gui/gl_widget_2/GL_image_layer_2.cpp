/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_image_layer_2.cpp 530 2009-01-28 19:17:09Z miklosb $
 */

#include <gui/gl_widget_2/GL_image_layer_2.h>
#include <gui/app/static/Application_settings.h>
#include <constants.h>

GL_image_layer_2::GL_image_layer_2(const QString& n) : GL_draw_layer_2(n, "Shows an image in the scene") {
	QSettings settings;
	if (!settings.contains("image-filename-from-settings")) 
		settings.setValue("image-filename-from-settings", IMAGE_FROM_SETTINGS);
	Application_settings::add_setting("image-filename-from-settings", AS_BOOL);

	color = QColor(255,255,255);
	image_name="";
	has_new_range = false;
}

void GL_image_layer_2::draw_commands() {
	if (image_name!="") {
//		std::cout << "drawing: " << image_name.toStdString() << std::endl;
		*this << QPixmap(image_name);
	}
}
	
bool GL_image_layer_2::has_property(Layer_property prop) {
	switch (prop) {
		case COLOR_EDITABLE:
		case IMAGE_EDITABLE:
			return true;
			break;
		default:
			return false;
			break;
	}
}

void GL_image_layer_2::set_active(bool a) {
	if (QFile::exists(image_name)) active = a;
	else active = false;
}

QBrush GL_image_layer_2::symbol_brush() {
	return QBrush(QPixmap(image_name).scaledToWidth(16));
}

bool GL_image_layer_2::restoreState (const QByteArray & state) {
	QSettings settings;
	if (settings.value("image-filename-from-settings", true).toBool()) {
		QString file_name = settings.value("image-layer-filename").toString();
		if (QFile::exists(file_name)) image_name = file_name;
		else image_name = "";
	}
	return GL_draw_layer_2::restoreState(state);
}

void GL_image_layer_2::application_settings_changed(const QString& settings_name) {
	if (settings_name == "image-filename-from-settings") {
		QSettings settings;
		if (settings.value("image-filename-from-settings").toBool()) {
			image_name = settings.value("image-layer-filename").toString();\
			invalidate_cache();
			if (widget) widget->repaintGL();
		}
	}
}

void GL_image_layer_2::invalidate_cache() {
	std::cout << "Image layer invalidate!" << std::endl;
	GL_draw_layer_2::invalidate_cache();
}