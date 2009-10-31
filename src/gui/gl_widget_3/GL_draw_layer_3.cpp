/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_draw_layer_3.cpp 737 2009-05-16 15:40:46Z miklosb $
 */

#include <constants.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>
#include <QGLViewer/vec.h>
#include <gui/app/static/Color_map.h>
#include <gui/app/static/Color_map_factory.h>
#include <gui/app/static/Application_settings.h>

GL_draw_layer_3::GL_draw_layer_3(const QString& n, const QString& ttip) : Managable_layer(), cache_has_data(false), 
	point_size(5), line_width(1), color(rand()%256,rand()%256,rand()%256), name(n), fill_flag(RENDER_NO_FILL), bounding_flag(DO_BOUNDING),
	ambient(0,0,0), diffuse(51,51,51), specular(100,100,100), emission(0,0,0), shininess(0.2f), scalar_flag(NO_SCALAR),
	skip_planes(false), render_hint(NONE), need_rescale_colormap(true), display_list(0), use_display_list(true) {  tooltip = ttip;
		QSettings settings;

}

GL_draw_layer_3::~GL_draw_layer_3() {
	if (!widget) return;
	widget->makeCurrent();
//	std::cout << "deleting display layer from layer " << name.toStdString() << " number " << display_list << std::endl;
	if (display_list!=0) glDeleteLists(display_list,1);
}

void GL_draw_layer_3::draw_into_cache() {
	widget->makeCurrent();

	// create displaylist
	display_list = glGenLists(1);
	if (display_list != 0) {
		glNewList(display_list, GL_COMPILE);
		// GL commands
		draw_commands();
		glEndList();
//		std::cout << "layer " << name.toStdString() << " has display list: " << display_list << std::endl;
	} else {
		/// \todo handle exception no display list could be generated
		std::cout << LOG_ERROR << "Could not compile display list, glError(): " << glGetError() << std::endl;
		assert(display_list);
	}
	cache_has_data = true;
}

void GL_draw_layer_3::draw() {
	widget->makeCurrent(); 
	
//	std::cout << LOG_BLUE << "repaint with color: " << color.redF() << " " << color.greenF() << " " << color.blueF() << " " << color.alphaF() << std::endl;
	if (has_property(POINT_SIZE_EDITABLE) || has_property(LINE_WIDTH_EDITABLE)) {
		glDisable(GL_LIGHTING);
		glEnable( GL_COLOR_MATERIAL );
		glColor4f( color.redF(), color.greenF(), color.blueF(), color.alphaF());
	} else {
		glEnable(GL_LIGHTING);
		glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
		glDisable( GL_COLOR_MATERIAL );
		glShadeModel(GL_SMOOTH);

		GLfloat specular_color[4] = { shininess, shininess, shininess, color.alphaF()};
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  specular_color);
		GLfloat ambient_color[4] = { color.redF(), color.greenF(), color.blueF(), color.alphaF() };
		//std::cout << LOG_GREEN << "repaint with ambient: " << ambient_color[0] << " " << ambient_color[1] << " " << ambient_color[2] << " " << ambient_color[3] << std::endl;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,  ambient_color);
	}

	bool set_clip_back = false;
	if (skip_planes && glIsEnabled(GL_CLIP_PLANE0)) {
		set_clip_back = true;
		glDisable(GL_CLIP_PLANE0);
		glDisable(GL_CLIP_PLANE1);
	}

	glLineWidth(line_width);
	glPointSize(point_size);

	if (widget->get_use_display_lists() && get_use_display_list()) {
		if (!cache_has_data){
			bool old = need_rescale_colormap;
			if (has_new_range) {
				scalar_range_max = scalar_range_min = 0;
				need_rescale_colormap = true;
				//						std::cout << LOG_RED << "need rescale colormar in layer " << name.toStdString() << std::endl;
			}
			//					if (has_new_range) std::cout << name.toStdString() << " has new range " << std::endl;
			draw_into_cache();
			need_rescale_colormap = old;
			while (has_new_range && has_property(SCALAR_EDITABLE)) {
				has_new_range = false;
//				std::cout << "deleting display layer from layer " << name.toStdString() << " number " << display_list << std::endl;
				if (display_list!=0) glDeleteLists(display_list,1);
				scalar_max = scalar_range_max;
				scalar_min = scalar_range_min;
				//Color_map* cm = Color_map_factory::get_color_map(color_map);
				//if (cm!=0) cm->set_scale_range(scalar_min, scalar_max);
				draw_into_cache();
			}
		}
		//std::cout << "display_list: " << display_list << std::endl;
		glCallList(display_list);
	} else {
		draw_commands();
	}

	if (skip_planes && set_clip_back) {
		glEnable(GL_CLIP_PLANE0);
		glEnable(GL_CLIP_PLANE1);
	}
	glFlush();
}

