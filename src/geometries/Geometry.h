/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Geometry.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_GEOMETRY_H
#define MESECINA_GEOMETRY_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>

#include <QtCore/QPointF>
#include <QtCore/QString>
#include <QObject>
#include <list>
#include <vector>
#include <string>
#include <gui/app/Application_variable.h>
#include <gui/app/Point3D.h>

//class Main_window;
class Managable_layer;
//class Evolution;

class Geometry : public QObject {
	Q_OBJECT

public:
	
	//virtual functions

	Geometry();
	virtual ~Geometry() {}
	virtual Geometry* clone() { return 0;	}

#ifdef MESECINA_3D
	virtual void add_points(std::list<Point3D>*) {}
	virtual std::list<Point3D>* get_points() { return 0; }

	virtual void add_weighted_points(std::list<Point4D>*) {}
	virtual std::list<Point4D>* get_weighted_points() { return 0; }
#else
	virtual void add_point(double, double) {}
	virtual void add_points(std::list<QPointF>*) {}
	virtual std::list<QPointF>* get_points() { return 0; }

	virtual void add_weighted_point(const Point3D&) {}
	virtual void add_weighted_points(const std::list<Point3D>*) {}
	virtual std::list<Point3D>* get_weighted_points() { return 0; }
#endif

	virtual void load_generic_file(const std::string&) {}
	virtual void save_generic_file(const std::string&) {}

	virtual std::list<std::string> offer_structures() { return std::list<std::string>(); }
	virtual void* give_structure(const std::string&) { return 0; }
	virtual void receive_structure_changed(const std::string&) {}
	virtual void application_settings_changed(const QString&) {}
	virtual void apply_modification(const std::string& ) {}

	virtual size_t layers_size() { return l_layers.size(); }
	virtual Managable_layer* layers(size_t i) { return l_layers[i]; }

	virtual size_t evolutions_size() { return l_evolutions.size(); }
	virtual std::string evolutions(size_t i) { return l_evolutions[i]; }

	virtual size_t app_variables_size() { return l_app_variables.size(); }
	virtual Application_variable app_variables(size_t i) { return l_app_variables[i]; }

signals:
	// communicate with gui
	void announce_structure_changed(const std::string& name);
	void* request_structure(const std::string& name);
	void do_widget_repaint();
	void do_statistics_repaint();

protected:
	//self managing functions

	// to be called at the construction time
	void add_layer(Managable_layer* l);
	void add_evolution(const std::string& name);

	// can be called whenever
	void add_variable(const char* name, float val);
	void add_variable(const char* name, const char* val);

	void invalidate_all_layers();

	std::vector<Application_variable> l_app_variables;
	std::vector<Managable_layer*> l_layers;
	std::vector<std::string> l_evolutions;
};

typedef CGAL::Exact_predicates_inexact_constructions_kernel Double_kernel;
//typedef CGAL::Exact_predicates_exact_constructions_kernel Exact_kernel;
//typedef CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt Exact_kernel;


#ifdef _DEBUG
typedef CORE::Expr Lazy_core;
#else
typedef CGAL::Lazy_exact_nt< CORE::Expr > Lazy_core;
#endif
typedef CGAL::Simple_cartesian<Lazy_core >  Exact_kernel;

#endif //MESECINA_GEOMETRY_H