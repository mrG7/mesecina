/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Logger.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_LOGGER_H
#define MESECINA_LOGGER_H

#include <QtGui/QTextEdit>
#include <QtCore/QTime>
#include <gui/app/Logger_buffer.h>
#include <gui/app/Application_variable.h>
#include <QtCore/QMutex>

class Logger : public QTextEdit {
	
	Q_OBJECT

public:
	Logger(QWidget * parent = 0);
	~Logger();

	std::ostream& get_output_stream();

	void grab_cout();
	void release_cout();

	void print_to_log(char* s);
	void set_next_line();

public slots:
	void error(QString s);
	void msg(QString s);
	void msg(QString s, int level);

signals:
	void setProgressMaximum( int maximum);
	void setProgressMinimum( int minimum);
	void setProgressValue( int value);
	void resetProgress();
	void setProgressStatus(const QString& n, int timeout);
	void set_variable(Application_variable v);

private:
	std::ostream lout;
	std::streambuf* strm_buffer,*err_buffer;
	bool virgin_line;
	QMutex mutex;

	QTime line_time, process_start;
	QString line_message;
	QString last_progress_status;
	bool progress_on;
};

#endif //MESECINA_LOGGER_H