#include <console/Console_logger.h>
#include <iostream>
#include <fstream>
#include <console/Console_run_3.h>
#include <gui/app/static/Application_settings.h>
#include <gui/app/Application_variable.h>
#include <gui/app/Settings_table_model.h>
#include <windows.h>

//#include "Asia_structure_3.h"


Console_run_3::Console_run_3(int argc, char* argv[]) : Console_run(argc, argv) { }

void Console_run_3::do_processing() {

//	Asia_structure_3 s(0);

	// load settings
	char* settingspath = "";
	assign_string_parameter_if_exists(argc, argv, "-settings", settingspath);
	if (QString(settingspath)!="") {
//		std::cout << "Load settings from: " << settingspath << std::endl;
		Settings_table_model::load_current_settings(settingspath);
	}
	print_settings("p-");
	std::cout << std::endl;

	// open file
	char* filename = "";
	assign_string_parameter_if_exists(argc, argv, "-input", filename);	
	std::fstream infile;
	infile.open (filename);
	if (infile.is_open()) {
		infile.close();
		std::cout << "Input file: " << filename << std::endl;
//		s.load_generic_file(std::string(filename));	
	} else {
		std::cout << "Input file could not be opened: " << filename << std::endl;
		return;
	}

	char* outfilename = "out.stg";
	assign_string_parameter_if_exists(argc, argv, "-output", outfilename);	
	std::string ofname(outfilename);
	

	// compute set cover
	std::cout << std::endl << "COMPUTE BEGIN" << std::endl<< std::endl;
//	s.get_output_aggregate();
	std::cout << std::endl << "COMPUTE END" << std::endl<< std::endl;

	std::cout << std::endl << "STATISTICS BEGIN" << std::endl<< std::endl;
//	print_statistics(&s);
	std::cout << std::endl << "STATISTICS END" << std::endl<< std::endl;

//	s.save_generic_file(ofname);



}

void Console_run_3::print_statistics(Asia_structure_3* /*s*/) {

	//int n = s->get_variable_count();
	//for (int i=0; i<n; i++) {
	//	Application_variable av = s->get_variable(i);
	//	std::cout << av.first.toStdString() << ": " << av.second.toStdString() << std::endl;

	//}
}
