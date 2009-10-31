/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info at http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Main_window.cpp 737 2009-05-16 15:40:46Z miklosb $
 */

#include <geometries/Geometry.h>

#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtCore/QDir>

#include <gui/app/static/Color_map_factory.h>
#include <gui/app/Main_window.h>
#include <constants.h>
#ifdef MESECINA_3D
#include <gui/gl_widget_3/GL_widget_3.h>
#include <gui/app/Point3D.h>
#else
#include <gui/gl_widget_2/GL_navigate_layer_2.h>
#include <gui/app/Input_mode_toolbar.h>
#endif

#include <gui/app/Geometry_factory.h>
#include <gui/app/static/Application_settings.h>

#include <gui/layer_management/Layer_list_model.h>
#include <gui/layer_management/Layer_list_view.h>
#include <gui/layer_management/Layer_list_delegate.h>
#include <gui/layer_management/Layer_list_widget.h>

#include <gui/layer_management/Bundle_list_model.h>
#include <gui/layer_management/Bundle_list_view.h>
#include <gui/layer_management/Bundle_list_widget.h>

//#include "Computation_thread.h"

#ifdef _DEBUG
#define DEBUG_MSG(x) msg(x,1);
#else
#define DEBUG_MSG(x)
#endif;


Main_window::Main_window(int x, int y, QString app_name) {
    QCoreApplication::setOrganizationName(COMPANY_NAME);
    QCoreApplication::setApplicationName(app_name);


	Color_map_factory::add_color_map(new Color_map("", "Solid"));
	Application_settings::add_int_setting("random-colormap-resolution",1000);
	Color_map_factory::add_color_map(new Random_color_map(Application_settings::get_int_setting("random-colormap-resolution"), "Random"));

	QDir dir(":/colormap/", "*.map", QDir::Name, QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList list = dir.entryInfoList();
	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		Color_map* cm = new Color_map(fileInfo.absoluteFilePath(), fileInfo.baseName());
		Color_map_factory::add_color_map(cm);
	}

#ifdef MESECINA_3D
	generic_file_types.append("off"); generic_file_description.append("OFF");
	generic_file_types.append("woff"); generic_file_description.append("Weighted OFF");
	generic_file_types.append("moff"); generic_file_description.append("Medial axis OFF");
	generic_file_types.append("stg");  generic_file_description.append("Stage representation");
#else
	generic_file_types.append("poff");  generic_file_description.append("Planar OFF");
	generic_file_types.append("wpoff");  generic_file_description.append("Weighted planar OFF");
	generic_file_types.append("pstg");  generic_file_description.append("Planar stage representation");
#endif

	Geometry_factory::fill_available_geometries();


	settings = new QSettings();
	current_file_name = QString();
	record_directory = QString();
	kernel = settings->value("kernel").toString();
	needs_stats_update = false;

	//reload position and size of main window
	resize(settings->value("mainwindow/size", QSize(x,y)).toSize());
	move(settings->value("mainwindow/pos", QPoint(100,100)).toPoint());

	// order is important!!! Dont change it!
	add_widgets();
	DEBUG_MSG("widgets added");
	
	add_toolbars();
	DEBUG_MSG("toolbars added");

	set_geometries();
	DEBUG_MSG("geometries added");

	add_layers();
	DEBUG_MSG("layers added");

	//add_evolutions();
	//DEBUG_MSG("evolutions added");

	Application_settings::add_double_setting("ball-loading-perturb", 1.0001);

	// add windows which depend on evolutions, layers, geometries
	settings_dialog = new Settings_dialog(this);
	connect(settings_dialog, SIGNAL(closing(bool)), this, SLOT(show_settings(bool)));
	connect(settings_dialog->settings_list_widget, SIGNAL(application_settings_changed(const QString&)), this, SLOT(application_settings_changed(const QString&)));

	add_menu();
	DEBUG_MSG("menus added");

	//computation_thread = new Computation_thread(widget);


	if (settings->contains("mainwindow/state"))
		restoreState(settings->value("mainwindow/state").toByteArray());

	if (settings->contains("mainwindow/v_log_splitter"))
		v_log_splitter->restoreState(settings->value("mainwindow/v_log_splitter").toByteArray());

	if (settings->contains("mainwindow/v_layer_splitter"))
		v_layer_splitter->restoreState(settings->value("mainwindow/v_layer_splitter").toByteArray());

	if (settings->contains("mainwindow/h_splitter"))
		h_splitter->restoreState(settings->value("mainwindow/h_splitter").toByteArray());

	if (settings->contains("bundles"))
		bundle_list_model->restoreState(settings->value("bundles").toByteArray());
	else {
		QFile file(":/standard.bnd");
		if (!file.open(QIODevice::ReadOnly)) {
			std::cout << "Could not find standard.bnd" << std::endl;
		} else {
			bundle_list_model->restoreState(file.readAll(), true);
			file.close();
		}
	}

	if (settings->contains("lastlayerbundle")) {
		Bundle b("active");
		b.restoreState(settings->value("lastlayerbundle").toByteArray());
		layer_list_model->import_from_bundle(b);
	} else {
		if (bundle_list_model->rowCount()>0) {
			Bundle b = bundle_list_model->get_bundle(0);
			layer_list_model->import_from_bundle(b);
			}
	}
	layer_list_model->filter_apply("",false);
	settings_dialog->settings_list_widget->filter_changed("");

	widget->repaintGL();
	widget->set_window_to_boundingbox();
	widget->repaintGL();
	connect(widget, SIGNAL(msg(QString)), logger, SLOT(msg(QString)));
	connect(widget, SIGNAL(settings_changed()), this, SLOT(update_settings()));
	connect(logger, SIGNAL(set_variable(Application_variable)), statistics_dialog->time_widget, SLOT(set_variable(Application_variable)));
	statusBar()->showMessage(tr("Ready"),2000);
	msg(tr("%1 %2 started...").arg(APPLICATION_NAME).arg(APPLICATION_VERSION));
	setFocus();

}

