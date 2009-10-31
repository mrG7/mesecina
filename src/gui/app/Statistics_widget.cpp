/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Statistics_widget.cpp 98 2007-05-01 23:11:20Z miklosb $
 */

#include <gui/app/Statistics_widget.h>
#include <QtGui/QColorGroup>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

#include <QtCore/QAbstractTableModel>
#include <QtGui/QHeaderView>

//#include <QtGui/QTableWidget>
//#include <QtGui/QTableWidgetItem>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>




Statistics_widget::Statistics_widget(QWidget* parent, QString name): QFrame(parent) {
//	setModal(false);
	setFrameStyle(QFrame::Panel | QFrame::Sunken);


	//QTableWidget *table_widget = new QTableWidget(this);
	//table_widget->setColumnCount(2);
	//table_widget->setRowCount(2);
	//table_widget->setSelectionMode(QAbstractItemView::NoSelection);

	//QTableWidgetItem *test_item = new QTableWidgetItem("test");
	//table_widget->setItem(1,1,test_item);
	//table_widget->setItem(2,1,test_item);
	//table_widget->setItem(1,2,test_item);
	//table_widget->setItem(2,2,test_item);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(2);
    layout->setMargin(0);

	QWidget* spacer = new QWidget(this);
	#ifdef Q_WS_WIN
	spacer->setMinimumWidth(2);
	spacer->setMaximumWidth(2);
	#else
	spacer->setMinimumWidth(3);
	spacer->setMaximumWidth(3);	
	#endif //Q_WS_WIN
	layout->addWidget(spacer);

	QLabel* title_label = new QLabel(this);
	if (name=="Times") title_label->setText("B   E   N   C   H   M   A   R   K");
	else title_label->setText("S   T   A   T   S");
	title_label->setAlignment(Qt::AlignCenter);
	layout->addWidget(title_label);


	QHBoxLayout *filter_layout = new QHBoxLayout();
	filter_layout->setSpacing(2);
    filter_layout->setMargin(0);

	QWidget* spacer2 = new QWidget(this);
	spacer2->setMinimumWidth(2);
	spacer2->setMaximumWidth(2);
	filter_layout->addWidget(spacer2);

	filter_box = new QLineEdit(this);
//	filter_box->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	filter_layout->addWidget(filter_box);

	QPushButton* filter_delete_button = new QPushButton();
	filter_delete_button->setText("X");
	filter_delete_button->setFlat(true);
//	filter_delete_button->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	#ifdef Q_WS_WIN
		filter_delete_button->setMaximumWidth(20);
	#else
		filter_delete_button->setMaximumWidth(30);
	#endif //Q_WS_WIN
	filter_layout->addWidget(filter_delete_button);

	table_view = new QTableView();
	#ifdef Q_WS_WIN
		QFont bigger_font = table_view->font();
		bigger_font.setPointSize(font().pointSize()+2);
		table_view->setFont(bigger_font);
	#endif //Q_WS_WIN

	table_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	table_model = new Statistics_table_model();
	table_view->setModel(table_model);
	table_view->setSelectionMode(QAbstractItemView::NoSelection);
	table_view->verticalHeader()->hide();
	table_view->horizontalHeader()->hide();
	table_view->setShowGrid(false);
	table_view->setRowHeight(0,3);
	for (int i=1; i<table_model->rowCount(); i++) table_view->setRowHeight(i,18);

//    layout->setColumnStretch(1, 1);
//	layout->addWidget(table_widget);
	layout->addLayout(filter_layout);
	layout->addWidget(table_view);

	setLayout(layout);
//	addWidget(frame);

	connect(filter_delete_button, SIGNAL(clicked(bool)), this, SLOT(delete_filter_text(bool)));
	connect(filter_box, SIGNAL(textChanged(const QString&)), table_model, SLOT(filter_apply(const QString&)));
	connect(filter_box, SIGNAL(textChanged(const QString&)), this, SLOT(filter_changed(const QString&)));

	filter_box->setFocus();
}


void Statistics_widget::set_variable(Application_variable v, bool apply_filter) {
	table_model->set_variable(v, apply_filter);
	//table_view->resizeColumnToContents(0);
	//table_view->resizeColumnToContents(1);
	table_view->setRowHeight(0,3);
	for (int i=1; i<table_model->rowCount(); i++) table_view->setRowHeight(i,18);
}

void Statistics_widget::clear() {
	table_model->clear();
	filter_box->setFocus();
}


void Statistics_widget::delete_filter_text(bool) {
	filter_box->setText("");
}

void Statistics_widget::filter_changed(const QString& text) {
	//table_model->process_filter();
	table_model->filter_apply(text);
	table_view->setRowHeight(0,3);
	for (int i=1; i<table_model->rowCount(); i++) table_view->setRowHeight(i,18);
//	table_view->repaint();
}


void Statistics_widget::resizeEvent(QResizeEvent * e) {
	QFrame::resizeEvent(e);
	int width = e->size().width()-20;
	int number_width = 70;
	if (number_width < width / 4) number_width = width / 4;
	int name_width = width - number_width;
	table_view->horizontalHeader()->resizeSection ( 0, name_width > 0 ? name_width : 0 );
	table_view->horizontalHeader()->resizeSection ( 1, number_width );
}

void Statistics_widget::process_filter() {
	table_model->process_filter();
	table_view->setRowHeight(0,3);
	for (int i=1; i<table_model->rowCount(); i++) table_view->setRowHeight(i,18);
	//table_view->repaint();
}


