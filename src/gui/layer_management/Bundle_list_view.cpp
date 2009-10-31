/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Bundle_list_view.cpp 120 2007-06-22 01:50:19Z miklosb $
 */

#include <gui/layer_management/Bundle_list_view.h>

#include <QtGui/QMouseEvent>
#include <QtGui/QAction>
#include <QtGui/QMenu>


Bundle_list_view::Bundle_list_view(QWidget *parent) : QListView(parent) {
	#ifdef Q_WS_WIN
		QFont bigger_font = font();
		bigger_font.setPointSize(font().pointSize()+2);
		setFont(bigger_font);
	#endif //Q_WS_WIN
}

Bundle_list_view::~Bundle_list_view() {
}

void Bundle_list_view::context_menu(QMouseEvent * event, const QModelIndex &index) {
	if (!index.isValid()) return;
    QMenu menu(this);
    QAction del(tr("&Delete"), this);
    menu.addAction(&del);
    QAction exp(tr("&Export to file..."), this);
	menu.addAction(&exp);
	QAction* result = menu.exec(event->globalPos());
	if (result == &del) {
		emit remove_row(index.row());
	}
	else if (result == &exp) {
		emit export_bundle(index.row());
	}
}

void Bundle_list_view::mouseReleaseEvent(QMouseEvent * event) {
	QListView::mouseReleaseEvent(event);

	QPoint pos = event->pos();
	QPersistentModelIndex index = indexAt(pos);
	if (event->button() & Qt::RightButton)
		context_menu(event, index);
}