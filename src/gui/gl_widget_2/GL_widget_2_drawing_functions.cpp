/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_widget_2_drawing_functions.cpp 533 2009-02-14 23:24:47Z miklosb $
 */

#include <gui/gl_widget_2/GL_draw_layer_2.h>
#include <constants.h>
#include <QtOpenGL/QGLPixelBuffer>


QRectF GL_widget_2::draw_arc(double orig_x, double orig_y, double radius, double start, double span) {
	if (renderer_type == OPEN_GL) {
//		makeCurrent();
		GLfloat vectorX, vectorY, originX, originY;
		float angle;
		originX = orig_x;
		originY = orig_y;

		glBegin(GL_LINE_STRIP);	
		double step = 2.0/57.29577957795135;
//		start =0; span = 2*3.14;// step = 2;
		for(double i=start;i<=start+span; i=i+step)	{
			angle=i;//(float)(((double)i)/57.29577957795135);	
			vectorX=originX+(radius*(float)cos(angle));
			vectorY=originY+(radius*(float)sin(angle));
			//std::cout << vectorX << " " << vectorY << " ";
			glVertex2d(vectorX,vectorY);
		}
		angle = start + span;
		vectorX=originX+(radius*(float)cos(angle));
		vectorY=originY+(radius*(float)sin(angle));
		glVertex2d(vectorX,vectorY);

		//std::cout << std::endl;
		glEnd();
	}// else
		//if (renderer_type == QT_PAINTER) {
		//	painter->setPen(line_pen);
		//	double rx = x_pixel_dist_d(radius);
		//	double ry = x_pixel_dist_d(radius);
		//	painter->drawEllipse(x_pixel_d(orig_x)-rx, y_pixel_d(orig_y)-ry, 2*rx, 2*ry);
		//}
	return QRectF(orig_x-radius, orig_y-radius, 2*radius, 2*radius);
}

QRectF GL_widget_2::draw_circle(double orig_x, double orig_y, double radius) {
	if (renderer_type == OPEN_GL) {
//		makeCurrent();
		GLfloat vectorX, vectorY, originX, originY;
		float angle;
		originX = orig_x;
		originY = orig_y;

		glBegin(GL_LINE_STRIP);	
		for(int i=0;i<=360;i+=2)	{
			angle=(float)(((double)i)/57.29577957795135);	
			vectorX=originX+(radius*(float)sin((double)angle));
			vectorY=originY+(radius*(float)cos((double)angle));
			//std::cout << vectorX << " " << vectorY << " ";
			glVertex2d(vectorX,vectorY);
		}
		//std::cout << std::endl;
		glEnd();
	} else
		if (renderer_type == QT_PAINTER) {
			painter->setPen(line_pen);
			double rx = x_pixel_dist_d(radius);
			double ry = x_pixel_dist_d(radius);
			painter->drawEllipse(x_pixel_d(orig_x)-rx, y_pixel_d(orig_y)-ry, 2*rx, 2*ry);
		}
	return QRectF(orig_x-radius, orig_y-radius, 2*radius, 2*radius);
}

void GL_widget_2::set_color(float red, float green, float blue, float alpha) {
	if (renderer_type == OPEN_GL) {
//		makeCurrent();
		glColor4f(red, green, blue, alpha);

	} else
		if (renderer_type == QT_PAINTER) {
			QPainter* painter = get_painter();

			QPen p = painter->pen();
			QColor c; c.setRgbF(red, green, blue, alpha);
			p.setColor(c);
			set_point_pen(p);
			set_line_pen(p);
		}
}

QRectF GL_widget_2::draw_disk(double orig_x, double orig_y, double radius) {
	if (renderer_type == OPEN_GL) {
//		makeCurrent();
		GLfloat vectorX, vectorY, originX, originY;
		float angle;
		originX = orig_x;
		originY = orig_y;

		glBegin(GL_TRIANGLE_FAN);	
		glVertex2d(originX,originY);
		for(int i=0;i<=360;i+=2)	{
			angle=(float)(((double)i)/57.29577957795135);	
			vectorX=originX+(radius*(float)sin((double)angle));
			vectorY=originY+(radius*(float)cos((double)angle));		
			glVertex2d(vectorX,vectorY);
		}
		glEnd();
	} else
		if (renderer_type == QT_PAINTER) {
			QPen temp_pen = line_pen;
			temp_pen.setWidth(0);
			painter->setPen(temp_pen);
			QBrush brush_before = painter->brush();
			painter->setBrush(temp_pen.color());
			double rx = x_pixel_dist_d(radius);
			double ry = x_pixel_dist_d(radius);
			painter->drawEllipse(x_pixel_d(orig_x)-rx, y_pixel_d(orig_y)-ry, 2*rx, 2*ry);
			painter->setBrush(brush_before);
		}
	return QRectF(orig_x-radius, orig_y-radius, 2*radius, 2*radius);
}

