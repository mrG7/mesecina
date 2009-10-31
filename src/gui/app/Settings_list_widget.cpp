/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Settings_list_widget.cpp 737 2009-05-16 15:40:46Z miklosb $
 */

#include <gui/app/Settings_list_widget.h>
#include <QtGui/QColorGroup>
#include <QtGui/QFileDialog>
#include <QtCore//QSettings>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

#include <QtCore/QAbstractTableModel>
#include <QtGui/QHeaderView>


#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QItemEditorFactory>
#include <QtGui/QItemDelegate>
#include <QtGui/QStandardItemEditorCreator>

#include <gui/app/QSpinBoxEditor.h>
#include <gui/app/Popup_animate_application_setting.h>

#include <iostream>



Settings_list_widget::Settings_list_widget(QWidget* parent, QString name): QFrame(parent) {
	setFrameStyle(QFrame::Panel | QFrame::Sunken);


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


	QHBoxLayout *filter_layout = new QHBoxLayout();
	filter_layout->setSpacing(2);
    filter_layout->setMargin(0);

	QWidget* spacer2 = new QWidget(this);
	spacer2->setMinimumWidth(2);
	spacer2->setMaximumWidth(2);
	filter_layout->addWidget(spacer2);

	QPushButton* filter_save_button = new QPushButton();
	filter_save_button->setText("S");
	filter_save_button->setToolTip("Save *filtered* (the ones currently displayed) settings to a file");
	filter_save_button->setFlat(true);
//	filter_save_button->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	#ifdef Q_WS_WIN
		filter_save_button->setMaximumWidth(20);
	#else
		filter_save_button->setMaximumWidth(30);
	#endif //Q_WS_WIN
	filter_layout->addWidget(filter_save_button);

	QPushButton* filter_load_button = new QPushButton();
	filter_load_button->setText("L");
	filter_load_button->setFlat(true);
	filter_load_button->setToolTip("Load some settings from a file");
//	filter_load_button->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	#ifdef Q_WS_WIN
		filter_load_button->setMaximumWidth(20);
	#else
		filter_load_button->setMaximumWidth(30);
	#endif //Q_WS_WIN
	filter_layout->addWidget(filter_load_button);


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

	table_view = new Settings_table_view();
	#ifdef Q_WS_WIN
		QFont bigger_font = table_view->font();
		bigger_font.setPointSize(font().pointSize()+2);
		table_view->setFont(bigger_font);
	#endif //Q_WS_WIN

	table_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	table_model = new Settings_table_model();
	connect(table_view->animate_popup, SIGNAL(application_variable_changed(const QString&)), this,SLOT(settings_changed(const QString&)));
	connect(table_model, SIGNAL(application_settings_changed(const QString&)), this, SLOT(settings_changed(const QString&)));
	table_view->setModel(table_model);
	table_view->setSelectionMode(QAbstractItemView::NoSelection);
	table_view->verticalHeader()->hide();
	table_view->horizontalHeader()->hide();
	table_view->setShowGrid(false);
	table_view->setRowHeight(0,3);
	table_view->setItemDelegate(new QSpinBoxEditor(table_model));

	for (int i=1; i<150; i++) table_view->setRowHeight(i,18);

	layout->addLayout(filter_layout);
	layout->addWidget(table_view);

	setLayout(layout);

	connect(filter_delete_button, SIGNAL(clicked(bool)), this, SLOT(delete_filter_text(bool)));
	connect(filter_load_button, SIGNAL(clicked(bool)), this, SLOT(load_button(bool)));
	connect(filter_save_button, SIGNAL(clicked(bool)), this, SLOT(save_button(bool)));
	connect(filter_box, SIGNAL(textChanged(const QString&)), table_model, SLOT(filter_apply(const QString&)));
	connect(filter_box, SIGNAL(textChanged(const QString&)), this, SLOT(filter_changed(const QString&)));

	filter_box->setFocus();
}

void Settings_list_widget::clear() {
//	table_model->clear();
	filter_box->setFocus();
}

void Settings_list_widget::settings_changed(const QString& settings_name) {
	emit application_settings_changed(settings_name);
}

void Settings_list_widget::delete_filter_text(bool) {
	filter_box->setText("");
}

void Settings_list_widget::load_button(bool) {
	QSettings settings;
    QString file_name = QFileDialog::getOpenFileName(
                    this,
                    "Choose a filename to load settings",
                    settings.value("last-data-directory",QString()).toString(),
					"Settings (*.ini)");
	if (file_name=="") return;
    if (!file_name.endsWith(".ini")) file_name += ".ini";
	QString save_directory = file_name;
	save_directory.truncate(file_name.lastIndexOf('/'));
	settings.setValue("last-data-directory",save_directory);
	
	Settings_table_model::load_current_settings(file_name);

	process_filter();

}
void Settings_list_widget::save_button(bool) {
	QSettings settings;
    QString file_name = QFileDialog::getSaveFileName(
                    this,
                    "Choose a filename to save settings",
                    settings.value("last-data-directory",QString()).toString(),
					"Settings (*.ini)");
	if (file_name=="") return;
    if (!file_name.endsWith(".ini")) file_name += ".ini";
	QString save_directory = file_name;
	save_directory.truncate(file_name.lastIndexOf('/'));
	settings.setValue("last-data-directory",save_directory);
	
	table_model->save_current_settings(file_name);
}

void Settings_list_widget::filter_changed(const QString& text) {
	table_model->filter_apply(text);
	table_view->setRowHeight(0,3);
	for (int i=1; i<150; i++) table_view->setRowHeight(i,18);
}


void Settings_list_widget::resizeEvent(QResizeEvent * e) {
	QFrame::resizeEvent(e);
	int width = e->size().width()-20;
	int number_width = 70;
	if (number_width < width / 4) number_width = width / 4;
	int name_width = width - number_width;
	table_view->horizontalHeader()->resizeSection ( 0, name_width > 0 ? name_width : 0 );
	table_view->horizontalHeader()->resizeSection ( 1, number_width );
}

void Settings_list_widget::process_filter() {
	table_model->process_filter();
	table_view->setRowHeight(0,3);
	for (int i=1; i<150; i++) table_view->setRowHeight(i,18);
}


