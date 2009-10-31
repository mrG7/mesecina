#include <iostream>
#include <fstream>
#include <console/Console_run.h>
#include <gui/app/static/Application_settings.h>
#include <gui/app/Settings_table_model.h>
#include <windows.h>


Console_run::Console_run(int argc, char* argv[]) {
	std::cout << std::endl << "Mesecina running in console mode..." << std::endl << std::endl;
	logger.grab_cout();
	this->argc = argc;
	this->argv = argv;
}

Console_run::~Console_run() {
	logger.release_cout();
}

void Console_run::print_settings(QString start) {
	std::cout << "Settings: " << std::endl;
	std::vector<QString>::iterator n_it, n_end =Application_settings::setting_names.end();
	for (n_it = Application_settings::setting_names.begin(); n_it!=n_end; n_it++) {
		if (n_it->startsWith(start)) {
			std::cout << n_it->toStdString() << ": " << Application_settings::get_string_setting(*n_it) << std::endl;
		}
	}
}

// command line arguments parsing functions
char* Console_run::get_param_variable(int argc, char *argv[], char* filter) {
	for (int i=0; i<argc; i++) {
		if (strcmp(argv[i], filter)==0 && argc > i+1) 
			return argv[i+1];
	}
	return NULL;
}
bool Console_run::assign_int_parameter_if_exists(int argc, char *argv[], char* filter, int &variable) {
	char* val = get_param_variable(argc, argv, filter);
	if (val) { variable = atoi(val); return true; }
	else return false;
}
bool Console_run::assign_string_parameter_if_exists(int argc, char *argv[], char* filter, char* &variable) {
	char* val = get_param_variable(argc, argv, filter);
	if (val) { variable = val; return true; }
	else return false;
}