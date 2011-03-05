/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Ball_face_base_2.h 78 2007-03-25 03:19:20Z miklosb $
 */

#include <gui/app/Geometry_factory.h>

#ifdef MESECINA_3D

#include <geometries/mesh_view_3/Mesh_view_3.h>
#include <geometries/power_crust_3/Power_crust_3.h>
#include <geometries/power_crust_3/Power_crust_3_implement.h>
#include <geometries/skin_surface_3/Skin_surface_3.h>
#include <geometries/skin_surface_3/Skin_surface_3_implement.h>
//#include <geometries/structure_3/Asia_structure_3.h>
#include <geometries/moebius_3/Moebius_3.h>
#include <geometries/medial_explore_3/Medial_explore_3.h>
#include <geometries/medrep_view_3/Medrep_view_3.h>

#include <geometries/union_of_balls_3/Union_of_balls_3.h>
#include <geometries/union_of_balls_3/Union_of_balls_3_implement.h>

extern template class __declspec(dllimport) Power_crust_3<Double_kernel>;
//extern template class __declspec(dllimport) Power_crust_3<Exact_kernel>;
extern template class __declspec(dllimport) Union_of_balls_3<Double_kernel>;
//extern template class __declspec(dllimport) Union_of_balls_3<Exact_kernel>;
extern template class __declspec(dllimport) Skin_surface_3<Double_kernel>;
extern template class __declspec(dllimport) Moebius_3<Double_kernel>;

typedef Union_of_balls_3<Exact_kernel> Union_of_balls_exact_3;
typedef Union_of_balls_3<Double_kernel> Union_of_balls_double_3;

typedef Power_crust_3<Exact_kernel> Power_crust_exact_3;
typedef Power_crust_3<Double_kernel> Power_crust_double_3;

typedef Skin_surface_3<Double_kernel> Skin_surface_double_3;
typedef Moebius_3<Double_kernel> Moebius_double_3;

void Geometry_factory::fill_available_geometries() {
	available_geometries.push_back("Mesh view");
	available_geometries.push_back("Power crust - double");
//	available_geometries.push_back("Power crust - lazy exact");
	available_geometries.push_back("Union of balls - double");
//	available_geometries.push_back("Union of balls - lazy exact");
	available_geometries.push_back("Skin surface");
	available_geometries.push_back("Skin surface - scale surface");
//	available_geometries.push_back("Structure");
	available_geometries.push_back("Moebius");
	available_geometries.push_back("Medial explore");
	available_geometries.push_back("Medrep view");
}

Geometry* Geometry_factory::create_new_geometry(const std::string& name) {
	if (name == "Mesh view") {
		return new Mesh_view_3();
	}
	if (name == "Medrep view") {
		return new Medrep_view_3();
	}
	if (name == "Medial explore") {
		return new Medial_explore_3();
	}
	if (name == "Moebius") {
		return new Moebius_double_3();
	}
//	if (name == "Structure") {
//		return new Asia_structure_3();
//	}
	if (name == "Power crust - double") {
		return new Power_crust_double_3();
	}
	//if (name == "Power crust - lazy exact") {
	//	return new Power_crust_exact_3();
	//}
	if (name == "Union of balls - double") {
		return new Union_of_balls_double_3();
	}
	//if (name == "Union of balls - lazy exact") {
	//	return new Union_of_balls_exact_3();
	//}
	if (name == "Skin surface") {
		return new Skin_surface_double_3(SHARED_INNER_DOUBLE_BALLS);
	}
	if (name == "Skin surface - scale surface") {
		return new Skin_surface_double_3(SHARED_SCALE_DOUBLE_BALLS);
	}
	return 0;
}

std::list<std::string> Geometry_factory::depends_on(const std::string& name) {
	std::list<std::string> names;
	if (name == "Union of balls - double") {
		names.push_back("Power crust - double");
	}
	if (name == "Union of balls - lazy exact") {
		names.push_back("Power crust - lazy exact");
	}
	return names;
}

std::list<std::string> Geometry_factory::conflicts_with(const std::string& name) {
	std::list<std::string> names;
	if (name == "Power crust - double") {
		names.push_back("Power crust - lazy exact");
	}
	if (name == "Power crust - lazy exact") {
		names.push_back("Power crust - double");
	}
	if (name == "Union of balls - double") {
		names.push_back("Union of balls - lazy exact");
	}
	if (name == "Union of balls - lazy exact") {
		names.push_back("Union of balls - double");
	}
	return names;
}

