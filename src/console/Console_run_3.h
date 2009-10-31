#ifndef MESECINA_CONSOLE_RUN_3
#define MESECINA_CONSOLE_RUN_3

#include <QtCore/QString>
#include "Console_run.h"

class Asia_structure_3;

class Console_run_3 : public Console_run {

public:
	Console_run_3(int argc, char* argv[]);

	virtual void do_processing();
	void print_statistics(Asia_structure_3* s);
};

#endif //MESECINA_CONSOLE_RUN_3