bool GL_draw_layer_3::has_valid_cache() {
	return cache_has_data;
}
void GL_draw_layer_3::invalidate_cache() {
	if (widget) {
		widget->makeCurrent();
//		std::cout << "deleting display layer from layer " << name.toStdString() << " number " << display_list << std::endl;
		if (display_list!=0) glDeleteLists(display_list,1);
		glDeleteTextures(1, &texture);
	}
	if (has_property(SCALAR_EDITABLE)) { 
		has_new_range = true;
//		std::cout << "Layer " << name.toStdString() << " has been set to has_new_range in the invalidate_cache()" << std::endl;
	}
	//scalar_range_max = scalar_range_min = 0;
	cache_has_data = false;
	default_bounding_box = true;
}

QColor GL_draw_layer_3::get_color() { return color; }
void GL_draw_layer_3::set_color(const QColor& q) { 
	color = q;
}

QColor GL_draw_layer_3::get_ambient() { return ambient; }
void GL_draw_layer_3::set_ambient( const QColor &q) { ambient = q; }

QColor GL_draw_layer_3::get_diffuse() { return diffuse; }
void GL_draw_layer_3::set_diffuse( const QColor &q) { diffuse = q; }

QColor GL_draw_layer_3::get_specular() { return specular; }
void GL_draw_layer_3::set_specular( const QColor &q) { specular = q; }

QColor GL_draw_layer_3::get_emission() { return emission ;}
void GL_draw_layer_3::set_emission( const QColor &q) { emission = q; }

float GL_draw_layer_3::get_shininess() { return shininess; }
void GL_draw_layer_3::set_shininess( float i) { shininess = i; }

float GL_draw_layer_3::get_width() { return width; }
void GL_draw_layer_3::set_width(float w) { width = w; }

float GL_draw_layer_3::get_point_size() { return point_size; }
void GL_draw_layer_3::set_point_size(float size) { point_size = size; }

float GL_draw_layer_3::get_line_width() { return line_width; }
void GL_draw_layer_3::set_line_width(float width) { line_width = width; }

bool GL_draw_layer_3::get_skip_clipping_planes() { return skip_planes; }
void GL_draw_layer_3::set_skip_clipping_planes(bool skip) { skip_planes=skip; }

bool GL_draw_layer_3::get_use_display_list() { 
//	std::cout << "call from gl_draw_layer_3" << std::endl;
	return use_display_list; }
void GL_draw_layer_3::set_use_display_list(bool use) { use_display_list=use; }

bool GL_draw_layer_3::get_need_rescale_colormap() { return need_rescale_colormap; }
void GL_draw_layer_3::set_need_rescale_colormap(bool nrs) { 
	need_rescale_colormap = nrs;
}

QColor GL_draw_layer_3::get_scalar_color(float val) {
	if (color_map=="Solid" || color_map=="") return color;
	Color_map* cm = Color_map_factory::get_color_map(color_map);
	if (!cm) {
		std::cout << "Color map " << color_map.toStdString() << " could not be instantiated in GL_draw_layer_2 " << std::endl;
		return QColor();
	}
	if (need_rescale_colormap)
		cm->set_scale_range(scalar_min, scalar_max);
//	else std::cout << " no set_scale" << std::endl;
	QColor c;
	if (scalar_flag == DO_INVERSE_SCALAR) c = cm->get_color(cm->get_scale_max() - val + cm->get_scale_min());
	else c = cm->get_color(val);
	c.setAlphaF(color.alphaF());
	return c;
}

QString GL_draw_layer_3::get_image_file_name() { return image_name; }
void GL_draw_layer_3::set_image_file_name(QString s) { image_name = s; }

QByteArray GL_draw_layer_3::saveState() {
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);

	stream << qint32(layer_token);
	stream << qint32(version);
	stream << color ;
	stream << point_size;
	stream << line_width;
	stream << active;
	stream << image_name;
	stream << shininess;
	stream << width;
	stream << color_map;
	stream << skip_planes;
	stream << need_rescale_colormap;
	return data;
}

