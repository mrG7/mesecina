#include <gui/gl_widget_3/GL_selection_layer_3.h>
//#include <QString>
#include <gui/app/static/Application_settings.h>

GL_selection_layer_3::GL_selection_layer_3(const QString& name) : GL_draw_layer_3(name,"") {
	Application_settings::add_double_setting("selection-point-size",3);
	set_point_size(Application_settings::get_double_setting("selection-point-size"));
	points.clear();
}

void GL_selection_layer_3::draw_commands() {
//	std::cout << LOG_GREEN << "drawing selection" << std::endl;
	point_size = Application_settings::get_double_setting("selection-point-size");
//	std::cout << "point size: " << get_point_size() << std::endl;
	std::list<Point3D>::iterator p_it, p_end = points.end();
	for (p_it = points.begin(); p_it != p_end; p_it++)
		widget->draw_point(p_it->x, p_it->y, p_it->z);

}

void GL_selection_layer_3::add_point_to_selection(double x, double y, double z) {
	points.push_back(Point3D(x,y,z));
//	std::cout << "added point to selection, current length: " << points.size() << std::endl;
	invalidate_cache();
	widget->repaintGL();
}

void GL_selection_layer_3::remove_point_from_selection(double x, double y, double z) {

	std::cout << "should remove a specific point from selection " << points.size() << std::endl;

}

void GL_selection_layer_3::application_settings_changed(const QString& settings_name) {
	if (settings_name=="selection-point-size") {
		set_point_size(Application_settings::get_double_setting("selection-point-size"));
		invalidate_cache();
		widget->request_repaint();

	}
}


void GL_selection_layer_3::clear_selection() {
	points.clear();
//	std::cout << "No more points in selection"<< std::endl;
	invalidate_cache();
	widget->repaintGL();
}

bool GL_selection_layer_3::has_property(Layer_property prop) {
	switch (prop) {
		case COLOR_EDITABLE:
		case POINT_SIZE_EDITABLE:
			return true;
			break;
		default:
			return false;
			break;
	}
}