QRectF GL_widget_2::draw_point(double x, double y) {
	if (renderer_type == OPEN_GL) {
//		makeCurrent();
		glBegin(GL_POINTS);	
		glVertex2d(x,y);
		glEnd();
	} else 
		if (renderer_type == QT_PAINTER) {
			double size = point_pen.widthF();
			QPen temp_pen = point_pen;
			temp_pen.setWidth(0);
			painter->setPen(temp_pen);
			QBrush brush_before = painter->brush();
			painter->setBrush(temp_pen.color());
			painter->drawEllipse(x_pixel_d(x)-size/2, y_pixel_d(y)-size/2, size, size);
			painter->setBrush(brush_before);
			//painter->setPen(point_pen);
			//painter->drawPoint(x_pixel(x),y_pixel(y));
		}
		return QRectF(x,y,0,0);
}

QRectF GL_widget_2::draw_segment(double x1, double y1, double x2, double y2, bool with_begin_end) {
	double xmin, xmax, ymin, ymax;
	xmin = x1; xmax = x2;
	ymin = y1; ymax = y2;
	if (xmax<xmin) { double temp = xmin; xmin = xmax; xmax = temp; }
	if (ymax<ymin) { double temp = ymin; ymin = ymax; ymax = temp; }

	switch (renderer_type) {
		case OPEN_GL:
//			makeCurrent();
			if (with_begin_end) glBegin(GL_LINES);
			glVertex2d(x1,y1);
			glVertex2d(x2,y2);
			if (with_begin_end) glEnd();
			break;
		case QT_PAINTER:
			painter->setPen(line_pen);
			painter->drawLine(x_pixel_d(x1),y_pixel_d(y1),x_pixel_d(x2),y_pixel_d(y2));
			break;
		default:
			//never should happen;
			std::cout << LOG_ERROR << "Not good renderer for GL_draw_layer" << std::endl;
	}
	return QRectF(xmin, ymin, xmax-xmin, ymax-ymin);
}

QRectF GL_widget_2::draw_line(double px, double py, double dirx, double diry, bool with_begin_end) {
	double x1, y1, x2, y2;
	if (renderer_type==OPEN_GL) {
		x1 = y1 = -GL_MAX_DRAWING_COORDINATE;
		x2 = y2 = GL_MAX_DRAWING_COORDINATE;
	} else {
		x1 = xmin;
		y1 = ymin;
		x2 = xmax;
		y2 = ymax;
	}

	double p1x, p1y, p2x, p2y; // the same notation as in CGAL
	p1x = px; p1y = py;
	p2x = px + dirx; p2y = py + diry;

	double dx = p1x - p2x, dy = p1y - p2y;

	if (dx==0 && dy==0) return QRectF();

	if (std::fabs(dx)>std::fabs(dy)) {
		y1=p1y+(x1-p1x)*dy/dx;
		y2=p1y+(x2-p1x)*dy/dx;
	} else {
		x1=p1x+(y1-p1y)*dx/dy;
		x2=p1x+(y2-p1y)*dx/dy;
	}

	switch (renderer_type) {
		case OPEN_GL:
//			makeCurrent();
			if (with_begin_end) glBegin(GL_LINES);	
			glVertex2d(x1,y1);
			glVertex2d(x2,y2);
			if (with_begin_end) glEnd();
			break;
		case QT_PAINTER:
			painter->setPen(line_pen);
			painter->drawLine(x_pixel_d(x1),y_pixel_d(y1),x_pixel_d(x2),y_pixel_d(y2));
			break;
		default:
			//never should happen;
			std::cout << LOG_ERROR << "Not good renderer for GL_draw_layer" << std::endl;
	}
	return QRectF(px, py, 0, 0);
}

QRectF GL_widget_2::draw_ray(double p1dx, double p1dy, double p2dx, double p2dy, bool with_begin_end) {

	double draw_x_min, draw_x_max, draw_y_min, draw_y_max;
	if (renderer_type==OPEN_GL) {
		draw_x_min = draw_y_min = -GL_MAX_DRAWING_COORDINATE;
		draw_x_max = draw_y_max = GL_MAX_DRAWING_COORDINATE;
	} else {
		draw_x_min = xmin;
		draw_y_min = ymin;
		draw_x_max = xmax;
		draw_y_max = ymax;
	}

	const double dx=p1dx-p2dx, dy=p1dy-p2dy;
	if (dx==0 && dy==0) return QRectF(p1dx,p1dy,0,0);

	double x,y;
	if (std::fabs(dx)>std::fabs(dy)) {
		if (p1dx<p2dx) x = draw_x_max;
		else x = draw_x_min;
		y=p1dy+(x-p1dx)*dy/dx;
	}
	else {
		if (p1dy<p2dy) y = draw_y_max;
		else y = draw_y_min;
		x=p1dx+(y-p1dy)*dx/dy;
	}

	switch (renderer_type) {
		case OPEN_GL:
//			makeCurrent();
			if (with_begin_end) glBegin(GL_LINES);	
			glVertex2d(p1dx,p1dy);
			glVertex2d(x,y);
			if (with_begin_end) glEnd();
			break;
		case QT_PAINTER:
			painter->setPen(line_pen);
			painter->drawLine(x_pixel_d(p1dx),y_pixel_d(p1dy),x_pixel_d(x),y_pixel_d(y));
			break;
		default:
			//never should happen;
			std::cout << LOG_ERROR << "Not good renderer for GL_draw_layer" << std::endl;
	}
	return QRectF(p1dx, p1dy, 0, 0);
}




