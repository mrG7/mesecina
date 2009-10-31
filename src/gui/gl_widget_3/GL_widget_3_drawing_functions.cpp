/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_widget_3_drawing_functions.cpp 378 2008-08-03 18:50:59Z miklosb $
 */

#include <stdlib.h>
//#include <GL/glut.h>
#include <gui/gl_widget_3/GL_widget_3.h>
#include <constants.h>

Box3D GL_widget_3::draw_sphere(double orig_x, double orig_y, double orig_z, double radius) {
	glPushMatrix();
	glTranslated(orig_x, orig_y, orig_z);
	glScaled(radius, radius, radius);
		glCallList(sphere_dl);
	glPopMatrix();
//	std::cout << "sphere display list " << sphere_dl << std::endl;

	return Box3D(orig_x-radius, orig_y-radius, orig_z-radius,
				 orig_x+radius, orig_y+radius, orig_z+radius);
}

Box3D GL_widget_3::draw_sphere_wireframe(double orig_x, double orig_y, double orig_z, double radius) {
	glPushMatrix();
	glTranslated(orig_x, orig_y, orig_z);
	glScaled(radius, radius, radius);
		glCallList(sphere_wire_dl);
	glPopMatrix();
//	std::cout << "sphere display list " << sphere_dl << std::endl;

	return Box3D(orig_x-radius, orig_y-radius, orig_z-radius,
				 orig_x+radius, orig_y+radius, orig_z+radius);

}

Box3D GL_widget_3::draw_polygon(const QVector<Point3D>& polygon) {
	double xmin, xmax, ymin, ymax, zmin, zmax;
//	glNormal3f(0.0,1.0,0.0);
	glBegin(GL_POLYGON);
	//glVertex3d(0,0,0);
	//glVertex3d(0,0,1);
	//glVertex3d(0,0,1);
	//glVertex3d(0,1,1);
	//glVertex3d(2,1,0);
	//glEnd();
	//return Box3D(0, 0, 0, 2, 1, 1);

//	std::cout << LOG_GREEN << "polygon start" << std::endl;
	QVector<Point3D>::const_iterator p_it, p_end = polygon.end(); /*int i = 0;*/
	for (p_it = polygon.begin(); p_it!=p_end/* && i<3*/; p_it++/*, i++*/) {
//		std::cout << "p: " << p_it->x << ", " << p_it->y << ", " << p_it->z << std::endl;
		glVertex3d(p_it->x,p_it->y,p_it->z);
		if (p_it==polygon.begin()) {
			xmin = xmax = p_it->x;
			ymin = ymax = p_it->y;
			zmin = zmax = p_it->z;
		} else {
			if (p_it->x < xmin) xmin = p_it->x; if (p_it->x > xmax) xmax = p_it->x;
			if (p_it->y < ymin) ymin = p_it->y; if (p_it->y > ymax) ymax = p_it->y;
			if (p_it->z < zmin) zmin = p_it->z; if (p_it->z > zmax) zmax = p_it->z;
		}
	}
	glEnd();

	return Box3D(xmin, ymin, zmin, xmax, ymax, zmax);
}

Box3D GL_widget_3::draw_polygon_lines(const QVector<Point3D>& polygon) {
	double xmin, xmax, ymin, ymax, zmin, zmax;
	glBegin(GL_LINE_LOOP);

//	std::cout << LOG_GREEN << "polygon start" << std::endl;
	QVector<Point3D>::const_iterator p_it, p_end = polygon.end(); /*int i = 0;*/
	for (p_it = polygon.begin(); p_it!=p_end/* && i<3*/; p_it++/*, i++*/) {
//		std::cout << "p: " << p_it->x << ", " << p_it->y << ", " << p_it->z << std::endl;
		glVertex3d(p_it->x,p_it->y,p_it->z);
		if (p_it==polygon.begin()) {
			xmin = xmax = p_it->x;
			ymin = ymax = p_it->y;
			zmin = zmax = p_it->z;
		} else {
			if (p_it->x < xmin) xmin = p_it->x; if (p_it->x > xmax) xmax = p_it->x;
			if (p_it->y < ymin) ymin = p_it->y; if (p_it->y > ymax) ymax = p_it->y;
			if (p_it->z < zmin) zmin = p_it->z; if (p_it->z > zmax) zmax = p_it->z;
		}
	}
	glEnd();

	return Box3D(xmin, ymin, zmin, xmax, ymax, zmax);
}

