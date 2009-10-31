#ifndef MESECINA_CONSOLE_RUN
#define MESECINA_CONSOLE_RUN

#include <console/Console_logger.h>
#include <QtCore/QString>

class Console_run {

public:
	Console_run(int argc, char* argv[]);
	~Console_run();

	virtual void do_processing() {}

protected:
	void print_settings(QString start);
	Console_logger logger;
	int argc;
	char** argv;

	static char* get_param_variable(int argc, char *argv[], char* filter);
	static bool assign_int_parameter_if_exists(int argc, char *argv[], char* filter, int &variable);
	static bool assign_string_parameter_if_exists(int argc, char *argv[], char* filter, char* &variable);
};

#endif //MESECINA_CONSOLE_RUN