#else

#include <geometries/power_crust_2/Power_crust_2.h>
#include <geometries/power_crust_2/Power_crust_2_implement.h>
#include <geometries/union_of_balls_2/Union_of_balls_2.h>
#include <geometries/union_of_balls_2/Union_of_balls_2_implement.h>
#include <geometries/moebius_2/Moebius_2.h>
#include <geometries/moebius_2/Moebius_2_implement.h>
#include <geometries/second_voronoi_2/Second_voronoi_2.h>
#include <geometries/second_voronoi_2/Second_voronoi_2_implement.h>
#include <geometries/witness_2/Witness_2.h>
#include <geometries/witness_2/Witness_2_implement.h>
#include <geometries/scale_analysis_2/Scale_analysis_2.h>

extern template class __declspec(dllimport) Power_crust_2<Double_kernel>;
extern template class __declspec(dllimport) Power_crust_2<Exact_kernel>;

typedef Power_crust_2<Exact_kernel> Power_crust_exact_2;
typedef Power_crust_2<Double_kernel> Power_crust_double_2;

extern template class __declspec(dllimport) Union_of_balls_2<Double_kernel>;
extern template class __declspec(dllimport) Union_of_balls_2<Exact_kernel>;

extern template class __declspec(dllimport) Second_voronoi_2<Double_kernel, Power_crust_double_2::Triangulation_2>;
extern template class __declspec(dllimport) Second_voronoi_2<Exact_kernel, Power_crust_exact_2::Triangulation_2>;
extern template class __declspec(dllimport) Second_voronoi_2<Double_kernel, Witness_2<Double_kernel>::Witness_complex_triangulation >;
extern template class __declspec(dllimport) Second_voronoi_2<Exact_kernel, Witness_2<Exact_kernel>::Witness_complex_triangulation >;

extern template class __declspec(dllimport) Moebius_2<Double_kernel>;

extern template class __declspec(dllimport) Witness_2<Double_kernel>;
extern template class __declspec(dllimport) Witness_2<Exact_kernel>;

typedef Union_of_balls_2<Exact_kernel> Union_of_balls_exact_2;
typedef Union_of_balls_2<Double_kernel> Union_of_balls_double_2;
typedef Moebius_2<Double_kernel> Moebius_double_2;
typedef Second_voronoi_2<Exact_kernel, Power_crust_exact_2::Triangulation_2> Second_voronoi_classified_exact_2;
typedef Second_voronoi_2<Double_kernel, Power_crust_double_2::Triangulation_2> Second_voronoi_classified_double_2;
typedef Second_voronoi_2<Double_kernel, Witness_2<Double_kernel>::Witness_complex_triangulation > Second_voronoi_landmark_double_2;
typedef Second_voronoi_2<Exact_kernel, Witness_2<Exact_kernel>::Witness_complex_triangulation > Second_voronoi_landmark_exact_2;
typedef Witness_2<Double_kernel> Witness_double_2;
typedef Witness_2<Exact_kernel> Witness_exact_2;

void Geometry_factory::fill_available_geometries() {
	available_geometries.push_back("Power crust - double");
	available_geometries.push_back("Power crust - lazy exact");
	available_geometries.push_back("Union of balls all Voronoi - double");
	available_geometries.push_back("Union of balls all Voronoi - lazy exact");
	available_geometries.push_back("Moebius of balls - double");
	available_geometries.push_back("Medial axis of scaled moebius - lazy exact");
	available_geometries.push_back("Second order Voronoi - double");
	available_geometries.push_back("Second order Voronoi - lazy exact");
	available_geometries.push_back("Witness complex - double");
	available_geometries.push_back("Witness complex - lazy exact");	
	available_geometries.push_back("Landmark 2nd Voronoi - double");
	available_geometries.push_back("Landmark 2nd Voronoi - lazy exact");	
	available_geometries.push_back("Scale analysis - double");	
}

