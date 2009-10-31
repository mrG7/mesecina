/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_draw_layer_2.cpp 586 2009-04-03 20:01:21Z miklosb $
 */

#include <gui/gl_widget_2/GL_draw_layer_2.h>
#include <gui/app/static/Color_map_factory.h>
#include <constants.h>

GL_draw_layer_2::GL_draw_layer_2(const QString& n, const QString& tt) : Managable_layer(tt), cache_has_data(false), 
	point_size(5), line_width(1), color(rand()%256,rand()%256,rand()%256), name(n), fill_flag(RENDER_NO_FILL), bounding_flag(DO_BOUNDING), texture(0),
	scalar_flag(NO_SCALAR), need_rescale_colormap(true), display_list(0) { has_new_range = true;
}


GL_draw_layer_2::~GL_draw_layer_2() {
	if (!widget) return;
//	widget->pbuffer->makeCurrent();
	if (display_list!=0) glDeleteLists(display_list,1);
}

void GL_draw_layer_2::draw_into_cache() {
	widget->makeCurrent();

	display_list = glGenLists(1);
	if (display_list != 0) {
		glNewList(display_list, GL_COMPILE);
		// GL commands
		draw_commands();
		glEndList();
	} else {
		/// \todo handle exception no display list could be generated
		assert(display_list);
	}
	cache_has_data = true;
}

void GL_draw_layer_2::prepare_layer() {
	if (widget->get_use_display_lists() && !cache_has_data) {
		draw_into_cache();
		while (has_new_range) {
			has_new_range = false;
			//glDeleteLists(display_list,1);
			scalar_max = scalar_range_max;
			scalar_min = scalar_range_min;
			draw_into_cache();
		}
	} 
}


void GL_draw_layer_2::draw() {
	if (widget->get_renderer_type() == GL_widget_2::OPEN_GL) {
		//widget->makeCurrent();

		glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
		glLineWidth(line_width);
		glPointSize(point_size);

		if (name=="Image")
			draw_commands();
		else 

			if (widget->get_use_display_lists()) {
				if (!cache_has_data) {
					bool old = need_rescale_colormap;
					if (has_new_range) {
						scalar_range_max = scalar_range_min = 0;
						need_rescale_colormap = true;
//						std::cout << LOG_RED << "need rescale colormar in layer " << name.toStdString() << std::endl;
					}
//					if (has_new_range) std::cout << name.toStdString() << " has new range " << std::endl;
					draw_into_cache();
					need_rescale_colormap = old;
					while (has_new_range) {
						has_new_range = false;
//						glDeleteLists(display_list,1);
						scalar_max = scalar_range_max;
						scalar_min = scalar_range_min;
						//Color_map* cm = Color_map_factory::get_color_map(color_map);
						//if (cm) // cm->set_scale_range(scalar_min,scalar_max);
						//	std::cout << name.toStdString() << " colormap " << cm->get_scale_min() << " " << cm->get_scale_max() << std::endl;
//						std::cout << name.toStdString() << " new min max: " << scalar_min << ","  << scalar_max << std::endl;
						draw_into_cache();
					}
				}
				glCallList(display_list);
			} else {
				/*if (name!="Image")*/ draw_commands();
			}
	} else
		if (widget->get_renderer_type() == GL_widget_2::QT_PAINTER) {
			QPainter* painter = widget->get_painter();

			QPen p = painter->pen();
			p.setColor(color);
			p.setJoinStyle(	Qt::RoundJoin);
			p.setWidthF(line_width); 
			widget->set_line_pen(p);
			p.setWidthF(point_size);
			widget->set_point_pen(p);

			draw_commands();
		}
}

bool GL_draw_layer_2::has_valid_cache() {
	return cache_has_data;
}
void GL_draw_layer_2::invalidate_cache() {
	if (widget) {
		widget->makeCurrent();
		if (display_list!=0) glDeleteLists(display_list,1);
		if (texture>0) {
//			std::cout << "delete texture of layer " << name.toStdString() << " texture: " << texture << std::endl;
//			glDeleteTextures(1, &texture);
			widget->deleteTexture(texture);	
			texture = 0;
		}
	}
	if (has_property(SCALAR_EDITABLE)) { 
		has_new_range = true;
//		std::cout << "Layer " << name.toStdString() << " has been set to has_new_range in the invalidate_cache()" << std::endl;
	}
	//scalar_range_max = scalar_range_min = 0;
	cache_has_data = false;
	default_bounding_box = true;
}

void GL_draw_layer_2::reset_range() {
	scalar_range_max = scalar_range_min = 0;
}

QColor GL_draw_layer_2::get_color() { return color; }
void GL_draw_layer_2::set_color(const QColor& q) { 
	color = q;
}

float GL_draw_layer_2::get_point_size() { return point_size; }
void GL_draw_layer_2::set_point_size(float size) { point_size = size; }

