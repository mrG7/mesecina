/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Union_of_balls_2.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_DUMMY_3_H
#define MESECINA_DUMMY_3_H

#include <geometries/Geometry.h>

#include <list>
#include <vector>
#include <string>


template <class K> // Kernel used in all computations
class Dummy_3 : public Geometry {

public:
	Dummy_3();
	virtual ~Dummy_3();
	virtual Geometry* clone();
	
	// methods to communicate with other geometries
	virtual std::list<std::string> offer_structures();
	virtual void* give_structure(const std::string& name);
	virtual void receive_structure_changed(const std::string& name);
	
	// points communication with user interface
	virtual void add_points(std::list<Point3D>*);
	virtual std::list<Point3D>* get_points();

	// ball communication with user interface
	virtual void add_weighted_points(std::list<Point4D>*);
	virtual std::list<Point4D>* get_weighted_points();

	// directly handle file io
	virtual void load_generic_file(const std::string&);
	virtual void save_generic_file(const std::string&);

	// receive application settings changes
	virtual void application_settings_changed(const QString&);
	// modification for evolutions
	virtual void apply_modification(const std::string& );

	void invalidate_cache();
};

#endif //MESECINA_DUMMY_3_H