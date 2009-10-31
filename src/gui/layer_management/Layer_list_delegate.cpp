/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Layer_list_delegate.cpp 105 2007-05-22 14:11:04Z miklosb $
 */

#include <gui/layer_management/Layer_list_delegate.h>

#include <QtCore/QModelIndex>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QWindowsStyle>


Layer_list_delegate::Layer_list_delegate(QObject *parent) : QItemDelegate(parent) {
}

void Layer_list_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QStyleOptionViewItem new_option(option);
	if (index.isValid()) {
		QVariant value = index.data(Qt::UserRole);
		QVariant checked = index.data(33);
		if (checked.toInt() == 0) new_option.palette.setColor(QPalette::Text, Qt::darkGray);
		if (value.isValid()) {
			QBrush brush(qvariant_cast<QBrush>(value));
			if (brush!=QBrush()) new_option.palette.setBrush(QPalette::LinkVisited, brush); //misused LinkVisited brush for checkbox background
		}
	}
	QItemDelegate::paint(painter, new_option, index);
}


void Layer_list_delegate::drawCheck(QPainter *painter,
                              const QStyleOptionViewItem &option,
							  const QRect &rect, Qt::CheckState state) const {
    if (!rect.isValid())
        return;

    QStyleOptionViewItem opt(option);
    opt.rect = rect;
    opt.state = opt.state & ~QStyle::State_HasFocus;

    switch (state) {
    case Qt::Unchecked:
        opt.state |= QStyle::State_Off;
        break;
    case Qt::PartiallyChecked:
        opt.state |= QStyle::State_NoChange;
        break;
    case Qt::Checked:
        opt.state |= QStyle::State_On;
        break;
    }

	// paint background
	if (state == Qt::Checked) {
		// create the rectangle we want to paint
		QRect dec;
		#ifdef Q_WS_WIN
		dec = rect.translated(-2,-2);
		dec.setWidth(dec.width()+3);
		dec.setHeight(dec.height()+3);
		#else
		dec = rect.translated(-2,-2);
		//dec.setWidth(dec.width()-1);
		//dec.setHeight(dec.height()-1);
		#endif //Q_WS_WIN


		// paint the background rectangle
		QPointF oldBO = painter->brushOrigin();
		painter->setBrushOrigin(dec.topLeft());
		QColor background_color = opt.palette.brush(QPalette::LinkVisited).color();
		painter->fillRect(dec, opt.palette.brush(QPalette::LinkVisited));

		//set the font color so that one can see it on the background
		if (background_color.red() > 128 || background_color.green() > 128 || background_color.alpha() < 128)
			opt.palette.setColor(QPalette::Text, Qt::black);
		else opt.palette.setColor(QPalette::Text, Qt::white);
		// set back the brush origin to the original place
		painter->setBrushOrigin(oldBO);
	}
	
//	#ifdef Q_WS_WIN
	QWindowsStyle windows_style;
	windows_style.drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &opt, painter);
//	#else
//	QApplication::style()->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &opt, painter);
//	#endif //Q_WS_WIN


}