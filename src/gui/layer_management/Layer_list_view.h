/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Layer_list_view.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef LAYER_LIST_VIEW_H
#define LAYER_LIST_VIEW_H

#include <QtGui/QListView>
#include <vector>


class Layer_list_view : public QListView {
	Q_OBJECT

public:
	Layer_list_view(QWidget *parent = 0);
	~Layer_list_view();

signals:
	void context_menu(QMouseEvent * event, const QModelIndex &index, QWidget *parent);

protected:
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);


private:
    void change_check(const QModelIndex &index);
	QPoint pos;


};

#endif //LAYER_LIST_VIEW_H