bool GL_draw_layer_3::restoreState(const QByteArray &state) {
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);

	qint32 token;
    qint32 v;
    stream >> token;
    stream >> v;
    if (token != layer_token || v != version)
        return false;

    stream >> color;
    stream >> point_size;
    stream >> line_width;
	stream >> active;
	stream >> image_name;
	stream >> shininess;
	stream >> width;
	stream >> color_map;
	stream >> skip_planes;
	stream >> need_rescale_colormap;

	// calls the set_active, so that if image file not available, then image layer will be inactive for sure
	set_active(active);

    return true;
}

QString GL_draw_layer_3::get_name() { return name; }

QBrush GL_draw_layer_3::symbol_brush() {
	if (color_map!="Solid" && color_map!="") {
		return *Color_map_factory::get_color_map(color_map)->get_brush();
	}
	return QBrush(color);
}

bool GL_draw_layer_3::has_property(Layer_property prop) {
	return false;
	switch (prop) {
		case COLOR_EDITABLE:
		case POINT_SIZE_EDITABLE:
		case LINE_WIDTH_EDITABLE:
			return true;
			break;
		default:
			return false;
			break;
	}
}


GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const GL_draw_layer_3::Fill_flagger f)
{
	l.fill_flag = f;
	return l;
}

GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const GL_draw_layer_3::Bounding_flagger f)
{
	l.bounding_flag = f;
	return l;
}

GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const GL_draw_layer_3::Render_hint f)
{
//	assert(f!=l.render_hint);
	l.render_hint = f;
	switch (f) {
		case GL_draw_layer_3::SEGMENTS_START: l.widget->start_segments(); break;
		case GL_draw_layer_3::SEGMENTS_END: l.widget->end_segments(); break;
		case GL_draw_layer_3::TRIANGLES_START: l.widget->start_triangles(); break;
		case GL_draw_layer_3::TRIANGLES_END: l.widget->end_triangles(); break;
		case GL_draw_layer_3::POINTS_START: l.widget->start_points(); break;
		case GL_draw_layer_3::POINTS_END: l.widget->end_points(); break;
	}
	return l;
}

GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const GL_draw_layer_3::Scalar_value_flagger f) {
	if (f==GL_draw_layer_3::DO_SCALAR || f==GL_draw_layer_3::DO_INVERSE_SCALAR) {
		if (!l.has_valid_cache() && l.has_new_range) { l.has_no_value = true; }
	}
	if (f==GL_draw_layer_3::NO_SCALAR) {
	}
	l.scalar_flag = f;
	return l;
}

GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const QVector<Point3D>& polygon) {
	SCALAR_FILTERING_CODE
	l.update_bounding_box(l.widget->draw_polygon(polygon));
	return l;
}

GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const float scalar) {
	l.scalar_value = scalar;
	if (l.scalar_range_min>l.scalar_value || l.has_no_value) {
		l.scalar_range_min = l.scalar_value;
		if (l.get_need_rescale_colormap()) l.scalar_min = l.scalar_range_min;
		l.has_new_range = true;
		l.has_no_value = false;
	}
	if (l.scalar_range_max<l.scalar_value || l.has_no_value) {
		l.scalar_range_max = l.scalar_value;
		if (l.get_need_rescale_colormap()) l.scalar_max = l.scalar_range_max;
		l.has_new_range = true;
		l.has_no_value = false;
	}
	return l;
}

//GL_draw_layer_3& operator<<(GL_draw_layer_3& l, const QPixmap& pixmap) {
//	l.update_bounding_box(l.widget->draw_pixmap(pixmap, l.texture));
//	return l;
//}

void GL_draw_layer_3::update_bounding_box(const Box3D& r) {
	if (bounding_flag==NO_BOUNDING) return;
	if (default_bounding_box || r.xmin < bounding_xmin) bounding_xmin = r.xmin;
	if (default_bounding_box || r.xmax > bounding_xmax) bounding_xmax = r.xmax;
	if (default_bounding_box || r.ymin < bounding_ymin) bounding_ymin = r.ymin;
	if (default_bounding_box || r.ymax > bounding_ymax) bounding_ymax = r.ymax;
	if (default_bounding_box || r.zmin < bounding_zmin) bounding_zmin = r.zmin;
	if (default_bounding_box || r.zmax > bounding_zmax) bounding_zmax = r.zmax;
	default_bounding_box = false;
}

void GL_draw_layer_3::application_settings_changed(const QString& settings_name) {
	if (has_property(GLUT_SPHERE) &&
		(settings_name == "sphere-rendering-resolution") ) {
//		std::cout << "Layer " << name.toStdString() << " detected that settings changed " << std::endl;
		invalidate_cache();
	}
}