void GL_widget_2::start_segments() {
	switch (renderer_type) {
		case OPEN_GL:
//			makeCurrent();
			glBegin(GL_LINES);	
			break;
		case QT_PAINTER:
			// nothing to do
			break;
		default:
			//never should happen;
			std::cout << LOG_ERROR << "Not good renderer for GL_draw_layer" << std::endl;

	}
}

void GL_widget_2::end_segments() {
	switch (renderer_type) {
		case OPEN_GL:
//			makeCurrent();
			glEnd();
			break;
		case QT_PAINTER:
			// nothing to do
			break;
		default:
			//never should happen;
			std::cout << LOG_ERROR << "Not good renderer for GL_draw_layer" << std::endl;

	}
}

QRectF GL_widget_2::draw_pixmap(const QPixmap& pixmap, GLuint &texture) {
	double half_width = pixmap.width()/2.0;
	double half_height = pixmap.height()/2.0;
	if (renderer_type == OPEN_GL) {
		makeCurrent();
		texture = bindTexture(pixmap, GL_TEXTURE_2D);
//		std::cout << "texture  number: " << texture << std::endl;
		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);
			glTexCoord2d(0,0);
			glVertex2d(-half_width, -half_height);
			glTexCoord2d(1,0);
			glVertex2d(half_width, -half_height);
			glTexCoord2d(1,1);
			glVertex2d(half_width, half_height);
			glTexCoord2d(0,1);
			glVertex2d(-half_width, half_height);
		glEnd();
//		deleteTexture(texture);
		glBindTexture(GL_TEXTURE_2D,0); //unbind texture

	}
	if (renderer_type == QT_PAINTER) {
		/// \todo draw_pixmap for QT_PAINTER
	}
	return QRectF(-half_width, -half_height, pixmap.width(), pixmap.height());
}

QRectF GL_widget_2::draw_polygon(const QPolygonF& polygon, bool with_begin_end) {
	if (with_begin_end) start_segments();
	for (int i=0; i<polygon.size()-1; i++) {
		QPointF start = polygon.at(i);
		QPointF end = polygon.at(i+1);
		draw_segment(start.x(), start.y(), end.x(), end.y(), false);
	}
	if (with_begin_end) end_segments();
	return polygon.boundingRect();
}

QRectF GL_widget_2::draw_triangle(double x1, double y1, double x2, double y2, double x3, double y3, bool with_begin_end) {
	double xmin, ymin, xmax, ymax;
	xmin = xmax = x1; ymin = ymax = y1;
	if (x2<xmin) xmin = x2; if (x2>xmax) xmax = x2;
	if (y2<ymin) ymin = y2; if (y2>ymax) ymax = y2;
	if (x3<xmin) xmin = x3; if (x3>xmax) xmax = x3;
	if (y3<ymin) ymin = y3; if (y3>ymax) ymax = y3;
	if (renderer_type == OPEN_GL) {
//			makeCurrent();
			if (with_begin_end) glBegin(GL_TRIANGLES);	
			glVertex2d(x1,y1);
			glVertex2d(x2,y2);
			glVertex2d(x3,y3);
			if (with_begin_end) glEnd();
	}
	if (renderer_type == QT_PAINTER) {
		QPen temp_pen = line_pen;
		temp_pen.setWidth(0);
		painter->setPen(temp_pen);
		QBrush brush_before = painter->brush();
		painter->setBrush(temp_pen.color());
		QPointF points[3] = {
			QPointF(x_pixel_d(x1), y_pixel_d(y1)),
			QPointF(x_pixel_d(x2), y_pixel_d(y2)),
			QPointF(x_pixel_d(x3), y_pixel_d(y3)),
		 };

		painter->drawPolygon(points,3);
		painter->setBrush(brush_before);
	}
	return QRectF(xmin, ymin, xmax-xmin, ymax-ymin);
}


void GL_widget_2::start_triangles() {
	switch (renderer_type) {
		case OPEN_GL:
//			makeCurrent();
			glBegin(GL_TRIANGLES);	
			break;
		case QT_PAINTER:
			// nothing to do
			break;
		default:
			//never should happen;
			std::cout << LOG_ERROR << "Not good renderer for GL_draw_layer" << std::endl;
	}
}

void GL_widget_2::end_triangles() {
	end_segments();
}