//QRectF GL_widget_3::draw_disk(double orig_x, double orig_y, double radius) {
//	if (renderer_type == OPEN_GL) {
//		makeCurrent();
//		GLfloat vectorX, vectorY, originX, originY;
//		float angle;
//		originX = orig_x;
//		originY = orig_y;
//
//		glBegin(GL_TRIANGLE_FAN);	
//		glVertex3d(originX,originY);
//		for(int i=0;i<=360;i+=2)	{
//			angle=(float)(((double)i)/57.29577957795135);	
//			vectorX=originX+(radius*(float)sin((double)angle));
//			vectorY=originY+(radius*(float)cos((double)angle));		
//			glVertex3d(vectorX,vectorY);
//		}
//		glEnd();
//	} else
//		if (renderer_type == QT_PAINTER) {
//			QPen temp_pen = line_pen;
//			temp_pen.setWidth(0);
//			painter->setPen(temp_pen);
//			QBrush brush_before = painter->brush();
//			painter->setBrush(temp_pen.color());
//			double rx = x_pixel_dist_d(radius);
//			double ry = x_pixel_dist_d(radius);
//			painter->drawEllipse(x_pixel_d(orig_x)-rx, y_pixel_d(orig_y)-ry, 2*rx, 2*ry);
//			painter->setBrush(brush_before);
//		}
//	return QRectF(orig_x-radius, orig_y-radius, 2*radius, 2*radius);
//}

void GL_widget_3::set_normal(double x, double y, double z) {
	makeCurrent();
	glNormal3f(x,y,z);
//	std::cout << "normal:  " << x << " " << y << " " << z << std::endl;
}

Box3D GL_widget_3::draw_point(double x, double y, double z, bool with_begin_end) {
	makeCurrent();
	if (with_begin_end) glBegin(GL_POINTS);	
	glVertex3d(x,y,z);
	if (with_begin_end) glEnd();
	return Box3D(x,y,z,x,y,z);
}

Box3D GL_widget_3::draw_segment(double x1, double y1, double z1, double x2, double y2, double z2, bool with_begin_end) {
	double xmin, xmax, ymin, ymax, zmin, zmax;
	xmin = x1; xmax = x2; 
	ymin = y1; ymax = y2;
	zmin = z1; zmax = z2;
	if (xmax<xmin) { double temp = xmin; xmin = xmax; xmax = temp; }
	if (ymax<ymin) { double temp = ymin; ymin = ymax; ymax = temp; }
	if (zmax<zmin) { double temp = zmin; zmin = zmax; zmax = temp; }


	makeCurrent();
//	std::cout << "segment rendered with GL_LINES: " << with_begin_end << std::endl;
	if (with_begin_end) glBegin(GL_LINES);
	glVertex3d(x1,y1,z1);
	glVertex3d(x2,y2,z2);
	if (with_begin_end) glEnd();
	return Box3D(xmin, ymin, zmin, xmax, ymax, zmax);
}

//QRectF GL_widget_3::draw_line(double px, double py, double dirx, double diry, bool with_begin_end) {
//	double x1, y1, x2, y2;
//	if (renderer_type==OPEN_GL) {
//		x1 = y1 = -GL_MAX_DRAWING_COORDINATE;
//		x2 = y2 = GL_MAX_DRAWING_COORDINATE;
//	} else {
//		x1 = xmin;
//		y1 = ymin;
//		x2 = xmax;
//		y2 = ymax;
//	}
//
//	double p1x, p1y, p2x, p2y; // the same notation as in CGAL
//	p1x = px; p1y = py;
//	p2x = px + dirx; p2y = py + diry;
//
//	double dx = p1x - p2x, dy = p1y - p2y;
//
//	if (dx==0 && dy==0) return QRectF();
//
//	if (std::fabs(dx)>std::fabs(dy)) {
//		y1=p1y+(x1-p1x)*dy/dx;
//		y2=p1y+(x2-p1x)*dy/dx;
//	} else {
//		x1=p1x+(y1-p1y)*dx/dy;
//		x2=p1x+(y2-p1y)*dx/dy;
//	}
//
//	switch (renderer_type) {
//		case OPEN_GL:
//			makeCurrent();
//			if (with_begin_end) glBegin(GL_LINES);	
//			glVertex3d(x1,y1);
//			glVertex3d(x2,y2);
//			if (with_begin_end) glEnd();
//			break;
//		case QT_PAINTER:
//			painter->setPen(line_pen);
//			painter->drawLine(x_pixel_d(x1),y_pixel_d(y1),x_pixel_d(x2),y_pixel_d(y2));
//			break;
//		default:
//			//never should happen;
//			bool not_good_renderer_in_GL_draw_layer_2 = true;
//			assert(not_good_renderer_in_GL_draw_layer_2);
//	}
//	return QRectF(px, py, 0, 0);
//}

