/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Layer_list_widget.cpp 78 2007-03-25 03:19:20Z miklosb $
 */

#include <gui/layer_management/Layer_list_widget.h>

Layer_list_widget::Layer_list_widget(QListView* list_view, QWidget *parent) : QFrame(parent), layer_list(list_view) { 
	setup_ui();
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

void Layer_list_widget::setup_ui() {
	widget_layout = new QVBoxLayout(this);
	widget_layout->setSpacing(2);
    widget_layout->setMargin(0);

	title_label = new QLabel(this);
	title_label->setText("L   A   Y   E   R   S");
	title_label->setAlignment(Qt::AlignCenter);
	widget_layout->addWidget(title_label);

	filter_layout = new QHBoxLayout();
	filter_layout->setSpacing(2);
    filter_layout->setMargin(0);

	QWidget* spacer = new QWidget(this);
	spacer->setMinimumWidth(2);
	spacer->setMaximumWidth(2);
	filter_layout->addWidget(spacer);

	filter_checkbox = new QCheckBox(this);
	filter_checkbox->setText("");
	#ifdef Q_WS_WIN
	filter_checkbox->setMaximumWidth(14);
	#else
	filter_checkbox->setMaximumWidth(18);
	#endif //Q_WS_WIN
	filter_layout->addWidget(filter_checkbox);

	QWidget* spacer2 = new QWidget(this);
	#ifdef Q_WS_WIN
	spacer2->setMinimumWidth(2);
	spacer2->setMaximumWidth(2);
	#else
	spacer2->setMinimumWidth(3);
	spacer2->setMaximumWidth(3);	
	#endif //Q_WS_WIN
	filter_layout->addWidget(spacer2);

	filter_box = new QLineEdit(this);
	filter_layout->addWidget(filter_box);

	filter_delete_button = new QPushButton(this);
	filter_delete_button->setText("X");
	filter_delete_button->setFlat(true);
	#ifdef Q_WS_WIN
		filter_delete_button->setMaximumWidth(20);
	#else
		filter_delete_button->setMaximumWidth(30);
	#endif //Q_WS_WIN
	filter_layout->addWidget(filter_delete_button);
	widget_layout->addLayout(filter_layout);

	widget_layout->addWidget(layer_list);

	save_bundle_layout = new QHBoxLayout();
	save_bundle_layout->setSpacing(2);
    save_bundle_layout->setMargin(0);
	bundle_combo_box = new QComboBox(this);
	bundle_combo_box->setEditable(true);
	bundle_combo_box->setInsertPolicy(QComboBox::InsertAtBottom);
	save_bundle_layout->addWidget(bundle_combo_box);
	save_bundle_button = new QPushButton(this);
	#ifdef Q_WS_WIN
		save_bundle_button->setMaximumWidth(72);
	#else
		save_bundle_button->setMaximumWidth(120);
	#endif //Q_WS_WIN
	save_bundle_button->setText("Save bundle");
	save_bundle_layout->addWidget(save_bundle_button);
	widget_layout->addLayout(save_bundle_layout);

	connect(filter_delete_button, SIGNAL(clicked(bool)), this, SLOT(delete_filter_text(bool)));
	connect(filter_checkbox, SIGNAL(stateChanged(int)), this, SLOT(filter_check_changed(int)));
	connect(filter_box, SIGNAL(textChanged(const QString&)), this, SLOT(filter_text_changed(const QString&)));

}


void Layer_list_widget::delete_filter_text(bool) {
	filter_box->setText("");
}

void Layer_list_widget::filter_check_changed(int ) {
	emit new_filter(filter_box->text(), filter_checkbox->checkState()==Qt::Checked);
}

void Layer_list_widget::filter_text_changed(const QString&) {
	emit new_filter(filter_box->text(), filter_checkbox->checkState()==Qt::Checked);
}