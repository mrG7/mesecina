/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Statistics_widget.h 98 2007-05-01 23:11:20Z miklosb $
 */

#ifndef STATISTICS_WIDGET_H
#define STATISTICS_WIDGET_H

#include <QtGui/QFrame>
#include <QtGui/QCloseEvent>
#include <QtGui/QLineEdit>
#include <QtGui/QTableView>
#include <QtGui/QSortFilterProxyModel>

#include <gui/app/Statistics_table_model.h>
#include <gui/app/Application_variable.h>


class Statistics_widget : public QFrame {
	Q_OBJECT

public:
	Statistics_widget(QWidget * parent = 0, QString name = "Statistics");
	void process_filter();

signals:
	void closing(bool);

public slots:
	void set_variable(Application_variable v, bool apply_filter=true);


private slots:
	void delete_filter_text(bool);
	void filter_changed(const QString&);


public:
	virtual void resizeEvent ( QResizeEvent * e);
	void clear();

private:
	QLineEdit* filter_box;
	Statistics_table_model* table_model;
	QTableView *table_view;
	QSortFilterProxyModel *filterproxy;

};

#endif //STATISTICS_WIDGET_H