//QRectF GL_widget_3::draw_ray(double p1dx, double p1dy, double p2dx, double p2dy, bool with_begin_end) {
//
//	double draw_x_min, draw_x_max, draw_y_min, draw_y_max;
//	if (renderer_type==OPEN_GL) {
//		draw_x_min = draw_y_min = -GL_MAX_DRAWING_COORDINATE;
//		draw_x_max = draw_y_max = GL_MAX_DRAWING_COORDINATE;
//	} else {
//		draw_x_min = xmin;
//		draw_y_min = ymin;
//		draw_x_max = xmax;
//		draw_y_max = ymax;
//	}
//
//	const double dx=p1dx-p2dx, dy=p1dy-p2dy;
//	if (dx==0 && dy==0) return QRectF(p1dx,p1dy,0,0);
//
//	double x,y;
//	if (std::fabs(dx)>std::fabs(dy)) {
//		if (p1dx<p2dx) x = draw_x_max;
//		else x = draw_x_min;
//		y=p1dy+(x-p1dx)*dy/dx;
//	}
//	else {
//		if (p1dy<p2dy) y = draw_y_max;
//		else y = draw_y_min;
//		x=p1dx+(y-p1dy)*dx/dy;
//	}
//
//	switch (renderer_type) {
//		case OPEN_GL:
//			makeCurrent();
//			if (with_begin_end) glBegin(GL_LINES);	
//			glVertex3d(p1dx,p1dy);
//			glVertex3d(x,y);
//			if (with_begin_end) glEnd();
//			break;
//		case QT_PAINTER:
//			painter->setPen(line_pen);
//			painter->drawLine(x_pixel_d(p1dx),y_pixel_d(p1dy),x_pixel_d(x),y_pixel_d(y));
//			break;
//		default:
//			//never should happen;
//			bool not_good_renderer_in_GL_draw_layer_2 = true;
//			assert(not_good_renderer_in_GL_draw_layer_2);
//	}
//	return QRectF(p1dx, p1dy, 0, 0);
//}

void GL_widget_3::set_color(float red, float green, float blue, float alpha) {
	makeCurrent();

	GLfloat ambient_color[4] = { red, green, blue, alpha };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,  ambient_color);

	glColor4f(red, green, blue, alpha);
}


void GL_widget_3::start_segments() {
	makeCurrent();
	glBegin(GL_LINES);	

}

void GL_widget_3::end_segments() {
	makeCurrent();
	glEnd();
}


void GL_widget_3::start_points() {
	makeCurrent();
	glBegin(GL_POINTS);	

}

void GL_widget_3::end_points() {
	makeCurrent();
	glEnd();
}

Box3D GL_widget_3::draw_triangle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double xn, double yn, double zn, bool with_begin_end) {
	double xmin, ymin, zmin, xmax, ymax, zmax;
	xmin = xmax = x1; ymin = ymax = y1; zmin = zmax = z1;
	if (x2<xmin) xmin = x2; if (x2>xmax) xmax = x2;
	if (y2<ymin) ymin = y2; if (y2>ymax) ymax = y2;
	if (z2<zmin) zmin = z2; if (z2>zmax) zmax = z2;
	if (x3<xmin) xmin = x3; if (x3>xmax) xmax = x3;
	if (y3<ymin) ymin = y3; if (y3>ymax) ymax = y3;
	if (z3<zmin) zmin = z3; if (z3>zmax) zmax = z3;
	makeCurrent();
	glNormal3f(xn,yn,zn);
	if (with_begin_end) glBegin(GL_TRIANGLES);	
	glVertex3d(x1,y1,z1);
//	glNormal3f(xn,yn,zn);
	glVertex3d(x2,y2,z2);
//	glNormal3f(xn,yn,zn);
	glVertex3d(x3,y3,z3);
	if (with_begin_end) glEnd();

	return Box3D(xmin, ymin, zmin, xmax, ymax, zmax);
}


void GL_widget_3::start_triangles() {

	makeCurrent();
	glBegin(GL_TRIANGLES);	
}

void GL_widget_3::end_triangles() {
	end_segments();
}