void Main_window::add_widgets() {
	coord_label = new QLabel();
	
	#ifdef Q_WS_WIN
	QFont bigger_font = coord_label->font();
	bigger_font.setPointSize(font().pointSize()+1);
	coord_label->setFont(bigger_font);
	#endif //Q_WS_WIN

	
	coord_label->setText("Honey");
	coord_label->setMinimumWidth(400);
	statusBar()->addPermanentWidget(coord_label);
	load_view = new QPushButton("LV...");
	load_view->setMaximumWidth(50);
	load_view->setToolTip("Load view from a file");
	save_view = new QPushButton("SV...");
	save_view->setToolTip("Save current view to a file");
	save_view->setMaximumWidth(60);
	statusBar()->addPermanentWidget(load_view);
	statusBar()->addPermanentWidget(save_view);
	progress_bar = new QProgressBar();
	progress_bar->setMaximumWidth(200);
	statusBar()->addPermanentWidget(progress_bar);
	coord_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	coord_label->setAlignment(Qt::AlignRight);
	coord_label->setMinimumWidth(120);

	// the v_log_splitter between the GL_widget and the logger window
	v_log_splitter = new QSplitter(this);
	v_log_splitter->setOrientation(Qt::Vertical);
	setCentralWidget(v_log_splitter);

	// the h_splitter between the main window and the right area of the layers
	h_splitter = new QSplitter(v_log_splitter);


#ifdef MESECINA_3D
	widget = new GL_widget_3(h_splitter);
#else
	widget = new GL_widget_2(h_splitter);
#endif

	widget->setMouseTracking(true);
	widget->setFocusPolicy(Qt::ClickFocus);
	widget->setFocus();
	connect(widget, SIGNAL(widget_resized_to(const QRect&)), this, SLOT(gl_widget_resized(const QRect&)));
#ifndef MESECINA_3D
	connect (widget, SIGNAL(mouse_at(const QPointF&)), this, SLOT(mouse_moved(const QPointF&)));
#endif
	connect(save_view, SIGNAL(clicked ( bool)), widget, SLOT(save_view(bool)));
	connect(load_view, SIGNAL(clicked ( bool)), widget, SLOT(load_view(bool)));
	// the v_layer_splitter
	v_layer_splitter = new QSplitter(h_splitter);
	v_layer_splitter->setOrientation(Qt::Vertical);
	v_layer_splitter->setHandleWidth(7);

	bundle_list_model = new Bundle_list_model();
	connect(bundle_list_model, SIGNAL(bundle_list_changed()), this, SLOT(save_bundle_list()));
	// bundle list view
	Bundle_list_view *bundle_list_view = new Bundle_list_view();
	bundle_list_view->setModel(bundle_list_model);
	bundle_list_view->setDragEnabled(true);
	bundle_list_view->setAcceptDrops(true);
	bundle_list_view->setDropIndicatorShown(true);
	bundle_list_view->setDragDropMode(QAbstractItemView::InternalMove);
    connect(bundle_list_view, SIGNAL(remove_row(int)), this, SLOT(remove_bundle(int)));
	connect(bundle_list_view, SIGNAL(export_bundle(int)), this, SLOT(export_bundle(int)));
	connect(bundle_list_view->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		    this, SLOT(activate_selected_bundle(const QItemSelection&, const QItemSelection&)));
	Bundle_list_widget* bundle_list_widget = new Bundle_list_widget(bundle_list_view, v_layer_splitter);


	// layer list model
	layer_list_model = new Layer_list_model();
    connect(layer_list_model, SIGNAL(widget_needs_repaint()), widget, SLOT(repaintGL()));
    connect(layer_list_model, SIGNAL(widget_needs_repaint()), this, SLOT(update_stats_if_needed()));
    connect(layer_list_model, SIGNAL(layers_reordered()), this, SLOT(reorder_layers_on_widget()));
	connect(layer_list_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(layers_changed(const QModelIndex &, const QModelIndex &)));
	//layer list view
	Layer_list_view *layer_list_view = new Layer_list_view();
	layer_list_view->setModel(layer_list_model);
	layer_list_view->setItemDelegate(new Layer_list_delegate);
	layer_list_view->setSelectionMode(QAbstractItemView::SingleSelection);
	layer_list_view->setDragEnabled(true);
	layer_list_view->setAcceptDrops(true);
	layer_list_view->setDropIndicatorShown(true);
	layer_list_view->setDragDropMode(QAbstractItemView::InternalMove);
	connect(layer_list_view, SIGNAL(context_menu(QMouseEvent*, const QModelIndex &, QWidget*)),
		    layer_list_model, SLOT(context_menu(QMouseEvent*, const QModelIndex &, QWidget*)));
	layer_list_widget = new Layer_list_widget(layer_list_view, v_layer_splitter);
	layer_list_widget->bundle_combo_box->setModel(bundle_list_model);
    connect(layer_list_widget->save_bundle_button, SIGNAL(clicked(bool)), this, SLOT(save_bundle(bool)));
	connect(layer_list_widget->bundle_combo_box->lineEdit(), SIGNAL(returnPressed ()), this, SLOT(save_bundle()));
	connect(layer_list_widget->bundle_combo_box, SIGNAL(editTextChanged  ( const QString & )), this, SLOT(bundle_text_changed( const QString &)));
	
	connect(layer_list_widget, SIGNAL(new_filter(const QString&, bool)), layer_list_model, SLOT(filter_apply(const QString&, bool)));

	// v_layer_splitter strech policy
	v_layer_splitter->setStretchFactor(v_layer_splitter->indexOf(bundle_list_widget), 0);
	v_layer_splitter->setStretchFactor(v_layer_splitter->indexOf(layer_list_widget), 1);


	// big h_splitter strech policy
	h_splitter->setStretchFactor(h_splitter->indexOf(widget), 1);
	h_splitter->setStretchFactor(h_splitter->indexOf(v_layer_splitter), 0);
	//QList<int> h_sl;
	//h_sl.push_back(550);
	//h_sl.push_back(50);
	//h_splitter->setSizes(h_sl);

	logger = new Logger(v_log_splitter);
	v_log_splitter->setStretchFactor(v_log_splitter->indexOf(h_splitter), 1);
	v_log_splitter->setStretchFactor(v_log_splitter->indexOf(logger), 0);
	QList<int> v_sl;
	v_sl.push_back(450);
	v_sl.push_back(50);
	v_log_splitter->setSizes(v_sl);

	connect(logger, SIGNAL(setProgressMinimum(int)), progress_bar, SLOT(setMinimum(int)));
	connect(logger, SIGNAL(setProgressMaximum(int)), progress_bar, SLOT(setMaximum(int)));
	connect(logger, SIGNAL(setProgressValue(int)), progress_bar, SLOT(setValue(int)));
	connect(logger, SIGNAL(resetProgress()), progress_bar, SLOT(reset()));
	connect(logger, SIGNAL(setProgressStatus(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
	logger->grab_cout();


	statistics_dialog = new Statistics_dialog(this);
	connect(statistics_dialog, SIGNAL(closing(bool)), this, SLOT(show_statistics(bool)));

	help_dialog = new Help_browser_dialog(this);
	help_dialog->text_browser->setContentsMargins(5,5,5,5);
	help_dialog->text_browser->setSource(QUrl("qrc:/manual.html"));
	connect(help_dialog, SIGNAL(closing(bool)), this, SLOT(show_help(bool)));


}

void Main_window::bundle_text_changed ( const QString & text) {
	if (text!="") last_edited_bundle_name = text;
}

void Main_window::save_bundle() {
	//QString bundle_name = layer_list_widget->bundle_combo_box->lineEdit()->text();
	//QString other =layer_list_widget->bundle_combo_box->currentText();
	//if (last_edited_bundle_name=="" && bundle_name=="") return;
	//if (last_edited_bundle_name!="") bundle_name = last_edited_bundle_name;
	if (last_edited_bundle_name == "")
		QMessageBox::critical(this, APPLICATION_NAME,
                              QString("Bundle has not been saved!\nPlease provide a name for the bundle."));
	else {
		int idx = bundle_list_model->find_bundle(last_edited_bundle_name);
		Bundle b = layer_list_model->create_bundle_from_current(last_edited_bundle_name);
		if (idx==-1) {
			// bundle not found
			bundle_list_model->add_bundle(b);
		} else {
			QMessageBox msgBox;
			msgBox.setText(tr("Are you sure you want to overwrite the <%1> bundle with the current layers?").arg(last_edited_bundle_name));
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			switch (msgBox.exec()) {
				case QMessageBox::Yes:
					bundle_list_model->modify_bundle(idx, b.saveState());
					break;
				default:
					// we don't save the bundle
				break;
			}
		}
	}
	last_edited_bundle_name = "";
}

void Main_window::save_bundle_list() {
	settings->setValue("bundles",bundle_list_model->saveState());
}


void Main_window::remove_bundle(int row) {
	bundle_list_model->remove_bundle(row);
}

void Main_window::export_bundle(int row) {
	QString bundle_filepath = QFileDialog::getSaveFileName(
		this,
		"Export bundle",
		settings->value("last-bundle-directory",QString()).toString(),
		"Bundles (*.bnd)");
	if (bundle_filepath=="") return;
	if (!bundle_filepath.endsWith(".bnd")) bundle_filepath += ".bnd";
	QString bundle_directory = bundle_filepath;
	bundle_directory.truncate(bundle_filepath.lastIndexOf('/'));
	settings->setValue("last-bundle-directory",bundle_directory);	

	QFile file(bundle_filepath);
	if (!file.open(QIODevice::WriteOnly)) {
		std::cout << "Could not write bundle file" << std::endl;
		return;
	}
	file.write(bundle_list_model->saveState(row));
	file.close();

}

void Main_window::save_bundle(bool) {
	last_edited_bundle_name = layer_list_widget->bundle_combo_box->lineEdit()->text();
	save_bundle();
}

void Main_window::add_layers() {

#ifndef MESECINA_3D
	GL_navigate_layer_2* n = new GL_navigate_layer_2();
	n->activate();
	widget->attach(n);
	widget->setCursor(QCursor( QPixmap( (const char**)hand_xpm)));


	insert_point_layer = new GL_insert_point_layer_2();
	insert_point_layer->activate();
	widget->attach(insert_point_layer);
	connect(insert_point_layer, SIGNAL(point_created(double, double)), this, SLOT(point_created(double, double)));
	connect(insert_point_layer, SIGNAL(points_created(std::list<QPointF>*)), this, SLOT(points_created(std::list<QPointF>*)));
//	connect(insert_point_layer, SIGNAL(msg(QString,int)), logger, SLOT(msg(QString,int)));

	insert_ball_layer = new GL_insert_ball_layer_2();
	insert_ball_layer->activate();
	widget->attach(insert_ball_layer);
	connect(insert_ball_layer, SIGNAL(ball_created(Point3D)), this, SLOT(ball_created(Point3D)));
	connect(insert_ball_layer, SIGNAL(balls_created(std::list<Point3D>*)), this, SLOT(balls_created(std::list<Point3D>*)));


	selection_layer = new GL_selection_layer_2("selections");
	selection_layer->activate();
	widget->attach(selection_layer);
	connect(selection_layer, SIGNAL(selection_was_deleted()), this, SLOT(update_stats()));
//	connect(selection_layer, SIGNAL(msg(QString,int)), logger, SLOT(msg(QString,int)));

	// signals between insert_point_layer and selection_layer
	connect(insert_point_layer, SIGNAL(add_point_to_selection(QPointF)), selection_layer, SLOT(add_point(QPointF)));
	connect(insert_point_layer, SIGNAL(clear_points_from_selection()), selection_layer, SLOT(clear_points()));

	connect(insert_ball_layer, SIGNAL(add_ball_to_selection(QPointF, float)), selection_layer, SLOT(add_circle(QPointF, float)));
	connect(insert_ball_layer, SIGNAL(add_point_to_selection(QPointF)), selection_layer, SLOT(add_point(QPointF)));
	connect(insert_ball_layer, SIGNAL(clear_balls_from_selection()), selection_layer, SLOT(clear_circles()));
	connect(insert_ball_layer, SIGNAL(clear_points_from_selection()), selection_layer, SLOT(clear_points()));

	// signals between widget and selection_layer (to add primitives to be drawn to the selection_layer)
	connect(widget, SIGNAL(add_point_to_selection(QPointF)), selection_layer, SLOT(add_point(QPointF)));
	connect(widget, SIGNAL(add_line_to_selection(QLineF)), selection_layer, SLOT(add_line(QLineF)));
	connect(widget, SIGNAL(add_circle_to_selection(QPointF, float)), selection_layer, SLOT(add_circle(QPointF, float)));
	connect(widget, SIGNAL(remove_point_from_selection(QPointF)), selection_layer, SLOT(remove_point(QPointF)));
	connect(widget, SIGNAL(remove_line_from_selection(QLineF)), selection_layer, SLOT(remove_line(QLineF)));
	connect(widget, SIGNAL(remove_circle_from_selection(QPointF, float)), selection_layer, SLOT(remove_circle(QPointF, float)));
	connect(widget, SIGNAL(points_modified()), this, SLOT(points_modified()));

	image_layer = new GL_image_layer_2("Image");
	widget->attach(image_layer); /// \todo attach image_layer... 
	layer_list_model->add_layer(image_layer);

	QSettings settings;
	bool cir = settings.value("last-input-mode-circle").toBool();
	if (cir) {
		input_toolbar->circle_button->setChecked(true);
		input_mode_selected(CIRCLE_INPUT_MODE);
	}
	else {
		input_toolbar->point_button->setChecked(true);
		input_mode_selected(POINT_INPUT_MODE);
	}
#else
	cutting_plane_layer = new GL_cutting_plane_layer_3("cutting plane");
	widget->attach(cutting_plane_layer);
	layer_list_model->add_layer(cutting_plane_layer);

	selection_layer = new GL_selection_layer_3("selection");
	selection_layer->activate();
	widget->attach(selection_layer);

	connect(widget, SIGNAL(s_add_point_to_selection(double , double , double )), selection_layer, SLOT(add_point_to_selection(double , double , double )));
	connect(widget, SIGNAL(s_remove_point_to_selection(double , double , double )), selection_layer, SLOT(remove_point_to_selection(double , double , double )));
	connect(widget, SIGNAL(s_clear_selection()), selection_layer, SLOT(clear_selection()));
#endif
}

void Main_window::add_toolbars() {
/*
	evolutions_toolbar = new Evolution_slider_toolbar(this);
	this->addToolBar(evolutions_toolbar);
//	connect(evolutions_toolbar, SIGNAL(set_geometries(std::list< Geometry* > *)),
//			this, SLOT(set_geometries(std::list<Geometry*> *)));

	record_toolbar = new Record_toolbar();
	this->addToolBar(record_toolbar);
//	this->insertToolBar(evolutions_toolbar, record_toolbar);
	connect(record_toolbar->record_button, SIGNAL(toggled(bool)), this, SLOT(record_status_change(bool)));

#ifndef MESECINA_3D
	input_toolbar = new Input_mode_toolbar();
//	this->insertToolBar(record_toolbar, input_toolbar);
	this->addToolBar(input_toolbar);
	connect(input_toolbar, SIGNAL(input_mode_selected(Input_mode)), this, SLOT(input_mode_selected(Input_mode)));
#endif
*/

#ifndef MESECINA_3D
	input_toolbar = new Input_mode_toolbar();
//	this->insertToolBar(record_toolbar, input_toolbar);
	this->addToolBar(input_toolbar);
	connect(input_toolbar, SIGNAL(input_mode_selected(Input_mode)), this, SLOT(input_mode_selected(Input_mode)));
#endif

	record_toolbar = new Record_toolbar();
	this->addToolBar(record_toolbar);
//	this->insertToolBar(evolutions_toolbar, record_toolbar);
	connect(record_toolbar->record_button, SIGNAL(toggled(bool)), this, SLOT(record_status_change(bool)));

	evolutions_toolbar = new Evolution_slider_toolbar(this);
	this->addToolBar(evolutions_toolbar);
//	connect(evolutions_toolbar, SIGNAL(set_geometries(std::list< Geometry* > *)),
//			this, SLOT(set_geometries(std::list<Geometry*> *)));





//	this->addToolBarBreak();


}

void Main_window::record_status_change(bool checked) {
	if (checked) {
		if (record_directory=="") ask_recording_directory();
		if (record_directory!="") {
			record_action->setChecked(true);
			record_toolbar->record_button->setChecked(true);
			connect(widget, SIGNAL(repaitedGL()), this, SLOT(export_next_image()));
		}
	} else {
		record_action->setChecked(false);
		record_toolbar->record_button->setChecked(false);
		disconnect(widget, SIGNAL(repaitedGL()), this, SLOT(export_next_image()));
	}
}


#ifndef MESECINA_3D
void Main_window::input_mode_selected(Input_mode im) {
	if (im==POINT_INPUT_MODE) {
		insert_point_layer->activate();
		insert_ball_layer->deactivate();
		std::cout << "point input mode" << std::endl;
	}
	else { 
		insert_point_layer->deactivate();
		insert_ball_layer->activate();
		std::cout << "circle input mode" << std::endl;
	}

}
#endif

void Main_window::reorder_layers_on_widget() {
	for (int i=0; i < layer_list_model->non_filtered_count(); i++)
		widget->detach(layer_list_model->get_layer(i));
	for (int i=0; i < layer_list_model->non_filtered_count(); i++)
		widget->attach(layer_list_model->get_layer(i));
	// move selection on the top
#ifndef MESECINA_3D
	widget->detach(selection_layer);
	widget->attach(selection_layer);
#endif
	widget->repaintGL();
}

void Main_window::activate_selected_bundle(const QItemSelection & selected, const QItemSelection & ) {
    QModelIndex index;
	QModelIndexList items = selected.indexes();

    foreach(index, items) {
		Bundle b = bundle_list_model->get_bundle(index.row());
		layer_list_model->import_from_bundle(b);
    }
	if (QFile::exists(BUNDLE_IN_SOUND)) QSound::play(BUNDLE_IN_SOUND);
	update_stats();
}

void Main_window::create_actions() {
	new_action = new QAction(tr("&New"), this);
	new_action->setShortcut(tr("CTRL+N"));
	new_action->setStatusTip(tr("Clear widget and all data"));
	connect(new_action, SIGNAL(triggered()), this, SLOT(restart_geometry()));

	load_action = new QAction(tr("&Load file..."), this);
	load_action->setShortcut(tr("CTRL+L"));
	load_action->setStatusTip(tr("Load pointset from a file"));
	connect(load_action, SIGNAL(triggered()), this, SLOT(load_generic_file()));

	save_action = new QAction(tr("&Save file"), this);
	save_action->setShortcut(tr("CTRL+S"));
	save_action->setStatusTip(tr("Save current pointset"));
	connect(save_action, SIGNAL(triggered()), this, SLOT(save_generic_file()));

	save_as_action = new QAction(tr("Export..."), this);
	save_as_action->setShortcut(tr("CTRL+A"));
	save_as_action->setStatusTip(tr("Save current pointset in a new file"));
	connect(save_as_action, SIGNAL(triggered()), this, SLOT(save_as_generic_file()));

	for (int i = 0; i < MaxRecentFiles; ++i) {
		recent_file_actions[i] = new QAction(this);
		recent_file_actions[i]->setVisible(false);
		recent_file_actions[i]->setShortcut(tr("CTRL+%1").arg(i+1));
		connect(recent_file_actions[i], SIGNAL(triggered()),
			this, SLOT(openRecentFile()));
	}

	print_action = new QAction(tr("&Print..."), this);
	print_action->setShortcut(tr("CTRL+P"));
	print_action->setStatusTip(tr("Save current pointset in a new file"));
	connect(print_action, SIGNAL(triggered()), this, SLOT(print()));
	
	exit_action = new QAction(tr("E&xit"), this);
	exit_action->setShortcut(tr("Ctrl+X"));
	exit_action->setStatusTip(tr("Exit the application"));
	connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));

	export_bundles_action = new QAction(tr("&Export bundles..."), this);
	export_bundles_action->setStatusTip(tr("Export current bundles"));
	connect(export_bundles_action, SIGNAL(triggered()), this, SLOT(export_bundles()));

	import_bundles_action = new QAction(tr("&Import bundles..."), this);
	import_bundles_action->setStatusTip(tr("Import bundles"));
	connect(import_bundles_action, SIGNAL(triggered()), this, SLOT(import_bundles()));

	clear_all_bundles_action = new QAction(tr("&Clear all bundles"), this);
	clear_all_bundles_action->setStatusTip(tr("Clear all bundles"));
	connect(clear_all_bundles_action, SIGNAL(triggered()), this, SLOT(clear_bundles()));



	export_image_action = new QAction(tr("Export &image..."), this);
	export_image_action->setShortcut(tr("CTRL+I"));
	export_image_action->setStatusTip(tr("Export current view to an image"));
	connect(export_image_action, SIGNAL(triggered()), this, SLOT(export_image()));

	snapshot_action = new QAction(tr("Snapshot"), this);
	snapshot_action->setShortcut(tr("F3"));
	snapshot_action->setStatusTip(tr("Take snapshot to recording directory"));
	connect(snapshot_action, SIGNAL(triggered()), this, SLOT(export_next_image()));

	record_action = new QAction(tr("Recording"), this);
	record_action->setShortcut(tr("F4"));
	record_action->setCheckable(true);
	record_action->setStatusTip(tr("Start capturing to recording directory"));
	connect(record_action, SIGNAL(toggled(bool)), this, SLOT(record_status_change(bool)));

	record_directory_action = new QAction(tr("Directory..."),this);
	record_directory_action->setToolTip(tr("Change the recording directory"));
	connect(record_directory_action, SIGNAL(triggered()), this, SLOT(ask_recording_directory()));

	record_png_action = new QAction(tr("PNG"),this);
	record_png_action->setToolTip(tr("Capture in to PNG"));
	record_png_action->setCheckable(true);
	connect(record_png_action, SIGNAL(triggered()), this, SLOT(set_png_recording()));
	
	record_pdf_action = new QAction(tr("PDF"),this);
	record_pdf_action->setToolTip(tr("Capture in to PDF"));
	record_pdf_action->setCheckable(true);
	connect(record_pdf_action, SIGNAL(triggered()), this, SLOT(set_pdf_recording()));
	
	record_ps_action = new QAction(tr("PS"),this);
	record_ps_action->setToolTip(tr("Capture in to PS"));
	record_ps_action->setCheckable(true);
	connect(record_ps_action, SIGNAL(triggered()), this, SLOT(set_ps_recording()));

	record_format = settings->value("record-format", "png").toString();
	update_record_format_menus();


	std::list<std::string>::iterator gn_it, gn_end = Geometry_factory::available_geometries.end();
	for (gn_it = Geometry_factory::available_geometries.begin(); gn_it!=gn_end; gn_it++) {
		QAction* gact = new QAction(gn_it->c_str(), this);
		gact->setCheckable(true);
		gact->setChecked(settings->value(tr("geometry-selected-%1").arg(gn_it->c_str()),false).toBool());
		geometry_actions.push_back(gact);
		connect(gact, SIGNAL(triggered()), this, SLOT(change_geometry_activation()));
	}

	//double_kernel_action = new QAction(tr("double"),this);
	//double_kernel_action->setToolTip(tr("c++ double type used for geometry"));
	//double_kernel_action->setCheckable(true);
	//connect(double_kernel_action, SIGNAL(triggered()), this, SLOT(set_double_kernel()));

	//coreexpr_kernel_action = new QAction(tr("CORE::Expr"),this);
	//coreexpr_kernel_action->setToolTip(tr("CORE's exact arithmetic"));
	//coreexpr_kernel_action->setCheckable(true);
	//connect(coreexpr_kernel_action, SIGNAL(triggered()), this, SLOT(set_coreexpr_kernel()));

	kernel = settings->value("kernel", "coreexpr").toString();
	update_selected_geometries();
//	update_kernel_menus();

	about_action = new QAction(tr("About"), this);
	about_action->setStatusTip(tr("Show information about %1").arg(APPLICATION_NAME));
	connect(about_action, SIGNAL(triggered()), this, SLOT(about()));

	statistics_action = new QAction(tr("Statistics"), this);
	statistics_action->setStatusTip(tr("Show/hide statistics dialog"));
	statistics_action->setCheckable(true);
	bool checked = settings->value("show-statistics",false).toBool();
	statistics_action->setChecked(checked);
	statistics_action->setShortcut(tr("Ctrl+W"));
	show_statistics(checked);
	connect(statistics_action, SIGNAL(toggled(bool)), this, SLOT(show_statistics(bool)));

	settings_action = new QAction(tr("Settings"), this);
	settings_action->setStatusTip(tr("Show/hide settings dialog"));
	settings_action->setCheckable(true);
	checked = settings->value("show-settings",false).toBool();
	settings_action->setChecked(checked);
	settings_action->setShortcut(tr("Ctrl+E"));
	show_settings(checked);
	connect(settings_action, SIGNAL(toggled(bool)), this, SLOT(show_settings(bool)));


	help_action = new QAction(tr("Manual"), this);
	help_action->setStatusTip(tr("Show/hide manual"));
	help_action->setCheckable(true);
	checked = settings->value("show-help",true).toBool();
	help_action->setChecked(checked);
	help_action->setShortcut(tr("F1"));
	show_help(checked);
	connect(help_action, SIGNAL(toggled(bool)), this, SLOT(show_help(bool)));
}

void Main_window::add_menu() {

	create_actions();

	file_menu = menuBar()->addMenu(tr("&File"));
	file_menu->addAction(new_action);
	file_menu->addAction(load_action);
	file_menu->addAction(save_action);
	file_menu->addAction(save_as_action);
	file_menu->addSeparator();
	file_menu->addAction(print_action);
	separator_action = file_menu->addSeparator();

    for (int i = 0; i < MaxRecentFiles; ++i)
		file_menu->addAction(recent_file_actions[i]);
	#ifdef Q_WS_WIN		
	file_menu->addSeparator();
	file_menu->addAction(exit_action);
	#endif //Q_WS_WIN

    update_recent_file_actions();

	bundles_menu = menuBar()->addMenu(tr("&Bundles"));
	bundles_menu->addAction(export_bundles_action);
	bundles_menu->addAction(import_bundles_action);
	bundles_menu->addAction(clear_all_bundles_action);

	record_menu = menuBar()->addMenu(tr("&Record"));
	record_menu->addAction(export_image_action);
	record_menu->addAction(snapshot_action);
	record_menu->addAction(record_action);
	record_menu->addSeparator();
	record_menu->addAction(record_directory_action);
	record_format_menu = record_menu->addMenu(tr("Format"));
	record_format_menu->addAction(record_png_action);
	record_format_menu->addAction(record_pdf_action);
	record_format_menu->addAction(record_ps_action);

	geometries_menu = menuBar()->addMenu(tr("&Geometries"));
	std::list<QAction*>::iterator g_it, g_end = geometry_actions.end();
	for (g_it = geometry_actions.begin(); g_it!=g_end; g_it++) {
		geometries_menu->addAction(*g_it);
	}

	windows_menu = menuBar()->addMenu(tr("&Window"));
	windows_menu->addAction(statistics_action);
	windows_menu->addAction(settings_action);
	windows_menu->addAction(help_action);

	menuBar()->addAction(about_action);
	//help_menu = menuBar()->addMenu(tr("&Help"));
	//help_menu->addAction(about_action);
}

Main_window::~Main_window() {
	delete settings;
	logger->release_cout();
}

void Main_window::closeEvent(QCloseEvent* ce) {
	settings->setValue("lastlayerbundle",layer_list_model->create_bundle_from_current("active").saveState());
	settings->setValue("mainwindow/state", saveState());
	settings->setValue("mainwindow/v_log_splitter", v_log_splitter->saveState());
	settings->setValue("mainwindow/v_layer_splitter", v_layer_splitter->saveState());
	settings->setValue("mainwindow/h_splitter", h_splitter->saveState());
    settings->setValue("mainwindow/size", size());
    settings->setValue("mainwindow/pos", pos());




	bool checked = settings->value("show-statistics").toBool();
	if (!statistics_dialog->isHidden()) {
		show_statistics(false);
		settings->setValue("show-statistics",checked);
	}
	checked = settings->value("show-help").toBool();
	if (!help_dialog->isHidden()) {
		show_help(false);
		settings->setValue("show-help",checked);
	}
	checked = settings->value("show-settings").toBool();
	if (!settings_dialog->isHidden()) {
		show_settings(false);
		settings->setValue("show-settings",checked);
	}

	settings->setValue("last-evolutions-name", evolutions_toolbar->evolutions_combo_box->currentText());
#ifndef MESECINA_3D
	settings->setValue("last-input-mode-circle", input_toolbar->input_mode==CIRCLE_INPUT_MODE);
#endif
	ce->accept();
}

void Main_window::gl_widget_resized(const QRect& size) {
	coord_label->setText(tr("[window] width=%1, height=%2        [visualization] width=%3, height=%4").arg(width()).arg(height()).arg(size.width()).arg(size.height()));
	coord_label->repaint();
}

void Main_window::ask_recording_directory() {
	QString directory = QFileDialog::getExistingDirectory(
		this,
		"Choose a directory for the recorder images",
		settings->value("last-record-directory",QString()).toString(),
		QFileDialog::ShowDirsOnly);
	if (directory=="") {
		record_toolbar->record_button->setChecked(false);
		record_action->setChecked(false);
		return;
	}
	record_directory = directory;
	//directory.truncate(directory.lastIndexOf('/'));
	//directory.truncate(directory.lastIndexOf('/'));
	settings->setValue("last-record-directory",directory);
	record_toolbar->record_button->setToolTip(tr("Recording images to %1").arg(record_directory));
	snapshot_action->setStatusTip(tr("Take a snapshot to %1 directory").arg(record_directory));
	record_action->setStatusTip(tr("Record to %1 directory").arg(record_directory));
	record_directory_action->setStatusTip(tr("Change the recording directory from %1").arg(record_directory));
}

//void Main_window::set_double_kernel() {
//	kernel = "double";
//	update_kernel_menus();
//}
//
//void Main_window::set_coreexpr_kernel() {
//	kernel = "coreexpr";
//	update_kernel_menus();
//}

void Main_window::update_selected_geometries() {


	if (geometries.size() > 0) {
#ifndef MESECINA_3D
		std::list<QPointF>* points = (*geometries.begin())->get_points();
		std::list<QPointF> local_points;
		if (points) local_points = std::list<QPointF>(points->begin(), points->end());
#else
		std::list<Point3D>* points = (*geometries.begin())->get_points();
		std::list<Point3D> local_points;
		if (points) local_points = std::list<Point3D>(points->begin(), points->end());
#endif
		set_geometries();
		reset_evolutions_to_current_geometries();
		//					add_evolutions();
		// reload points

		if (points) {
			std::list<Geometry*>::iterator g_it, g_end = geometries.end();
			for (g_it = geometries.begin(); g_it != g_end; g_it++) {
				Geometry *g = *g_it;
				g->add_points(&local_points);
			}
		}
	} else {
		set_geometries();
		reset_evolutions_to_current_geometries();
		settings_dialog->settings_list_widget->filter_changed(settings_dialog->settings_list_widget->filter_box->text());
		layer_list_model->filter_apply("",false);
	}

//	std::cout << "update selected geometries" << std::endl;
}

//void Main_window::update_kernel_menus() {
//	if (kernel != settings->value("kernel").toString()) {
//		bool doit= (settings->value("kernel").toString()=="");
//		QMessageBox msgBox;
//		msgBox.setText(tr("Are you sure you want to switch to <%1> kernel?\n\nYour geometries will be reset with this operation").arg(kernel));
//		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//		if (doit || msgBox.exec() == QMessageBox::Yes ) {
//				settings->setValue("kernel", kernel);
//				std::cout << "Kernel set to " << kernel.toStdString() << std::endl;
//				//get points from current state
//				if (geometries.size() > 0) {
//#ifndef MESECINA_3D
//					std::list<QPointF>* points = (*geometries.begin())->get_points();
//					std::list<QPointF> local_points;
//					if (points) local_points = std::list<QPointF>(points->begin(), points->end());
//#else
//					std::list<Point3D>* points = (*geometries.begin())->get_points();
//					std::list<Point3D> local_points;
//					if (points) local_points = std::list<Point3D>(points->begin(), points->end());
//#endif
//					set_geometries();
//					reset_evolutions_to_current_geometries();
////					add_evolutions();
//					// reload points
//					if (!doit && points) {
//						std::list<Geometry*>::iterator g_it, g_end = geometries.end();
//						for (g_it = geometries.begin(); g_it != g_end; g_it++) {
//							Geometry *g = *g_it;
//							g->add_points(&local_points);
//						}
//					}
//				}
//		} else {
//			kernel = settings->value("kernel").toString();
//		}
//	}
//	double_kernel_action->setChecked(kernel=="double");
//	coreexpr_kernel_action->setChecked(kernel=="coreexpr");
//}

void Main_window::set_png_recording() {
	record_format = "png";
	update_record_format_menus();
}

void Main_window::set_pdf_recording() {
	record_format = "pdf";
	update_record_format_menus();
}

void Main_window::set_ps_recording() {
	record_format = "ps";
	update_record_format_menus();
}

void Main_window::update_record_format_menus() {
	record_png_action->setChecked(record_format=="png");
	record_pdf_action->setChecked(record_format=="pdf");
	record_ps_action->setChecked(record_format=="ps");
	settings->setValue("record-format", record_format);
//	msg(tr("Recording format set to %1").arg(record_format));
}

void Main_window::export_next_image() {
	if (record_directory=="") ask_recording_directory();
	if (record_directory=="") return;
	QString next_file_name = get_next_image_filname();
	export_image(next_file_name);
}

QString Main_window::get_next_image_filname() {
	QDir dir(record_directory);
	if (!dir.exists()) {
		error("Current record directory does not exist");
		return "";
	}
	QStringList entries = dir.entryList();
	int number = 0;
	QString file_name;
	do {
		file_name = (tr("%1").arg(number));
		while (file_name.size()!=5) file_name = tr("0%1").arg(file_name);
		file_name.append(tr(".%1").arg(record_format));
		number++;
	} while (entries.contains(file_name));
	return tr("%1/%2").arg(record_directory).arg(file_name);
}

void Main_window::export_image() {
    QString image_filepath = QFileDialog::getSaveFileName(
			this,
			"Export current view to image",
			settings->value("last-image-directory",QString()).toString(),
            "Images (*.png)");
	if (image_filepath=="") return;
    if (!image_filepath.endsWith(".png")) image_filepath += ".png";
	QString image_directory = image_filepath;
	image_directory.truncate(image_filepath.lastIndexOf('/'));
	settings->setValue("last-image-directory",image_directory);
	export_image(image_filepath);
}

void Main_window::export_bundles() {
    QString bundle_filepath = QFileDialog::getSaveFileName(
			this,
			"Export bundles",
			settings->value("last-bundle-directory",QString()).toString(),
            "Bundles (*.bnd)");
	if (bundle_filepath=="") return;
    if (!bundle_filepath.endsWith(".bnd")) bundle_filepath += ".bnd";
	QString bundle_directory = bundle_filepath;
	bundle_directory.truncate(bundle_filepath.lastIndexOf('/'));
	settings->setValue("last-bundle-directory",bundle_directory);

	QFile file(bundle_filepath);
	if (!file.open(QIODevice::WriteOnly)) {
		std::cout << "Could not write bundle file" << std::endl;
		return;
	}
	file.write(bundle_list_model->saveState());
	file.close();
}

void Main_window::clear_bundles() {
	QMessageBox msgBox(this);
	msgBox.setText(tr("Are you sure you want to delete all bundles?"));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	switch (msgBox.exec()) {
				case QMessageBox::Yes:
					bundle_list_model->clear_bundles();
					break;
				default:
					// we don't save the bundle
					break;
	}	
}


void Main_window::import_bundles() {
    QString file_name = QFileDialog::getOpenFileName(
                    this,
                    "Choose a filename to load bundles",
                    settings->value("last-bundle-directory",QString()).toString(),
                    "Bundles (*.bnd)");
	if (file_name!="") {
		if (!file_name.endsWith(".bnd")) file_name += ".bnd";

		//load_file(file_name);
		QFile file(file_name);
		if (!file.open(QIODevice::ReadOnly)) {
			std::cout << "Could not read bundle file" << std::endl;
			return;
		}
		bundle_list_model->restoreState(file.readAll(), true);
		file.close();

		QString save_directory = file_name;
		save_directory.truncate(file_name.lastIndexOf('/'));
		settings->setValue("last-bundle-directory",save_directory);
	}
}

void Main_window::export_image(QString image_filepath) {
	if (image_filepath.endsWith("png")) {
		bool signal_manipulating = record_toolbar->record_button->isChecked();
		if (signal_manipulating) disconnect(widget, SIGNAL(repaitedGL()), this, SLOT(export_next_image()));
		widget->export_to_image(image_filepath);
		if (signal_manipulating) connect(widget, SIGNAL(repaitedGL()), this, SLOT(export_next_image()));
	} else // for pfd and ps
		print(image_filepath);
}

void Main_window::print(QString file_name) {
	if (!file_name.endsWith("ps") && !file_name.endsWith("pdf")) {
		error(tr("Image not saved! Unkown format for %1").arg(file_name));
		return;
	}
	bool signal_manipulating = record_toolbar->record_button->isChecked();
	if (signal_manipulating) disconnect(widget, SIGNAL(repaitedGL()), this, SLOT(export_next_image()));

	widget->export_to_vector(file_name);
	if (signal_manipulating) connect(widget, SIGNAL(repaitedGL()), this, SLOT(export_next_image()));
}

void Main_window::print() {
	widget->print_to_printer();
}


void Main_window::keyPressEvent(QKeyEvent *e) {
	widget->keyPressEvent(e);
	evolutions_toolbar->keyPressEvent(e);
//	statistics_dialog->keyPressEvent(e);
	e->setAccepted(false);
}

void Main_window::keyReleaseEvent(QKeyEvent *e) {
	widget->keyReleaseEvent(e);
	evolutions_toolbar->keyReleaseEvent(e);
//	statistics_dialog->keyReleaseEvent(e);
	e->setAccepted(false);
}

void Main_window::debug(QString m) {
	logger->msg(m,1);
	logger->repaint();
}

void Main_window::msg(QString m) {
	logger->msg(m,0);
	logger->repaint();
}

void Main_window::msg(QString m, int level) {
	logger->msg(m,level);
	logger->repaint();
}

void Main_window::error(QString m) {
	logger->msg(m,99);
	logger->repaint();
}

void Main_window::mouse_moved(const QPointF& pos) {
	coord_label->setText(tr("x=%1, y=%2").arg(pos.x(),6,'f',3).arg(pos.y(),6,'f',3));
}

#ifndef MESECINA_3D
void Main_window::point_created(double x, double y) {
	std::list<Geometry*>::iterator g_it, g_end = geometries.end();

	for (g_it = geometries.begin(); g_it != g_end; g_it++) (*g_it)->add_point(x,y);
	reset_evolutions_to_current_geometries();
//	add_evolutions();
	widget->repaintGL();
	needs_stats_update = true;
	update_stats_if_needed();
}

void Main_window::points_created(std::list<QPointF>* points) {
	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it != g_end; g_it++) (*g_it)->add_points(points);
	reset_evolutions_to_current_geometries();

//	add_evolutions();
	widget->repaintGL();
	needs_stats_update = true;
	update_stats_if_needed();
}