float GL_draw_layer_2::get_line_width() { return line_width; }
void GL_draw_layer_2::set_line_width(float width) { line_width = width; }

bool GL_draw_layer_2::get_need_rescale_colormap() { return need_rescale_colormap; }
void GL_draw_layer_2::set_need_rescale_colormap(bool nrs) { 
	need_rescale_colormap = nrs;
}

QColor GL_draw_layer_2::get_scalar_color(float val) {
	if (color_map=="Solid" || color_map=="") return color;
	Color_map* cm = Color_map_factory::get_color_map(color_map);
	if (!cm) {
		std::cout << "Color map " << color_map.toStdString() << " could not be instantiated in GL_draw_layer_2 " << std::endl;
		return QColor();
	}
	if (need_rescale_colormap) {
		cm->set_scale_range(scalar_min, scalar_max);
//		std::cout << " DO set_scale " << scalar_min << " " << scalar_max << " and val " << val << std::endl;
	}
//	else std::cout << " no set_scale" << std::endl;
	QColor c;
	if (scalar_flag == DO_INVERSE_SCALAR) c = cm->get_color(cm->get_scale_max() - val + cm->get_scale_min());
	else c = cm->get_color(val);
	c.setAlphaF(color.alphaF());
	return c;
}

QString GL_draw_layer_2::get_image_file_name() { return image_name; }
void GL_draw_layer_2::set_image_file_name(QString s) { 
	image_name = s; 
	QSettings settings;
	if (settings.value("image-filename-from-settings", true).toBool())
		settings.setValue("image-layer-filename",image_name);}

QByteArray GL_draw_layer_2::saveState() {
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);

	stream << qint32(layer_token);
	stream << qint32(version);
	stream << color ;
	stream << point_size;
	stream << line_width;
	stream << active;
	stream << color_map;
	QSettings settings;
	if (settings.value("image-filename-from-settings", true).toBool() == false) {
		stream << image_name;
//		std::cout << "saving imagename: " << image_name.toStdString() << std::endl;
	} else stream << " ";
	stream << need_rescale_colormap;
	return data;
}

bool GL_draw_layer_2::restoreState(const QByteArray &state) {
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
	stream >> color_map;
	QSettings settings;
	if (!settings.value("image-filename-from-settings", true).toBool()) {
		QString previous_name = image_name;
		stream >> image_name;
		if (previous_name!= image_name) {
//			std::cout << "Layer " << name.toStdString() << ": " << previous_name.toStdString() << " -> " << image_name.toStdString() << std::endl;
			invalidate_cache();
		}
	} else { QString dummy; stream >> dummy; }
	stream >> need_rescale_colormap;
//	std::cout << "restore for layer " << name.toStdString() << " need_rescale_colormap: " << need_rescale_colormap << std::endl;
	// calls the set_active, so that if image file not available, then image layer will be inactive for sure
	set_active(active);

    return true;
}

QString GL_draw_layer_2::get_name() { return name; }

QBrush GL_draw_layer_2::symbol_brush() {
	if (color_map!="Solid" && color_map!="") {
		Color_map* c = Color_map_factory::get_color_map(color_map);
		if (c!=0) return *c->get_brush();
		else return QBrush(color);
	}
	return QBrush(color);
}

bool GL_draw_layer_2::has_property(Layer_property prop) {
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


GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const GL_draw_layer_2::Fill_flagger f)
{
	l.fill_flag = f;
	return l;
}

GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const GL_draw_layer_2::Bounding_flagger f)
{
	l.bounding_flag = f;
	return l;
}

GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const GL_draw_layer_2::Scalar_value_flagger f)
{
	if (f==GL_draw_layer_2::DO_SCALAR || f==GL_draw_layer_2::DO_INVERSE_SCALAR) {
		if (!l.has_valid_cache() && l.has_new_range) { l.scalar_range_min = l.scalar_range_max = 0; }
	}
	if (f==GL_draw_layer_2::NO_SCALAR) {
	}
	l.scalar_flag = f;
	return l;
}


GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const QPixmap& pixmap) {
	l.update_bounding_box(l.widget->draw_pixmap(pixmap, l.texture));
	return l;
}

GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const QPolygonF& polygon) {
	l.update_bounding_box(l.widget->draw_polygon(polygon));
	return l;
}


GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const QPointF& point) {
	l.update_bounding_box(l.widget->draw_point(point.x(),point.y()));
	return l;
}

GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const float scalar) {
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


/// \todo start segment stuff

GL_draw_layer_2& operator<<(GL_draw_layer_2& l, const QLineF& line) {
	l.update_bounding_box(l.widget->draw_segment(line.x1(),line.y1(), line.x2(),line.y2()));
	return l;
}

void GL_draw_layer_2::update_bounding_box(const QRectF& r) {
	if (bounding_flag==NO_BOUNDING) return;
	Visualization_layer::update_bounding_box(r);
}