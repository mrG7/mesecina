/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 * 
 * Portions of this source file are based on the code of Dietmar Kuehl from
 * http://www.inf.uni-konstanz.de/~kuehl/iostream/
 *
 * $Id: Logger_buffer.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_LOGGER_BUFFER_H
#define MESECINA_LOGGER_BUFFER_H

#include <iostream>
#include <sstream>
#include <QtCore/QMutex>

class Logger;

class Logger_buffer: public std::streambuf {
private:
	Logger* logger;
	QMutex mutex;

	void put_buffer(void);
	void put_char(int);


protected:
	int	overflow(int);
	int	sync();

public:
	Logger_buffer(Logger* l, int bsize = 0);
	~Logger_buffer();
};

#endif //MESECINA_LOGGER_BUFFER_H