void Main_window::ball_created(Point3D p) {
	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it != g_end; g_it++) {
		Geometry *g = *g_it;
		g->add_weighted_point(p);
	}
//	std::cout << "Main_window: ball created: " << p.x <<", " << p.y <<", "<< p.z << std::endl;

	reset_evolutions_to_current_geometries();
	widget->repaintGL();
	needs_stats_update = true;
	update_stats_if_needed();

}
void Main_window::balls_created(std::list<Point3D>* points) {
	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it != g_end; g_it++) {
		Geometry *g = *g_it;
		g->add_weighted_points(points);
	}
//	std::cout << "Main_window: balls created.. this many: " << points->size() << std::endl;

	reset_evolutions_to_current_geometries();
	widget->repaintGL();
	needs_stats_update = true;
	update_stats_if_needed();

}
#endif

void Main_window::points_modified() {
	reset_evolutions_to_current_geometries();

//	add_evolutions(false);
	widget->repaintGL();
	needs_stats_update = true;
	update_stats_if_needed();
}


void Main_window::restart_geometry() {
	set_geometries();
	reset_evolutions_to_current_geometries();
//	add_evolutions(false);
	statistics_dialog->time_widget->clear();
	statistics_dialog->statistics_widget->clear();
	setWindowTitle(tr("%1").arg(APPLICATION_NAME));
	current_file_name = "";
	widget->repaintGL();
}


