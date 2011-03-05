/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Input_mesh_layer_3.h 388 2008-12-12 18:17:06Z miklosb $
 */

#ifndef MESECINA_MEDREP_INPUT_MESH_LAYER_3_H
#define MESECINA_MEDREP_INPUT_MESH_LAYER_3_H

//#include <geometries/medrep_view_3/Medrep_view_3.h>
#include "GLMesh_layer_3.h"

class Medrep_input_mesh_layer_3 : public GLMesh_layer_3 {
public:
	Medrep_input_mesh_layer_3(const QString& name, Medrep_view_3* m, QString ttip, bool wireframe) : GLMesh_layer_3(name, m, ttip, wireframe) {
	}

	virtual GLMesh* get_mesh() {
		return parent->get_input_mesh();
	}

};

class Medrep_skeleton_mesh_layer_3 : public GLMesh_layer_3 {
public:
	Medrep_skeleton_mesh_layer_3(const QString& name, Medrep_view_3* m, QString ttip, bool wireframe) : GLMesh_layer_3(name, m, ttip, wireframe) {
	}

	virtual GLMesh* get_mesh() {
		return parent->get_skeleton_mesh();
	}

};

class Medrep_reconstruction_mesh_layer_3 : public GLMesh_layer_3 {
public:
	Medrep_reconstruction_mesh_layer_3(const QString& name, Medrep_view_3* m, QString ttip, bool wireframe) : GLMesh_layer_3(name, m, ttip, wireframe) {
	}

	virtual GLMesh* get_mesh() {
		return parent->get_reconstruction_mesh();
	}

};
#endif //MESECINA_MEDREP_INPUT_MESH_LAYER_3_H