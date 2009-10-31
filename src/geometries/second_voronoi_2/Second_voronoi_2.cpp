#include "Second_voronoi_2.h"
#include "Second_voronoi_2_implement.h"

#include <geometries/power_crust_2/Power_crust_2.h>
#include <geometries/power_crust_2/Power_crust_2_implement.h>

#include <geometries/witness_2/Witness_2.h>
#include <geometries/witness_2/Witness_2_implement.h>


extern template class __declspec(dllimport) Power_crust_2<Double_kernel>;
extern template class __declspec(dllimport) Power_crust_2<Exact_kernel>;

extern template class __declspec(dllimport) Witness_2<Double_kernel>;
extern template class __declspec(dllimport) Witness_2<Exact_kernel>;

/*extern*/ template class __declspec(dllexport) Second_voronoi_2<Double_kernel, Power_crust_2<Double_kernel>::Triangulation_2 >;
/*extern*/ template class __declspec(dllexport) Second_voronoi_2<Exact_kernel, Power_crust_2<Exact_kernel>::Triangulation_2 >;
/*extern*/ template class __declspec(dllexport) Second_voronoi_2<Double_kernel, Witness_2<Double_kernel>::Witness_complex_triangulation >;
/*extern*/ template class __declspec(dllexport) Second_voronoi_2<Exact_kernel, Witness_2<Exact_kernel>::Witness_complex_triangulation >;