void Main_window::about() {
	QString svn_revision(SVN_REVISION);
	svn_revision.remove('$');
#ifndef GINA_2
	QMessageBox::about( this, tr("About %1 v%2").arg(APPLICATION_NAME).arg(APPLICATION_VERSION),
tr("This software is for visualization and studying of medial axis and related\n\
computational geometry structures. For more information and latest version visit: \n\n\
                        http://www.agg.ethz.ch/~miklosb/mesecina/\n\n\
%1 v%2 was developed using Qt 4.2.0 and CGAL 3.2.1\n\n\
Copyright Bálint Miklós, Applied Geometry Group, ETH Zurich, 2006-2008\n\
Thank you to Armin Häberling for his contribution\n\n\
SVN version: %3").arg(APPLICATION_NAME).arg(APPLICATION_VERSION).arg(svn_revision));
#else
	QMessageBox::about( this, tr("About %1 v%2").arg(APPLICATION_NAME).arg(APPLICATION_VERSION),
tr("This is a software framework for 2d geometry visualization and algorithm prototyping.\n\
It is derived from Mesecina. For more information about Mesecina visit: \n\n\
                        http://www.agg.ethz.ch/~miklosb/mesecina/\n\n\
%1 v%2 was developed using Qt 4.2.0 and CGAL 3.2.1\n\n\
Copyright Bálint Miklós, Applied Geometry Group, ETH Zurich, 2006-2008\n\n\
SVN version: %3").arg(APPLICATION_NAME).arg(APPLICATION_VERSION).arg(svn_revision));
#endif
}


