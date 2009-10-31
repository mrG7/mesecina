/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Popup_color_map_frame.h 258 2007-12-05 14:40:50Z miklosb $
 */

#ifndef POPUP_COLOR_MAP_FRAME_H
#define POPUP_COLOR_MAP_FRAME_H

#include <QtGui/QWidgetAction>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QComboBox>
#include <QtGui/QVBoxLayout>

#include <iostream>

class Clickable_label : public QLabel {
	Q_OBJECT
public:
	Clickable_label() : QLabel() {}
	virtual void mousePressEvent(QMouseEvent*) { 
		emit label_clicked(); 
	}
signals:
	void label_clicked();
};

class Popup_color_map_frame : public QWidgetAction {
	Q_OBJECT

public:
	Popup_color_map_frame(QWidget* parent);
	// destructor might be needed because we are are allocating memory in constructor
	
	void addItem ( const QString & text);
	void set_color_map(const QString & text);
	QComboBox *combobox; 
	Clickable_label* image_place;

	void set_pixmap(QPixmap *pm);
	void set_pixmap(const QColor& col);

private:

	QVBoxLayout *layout;
	
};

#endif //POPUP_COLOR_MAP_FRAME_H