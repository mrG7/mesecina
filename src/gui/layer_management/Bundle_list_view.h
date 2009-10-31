/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Bundle_list_view.h 102 2007-05-18 02:29:07Z miklosb $
 */

#ifndef BUNDLE_LIST_VIEW_H
#define BUNDLE_LIST_VIEW_H

#include <QtGui/QListView>
#include <vector>


class Bundle_list_view : public QListView {
	Q_OBJECT

public:
	Bundle_list_view(QWidget *parent = 0);
	~Bundle_list_view();

signals:
	void remove_row(int row);
	void export_bundle(int row);

protected:
	virtual void mouseReleaseEvent(QMouseEvent * event);

private:
	void context_menu(QMouseEvent * event, const QModelIndex &index);


};

#endif //BUNDLE_LIST_VIEW_H