void Main_window::load_generic_file() {
	QString current_filter = generic_current_file_filter_string();
    QString file_name = QFileDialog::getOpenFileName(
                    this,
                    "Choose a filename to load data",
                    settings->value("last-data-directory",QString()).toString(),
                    generic_file_filter_string(),
					&current_filter);
	if (file_name!="") {
		load_generic_file(file_name);
		QString save_directory = file_name;
		save_directory.truncate(file_name.lastIndexOf('/'));
		settings->setValue("last-data-directory",save_directory);
	}

}
void Main_window::load_generic_file(const QString &file_name) {
	current_file_name = file_name;
	QString short_file_name = current_file_name.right(current_file_name.length() - current_file_name.lastIndexOf("/") - 1);
	setWindowTitle(tr("%1 - %2").arg(APPLICATION_NAME).arg(short_file_name));

	//clean up current state and add geometries
	set_geometries();

	//call known functions in Main_window
	QString extension = file_name.right(file_name.length() - file_name.lastIndexOf(".")-1);
	if (extension=="poff" || extension=="off")
		load_file(file_name);
	else if (extension=="wpoff" || extension=="woff")
		load_balls(file_name);

	//broadcast to geometries
	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it != g_end; g_it++) {
		Geometry *g = *g_it;
		g->load_generic_file(file_name.toStdString());
	}

	//do recent files and repainting 

	// save current bundle
	Bundle temp_bundle(layer_list_model->create_bundle_from_current("temp_bundle"));
	// make layers need_rescale so that we are not stuck with the color map scales from before even if the scale is fixed
	for (int i=0; i < layer_list_model->non_filtered_count(); i++) {
		Managable_layer* l = layer_list_model->get_layer(i);
		if (l->has_property(Managable_layer::SCALAR_EDITABLE))
			l->set_need_rescale_colormap(true);
	}
	widget->repaint();
	// restore the settings for layers
	layer_list_model->import_from_bundle(temp_bundle);

	update_stats_if_needed(/*QModelIndex(),QModelIndex()*/);

