/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Point3D.h 529 2009-01-27 23:15:25Z miklosb $
 */

#ifndef MESECINA_POINT3D_H
#define MESECINA_POINT3D_H

class Point3D {
public:
	Point3D() { x = y = z = 0; }

	Point3D(double x_, double y_, double z_) {
		x = x_; y = y_; z = z_;
	}
	double x, y, z;
};

class Point4D {
public:
	Point4D() { x = y = z = w = 0; }

	Point4D(double x_, double y_, double z_, double w_) {
		x = x_; y = y_; z = z_; w = w_;
	}
	double x, y, z, w;
};

#endif //MESECINA_POINT3D_H