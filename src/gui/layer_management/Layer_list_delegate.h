/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Layer_list_delegate.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef LAYER_LIST_DELEGATE_H
#define LAYER_LIST_DELEGATE_H

#include <QtGui/QItemDelegate>

class Layer_list_delegate : public QItemDelegate {

public:
	Layer_list_delegate(QObject *parent = 0);

	virtual void drawCheck(QPainter *painter, const QStyleOptionViewItem &option,
		const QRect &rect, Qt::CheckState state) const;

	void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

private:

};

#endif //LAYER_LIST_DELEGATE_H