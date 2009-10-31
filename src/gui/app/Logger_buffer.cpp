/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 * 
 * Portions of this source file are based on the code of Dietmar Kuehl from
 * http://www.inf.uni-konstanz.de/~kuehl/iostream/
 *
 * $Id: Logger_buffer.cpp 737 2009-05-16 15:40:46Z miklosb $
 */

#include <gui/app/Logger_buffer.h>
#include <gui/app/Logger.h>

Logger_buffer::Logger_buffer(Logger* l, int bsize) : std::streambuf(), logger(l) {
	if (bsize)	{
		char *ptr = new char[bsize];
		setp(ptr, ptr + bsize);
	}
	else
		setp(0, 0);

	setg(0, 0, 0);

}

Logger_buffer::~Logger_buffer() {
	sync();
	delete[] pbase();
}


int	Logger_buffer::overflow(int c) {
//	mutex.lock();
	put_buffer();

	if (c != EOF)
		if (pbase() == epptr())
			put_char(c);
		else
			sputc(c);

	return 0;
}

int	Logger_buffer::sync() {
	put_buffer();
//	mutex.unlock();
	return 0;
}

void Logger_buffer::put_char(int chr) {
	char app[2];

	app[0] = chr;
	app[1] = 0;

	if (app[0]==10) {
		logger->set_next_line();
	} else logger->print_to_log(app); 
	// originally 
	// XmTextInsert(text, XmTextGetLastPosition(text), app);
}

void	Logger_buffer::put_buffer() {
	if (pbase() != pptr() && logger != 0) {
		int len = (pptr() - pbase());
		char *buffer = new char[len + 1];

		strncpy(buffer, pbase(), len);
		buffer[len] = 0;
		if (buffer[len-1]==10) {
			logger->set_next_line();
		} else logger->print_to_log(buffer); 
		// originally 
		// XmTextInsert(text, XmTextGetLastPosition(text), buffer);

		setp(pbase(), epptr());
		delete [] buffer;
	}
}