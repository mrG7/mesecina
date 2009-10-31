/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 * 
 * Portions of this source file are based on the code of Dietmar Kuehl from
 * http://www.inf.uni-konstanz.de/~kuehl/iostream/
 *
 * $Id: Logger_buffer.h 383 2008-11-03 20:12:36Z miklosb $
 */

#ifndef MESECINA_CONSOLE_LOGGER_BUFFER_H
#define MESECINA_CONSOLE_LOGGER_BUFFER_H

#include <iostream>
#include <QtCore/QMutex>

class Console_logger;

class Console_logger_buffer: public std::streambuf {
private:
	Console_logger* logger;
	QMutex mutex;

	void put_buffer(void);
	void put_char(int);

protected:
	int	overflow(int);
	int	sync();

public:
	Console_logger_buffer(Console_logger* l, int bsize = 0);
	~Console_logger_buffer();
};

#endif //MESECINA_CONSOLE_LOGGER_BUFFER_H