/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Armin Häberling, ETH Zurich
 *
 * $Id: Voronoi_Order_2_layer_2.h 342 2008-02-29 10:51:56Z arminha $
 */

#ifndef MESECINA_VORONOI_ORDER_2_LAYER_2_H
#define MESECINA_VORONOI_ORDER_2_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

template <class Second_voronoi_2>
class Second_voronoi_layer_2 : public GL_draw_layer_2 {

public:
	typedef typename Second_voronoi_2::Segment_2				Segment_2;
	typedef typename Second_voronoi_2::Line_2					Line_2;
	typedef typename Second_voronoi_2::Ray_2					Ray_2;
	typedef typename Second_voronoi_2::Triangulation			Triangulation;
	typedef typename Triangulation::Finite_edges_iterator Finite_edges_iterator;

	Second_voronoi_layer_2(const QString& name, Second_voronoi_2* m, const QString& tt) : GL_draw_layer_2(name, tt), parent(m) {
	}

	virtual void draw_commands() {
		Triangulation * t = parent->get_regular_triangulation();
		Finite_edges_iterator e_it, e_end = t->finite_edges_end();
		*this << NO_BOUNDING;
		for (e_it = t->finite_edges_begin(); e_it != e_end; e_it++) {
			// draw the dual
			CGAL::Object o = t->dual(e_it);
			Line_2  l;
			Ray_2   r;
			Segment_2 s;
			if (CGAL::assign(s,o)) *this << s;
			if (CGAL::assign(r,o)) *this << r;
			if (CGAL::assign(l,o)) *this << l;
		}
		*this << DO_BOUNDING;
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case LINE_WIDTH_EDITABLE:
			return true;
			break;
		case POINT_SIZE_EDITABLE:
		default:
			return false;
			break;
		}
	}

private:
	Second_voronoi_2 *parent;
};

#endif // MESECINA_VORONOI_ORDER_2_LAYER_2_H