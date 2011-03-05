/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info at http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Delaunay_layer_2.h 132 2007-07-25 16:50:33Z miklosb $
 */

#ifndef MESECINA_DELAUNAY_LAYER_2_H
#define MESECINA_DELAUNAY_LAYER_2_H

#include <gui/gl_widget_2/GL_draw_layer_2.h>

template <class Power_crust_2>
class Delaunay_layer_2 : public GL_draw_layer_2 {
public:
	typedef typename Power_crust_2::Triangulation_2						 Triangulation_2;
	typedef typename Triangulation_2::Face_handle							 Face_handle;
	typedef typename Triangulation_2::Finite_vertices_iterator	Finite_vertices_iterator;
	typedef typename Power_crust_2::Point_2  Point_2;
	typedef typename Power_crust_2::Line_2  Line_2;
	typedef typename Power_crust_2::Ray_2  Ray_2;
	typedef typename Power_crust_2::Segment_2 Segment_2;
	typedef typename Triangulation_2::Finite_edges_iterator		   Finite_edges_iterator;

	Delaunay_layer_2(const QString& name, Power_crust_2* m, const QString& tt = "", bool _do_voronoi = false, bool _do_crust = false, bool _do_normals = false) : 
		GL_draw_layer_2(name,tt), parent(m), do_voronoi(_do_voronoi), do_crust(_do_crust), do_normals(_do_normals) {
			Application_settings::add_double_setting("curve-normal-rendering-length",15);
			Application_settings::add_bool_setting("curve-normal-flip",false);
		}

	virtual void draw_commands() {
		if (do_crust) { // do only reconstruction edges
			Triangulation_2* t = parent->get_crust_classified_triangulation();
			if (do_normals) {
				QVector< QPointF >* ordered_normals = parent->get_ordered_normals();
				QVector< QPointF >* ordered_points = parent->get_ordered_points();
				double norm_length = Application_settings::get_double_setting("curve-normal-rendering-length");
				if (Application_settings::get_bool_setting("curve-normal-flip")) norm_length = -1 * norm_length;
				for (int i=0; i< ordered_normals->size(); i++) {
					*this << QLineF(ordered_points->at(i), ordered_points->at(i) + (ordered_normals->at(i)) * norm_length);
				}
			} else
				if (do_voronoi) {
					*this << NO_BOUNDING;
					for (Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
						Face_handle f = e_it->first;
						int id = e_it->second;
						if (f->get_edge_classification(id) != C_CRUST) {
							CGAL::Object o = t->dual(e_it);
							Line_2  l;
							Ray_2   r;
							Segment_2 s;
							if (CGAL::assign(s,o)) *this << s;
							if (CGAL::assign(r,o)) *this << r;
							if (CGAL::assign(l,o)) *this << l;
						}
					}
					*this << DO_BOUNDING;
				} else {
					for (Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
						Face_handle f = e_it->first;
						int id = e_it->second;
						if (f->get_edge_classification(id)==C_CRUST) {
							Point_2 p1 = f->vertex((id+1)%3)->point();
							Point_2 p2 = f->vertex((id+2)%3)->point();
							*this << Segment_2(p1,p2);
						}
					}
				}
		}
		else { // do whole delaunay triangulation/voronoi diagram
			Triangulation_2* t = parent->get_input_triangulation();
			if (do_voronoi) {
				*this << NO_BOUNDING;
				for (Finite_edges_iterator e_it = t->finite_edges_begin(), e_end = t->finite_edges_end();	e_it != e_end; ++e_it) {
					CGAL::Object o = t->dual(e_it);
					Line_2  l;
					Ray_2   r;
					Segment_2 s;
					if (CGAL::assign(s,o)) *this << s;
					if (CGAL::assign(r,o)) *this << r;
					if (CGAL::assign(l,o)) *this << l;
				}
				*this << DO_BOUNDING;
			} else
				*this << *t;
		}
	}

	virtual bool has_property(Layer_property prop) {
		switch (prop) {
		case COLOR_EDITABLE:
		case LINE_WIDTH_EDITABLE:
			return true;
			break;
		case USER_PROPERTY_1:
			return do_crust && !do_voronoi;
		case POINT_SIZE_EDITABLE:
		default:
			return false;
			break;
		}
	}

	virtual void application_settings_changed(const QString& settings_name) {
		if (do_crust && do_normals && (settings_name=="curve-normal-rendering-length" || settings_name == "curve-normal-flip")) {
			invalidate_cache();
			widget->request_repaint();
		}
	}

	virtual QString get_name_user_property(Layer_property l) { 
		if (l== USER_PROPERTY_1)
			return "Points normals to file";
		return "";
	}

	virtual void execute_user_property(Layer_property l) {
		if (l == USER_PROPERTY_1) {
			QSettings settings;
			QString file_name = QFileDialog::getSaveFileName(
				0,
				"Choose an off filename to save points and normals to",
				settings.value("last-data-directory",QString()).toString(),
				"NPOFF (*.npoff)");
			if (file_name=="") return;
			if (!file_name.endsWith(".npoff")) file_name += ".npoff";
			QString save_directory = file_name;
			save_directory.truncate(file_name.lastIndexOf('/'));
			settings.setValue("last-data-directory",save_directory);


			// open file
			QFile f(file_name);
			if ( !f.open( QIODevice::WriteOnly ) ) {
				std::cout << "File " << file_name.toStdString() << " could not be open for writing!" << std::endl;
				return;
			}


			QTextStream fs( &f );


				QVector< QPointF >* ordered_normals = parent->get_ordered_normals();
				QVector< QPointF >* ordered_points = parent->get_ordered_points();
				double norm_length = 1;
				if (Application_settings::get_bool_setting("curve-normal-flip")) norm_length = -1 * norm_length;

				int offset = 0;
				for (int i=0; i< ordered_normals->size(); i++) {
					if (i==ordered_normals->size()-1 && ordered_points->at(0) == ordered_points->at(i)) {
//						std::cout << "skip" << std::endl;
						offset = -1;
						continue;
					}
					QPointF p = ordered_points->at(i);
					fs << tr("%1 %2  ").arg(p.x(), 0, 'f', 9).arg(p.y(), 0, 'f', 9);
					p = ordered_normals->at(i) * norm_length;
					fs << tr("%1 %2\n").arg(p.x(), 0, 'f', 9).arg(p.y(), 0, 'f', 9);
				}

			f.close();
			std::cout << ordered_normals->size()+offset << " points + normals written in "<<file_name.toStdString() << std::endl;


		}
	}

private:
	Power_crust_2 *parent;
	bool do_voronoi, do_crust, do_normals;
};


#endif //MESECINA_DELAUNAY_LAYER_2_H