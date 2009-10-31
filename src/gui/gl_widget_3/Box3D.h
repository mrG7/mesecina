/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Box3D.h 88 2007-04-04 21:38:55Z miklosb $
 */

#ifndef MESECINA_BOX3D_H
#define MESECINA_BOX3D_H

#ifdef MESECINA_GL_WIDGET
#define W_API   __declspec( dllexport )
#else
#define W_API   __declspec( dllimport )
#endif

class W_API Box3D {
public:
	Box3D(double xmi, double ymi, double zmi, double xma, double yma, double zma) {
		xmin = xmi; ymin = ymi; zmin = zmi; 
		xmax = xma; ymax = yma; zmax = zma; 
	}
	double xmin, ymin, zmin, xmax, ymax, zmax;
};

#endif //MESECINA_BOX3D_H