Geometry* Geometry_factory::create_new_geometry(const std::string& name) {
	std::cout << "creating geometry " << name << std::endl;

	if (name == "Scale analysis - double") {
		return new Scale_analysis_2();
	}
	if (name == "Power crust - double") {
		return new Power_crust_double_2();
	}
	if (name == "Power crust - lazy exact") {
		return new Power_crust_exact_2();
	}
	if (name == "Witness complex - double") {
		return new Witness_double_2();
	}
	if (name == "Witness complex - lazy exact") {
		return new Witness_exact_2();
	}
	if (name == "Union of balls all Voronoi - double") {
		return new Union_of_balls_double_2(SHARED_INNER_VORONOI_BALLS, "A");
	}
	if (name == "Union of balls all Voronoi - lazy exact") {
		return new Union_of_balls_exact_2(SHARED_INNER_VORONOI_BALLS, "A");
	}
	if (name == "Moebius of balls - double") {
		return new Moebius_double_2();
	}
	if (name == "Medial axis of scaled moebius - lazy exact") {
		return new Union_of_balls_exact_2(SHARED_GROWN_BALLS_FROM_MOEBIUS, "M");
	}
	if (name == "Second order Voronoi - double") {
		return new Second_voronoi_classified_double_2(SHARED_INPUT_TRIANGULATION, "2nd order Voronoi");
	}
	if (name == "Second order Voronoi - lazy exact") {
		return new Second_voronoi_classified_exact_2(SHARED_INPUT_TRIANGULATION, "2nd order Voronoi");
	}
	if (name == "Landmark 2nd Voronoi - double") {
		return new Second_voronoi_landmark_double_2(SHARED_LANDMARK_TRIANGULATION, "L 2nd order Voronoi");
	}
	if (name == "Landmark 2nd Voronoi - lazy exact") {
		return new Second_voronoi_landmark_exact_2(SHARED_LANDMARK_TRIANGULATION, "L 2nd order Voronoi");
	}


	return 0;
}

std::list<std::string> Geometry_factory::depends_on(const std::string& name) {
	std::list<std::string> names;
	if (name == "Landmark 2nd Voronoi - lazy exact") {
		names.push_back("Witness complex - lazy exact");
	}
	if (name == "Landmark 2nd Voronoi - double") {
		names.push_back("Witness complex - double");
	}
	if (name == "Witness complex - lazy exact") {
		names.push_back("Power crust - lazy exact");
	}
	if (name == "Witness complex - double") {
		names.push_back("Power crust - double");
	}
	if (name == "Union of balls all Voronoi - lazy exact") {
		names.push_back("Power crust - lazy exact");
	}
	if (name == "Union of balls all Voronoi - double") {
		names.push_back("Power crust - double");
	}
	if (name == "Moebius of balls - double") {
		names.push_back("Power crust - lazy exact");
	}
	if (name == "Medial axis of scaled moebius - lazy exact") {
		names.push_back("Moebius of balls - double");
	}
	if (name == "Second order Voronoi - lazy exact") {
		names.push_back("Power crust - lazy exact");
	}
	if (name == "Second order Voronoi - double") {
		names.push_back("Power crust - double");
	}
	if (name == "Scale analysis - double") {
		names.push_back("Power crust - double");
	}
	return names;
}

std::list<std::string> Geometry_factory::conflicts_with(const std::string& name) {
	std::list<std::string> names;
	if (name == "Landmark 2nd Voronoi - double") {
		names.push_back("Landmark 2nd Voronoi - lazy exact");
	}
	if (name == "Landmark 2nd Voronoi - lazy exact") {
		names.push_back("Landmark 2nd Voronoi - double");
	}
	if (name == "Witness complex - double") {
		names.push_back("Witness complex - lazy exact");
	}
	if (name == "Witness complex - lazy exact") {
		names.push_back("Witness complex - double");
	}
	if (name == "Power crust - double") {
		names.push_back("Power crust - lazy exact");
	}
	if (name == "Power crust - lazy exact") {
		names.push_back("Power crust - double");
	}
	if (name == "Second order Voronoi - double") {
		names.push_back("Second order Voronoi - lazy exact");
	}
	if (name == "Second order Voronoi - lazy exact") {
		names.push_back("Second order Voronoi - double");
	}
	if (name == "Union of balls all Voronoi - double") {
		names.push_back("Power crust - lazy exact");
		names.push_back("Union of balls all Voronoi - lazy exact");
	}
	if (name == "Union of balls all Voronoi - lazy exact") {
		names.push_back("Power crust - double");
		names.push_back("Union of balls all Voronoi - double");
	}
	return names;
}

#endif

std::list<std::string> Geometry_factory::available_geometries;
