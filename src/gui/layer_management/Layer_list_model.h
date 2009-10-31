/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Layer_list_model.h 390 2008-12-14 19:25:13Z miklosb $
 */

#ifndef LAYER_LIST_MODEL_H
#define LAYER_LIST_MODEL_H

#include <gui/app/Managable_layer.h>
#include <gui/layer_management/Bundle.h>
#include <gui/layer_management/Popup_slider_frame.h>
#include <gui/layer_management/Popup_scalar_controls_frame.h>
#include <gui/layer_management/Popup_color_map_frame.h>

#include <QtCore/QAbstractListModel>
#include <QtCore/QSettings>
#include <QColorDialog>
#include <vector>


class Layer_list_model : public QAbstractListModel
{
	Q_OBJECT

public:
	Layer_list_model(QObject *parent = 0);
	~Layer_list_model();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int non_filtered_count() const;
	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value,
		int role = Qt::EditRole);
	void clear();

	Qt::ItemFlags flags(const QModelIndex &index) const;
	Qt::DropActions supportedDropActions() const;
	QStringList mimeTypes() const;
	QMimeData *mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData(const QMimeData *data,
		Qt::DropAction action, int row, int column, const QModelIndex &parent);

	void add_layer(Managable_layer* layer);
	Managable_layer *get_layer(int i);

	Bundle create_bundle_from_current(const QString& name);
	bool import_from_bundle(Bundle& bundle);
	bool first_time_color_dialog;

public slots:
	void context_menu(QMouseEvent * event, const QModelIndex &index, QWidget *parent);
	void mouse_tracker(QMouseEvent* event);
	void filter_apply(const QString& text, bool active);
	void apply_set_active();
	void color_changed(const QColor& color);

private slots:
	void point_slider_changed(int value);
	void line_slider_changed(int value);
	void transparency_slider_changed(int value);
	void shininess_slider_changed(int value);
	void width_slider_changed(int value);
	void scalar_min_slider_changed(int value);
	void scalar_max_slider_changed(int value);
	void scalar_range_changed();
	void scalar_color_map_changed(const QString & name);
//	void scalar_color_map_clicked(const QString & name);
	void scalar_color_image_clicked();
	void fix_colormap_toggled(bool checked);

signals:
	void widget_needs_repaint();
	void layers_reordered();


private:
	QColorDialog* color_dialog;
	std::vector<Managable_layer*> layers;
	std::vector<Managable_layer*> filtered_layers;
	void swap_in_layers(int i, int j);
	void create_menu_widgets(QWidget *parent);

	QModelIndex temp_index;
	QSettings settings;
	
	Managable_layer *l;
	QMenu* menu;
	QAction* color;
	QAction* fix_colormap;
	//QAction* ambient;
	//QAction* diffuse;
	//QAction* specular;
	//QAction* emission;
	QAction* shininess;
	QAction* image;
	QAction* copy_range;
	QAction* paste_range;
	QAction* skip_clipping_planes;
	QAction* no_display_list;
	QAction *user_action_1, *user_action_2, *user_action_3, *user_action_4, *user_action_5;
	Popup_slider_frame* point_size_slider; QMenu* point_size_slider_menu;
	Popup_slider_frame* line_width_slider; QMenu* line_width_slider_menu;
	Popup_slider_frame* transparency_slider; QMenu* transparency_slider_menu;
	Popup_slider_frame* shininess_slider; QMenu* shininess_slider_menu;
	Popup_slider_frame* width_slider; QMenu* width_slider_menu;
	Popup_scalar_controls_frame* scalar_popup; QMenu* scalar_popup_menu;
	Popup_color_map_frame* color_map_popup; QMenu* color_map_popup_menu;
	QPoint last_mouse_position;

	bool repaint_on_set_data;
	QString filter_text;
	bool show_only_active;
};

#endif //LAYER_LIST_MODEL_H