#ifdef MESECINA_3D
	widget->camera()->setRevolveAroundPoint(widget->sceneCenter());
	cutting_plane_layer->fr->setPosition(widget->sceneCenter());
#endif
	reset_evolutions_to_current_geometries();
//	add_evolutions(false);
	widget->repaintGL();
	widget->set_window_to_boundingbox();
	widget->repaintGL();
	set_current_file(file_name);
	update_stats_if_needed();

}
void Main_window::save_generic_file() {
	if (current_file_name != "") { save_generic_file(current_file_name); return; }
	save_as_generic_file();
}

void Main_window::save_as_generic_file() {
	QString extension = settings->value("last-export-extension",QString()).toString();
	QString current_filter = generic_one_file_filter_string(extension);
	QString current_dir = settings->value("last-export-directory",QString()).toString();
	if (current_dir=="") current_dir = settings->value("last-data-directory",QString()).toString();
    QString file_name = QFileDialog::getSaveFileName(
                    this,
                    "Choose a filename to save data",
                    current_dir,
                    generic_file_filter_string(),
					&current_filter);
	if (file_name=="") return;
	QString save_directory = file_name;
	save_directory.truncate(file_name.lastIndexOf('/'));
	settings->setValue("last-export-directory",save_directory);
	extension = file_name.right(file_name.length() - file_name.lastIndexOf(".")-1);
	settings->setValue("last-export-extension", extension);
	save_generic_file(file_name);
}

void Main_window::save_generic_file(const QString &file_name) {
	QString extension = file_name.right(file_name.length() - file_name.lastIndexOf(".")-1);

#ifdef MESECINA_3D
	if (extension=="off")
		save_file(file_name);
	else if (extension=="woff")
		save_balls(file_name);
#else
	if (extension=="poff")
		save_file(file_name);
	else if (extension=="wpoff")
		save_balls(file_name);
#endif
	//broadcast to geometries
	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it != g_end; g_it++) {
		Geometry *g = *g_it;
		g->save_generic_file(file_name.toStdString());
	}
	needs_stats_update = true;
	update_stats_if_needed();
}

QString Main_window::generic_file_filter_string() {
	QString s = "All types (";
	QStringList::iterator s_it, s_end = generic_file_types.end(); int i=0;
	for (s_it= generic_file_types.begin(); s_it!=s_end; s_it++, i++) {
		s = s + "*." + *s_it+" ";
	}
	s = s + ");;";
	for (i=0, s_it= generic_file_types.begin(); s_it!=s_end; s_it++, i++) {
		s = s + generic_file_description[i] + " (*."+*s_it+")";
		if (i!=generic_file_types.size()-1) s = s + ";;";
	}
	return s;
}

