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

QString Console_run::path_to_filename(const QString& path) {
	return path.right(path.length() - path.lastIndexOf("/") - 1);
}


void Console_run::print_settings(QString start) {
	std::cout << "Settings: " << std::endl;
	std::vector<Application_setting>::iterator n_it, n_end =Application_settings::settings.end();
	for (n_it = Application_settings::settings.begin(); n_it!=n_end; n_it++) {
		if (n_it->name.startsWith(start)) {
			std::cout << n_it->name.toStdString() << ": " << Application_settings::get_string_setting(n_it->name) << std::endl;
		}
	}
}

// command line arguments parsing functions
char* Console_run::get_param_variable(int argc, char *argv[], char* filter) {
	for (int i=0; i<argc; i++) {
		if (strcmp(argv[i], filter)==0 && argc > i+1) 
			if (i+1 < argc) return argv[i+1]; else return "";
	}
	return NULL;
}
bool Console_run::assign_int_parameter_if_exists(int argc, char *argv[], char* filter, int &variable) {
	char* val = get_param_variable(argc, argv, filter);
	if (val) { variable = atoi(val); return true; }
	else return false;
}
bool Console_run::assign_float_parameter_if_exists(int argc, char *argv[], char* filter, double &variable) {
	char* val = get_param_variable(argc, argv, filter);
	if (val) { variable = atof(val); return true; }
	else return false;
}
bool Console_run::assign_string_parameter_if_exists(int argc, char *argv[], char* filter, char* &variable) {
	char* val = get_param_variable(argc, argv, filter);
	if (val) { variable = val; return true; }
	else return false;
}