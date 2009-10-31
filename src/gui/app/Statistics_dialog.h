/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Statistics_dialog.h 98 2007-05-01 23:11:20Z miklosb $
 */

#ifndef STATISTICS_DIALOG_H
#define STATISTICS_DIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QSplitter>

#include <gui/app/Statistics_widget.h>
#include <gui/app/Application_variable.h>


class Statistics_dialog : public QDialog {
	Q_OBJECT

public:
	Statistics_dialog(QWidget * parent = 0, QString name = "Statistics");

signals:
	void closing(bool);

private slots:
	void closeEvent( QCloseEvent* );


public:
	virtual void keyPressEvent(QKeyEvent *e);

	Statistics_widget* statistics_widget;
	Statistics_widget* time_widget;

	QSplitter* splitter;


};

#endif //STATISTICS_DIALOG_H