/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Logger.h 536 2009-02-28 20:21:22Z cwormser $
 */

#ifndef MESECINA_CONSOLE_LOGGER_H
#define MESECINA_CONSOLE_LOGGER_H

#include <QtGui/QTextEdit>
#include <QtCore/QTime>
#include <console/Console_logger_buffer.h>
#include <gui/app/Application_variable.h>
#include <QtCore/QMutex>

class Console_logger {

public:
	Console_logger();
	~Console_logger();

	std::ostream& get_output_stream();

	void grab_cout();
	void release_cout();

	void print_to_log(QString s);
	void set_next_line();

private:
	std::ostream lout;
	std::ostream orig_cout;
	std::streambuf* strm_buffer,*err_buffer;
	bool virgin_line;

	QTime line_time, process_start;
	QString line_message;
	QString last_progress_status;

	bool progress_on;
};

#endif //MESECINA_CONSOLE_LOGGER_H