QString Main_window::generic_one_file_filter_string(const QString &extension) {
	int i = 0;
	QStringList::iterator s_it, s_end = generic_file_types.end(); int j=0;
	for (s_it= generic_file_types.begin(); s_it!=s_end; s_it++, j++) {
		if (extension==*s_it) i=j;
	}
	return generic_file_description[i] + " (*."+generic_file_types[i]+")";

}

QString Main_window::generic_current_file_filter_string() {
	QString s = "All types (";
	QStringList::iterator s_it, s_end = generic_file_types.end(); int i=0;
	for (s_it= generic_file_types.begin(); s_it!=s_end; s_it++, i++) {
		s = s + "*." + *s_it+" ";
	}
	s = s + ")";
	return s;

	//QString recent_extension = recent_file_actions[0]->text();
	//recent_extension = recent_extension.right(recent_extension.length() - recent_extension.lastIndexOf(".")-1);
	//return generic_one_file_filter_string(recent_extension);
}	


void Main_window::save_balls(QString file_name) {
	QString short_file_name = file_name.right(current_file_name.length() - file_name.lastIndexOf("/") - 1);
	setWindowTitle(tr("%1 - %2").arg(APPLICATION_NAME).arg(short_file_name));

	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
#ifdef MESECINA_3D
	std::list<Point4D>* points = 0;
#else
	std::list<Point3D>* points = 0;
#endif
	int i = 0;
	for (g_it = geometries.begin(); g_it!=g_end, points==0; g_it++) {
		std::cout << "Geometry get weighted points: " << i++ << std::endl;
		if (!points) points = (*g_it)->get_weighted_points();
	}
	
	if (points) {
		// open file
		QFile f(file_name);
		if ( !f.open( QIODevice::WriteOnly ) ) {
			error(tr("File `%1' could not be open for writing!").arg(file_name));
			statusBar()->showMessage(tr("Error in saving %1").arg(short_file_name));
			return;
		}

		msg(tr("Saving %1...").arg(file_name));
		QTextStream fs( &f );
#ifdef MESECINA_3D
		fs << tr("WOFF %1 0 0\n").arg((uint)points->size());
		std::list<Point4D>::iterator p_it, p_end = points->end();
#else
		fs << tr("WPOFF %1 0 0\n").arg((uint)points->size());
		std::list<Point3D>::iterator p_it, p_end = points->end();
#endif
		for (p_it = points->begin(); p_it != p_end; p_it++)
#ifdef MESECINA_3D
			fs << tr("%1 %2 %3 %4\n").arg(p_it->x, 0, 'f', 9).arg(p_it->y, 0, 'f', 9).arg(p_it->z, 0, 'f', 9).arg(sqrt(p_it->w), 0, 'f', 9);
#else
			fs << tr("%1 %2 %3\n").arg(p_it->x, 0, 'f', 9).arg(p_it->y, 0, 'f', 9).arg(p_it->z, 0, 'f', 9);
#endif
		f.close();
		msg(tr("%1 vertices written in %2").arg((uint)points->size()).arg(file_name));
		set_current_file(file_name);
		statusBar()->showMessage((tr("%1 saved").arg(short_file_name)),3000);
	}
}

void Main_window::save_file(QString file_name) {
#ifndef MESECINA_3D
	std::list<QPointF>* points = (*geometries.begin())->get_points();
#else
	std::list<Point3D>* points = (*geometries.begin())->get_points();
#endif
	if (points==0 || points->size() == 0) return;

	current_file_name = file_name;
	QString short_file_name = current_file_name.right(current_file_name.length() - current_file_name.lastIndexOf("/") - 1);
	setWindowTitle(tr("%1 - %2").arg(APPLICATION_NAME).arg(short_file_name));


	
	// open file
	QFile f(current_file_name);
	if ( !f.open( QIODevice::WriteOnly ) ) {
		error(tr("File `%1' could not be open for writing!").arg(current_file_name));
		statusBar()->showMessage(tr("Error in saving %1").arg(short_file_name));
		return;
	}

	msg(tr("Saving %1...").arg(current_file_name));
	QTextStream fs( &f );
#ifndef MESECINA_3D
	fs << tr("POFF %1 0 0\n").arg((uint)points->size());
	std::list<QPointF>::iterator p_it, p_end = points->end();
	for (p_it = points->begin(); p_it != p_end; p_it++)
		fs << tr("%1 %2\n").arg(p_it->x(), 0, 'f', 3).arg(p_it->y(), 0, 'f', 3);
#else
	fs << tr("OFF %1 0 0\n").arg((uint)points->size());
	std::list<Point3D>::iterator p_it, p_end = points->end();
	for (p_it = points->begin(); p_it != p_end; p_it++)
		fs << tr("%1 %2 %3\n").arg(p_it->x, 0, 'f', 3).arg(p_it->y, 0, 'f', 3).arg(p_it->z, 0, 'f', 3);
#endif
	f.close();
	msg(tr("%1 vertices written in %2").arg((uint)points->size()).arg(current_file_name));
	set_current_file(file_name);
	statusBar()->showMessage((tr("%1 saved").arg(short_file_name)),3000);
}

void Main_window::load_file(QString file_name) {
	// open file
	QFile f(file_name);
	if ( !f.open( QIODevice::ReadOnly ) ) {
		error(tr("File `%1' could not be open for reading!").arg(file_name));
		statusBar()->showMessage(tr("Error in loading %1").arg(file_name));
		return;
	}

	//clean up current state and add geometries
	set_geometries();

	msg(tr("Loading %1...").arg(current_file_name));
	statistics_dialog->time_widget->clear();
	QTextStream fs( &f );
	QString head; fs >> head;
#ifdef MESECINA_3D
	QString extension = "OFF";
#else
	QString extension = "POFF";
#endif
	if (QString::compare(head, extension)) {
		error(tr("File `%1' has no correct header!").arg(file_name));
		statusBar()->showMessage(tr("Error in loading %1").arg(file_name));					
		return;
	}	

	int n, dummy; double x, y;
#ifdef MESECINA_3D
	double z;
#endif
	fs >> n; fs >> dummy; fs >> dummy;

#ifdef MESECINA_3D
	std::list<Point3D> points;
	for (int i=0; i<n && !fs.atEnd(); i++) {
		fs >> x;
		fs >> y;
		fs >> z;
		points.push_back(Point3D(x,y,z));
	}
#else 
	std::list<QPointF> points;
	for (int i=0; i<n && !fs.atEnd(); i++) {
		fs >> x;
		fs >> y;
		points.push_back(QPointF(x,y));
	}
#endif
	f.close();

	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it != g_end; g_it++) {
		Geometry *g = *g_it;
		g->add_points(&points);
//		g->load_generic_file(file_name.toStdString());
	}
	msg(tr("%1 vertices loaded from %2").arg((uint)points.size()).arg(file_name));
	statusBar()->showMessage((tr("%1 loaded").arg(file_name)),3000);
}

void Main_window::load_balls(QString file_name) {
	// open file
	QFile f(file_name);
	if ( !f.open( QIODevice::ReadOnly ) ) {
		error(tr("File `%1' could not be open for reading!").arg(file_name));
		statusBar()->showMessage(tr("Error in loading %1").arg(file_name));
		return;
	}

	//clean up current state and add geometries
	set_geometries();


	msg(tr("Loading %1...").arg(file_name));
	statistics_dialog->time_widget->clear();
	QTextStream fs( &f );
	QString head; fs >> head;
	double perturb_w = Application_settings::get_double_setting("ball-loading-perturb");
	if (perturb_w==0) {
		std::cout << LOG_ERROR << "ball-loading-perturb value is 0, we set it to 1" << std::endl;
		Application_settings::set_setting("ball-loading-perturb", 1);
		perturb_w = 1;
	}
#ifdef MESECINA_3D
	QString extension = "WOFF";
#else
	QString extension = "WPOFF";

#endif
	if (QString::compare(head, extension)) {
		error(tr("File `%1' has no correct header!").arg(current_file_name));
		statusBar()->showMessage(tr("Error in loading %1").arg(file_name));					
		return;
	}	

	int n, dummy; double x, y, w;
#ifdef MESECINA_3D
	double z;
#endif
	fs >> n; fs >> dummy; fs >> dummy;

#ifdef MESECINA_3D
	std::list<Point4D> points;
	for (int i=0; i<n && !fs.atEnd(); i++) {
		fs >> x;
		fs >> y;
		fs >> z;
		fs >> w;
		w = w * perturb_w;
		w = w * w;
		points.push_back(Point4D(x,y,z,w));
	}
#else 
	std::list<Point3D> points;
	for (int i=0; i<n && !fs.atEnd(); i++) {
		fs >> x;
		fs >> y;
		fs >> w;
		w = w * perturb_w;
//		w = w * w;
		points.push_back(Point3D(x,y,w));
	}
#endif
	f.close();

//#ifdef MESECINA_3D
	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it != g_end; g_it++) {
		Geometry *g = *g_it;
		g->add_weighted_points(&points);
	}

