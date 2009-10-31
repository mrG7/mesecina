/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Help_browser_dialog.h 103 2007-05-19 03:16:07Z miklosb $
 */

#ifndef MESECINA_HELP_BROWSER_DIALOG_H
#define MESECINA_HELP_BROWSER_DIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QTextBrowser>

class Help_browser_dialog : public QDialog {
	Q_OBJECT

public:
	Help_browser_dialog(QWidget * parent = 0);

signals:
	void closing(bool);

private slots:
	void closeEvent( QCloseEvent* );

public:
	virtual void keyPressEvent(QKeyEvent *e);


	QTextBrowser* text_browser;
};


#endif //MESECINA_HELP_BROWSER_DIALOG_H