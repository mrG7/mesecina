/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Popup_color_map_frame.cpp 259 2007-12-05 16:19:23Z miklosb $
 */

#include <gui/layer_management/Popup_color_map_frame.h>
#include <gui/app/static/Color_map_factory.h>
#include <iostream>

#define IMAGE_WIDTH 125

Popup_color_map_frame::Popup_color_map_frame(QWidget* parent) : QWidgetAction(parent) {
//	setFrameShape(QFrame::StyledPanel);

	image_place = new Clickable_label();
	QSize image_place_size = image_place->minimumSize();
	image_place_size.setWidth(IMAGE_WIDTH);
	image_place->setMinimumSize(image_place_size);

	combobox = new QComboBox();
	QSize combobox_size = combobox->minimumSize();
	combobox_size.setWidth(IMAGE_WIDTH);
	combobox->setMinimumSize(combobox_size);

	QWidget* default_widget = new QWidget();

	layout = new QVBoxLayout(default_widget);
	layout->setSpacing(0);
    layout->setMargin(2);
	layout->addWidget(image_place);
	layout->addWidget(combobox);

	std::vector<Color_map*> cms = Color_map_factory::color_maps;
	for (unsigned i=0; i< cms.size(); i++) {
		addItem(cms[i]->get_name());
	}

	setDefaultWidget(default_widget);

}

void Popup_color_map_frame::addItem( const QString & text) {
	combobox->addItem(text);
}

void Popup_color_map_frame::set_color_map(const QString & text) {
	if (text=="") return;
	int i = combobox->findText(text);
	if (i==-1) std::cout << "ERROR: something wrong with the color maps, we want to set one which does not exist" << std::endl;
	else combobox->setCurrentIndex(i);
}

void Popup_color_map_frame::set_pixmap(QPixmap* pm) {
	image_place->setPixmap(*pm);
}

void Popup_color_map_frame::set_pixmap(const QColor& col) {

	QPixmap pix(IMAGE_WIDTH,15);
	pix.fill(col);
	image_place->setPixmap(pix);

	//image_place->setStyleSheet(tr("QLabel { background: rgb(%1, %2 %3) }").arg(col.red()).arg(col.green()).arg(col.blue()));
	//image_place->setText(" ");
	//image_place->repaint();
}