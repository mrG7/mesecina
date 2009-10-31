#include <iostream>
#include <fstream>
#include <console/Console_logger.h>
#include <console/Console_run_2.h>
//#include "Structure_2.h"
#include <gui/app/static/Application_settings.h>
#include <gui/app/Settings_table_model.h>
#include <constants.h>

Console_run_2::Console_run_2(int argc, char* argv[]) : Console_run(argc, argv) { }

void Console_run_2::do_processing() {
	// create empty Structure_2 object

//	Structure_2 s(0);

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

	char* outfilename = "out.pstg";
	assign_string_parameter_if_exists(argc, argv, "-output", outfilename);	
	std::ofstream outfile;
	outfile.open(outfilename);

	// compute set cover
	std::cout << std::endl << PROGRESS_STATUS << "Stage computation" << std::endl;
//	s.get_output_aggregate();
	std::cout << PROGRESS_DONE << std::endl;

//	s.print_all_data(outfile);
	std::cout << "Result written to " << outfilename << std::endl;
	outfile.close();
}
