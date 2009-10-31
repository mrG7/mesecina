/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Layer_list_view.cpp 92 2007-04-06 18:18:52Z miklosb $
 */

#include <gui/layer_management/Layer_list_view.h>

#include <QtGui/QMouseEvent>

Layer_list_view::Layer_list_view(QWidget *parent) : QListView(parent) {
	#ifdef Q_WS_WIN
		QFont bigger_font = font();
		bigger_font.setPointSize(font().pointSize()+2);
		setFont(bigger_font);
	#endif //Q_WS_WIN
}

Layer_list_view::~Layer_list_view() {
}

void Layer_list_view::change_check(const QModelIndex &index) {
	if (index.isValid())
		if (model()->data(index, Qt::CheckStateRole) == Qt::Checked)
			model()->setData(index, Qt::Unchecked, Qt::CheckStateRole);
		else 
			model()->setData(index, Qt::Checked, Qt::CheckStateRole);
}

void Layer_list_view::mousePressEvent(QMouseEvent * event) {
	QListView::mousePressEvent(event);
	pos = event->pos();
	event->accept();

}

void Layer_list_view::mouseReleaseEvent(QMouseEvent * event) {
	QListView::mouseReleaseEvent(event);
	event->accept();

	if (pos != event->pos()) return;
	QPersistentModelIndex index = indexAt(pos);
	if (event->button() & Qt::LeftButton)
		change_check(index);
	if (event->button() & Qt::RightButton &&
		index.isValid() && model()->data(index, Qt::CheckStateRole) == Qt::Checked)
		emit context_menu(event, index, this);
		
}