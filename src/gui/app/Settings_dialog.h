/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Settings_dialog.h 123 2007-07-01 21:43:01Z miklosb $
 */

#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QSplitter>

#include <gui/app/Settings_list_widget.h>
#include <gui/app/Application_variable.h>


class Settings_dialog : public QDialog {
	Q_OBJECT

public:
	Settings_dialog(QWidget * parent = 0, QString name = "Settings");

signals:
	void closing(bool);

private slots:
	void closeEvent( QCloseEvent* );


public:
	virtual void keyPressEvent(QKeyEvent *e);

	Settings_list_widget* settings_list_widget;


};

#endif //SETTINGS_DIALOG_H