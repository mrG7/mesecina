/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_MEDIAL_EXPLORE_3_H
#define MESECINA_MEDIAL_EXPLORE_3_H

#include <geometries/Geometry.h>

#include <list>
#include <vector>
#include <string>

#ifdef MESECINA_GEOMETRY_LIBRARY
#define MV_API   __declspec( dllexport )
#else
#define MV_API   __declspec( dllimport )
#endif

#include "Medial_axis_structure_3.h"

using namespace medial;

class MV_API Medial_explore_3 : public Geometry {

public:
	Medial_explore_3();
	virtual ~Medial_explore_3();
	virtual Geometry* clone();
	
	// methods to communicate with other geometries
	//virtual std::list<std::string> offer_structures();
	//virtual void* give_structure(const std::string& name);
	virtual void receive_structure_changed(const std::string& name);
	
	// points communication with user interface
	//virtual void add_points(std::list<Point3D>*);
	//virtual std::list<Point3D>* get_points();

	// ball communication with user interface
	//virtual void add_weighted_points(std::list<Point4D>*);
	//virtual std::list<Point4D>* get_weighted_points();

	// directly handle file io
	virtual void load_generic_file(const std::string&);
	//virtual void save_generic_file(const std::string&);

	// receive application settings changes
	virtual void application_settings_changed(const QString&);
	// modification for evolutions
	//virtual void apply_modification(const std::string& );

	Medial_axis_transform_3* get_medial_axis();
	Medial_axis_transform_3* get_topology_filtered_medial_axis();
	std::vector<std::set<Face*> >* get_medial_axis_sheets();
	std::vector<double>* get_sheet_topology_angle_stability();
	std::vector<Ball>* get_sampled_balls();
	void write_mesh_balls(const std::string& file_name, std::vector<Ball>* balls);

	void invalidate_cache();
	
	Medial_axis_transform_3 mat;
//private:
	bool has_topology_filtration, has_sheets, has_topology_sheets, has_mat, has_balls;

};

#endif //MESECINA_MEDIAL_EXPLORE_3_H