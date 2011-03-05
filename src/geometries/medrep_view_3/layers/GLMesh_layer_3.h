/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Input_mesh_layer_3.h 388 2008-12-12 18:17:06Z miklosb $
 */

#ifndef MESECINA_GLMESH_LAYER_3_H
#define MESECINA_GLMESH_LAYER_3_H

#include <geometries/medrep_view_3/Medrep_view_3.h>
#include <gui/gl_widget_3/GL_draw_layer_3.h>

class GLMesh_layer_3 : public GL_draw_layer_3 {
public:
	GLMesh_layer_3(const QString& name, Medrep_view_3* m, QString ttip, bool wireframe) : GL_draw_layer_3(name,ttip), parent(m) {
		do_wireframe = wireframe;
	}

	virtual void prepare_draw() {
		//std::cout << "preparing layer" << std::endl;
		//GLMesh* m = get_mesh();
		//if (!m) return;
		


	}

	void draw_triangle(const GLMesh* glmesh, GLsizei triangle_index) {
		// put out normal
		*this << NORMAL;
		GLsizei i = triangle_index;
		Point_3 normal(glmesh->face_normals[3*i], glmesh->face_normals[3*i+1], glmesh->face_normals[3*i+2]);
		*this << normal;

		// put out triangle
		GLuint vertex_index = glmesh->indices[3*i];
		Point_3 p0 = Point_3(
				glmesh->vertices[3*vertex_index],
				glmesh->vertices[3*vertex_index+1],
				glmesh->vertices[3*vertex_index+2]);
		vertex_index = glmesh->indices[3*i+1];
		Point_3 p1 = Point_3(
				glmesh->vertices[3*vertex_index],
				glmesh->vertices[3*vertex_index+1],
				glmesh->vertices[3*vertex_index+2]);
		vertex_index = glmesh->indices[3*i+2];
		Point_3 p2 = Point_3(
				glmesh->vertices[3*vertex_index],
				glmesh->vertices[3*vertex_index+1],
				glmesh->vertices[3*vertex_index+2]);
		*this << Triangle_3(p0, p1, p2);
	}

	void draw_edge(const GLMesh* glmesh, GLsizei edge_index) {
		GLsizei i = edge_index;

		// put out triangle
		GLuint vertex_index = glmesh->edge_indices[2*i];
		Point_3 p0 = Point_3(
				glmesh->vertices[3*vertex_index],
				glmesh->vertices[3*vertex_index+1],
				glmesh->vertices[3*vertex_index+2]);
		vertex_index = glmesh->edge_indices[2*i+1];
		Point_3 p1 = Point_3(
				glmesh->vertices[3*vertex_index],
				glmesh->vertices[3*vertex_index+1],
				glmesh->vertices[3*vertex_index+2]);
		*this << Segment_3(p0, p1);
	}

	virtual GLMesh* get_mesh() {
		return 0;
	}

	virtual void draw_commands() {
		GLMesh* m = get_mesh();
		if (!m) return;

		//glEnableClientState(GL_VERTEX_ARRAY);
		//glVertexPointer(3, GL_FLOAT, 0, m->vertices);

		//update_bounding_box(Box3D(0,0,0,1,1,1));

		//glBegin(GL_TRIANGLES);
		//for (GLsizei i=0; i<m->indices_count/3; ++i) {
		//	*this << NORMAL;
		//	Point_3 normal(m->face_normals[3*i], m->face_normals[3*i+1], m->face_normals[3*i+2]);
		//	*this << normal;
		//	//glDrawElements(GL_TRIANGLES, 3, GL_FLOAT, m->indices+3*i);
		//	//glArrayElement(3*i);
		//	//glArrayElement(3*i+1);
		//	//glArrayElement(3*i+2);
		//	glArrayElement(m->indices[3*i]);
		//	glArrayElement(m->indices[3*i+1]);
		//	glArrayElement(m->indices[3*i+2]);
		//}
		//glEnd();


//		std::cout << "draw medrep_input_mesh" << std::endl;
		//std::cout << "vertices: " << m->vertex_count << std::endl;
		//std::cout << "indices: " << m->indices_count << std::endl;
		if (do_wireframe) {
			*this << SEGMENTS_START;
//			std::cout << "edges: " << m->edge_indices_count << std::endl;
			for (GLsizei i=0; i<m->edge_indices_count/2; ++i) {
				draw_edge(m, i);
			}
			*this << SEGMENTS_END;
		} else {
			*this << TRIANGLES_START;
			for (GLsizei i=0; i<m->indices_count/3; ++i) {
				draw_triangle(m, i);
			}
			*this << TRIANGLES_END;
		}

	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
			return true;
			break;
		case LINE_WIDTH_EDITABLE:
			return do_wireframe;
		default:
			return false;
			break;
		}
	}

protected:
	Medrep_view_3 *parent;
	bool do_wireframe;
};


#endif //MESECINA_MESH_LAYER_3_H