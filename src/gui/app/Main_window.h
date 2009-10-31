/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Main_window.h 737 2009-05-16 15:40:46Z miklosb $
 */

#ifndef MESECINA_MAIN_WINDOW_H
#define MESECINA_MAIN_WINDOW_H

#ifdef MESECINA_3D
#include <gui/gl_widget_3/GL_cutting_plane_layer_3.h>
#include <gui/gl_widget_3/GL_selection_layer_3.h>
#include <gui/gl_widget_3/GL_widget_3.h>
#else
#include <gui/gl_widget_2/GL_selection_layer_2.h>
#include <gui/gl_widget_2/GL_insert_point_layer_2.h>
#include <gui/gl_widget_2/GL_insert_ball_layer_2.h>
#include <gui/gl_widget_2/GL_image_layer_2.h>
#endif


#include <gui/gl_widget/Visualization_widget.h>
#include <gui/app/Logger.h>
#include <gui/layer_management/Layer_list_model.h>
#include <gui/layer_management/Layer_list_widget.h>
#include <gui/layer_management/Bundle_list_model.h>
#include <gui/app/Record_toolbar.h>
#include <gui/app/Input_mode_toolbar.h>
#include <gui/app/Statistics_dialog.h>
#include <gui/app/Settings_dialog.h>
#include <gui/app/Help_browser_dialog.h>
#include <gui/app/Evolution_slider_toolbar.h>

#include <QtCore/QSettings>
#include <QtGui/QMainWindow>
#include <QtGui/QCloseEvent>
#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>

#include <string>
#include <queue>

class Geometry;
//class Computation_thread;

class Main_window : public QMainWindow {
	Q_OBJECT

public:
	Main_window(int x, int y, QString app_name);
	~Main_window();

	Logger* logger;

	QMenu *createPopupMenu ()  { return 0; } //to hide all context menus

	std::list<Geometry*>* clone_geometries();
	std::list<Geometry*>* clone_and_bind_geometries();
	void set_geometries(std::list<Geometry*> *new_geometries = 0);

public slots:
	void debug(QString msg);
	void msg(QString msg);
	void msg(QString msg, int level);
	void error(QString msg);
	void layers_changed(const QModelIndex &, const QModelIndex &);
	void update_stats_if_needed();
	void update_stats();
	void* get_shared_structure(const std::string& name);
	void shared_structure_changed(const std::string& name);
	void do_widget_repaint();


private slots:
	//void print();
	void openRecentFile();


	QString generic_file_filter_string();
	QString generic_one_file_filter_string(const QString &extension);
	QString generic_current_file_filter_string();
	void load_generic_file();
	void load_generic_file(const QString &file_name);
	void save_generic_file();
	void save_as_generic_file();
	void save_generic_file(const QString &file_name);

	void load_file(QString file_name);
	void save_file(QString file_name);

	void save_balls(QString file_name);
	void load_balls(QString file_name);

	void print();
    void closeEvent( QCloseEvent* );
	void restart_geometry();
	void export_image();
	void export_next_image();

	void record_status_change(bool checked);
	void ask_recording_directory();
#ifndef MESECINA_3D
	void input_mode_selected(Input_mode im);
#endif
	void set_png_recording();
	void set_pdf_recording();
	void set_ps_recording();

	//void set_double_kernel();
	//void set_coreexpr_kernel();

	void change_geometry_activation();

	void show_statistics(bool);
	void show_help(bool);
	void show_settings(bool checked);
	void update_settings();
	void application_settings_changed(const QString& settings_name);


	void about();

	void bundle_text_changed ( const QString & text);
	void save_bundle();
	void save_bundle(bool);
	void export_bundles();
	void import_bundles();
	void clear_bundles();
	void remove_bundle(int);
	void export_bundle(int);
	void reorder_layers_on_widget();
	void activate_selected_bundle(const QItemSelection & selected, const QItemSelection & deselected);
	void save_bundle_list();
	void gl_widget_resized(const QRect& size);
	void mouse_moved(const QPointF& pos);
#ifndef MESECINA_3D
	void point_created(double x, double y);
	void points_created(std::list<QPointF>* points);
	void ball_created(Point3D);
	void balls_created(std::list<Point3D>* points);
#endif

	void points_modified();

private:
	void insure_geometry_dependencies(const QAction* action);
	void export_image(QString image_filepath);
	void print(QString file_name);

	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);

	void add_widgets();
	void add_layers();
//	void reset_evolutions();
	void reset_evolutions_to_current_geometries();
	void add_menu();
	void add_toolbars();
	void create_actions();
	QString get_next_image_filname();
	void update_record_format_menus();

	void update_selected_geometries();
//	void update_kernel_menus();

    void set_current_file(const QString &fileName);
    void update_recent_file_actions();
    QString strippedName(const QString &fullFileName);


	QLabel* coord_label;
	QPushButton* load_view;
	QPushButton* save_view;
	QProgressBar* progress_bar;
	QSettings* settings;
	QString last_edited_bundle_name;
	QStringList generic_file_types;
	QStringList generic_file_description;

#ifdef MESECINA_3D
	GL_widget_3* widget;
#else
	GL_widget_2* widget;
	Input_mode_toolbar* input_toolbar;
#endif
//	Computation_thread* computation_thread;
	
	QString current_file_name;


    enum { MaxRecentFiles = 9 };


	QMenu* file_menu;
	QAction *new_action;
	QAction *save_action;
	QAction *save_as_action;
	QAction *load_action;

	QAction *print_action;
	QAction *separator_action;
    QAction *recent_file_actions[MaxRecentFiles];
	QAction *exit_action;

	QMenu* bundles_menu;
	QAction *export_bundles_action;
	QAction *import_bundles_action;
	QAction *clear_all_bundles_action;


	QMenu* options_menu;

	QMenu* record_menu;
	QAction *export_image_action;
	QAction *snapshot_action;
	QAction *record_action;
	QAction *record_directory_action;
	QMenu *record_format_menu;
	QAction *record_png_action;
	QAction *record_pdf_action;
	QAction *record_ps_action;

	QMenu* geometries_menu;
	std::list<QAction*> geometry_actions;

	QMenu* windows_menu;
	QAction* statistics_action;
	QAction* settings_action;
	QAction* help_action;

	QMenu* help_menu;	
	QAction *about_action;

	Record_toolbar* record_toolbar;
	QString record_directory;
	QString record_format;
	QString kernel;

	Evolution_slider_toolbar* evolutions_toolbar;

	QSplitter* v_log_splitter;
	QSplitter* h_splitter;
	QSplitter* v_layer_splitter;

	Statistics_dialog* statistics_dialog;
	Settings_dialog* settings_dialog;
	Help_browser_dialog* help_dialog;

	Layer_list_model *layer_list_model;
	Layer_list_widget *layer_list_widget;
	Bundle_list_model *bundle_list_model;


#ifdef MESECINA_3D
	GL_cutting_plane_layer_3* cutting_plane_layer;
	GL_selection_layer_3* selection_layer;
#else
	GL_insert_point_layer_2* insert_point_layer;
	GL_insert_ball_layer_2* insert_ball_layer;
	GL_selection_layer_2* selection_layer;
	GL_image_layer_2* image_layer;
#endif


	std::list<Geometry*> geometries;
	std::list<std::string> offered_structures;
	std::list<Geometry*> offering_geometries;
	void create_new_geometries();

	std::queue<QImage*> frames;
	bool needs_stats_update;
};

#endif //MESECINA_MAIN_WINDOW_H