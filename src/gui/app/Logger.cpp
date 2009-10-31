/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Logger.cpp 737 2009-05-16 15:40:46Z miklosb $
 */

#include <gui/app/Logger.h>
#include <QtGui/QScrollBar>
#include <QtGui/QLabel>
#include <QtCore/QEventLoop>
#include <QtCore/QMutexLocker>

#include <constants.h>

Logger::Logger(QWidget * parent) : QTextEdit(parent), lout(new Logger_buffer(this, 1)), virgin_line(true), strm_buffer(0), err_buffer(0) {
	setReadOnly(true); line_message = ""; progress_on = true;
}

Logger::~Logger() {
}

void Logger::error(QString s) {
	msg(s,99);
}

void Logger::msg(QString s) {
	msg(s,0);
}

void Logger::msg(QString s, int level) {
	setTextColor(Qt::lightGray);
	append(QTime::currentTime().toString("hh:mm:ss")+": ");
	moveCursor(QTextCursor::End);
	switch (level) {
		case 0: setTextColor(Qt::black); break;
		case 1: setTextColor(Qt::gray); break;
		case 99: setTextColor(Qt::red); break;
	}
	insertPlainText(s);
	repaint();
	QEventLoop el(this);
	el.processEvents(QEventLoop::ExcludeUserInputEvents);
}

void Logger::print_to_log(char* s) {
	if (s=="") return;
	if (virgin_line) {
		line_time = QTime::currentTime();
		virgin_line = false;
	}
	line_message.append(s);
}

void Logger::set_next_line() {
	QMutexLocker locker(&mutex);
	virgin_line = true;
	if (line_message.startsWith(PROGRESS_ON)) progress_on = true;
	if (line_message.startsWith(PROGRESS_OFF)) progress_on = false;
	if (line_message.startsWith(PROGRESS_HEADER)) {
		if (progress_on) {
			if (line_message.startsWith(PROGRESS_MINIMUM)) {
				int v = line_message.right(line_message.length() - line_message.lastIndexOf(":") - 2).toInt();
				process_start.start();
				emit setProgressMinimum(v);
			}
			if (line_message.startsWith(PROGRESS_MAXIMUM)) {
				int v = line_message.right(line_message.length() - line_message.lastIndexOf(":") - 2).toInt();
				emit setProgressMaximum(v);
			}
			if (line_message.startsWith(PROGRESS_VALUE)) {
				int v = line_message.right(line_message.length() - line_message.lastIndexOf(":") - 2).toInt();
				emit setProgressValue(v);
			}
			if (line_message == PROGRESS_DONE) {
				emit setProgressMinimum(0);
				emit setProgressMaximum(1);
				emit resetProgress();
				emit setProgressStatus("Done.",1000);
				setTextColor(Qt::lightGray);
				append(line_time.toString("hh:mm:ss")+": ");
				moveCursor(QTextCursor::End);
				setTextColor(Qt::black);
				float sec = (float)process_start.elapsed()/1000;
				int min = sec / 60;
				QString start_string = last_progress_status;
				if (start_string == "") start_string = "Processing time";
				if (min>0) {
					sec = sec - min*60;
					insertPlainText(tr("%1: %2 min %3 s").arg(start_string).arg(min).arg(sec));
				} else insertPlainText(tr("%1: %2 s").arg(start_string).arg(sec));
				emit set_variable(Application_variable(tr("%1").arg(last_progress_status), QString::number((float)process_start.elapsed()/1000)));
				repaint();

			}
			if (line_message.startsWith(PROGRESS_STATUS)) {
				process_start.start();
				last_progress_status = line_message.right(line_message.length() - line_message.lastIndexOf(":") - 2);
				emit setProgressStatus(last_progress_status,0);
			}
		}
	} else {
		setTextColor(Qt::lightGray);
		append(line_time.toString("hh:mm:ss")+": ");
		moveCursor(QTextCursor::End);
		if (line_message.startsWith(LOG_ERROR)) {
			setTextColor(QColor(168,0,0));
			line_message = line_message.right((int)(line_message.size() - std::string(LOG_ERROR).size()));
			line_message = tr("%1 %2").arg("ERROR: ").arg(line_message);
		} else if (line_message.startsWith(LOG_WARNING)) {
			setTextColor(Qt::blue);
			line_message = line_message.right((int)(line_message.size() - std::string(LOG_WARNING).size()));
			line_message = tr("%1 %2").arg("WARNING: ").arg(line_message);
		} else if (line_message.startsWith(LOG_GREEN)) {
			setTextColor(QColor(0,168,0));
			line_message = line_message.right((int)(line_message.size() - std::string(LOG_GREEN).size()));
		} else if (line_message.startsWith(LOG_BLUE)) {
			setTextColor(Qt::blue);
			line_message = line_message.right((int)(line_message.size() - std::string(LOG_BLUE).size()));
		} else if (line_message.startsWith(LOG_RED)) {
			setTextColor(Qt::red);
			line_message = line_message.right((int)(line_message.size() - std::string(LOG_RED).size()));
		} else	setTextColor(Qt::black);
		insertPlainText(line_message);
//		repaint();
	}
	//#ifdef Q_WS_WIN
	//QEventLoop el(this);
	//el.processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers );	
	//#endif //Q_WS_WIN
	line_message.clear();
}

std::ostream& Logger::get_output_stream() {
	return lout;
}

void Logger::grab_cout() {
	std::cout.flush();
	strm_buffer = std::cout.rdbuf(); // save output buffer
	std::cout.rdbuf(lout.rdbuf());

	//std::cerr.flush();
	//err_buffer = std::cerr.rdbuf();
	//std::cerr.rdbuf(lout.rdbuf());

}

void Logger::release_cout() {
	if (strm_buffer)
		std::cout.rdbuf(strm_buffer);
	//if (err_buffer)
	//	std::cerr.rdbuf(strm_buffer);

}
