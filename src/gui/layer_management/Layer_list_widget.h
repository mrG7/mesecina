/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Layer_list_widget.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef LAYER_LIST_WIDGET_H
#define LAYER_LIST_WIDGET_H

#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFrame>
#include <QtGui/QListView>

class Layer_list_widget : public QFrame {
	Q_OBJECT

public:
	Layer_list_widget(QListView* list_view, QWidget *parent = 0);
	//~Layer_list_widget();

	void setup_ui();
 
	QVBoxLayout *widget_layout;
	QHBoxLayout *filter_layout;
	QCheckBox *filter_checkbox;
	QLineEdit *filter_box;
	QPushButton *filter_delete_button;
	QLabel *title_label;
	QListView *layer_list;
	QHBoxLayout *save_bundle_layout;
	QComboBox *bundle_combo_box;
	QPushButton *save_bundle_button;

public slots:
	void delete_filter_text(bool);
	void filter_text_changed(const QString&);
	void filter_check_changed(int);

signals:
	void new_filter(const QString&, bool);

};

#endif //LAYER_LIST_WIDGET_H