//#endif
	msg(tr("%1 balls loaded from %2").arg((uint)points.size()).arg(file_name));
	statusBar()->showMessage((tr("%1 loaded").arg(file_name)),3000);

//	widget->repaintGL();
}

void Main_window::show_statistics(bool checked) {
	settings->setValue("show-statistics",checked);
	statistics_action->setChecked(checked);
	if (checked) {
		statistics_dialog->resize(settings->value("statistics/size", QSize(200,200)).toSize());
		statistics_dialog->move(settings->value("statistics/pos", QPoint(100,100)).toPoint());
		statistics_dialog->show();
		statistics_dialog->resize(settings->value("statistics/size", QSize(200,200)).toSize());
		statistics_dialog->move(settings->value("statistics/pos", QPoint(100,100)).toPoint());
		if (settings->contains("statistics/splitter"))
			statistics_dialog->splitter->restoreState(settings->value("statistics/splitter").toByteArray());
		layers_changed(QModelIndex(),QModelIndex());
		update_stats_if_needed();
//		this->setFocus();
	}
	else {
		if (!statistics_dialog->isHidden()) {
			settings->setValue("statistics/size", statistics_dialog->size());
			settings->setValue("statistics/pos", statistics_dialog->pos());
			settings->setValue("statistics/splitter", statistics_dialog->splitter->saveState());
			statistics_dialog->hide();
		}
	}
}

void Main_window::show_help(bool checked) {
	settings->setValue("show-help",checked);
	help_action->setChecked(checked);
	if (checked) {
		help_dialog->resize(settings->value("help/size", QSize(500,700)).toSize());
		help_dialog->move(settings->value("help/pos", QPoint(100,100)).toPoint());
		help_dialog->show();
//		help_dialog->resize(settings->value("help/size", QSize(500,700)).toSize());
		help_dialog->move(settings->value("help/pos", QPoint(100,100)).toPoint());
	}
	else {
		if (!help_dialog->isHidden()) {
			settings->setValue("help/size", help_dialog->size());
			settings->setValue("help/pos", help_dialog->pos());
			help_dialog->hide();
		}
	}
}

void Main_window::show_settings(bool checked) {
	settings->setValue("show-settings",checked);
	settings_action->setChecked(checked);
	if (checked) {
		settings_dialog->resize(settings->value("settings/size", QSize(500,700)).toSize());
		settings_dialog->move(settings->value("settings/pos", QPoint(100,100)).toPoint());
		settings_dialog->show();
//		settings_dialog->resize(settings->value("help/size", QSize(500,700)).toSize());
		settings_dialog->move(settings->value("settings/pos", QPoint(100,100)).toPoint());
	}
	else {
		if (!settings_dialog->isHidden()) {
			settings->setValue("settings/size", settings_dialog->size());
			settings->setValue("settings/pos", settings_dialog->pos());
			settings_dialog->hide();
		}
	}
}

void Main_window::update_settings() {
	if (settings->value("show-settings").toBool()) {
		settings_dialog->settings_list_widget->process_filter();
	}
}


void Main_window::update_stats_if_needed() {
//	std::cout << "Update stats" << std::endl;
	int count;
	if (!statistics_dialog->isHidden() && needs_stats_update) {
		statistics_dialog->statistics_widget->clear();
		//for (int i=0; i<layer_list_model->non_filtered_count(); i++) {
		//	Managable_layer* l = layer_list_model->get_layer(i);
		//	if (l->has_valid_cache())
		//		for (int j=0; j<l->get_variable_count(); j++)
		//			statistics_dialog->statistics_widget->set_variable(l->get_variable(j), false);
		//}
		std::list<Geometry*>::iterator g_it, g_end = geometries.end(); count=0;
//		std::cout << LOG_BLUE << "Geometries size: " << geometries.size() << std::endl;
		for (g_it = geometries.begin(); g_it!=g_end; ++g_it) {
			Geometry *g = *g_it;
			for (unsigned j=0; j<g->app_variables_size(); j++) {
				statistics_dialog->statistics_widget->set_variable(g->app_variables(j), false); count++;
//				std::cout << g->app_variables(j).first.toStdString() << std::endl;
			}
		}
		needs_stats_update = false;
		statistics_dialog->statistics_widget->process_filter();
	}
//	std::cout <<  LOG_BLUE << "number of stat calls: " << count << std::endl;
}

void Main_window::layers_changed(const QModelIndex &, const QModelIndex &) {
	needs_stats_update = true;
}

void Main_window::update_stats() {
	needs_stats_update = true;
	update_stats_if_needed();
}

QString Main_window::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

void Main_window::set_current_file(const QString &fileName)
{
	current_file_name = fileName;
	if (current_file_name.isEmpty())
		setWindowTitle(tr(APPLICATION_NAME));
	else
		setWindowTitle(tr("%1 - %2").arg(tr(APPLICATION_NAME))
		.arg(strippedName(current_file_name)));

	QStringList files = settings->value("recent-file-list").toStringList();
	files.removeAll(fileName);
	files.prepend(fileName);
	while (files.size() > MaxRecentFiles)
		files.removeLast();

	settings->setValue("recent-file-list", files);

	update_recent_file_actions();
}

void Main_window::update_recent_file_actions()
{

	QStringList files = settings->value("recent-file-list").toStringList();

	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
		recent_file_actions[i]->setText(text);
		recent_file_actions[i]->setData(files[i]);
		recent_file_actions[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
		recent_file_actions[j]->setVisible(false);

	separator_action->setVisible(numRecentFiles > 0);
	exit_action->setVisible(true);

}

void Main_window::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action) {
		load_generic_file(action->data().toString());
	}
}

void Main_window::change_geometry_activation() {
	QAction *action = qobject_cast<QAction *>(sender());
	if (action) {
//		std::cout << " a geometry menu has been clicked " << action->text().toStdString() << std::endl;
		QString settings_name = tr("geometry-selected-%1").arg(action->text());
		settings->setValue(settings_name, action->isChecked());

		insure_geometry_dependencies(action);
	}

	update_selected_geometries();
}

void Main_window::insure_geometry_dependencies(const QAction* action) {
	if (action->isChecked()) {
		std::list<QAction*>::iterator a_it, a_end = geometry_actions.end();
		for (a_it = geometry_actions.begin(); a_it != a_end; a_it++) {
			QAction* a = *a_it;

			// activate geometry if it depends on
			std::list<std::string> dn = Geometry_factory::depends_on(action->text().toStdString());
			std::list<std::string>::iterator n_it, n_end = dn.end();
			for (n_it = dn.begin(); n_it!=n_end; ++n_it) {
				if (a->text().toStdString() == *n_it && !a->isChecked()) {
					a->setChecked(true);
					QString settings_name = tr("geometry-selected-%1").arg(a->text());
					settings->setValue(settings_name, a->isChecked());
					insure_geometry_dependencies(a);
				}
			}

			// deactivate geometry if conflicting
			std::list<std::string> cn = Geometry_factory::conflicts_with(action->text().toStdString());
			n_it, n_end = cn.end();
			for (n_it = cn.begin(); n_it!=n_end; ++n_it) {
				if (a->text().toStdString() == *n_it && a->isChecked()) {
					a->setChecked(false);
					QString settings_name = tr("geometry-selected-%1").arg(a->text());
					settings->setValue(settings_name, a->isChecked());
					insure_geometry_dependencies(a);
				}
			}

		}
	}
	if (!action->isChecked()) {
		std::list<QAction*>::iterator a_it, a_end = geometry_actions.end();
		for (a_it = geometry_actions.begin(); a_it != a_end; a_it++) {
			QAction* a = *a_it;

			// deactivate geometry if it depends on
			std::list<std::string> dn = Geometry_factory::depends_on(a->text().toStdString());
			std::list<std::string>::iterator n_it, n_end = dn.end();
			for (n_it = dn.begin(); n_it!=n_end; ++n_it) {
				if (action->text().toStdString() == *n_it && a->isChecked()) {
					a->setChecked(false);
					QString settings_name = tr("geometry-selected-%1").arg(a->text());
					settings->setValue(settings_name, a->isChecked());
					insure_geometry_dependencies(a);
				}
			}

		}
	}
}


void Main_window::application_settings_changed(const QString& settings_name) {
//	std::cout << "Main_window received " << settings_name.toStdString() << " setting changed!" << std::endl;
	// send the message to the layers;
	widget->application_settings_changed(settings_name);

	// send the message to the geometries
	std::list<Geometry*>::iterator g_it, g_end = geometries.end();
	for (g_it = geometries.begin(); g_it != g_end; g_it++) {
		Geometry *g = *g_it;
		g->application_settings_changed(settings_name);
	}	

	// repaint statistics
	needs_stats_update = true;
	update_stats_if_needed();
}
