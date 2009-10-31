#ifndef MESECINA_CONSOLE_RUN_2
#define MESECINA_CONSOLE_RUN_2

#include <QtCore/QString>
#include "Console_run.h"

class Console_run_2 : public Console_run {
#include "Console_run.h"

public:
	Console_run_2(int argc, char* argv[]);

	virtual void do_processing();
};

#endif //MESECINA_CONSOLE_RUN_2