/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Layer_list_model.cpp 737 2009-05-16 15:40:46Z miklosb $
 */

#include <QtCore/QStringList>
#include <QtCore/QMimeData>
#include <QtCore/QSettings>
#include <QtGui/QColor>
#include <QtGui/QColorDialog>
#include <QtGui/QFontMetrics>
#include <QtGui/QFileDialog>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QSound>
#include <iostream>
#include <QWidgetAction>
#include <QSettings>

#include <gui/app/static/Color_map_factory.h>
#include <gui/layer_management/Layer_list_model.h>
#include <constants.h>
#include <gui/app/static/Application_settings.h>

#ifdef MESECINA_3D
#include <gui/gl_widget_3/GL_widget_3.h>
#else
#include <gui/gl_widget_2/GL_widget_2.h>
#endif

// uncomment this to invalidate cache of a layer for color change (ugly fix for color change bug in release mode)
//#define INVALIDATE_CACHE_AT_COLOR_CHANGE

Layer_list_model::Layer_list_model(QObject *parent) : QAbstractListModel(parent), repaint_on_set_data(true) {
	first_time_color_dialog = true;
	layers.clear();
	filtered_layers.clear();
	menu = 0;
	//ambient=0;
	//diffuse=0;
	//specular=0;
	//emission=0;
	shininess=0;
	//width=0;
	color = 0;
	fix_colormap = 0;
	//transparency = 0;
	image = 0;
	//point = 0;
	//line = 0;
	//scalar = 0;
	copy_range = 0;
	paste_range = 0;
	skip_clipping_planes = 0;
	no_display_list = 0;
	point_size_slider = line_width_slider = transparency_slider = shininess_slider = width_slider = 0;
	point_size_slider_menu = line_width_slider_menu = transparency_slider_menu = shininess_slider_menu = width_slider_menu = 0;
	scalar_popup = 0; scalar_popup_menu = 0;
	color_map_popup_menu = 0;
	color_map_popup = 0;
	user_action_1 = user_action_2 = user_action_3 = user_action_4 = user_action_5 = 0;
	filter_text = "";
	show_only_active = false;

	Application_settings::add_bool_setting("show-tooltips-for-layers", DEFAULT_TOOLTIPS_FOR_LAYERS);
	color_dialog = new QColorDialog(menu);
	color_dialog->setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::NoButtons);
	color_dialog->setWindowModality(Qt::ApplicationModal);
	connect(color_dialog, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(color_changed(const QColor& )));

}

Layer_list_model::~Layer_list_model() {
}

int Layer_list_model::rowCount(const QModelIndex &) const {
	return (int)filtered_layers.size();
}

int Layer_list_model::non_filtered_count() const {
	return (int)layers.size();
}


QVariant Layer_list_model::data(const QModelIndex &index, int role) const {
     if (!index.isValid())
         return QVariant();

     if (index.row() >= (int)filtered_layers.size())
         return QVariant();

	switch (role) {
		 case Qt::DisplayRole:
			 return filtered_layers[index.row()]->get_name();
			 break;
		 case Qt::CheckStateRole:
			 return filtered_layers[index.row()]->is_active() ? Qt::Checked : Qt::Unchecked;
			 break;
		 case Qt::UserRole:
			 return filtered_layers[index.row()]->symbol_brush();
			 break;
		 case 33:
			 return filtered_layers[index.row()]->has_valid_cache() ? 1 : 0;
		 case Qt::ToolTipRole:
//			 std::cout << "getting tooltip with name " << filtered_layers[index.row()]->get_name().toStdString() << " and tooltip " << filtered_layers[index.row()]->get_tooltip().toStdString() << std::endl;
			 if (settings.value("show-tooltips-for-layers", true).toBool() && filtered_layers[index.row()]->get_tooltip() != "") {
				 return filtered_layers[index.row()]->get_tooltip();
			 }
			else return QVariant();
		 default:
			 return QVariant();
	 }
}

Qt::ItemFlags Layer_list_model::flags(const QModelIndex &index) const {
     Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

     if (index.isValid())
         return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
     else
         return Qt::ItemIsDropEnabled | defaultFlags;
}

void Layer_list_model::apply_set_active() {
	filtered_layers[temp_index.row()]->set_active(true);
	if (!filtered_layers[temp_index.row()]->is_active()) {
		//the only way it is not active if the image is not there
		QSettings settings;
		QString file_name = QFileDialog::getOpenFileName(
			/*filtered_layers[index.row()]->widget,*/ 0,
			"Choose the path for the background image",
			settings.value("image-layer-filename").toString(),
			"Image (*.png)");
		if (file_name!="") {
			filtered_layers[temp_index.row()]->set_image_file_name(file_name);
			filtered_layers[temp_index.row()]->invalidate_cache();
			filtered_layers[temp_index.row()]->set_active(true);
		}
	}
	emit dataChanged(temp_index, temp_index);
	emit widget_needs_repaint();
}

bool Layer_list_model::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && role == Qt::CheckStateRole) {
		if (value == Qt::Checked) {
			temp_index = index;
//			QTimer::singleShot(0, this, SLOT(apply_set_active())); 
			apply_set_active();
			// do this trick to that we first process the user events and then
			// do eventual geometry processing with set_active. 
			// like this we can call the event loop in the Logger class and make sure
			// logs are updated real time and 
			if (QFile::exists(LAYER_IN_SOUND)) QSound::play(LAYER_IN_SOUND);
		} else {
			filtered_layers[index.row()]->set_active(value == Qt::Checked);
			emit dataChanged(index, index);
			emit widget_needs_repaint();
			if (QFile::exists(LAYER_OUT_SOUND)) QSound::play(LAYER_OUT_SOUND);
		}
		return true;
	}
	return false;
}


Qt::DropActions Layer_list_model::supportedDropActions() const {
//	if (filter_text != "") return Qt::IgnoreAction;
	return Qt::MoveAction;
}

QStringList Layer_list_model::mimeTypes() const {
	QStringList types;
	types << "application/mesecina.layer";
	return types;
}

QMimeData* Layer_list_model::mimeData(const QModelIndexList &indexes) const {
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	foreach (QModelIndex index, indexes) {
		if (index.isValid()) {
			int idx = index.row();
			stream << idx;
		}
	}

	mimeData->setData("application/mesecina.layer", encodedData);
	return mimeData;
}

bool Layer_list_model::dropMimeData(const QMimeData *data, Qt::DropAction action,
	int row, int column, const QModelIndex &parent) {

	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat("application/mesecina.layer"))
		return false;

	if (column > 0)
		return false;

	unsigned beginRow; 
	// adapt so that this beginRow is refering to the layer's beginRow although comes originally from the filtered_layers
	// and then the drag and drop works with filtering, as well.

	if (row != -1)
		beginRow = row;
	else if (parent.isValid())
		beginRow = parent.row();
	else
		beginRow = rowCount(QModelIndex());

	QByteArray encodedData = data->data("application/mesecina.layer");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);

	unsigned idx;
	stream >> idx;
	if (idx<beginRow && row == -1 && parent.isValid()) beginRow++;

	QString this_name = filtered_layers[idx]->get_name();

	for (unsigned i=0; i<layers.size(); i++) {
		if (layers[i]->get_name()== this_name) {
			idx = i;
			i = static_cast<int>(layers.size());
		}
	}

	if (beginRow < filtered_layers.size()) {
		QString this_name = filtered_layers[beginRow]->get_name();
		for (unsigned i=0; i<layers.size(); i++) {
			if (layers[i]->get_name()== this_name) {
				beginRow = i;
				i = static_cast<int>(layers.size());
			}
		}
	} else {
		beginRow = static_cast<int>(layers.size());
	}
	
	unsigned i;	std::vector<Managable_layer*>::iterator l_it = layers.begin();
	for (i=0; i != beginRow; l_it++, i++);
	layers.insert(l_it, layers[idx]);

	if (i<idx) idx++;
	l_it = layers.begin();
	for (i=0; i != idx; l_it++, i++);
	layers.erase(l_it);

	emit layers_reordered();
	filter_apply(filter_text, show_only_active);

	return true;
}


Bundle Layer_list_model::create_bundle_from_current(const QString& name) {
	Bundle b(name);
	std::vector<Managable_layer*>::iterator l_it = layers.begin(), l_end = layers.end();
	for (; l_it!=l_end; l_it++) {
		if (*l_it != 0)
			b.add_layer((*l_it)->get_name(), (*l_it)->saveState());
	}
	return b;
}

bool Layer_list_model::import_from_bundle(Bundle& bundle) {
	unsigned n = bundle.size();
	for (unsigned i = 0; i < n && i < layers.size(); i++) {
		unsigned layer_index = 0;
		QString layer_name = bundle.layer_name(i);
		QByteArray layer_state = bundle.layer_state(i);
		while (layer_index < layers.size() && layer_name != layers[layer_index]->get_name()) {
			layer_index++;
		}
		if (layer_index==layers.size()) continue;
		swap_in_layers(i, layer_index);
#ifdef INVALIDATE_CACHE_AT_COLOR_CHANGE
		layers[i]->invalidate_cache();/// \todo why do I need this for colors? it should work without invalidating with the display lists
#endif
		layers[i]->restoreState(bundle.layer_state(i));
	}
	// deactivate 
	for (unsigned i=0; i < layers.size(); i++) {
		bool found = false;
		QString name_from_layers = layers[i]->get_name();
		for (int j=0; j<bundle.size(); j++)
			if (name_from_layers == bundle.layer_name(j)) found = true;
		if (!found) layers[i]->set_active(false);
	}
	filter_apply(filter_text, show_only_active);
	emit dataChanged(QModelIndex(), QModelIndex());
	emit layers_reordered();
	return true;
}

void Layer_list_model::swap_in_layers(int i, int j) {
	if (i==j) return;
	Managable_layer *temp = layers[i];
	layers[i] = layers[j];
	layers[j] = temp;
}

Managable_layer *Layer_list_model::get_layer(int i) {
	return layers[i];
}

void Layer_list_model::add_layer(Managable_layer* layer) {
	std::vector<Managable_layer*>::iterator l_it, l_end = layers.end();
	for (l_it=layers.begin(); l_it!=l_end; l_it++)
		if (*l_it == layer) return;
	beginInsertRows(QModelIndex(), rowCount(), rowCount()+1);
	layers.push_back(layer);
//	filter_apply(filter_text);
	endInsertRows();
}

void Layer_list_model::point_slider_changed(int) {
	if (l == 0) return;
	l->set_point_size(point_size_slider->value_to_float());
	emit widget_needs_repaint();
}

void Layer_list_model::line_slider_changed(int) {
	if (l == 0) return;
	l->set_line_width(line_width_slider->value_to_float());
	emit widget_needs_repaint();
}

void Layer_list_model::transparency_slider_changed(int) {
	if (l == 0) return;
	QColor color = l->get_color();
	color.setAlphaF(transparency_slider->value_to_float());

	l->set_color(color);
	//std::cout << "Color set with transparency: " << transparency_slider->value_to_float() << std::endl;
	QColor ambient = l->get_ambient();
	ambient.setAlphaF(transparency_slider->value_to_float());
	QColor diffuse = l->get_diffuse();
	l->set_diffuse(diffuse);
	diffuse.setAlphaF(transparency_slider->value_to_float());
	QColor specular = l->get_specular();
	l->set_specular(specular);
	specular.setAlphaF(transparency_slider->value_to_float());
	QColor emission = l->get_emission();
	emission.setAlphaF(transparency_slider->value_to_float());
	l->set_emission(emission);
#ifdef INVALIDATE_CACHE_AT_COLOR_CHANGE
	l->invalidate_cache(); /// \todo why do I need this for colors? it should work without invalidating with the display lists
#endif
	if (l->has_property(Managable_layer::SCALAR_EDITABLE)) {
		l->invalidate_cache();
		l->has_new_range = false;
	}
	emit widget_needs_repaint();
}

void Layer_list_model::shininess_slider_changed(int) {
	if (l == 0) return;
	l->set_shininess(shininess_slider->value_to_float());
	emit widget_needs_repaint();
}

void Layer_list_model::width_slider_changed(int) {
	if (l == 0) return;
//	std::cout << "width set to: " << width_slider->value_to_float() << std::endl;
	l->set_width(width_slider->value_to_float());
	emit widget_needs_repaint();
}

void Layer_list_model::fix_colormap_toggled(bool checked) {
	if (l == 0) return;
	l->set_need_rescale_colormap(!checked);

}


void Layer_list_model::scalar_min_slider_changed(int ) {
	if (l == 0) return;
	l->scalar_min = scalar_popup->min_value_to_float();
	l->invalidate_cache();
	l->has_new_range = false;
	emit widget_needs_repaint();
}

void Layer_list_model::scalar_max_slider_changed(int ) {
	if (l == 0) return;
	l->scalar_max = scalar_popup->max_value_to_float();
	l->invalidate_cache();
	l->has_new_range = false;
	emit widget_needs_repaint();
}

void Layer_list_model::scalar_range_changed() {
	if (l == 0) return;
	l->scalar_max = scalar_popup->max_value_to_float();
	l->scalar_min = scalar_popup->min_value_to_float();
//	std::cout << "scalar range changed called for enter" << std::endl;
	l->invalidate_cache();
	l->has_new_range = false;
	emit widget_needs_repaint();
}

void Layer_list_model::scalar_color_map_changed(const QString & name) {
	if (l == 0) return;
	Color_map* cm = Color_map_factory::get_color_map(l->color_map);
	float min, max; min=max=0;
	if (cm!=0) {
		min = cm->get_scale_min();
		max = cm->get_scale_max();
	}
//	std::cout << LOG_BLUE << "Old colormap " << l->color_map.toStdString() << " min,max: " << min << "," << max << std::endl;
	l->color_map = name;
	cm = Color_map_factory::get_color_map(l->color_map);
	if (cm!=0) cm->set_scale_range(min,max);
	if (l->color_map == "Solid" || l->color_map == "") color_map_popup->set_pixmap(l->get_color());
	else color_map_popup->set_pixmap(Color_map_factory::get_color_map(name)->get_thumbnail());
	l->invalidate_cache();
	l->has_new_range = false;
	emit widget_needs_repaint();
}

void Layer_list_model::scalar_color_image_clicked() {
	if (l->color_map=="Solid" || l->color_map == "") {
		color_dialog->setCurrentColor(l->get_color().rgba());
		QSettings settings;
		color_dialog->move(settings.value("color_dialog_pos").toPoint());
		color_dialog->setVisible(true);
	}
}

void  Layer_list_model::color_changed(const QColor& new_color) {
	l->set_color(new_color);
	if (!first_time_color_dialog) {
		QSettings settings;
		settings.setValue("color_dialog_pos", color_dialog->pos());
//		std::cout << "position stored for color dialog: " << color_dialog->pos().x() << "," << color_dialog->pos().y() << std::endl;
	} else first_time_color_dialog = false;
	l->invalidate_cache();
	l->has_new_range = false;
	emit widget_needs_repaint();
}

//void Layer_list_model::scalar_color_map_clicked(const QString & ) {
//
////	color_map_popup->hide();
////	menu->hide();
//}

//void Layer_list_model::menu_action_hovered(QAction* action) {
//	/// \todo popup slider always on the left hand side... check for resolution and put it on the right/left side when needed
////	#ifdef Q_WS_WIN
//	QPoint menu_pos = menu->pos();
//	QFontMetrics fm(color->font());
//
//
//	if (action==point) {
//		menu_pos.setY((menu_pos.y()+ point_number * (fm.height()+6)));
//		point_size_slider->set_float_value(l->get_point_size());
//		point_size_slider->show();
//		//menu_pos.setX((menu_pos.x()+menu->width()));
//		menu_pos.setX((menu_pos.x()-point_size_slider->width()));
//		point_size_slider->move(menu_pos);
//		line_width_slider->hide();
//		transparency_slider->hide();
//		shininess_slider->hide();
//		width_slider->hide();
//		scalar_popup->hide();
////		color_map_popup->hide();
//	} else if (action==line) {
//		menu_pos.setY((menu_pos.y()+ line_number * (fm.height()+6)));
//		line_width_slider->set_float_value(l->get_line_width());
//		line_width_slider->show();
//		//menu_pos.setX((menu_pos.x()+menu->width()));
//		menu_pos.setX((menu_pos.x()-line_width_slider->width()));
//		line_width_slider->move(menu_pos);
//		point_size_slider->hide();
//		transparency_slider->hide();
//		shininess_slider->hide();
//		width_slider->hide();
//		scalar_popup->hide();
////		color_map_popup->hide();
//	} else if (action==transparency) {
//		menu_pos.setY((menu_pos.y()+ transparency_number * (fm.height()+6)));
//		transparency_slider->set_float_value(l->get_color().alphaF());
//		transparency_slider->show();
//		//menu_pos.setX((menu_pos.x()+menu->width()));
//		menu_pos.setX((menu_pos.x()-transparency_slider->width()));
//		transparency_slider->move(menu_pos);
//		point_size_slider->hide();
//		line_width_slider->hide();
//		shininess_slider->hide();
//		width_slider->hide();
//		scalar_popup->hide();
////		color_map_popup->hide();
//	} else if (action==shininess) {
//		menu_pos.setY((menu_pos.y()+ shininess_number * (fm.height()+6)));
//		shininess_slider->set_float_value(l->get_shininess());
//		shininess_slider->show();
//		//menu_pos.setX((menu_pos.x()+menu->width()));
//		menu_pos.setX((menu_pos.x()-shininess_slider->width()));
//		shininess_slider->move(menu_pos);
//		transparency_slider->hide();
//		point_size_slider->hide();
//		line_width_slider->hide();
//		width_slider->hide();
//		scalar_popup->hide();
////		color_map_popup->hide();
//	} else if (action==width) {
//		menu_pos.setY((menu_pos.y()+ width_number * (fm.height()+6)));
//		width_slider->set_float_value(l->get_width());
//		width_slider->show();
//		//menu_pos.setX((menu_pos.x()+menu->width()));
//		menu_pos.setX((menu_pos.x()-width_slider->width()));
//		width_slider->move(menu_pos);
//		transparency_slider->hide();
//		point_size_slider->hide();
//		line_width_slider->hide();
//		shininess_slider->hide();
//		scalar_popup->hide();
////		color_map_popup->hide();
//	} else if (action==scalar) {
//		menu_pos.setY((menu_pos.y()+ scalar_number * (fm.height()+6)));
//		scalar_popup->set_range(l->scalar_range_min, l->scalar_range_max);
//		scalar_popup->set_min_float_value(l->scalar_min);
//		scalar_popup->set_max_float_value(l->scalar_max);
//		scalar_popup->set_color_map(l->color_map);
//		scalar_popup->show();
//		menu_pos.setX((menu_pos.x()+menu->width()));
//		//menu_pos.setX((menu_pos.x()-scalar_popup->width()));
//		scalar_popup->move(menu_pos);
//		transparency_slider->hide();
//		point_size_slider->hide();
//		line_width_slider->hide();
//		shininess_slider->hide();
//		width_slider->hide();
////		color_map_popup->hide();
//	} else if (action==color && color_map_number >= 0) {
//		menu_pos.setY((menu_pos.y()+ color_map_number * (fm.height()+6)));
////		color_map_popup->show();
//		//menu_pos.setX((menu_pos.x()+menu->width()));
//	//	menu_pos.setX((menu_pos.x()-color_map_popup->width()));
//	//	color_map_popup->move(menu_pos);
//		transparency_slider->hide();
//		point_size_slider->hide();
//		line_width_slider->hide();
//		shininess_slider->hide();
//		width_slider->hide();
//		scalar_popup->hide();
//	} else {
//		line_width_slider->hide();
//		point_size_slider->hide();
//		transparency_slider->hide();
//		shininess_slider->hide();
//		width_slider->hide();
//		scalar_popup->hide();
////		color_map_popup->hide();
//	}
////	#endif //Q_WS_WIN
//
//}

void Layer_list_model::mouse_tracker(QMouseEvent* event) {
	last_mouse_position = event->pos();
}

void Layer_list_model::create_menu_widgets(QWidget *parent) {
	if (point_size_slider_menu == 0) {
		point_size_slider_menu = new QMenu("Point size");
	}
	if (point_size_slider == 0) {
		point_size_slider = new Popup_slider_frame(menu);
		point_size_slider->set_range(1,21);
		connect(point_size_slider->slider, SIGNAL(valueChanged(int)), this, SLOT(point_slider_changed(int)));
		point_size_slider_menu->addAction(point_size_slider);
	}
	if (line_width_slider_menu == 0) {
		line_width_slider_menu = new QMenu("Line width");
	}
	if (line_width_slider == 0) {
		line_width_slider = new Popup_slider_frame(menu);
		line_width_slider->set_range(1,10.5);
		connect(line_width_slider->slider, SIGNAL(valueChanged(int)), this, SLOT(line_slider_changed(int)));
		line_width_slider_menu->addAction(line_width_slider);
	}
	if (transparency_slider_menu == 0) {
		transparency_slider_menu = new QMenu("Opacity");
	}
	if (transparency_slider == 0) {
		transparency_slider = new Popup_slider_frame(menu);
		transparency_slider->set_range(0,1);
		connect(transparency_slider->slider, SIGNAL(valueChanged(int)), this, SLOT(transparency_slider_changed(int)));
		transparency_slider_menu->addAction(transparency_slider);
	}
	if (shininess_slider_menu == 0) {
		shininess_slider_menu = new QMenu("Specularity");
	}
	if (shininess_slider == 0) {
		shininess_slider = new Popup_slider_frame(menu);
		shininess_slider->set_range(0,1);
		connect(shininess_slider->slider, SIGNAL(valueChanged(int)), this, SLOT(shininess_slider_changed(int)));
		shininess_slider_menu->addAction(shininess_slider);
	}
	if (width_slider_menu == 0) {
		width_slider_menu = new QMenu("Width");
	}
	if (width_slider == 0) {
		width_slider = new Popup_slider_frame(menu);
		width_slider->set_range(0,1);
		connect(width_slider->slider, SIGNAL(valueChanged(int)), this, SLOT(width_slider_changed(int)));
		width_slider_menu->addAction(width_slider);
	}
	if (scalar_popup_menu == 0) {
		scalar_popup_menu = new QMenu("Specularity");
	}
	if (scalar_popup==0) {
		scalar_popup = new Popup_scalar_controls_frame(menu);
		scalar_popup->set_range(0,10);
		connect(scalar_popup->min_slider, SIGNAL(valueChanged(int)), this, SLOT(scalar_min_slider_changed(int)));
		connect(scalar_popup->max_slider, SIGNAL(valueChanged(int)), this, SLOT(scalar_max_slider_changed(int)));
		connect(scalar_popup->min_line_edit, SIGNAL(returnPressed()), this, SLOT(scalar_range_changed()));
		connect(scalar_popup->max_line_edit, SIGNAL(returnPressed()), this, SLOT(scalar_range_changed()));
		scalar_popup_menu->addAction(scalar_popup);

	//		connect(scalar_popup->color_map_combobox, SIGNAL(currentIndexChanged ( const QString &)), this, SLOT(scalar_color_map_changed(const QString &)));		
	}
	if (color_map_popup_menu==0) {
		color_map_popup_menu = new QMenu("Color");
	}
	if (color_map_popup==0) {
		color_map_popup = new Popup_color_map_frame(color_map_popup_menu);
		connect(color_map_popup->combobox, SIGNAL(highlighted ( const QString &)), this, SLOT(scalar_color_map_changed(const QString &)));		
//		connect(color_map_popup->combobox, SIGNAL(activated ( const QString &)), this, SLOT(scalar_color_map_clicked(const QString &)));
		connect(color_map_popup->image_place, SIGNAL(label_clicked ()), this, SLOT(scalar_color_image_clicked()));
		color_map_popup_menu->addAction(color_map_popup);
	}
	

	if (menu==0) { 
		menu = new QMenu(parent); 
		//menu->setMouseTracking(true); 
		//connect(menu, SIGNAL(hovered(QAction*)), this, SLOT(menu_action_hovered(QAction*)));
	}

	if (color==0) color = new QAction (tr("Color"), parent);
	if (fix_colormap==0) {
		fix_colormap = new QAction(tr("Fix colormap"), parent);
		fix_colormap->setCheckable(true);
		connect(fix_colormap, SIGNAL(toggled(bool)), this, SLOT(fix_colormap_toggled(bool)));
	}
	//if (ambient==0) ambient = new QAction (tr("Ambient"), parent);
	//if (diffuse==0) diffuse = new QAction (tr("Diffuse"), parent);
	//if (specular==0) specular = new QAction (tr("Specular"), parent);
	//if (emission==0) emission = new QAction (tr("Emission"), parent);
	if (shininess==0) shininess = new QAction (tr("Specularity"), parent);
	//if (width==0) width = new QAction( tr("Width"), parent);
	//if (scalar==0) scalar = new QAction( tr("Scalar"), parent);
	if (copy_range==0) copy_range = new QAction( tr("Copy range"), parent);
	if (paste_range==0) paste_range = new QAction( tr("Paste range"), parent);

	if (skip_clipping_planes==0) skip_clipping_planes = new QAction( tr("No clipping"), parent);
	skip_clipping_planes->setCheckable(true);

	if (no_display_list==0) no_display_list = new QAction( tr("No display list"), parent);
	no_display_list->setCheckable(true);

	if (user_action_1==0) user_action_1 = new QAction( tr("User action"), parent);
	if (user_action_2==0) user_action_2 = new QAction( tr("User action"), parent);
	if (user_action_3==0) user_action_3 = new QAction( tr("User action"), parent);
	if (user_action_4==0) user_action_4 = new QAction( tr("User action"), parent);
	if (user_action_5==0) user_action_5 = new QAction( tr("User action"), parent);


	if (image==0) image = new QAction(tr("Load image..."), parent);
	//if (point==0) point = new QAction(tr("Point size"), parent);
	//if (line==0) line = new QAction(tr("Line width"), parent);
//	if (transparency==0) transparency = new QAction(tr("Opacity"), parent);


}


void Layer_list_model::context_menu(QMouseEvent * event, const QModelIndex &index, QWidget *parent) {
	if (!index.isValid()) return;

	l = filtered_layers[index.row()];

	create_menu_widgets(parent);

	// set colormap data
	color_map_popup->set_color_map(l->color_map);
	if (l->color_map == "Solid" || l->color_map == "") color_map_popup->set_pixmap(l->get_color());
	else color_map_popup->set_pixmap(Color_map_factory::get_color_map(l->color_map)->get_thumbnail());

	// set pointmap data
	point_size_slider->set_float_value(l->get_point_size());
	// set linewidth data
	line_width_slider->set_float_value(l->get_line_width());
	// set transparency data
	transparency_slider->set_float_value(l->get_color().alphaF());
	// set specularity/shininess
	shininess_slider->set_float_value(l->get_shininess());
	// set width
	width_slider->set_float_value(l->get_width());
	// scalar popup
	scalar_popup->set_range(l->scalar_range_min, l->scalar_range_max);
	scalar_popup->set_min_float_value(l->scalar_min);
	scalar_popup->set_max_float_value(l->scalar_max);
	scalar_popup->set_color_map(l->color_map);

	menu->clear();



	if (l->has_property(Managable_layer::COLOR_EDITABLE) && !(l->has_property(Managable_layer::IMAGE_EDITABLE))) {
		if (l->has_property(Managable_layer::SCALAR_EDITABLE)) {
			menu->addMenu(color_map_popup_menu);
			menu->addAction(fix_colormap);
			fix_colormap->setChecked(!l->get_need_rescale_colormap());
		}
		else menu->addAction(color); 
		menu->addAction(no_display_list);
		no_display_list->setChecked(!l->get_use_display_list());
//		std::cout << "adding no display list to the layer, and layer's use_display list is: " << (l->get_use_display_list() ? "true" : "false") << std::endl;
#ifdef MESECINA_3D
//		menu->addAction(color); i++;
//		menu->addAction(ambient); i++;
//		menu->addAction(diffuse); i++;
//		menu->addAction(specular); i++;
//		menu->addAction(emission); i++;
		menu->addMenu(shininess_slider_menu); 
		menu->addAction(skip_clipping_planes);
		skip_clipping_planes->setChecked(l->get_skip_clipping_planes());
#endif
	}
	if (l->has_property(Managable_layer::COLOR_EDITABLE)) {
		menu->addMenu(transparency_slider_menu);
	}
	if (l->has_property(Managable_layer::LINE_WIDTH_EDITABLE)) {
		menu->addMenu(line_width_slider_menu);
	}
	if (l->has_property(Managable_layer::POINT_SIZE_EDITABLE)) {
		menu->addMenu(point_size_slider_menu);
	}
	if (l->has_property(Managable_layer::SCALAR_EDITABLE)) {
		scalar_popup_menu->setTitle(l->get_name_user_property(Managable_layer::SCALAR_EDITABLE));
		menu->addAction(copy_range);
		menu->addAction(paste_range);
		menu->addMenu(scalar_popup_menu);
	}
	if (l->has_property(Managable_layer::IMAGE_EDITABLE)) {
		menu->addAction(image);
	}
	if (l->has_property(Managable_layer::WIDTH_EDITABLE)) {
		menu->addMenu(width_slider_menu);
	}
	if (l->has_property(Managable_layer::USER_PROPERTY_1)) {
		user_action_1->setText(l->get_name_user_property(Managable_layer::USER_PROPERTY_1));
		menu->addAction(user_action_1);
	}
	if (l->has_property(Managable_layer::USER_PROPERTY_2)) {
		user_action_2->setText(l->get_name_user_property(Managable_layer::USER_PROPERTY_2));
		menu->addAction(user_action_2);
	}
	if (l->has_property(Managable_layer::USER_PROPERTY_3)) {
		user_action_3->setText(l->get_name_user_property(Managable_layer::USER_PROPERTY_3));
		menu->addAction(user_action_3);
	}
	if (l->has_property(Managable_layer::USER_PROPERTY_4)) {
		user_action_4->setText(l->get_name_user_property(Managable_layer::USER_PROPERTY_4));
		menu->addAction(user_action_4);
	}
	if (l->has_property(Managable_layer::USER_PROPERTY_5)) {
		user_action_5->setText(l->get_name_user_property(Managable_layer::USER_PROPERTY_5));
		menu->addAction(user_action_5);
	}

//	menu->setFocus();
	QAction* action = menu->exec(event->globalPos());
	if (action==color) {
		color_dialog->setCurrentColor(l->get_color().rgba());
		QSettings settings;
		color_dialog->setVisible(true);
		color_dialog->move(settings.value("color_dialog_pos").toPoint());
//		std::cout << "moved to " << settings.value("color_dialog_pos").toPoint().x() << ", " << settings.value("color_dialog_pos").toPoint().y() << std::endl;
	//} else if (action==ambient) {
	//	bool clicked_ok;
	//	QColor new_color;
	//	new_color.setRgba(QColorDialog::getRgba(l->get_ambient().rgba(), &clicked_ok, parent));
	//	if (clicked_ok) l->set_ambient(new_color);
	//} else if (action==diffuse) {
	//	bool clicked_ok;
	//	QColor new_color;
	//	new_color.setRgba(QColorDialog::getRgba(l->get_diffuse().rgba(), &clicked_ok, parent));
	//	if (clicked_ok) l->set_diffuse(new_color);
	//} else if (action==specular) {
	//	bool clicked_ok;
	//	QColor new_color;
	//	new_color.setRgba(QColorDialog::getRgba(l->get_specular().rgba(), &clicked_ok, parent));
	//	if (clicked_ok) l->set_specular(new_color);
	} else if (action == skip_clipping_planes) {
		l->set_skip_clipping_planes(skip_clipping_planes->isChecked());
		emit widget_needs_repaint(); 
	} else if (action == no_display_list) {
		l->set_use_display_list(!no_display_list->isChecked());
		emit widget_needs_repaint(); 
	} else if (action == copy_range) {
		QClipboard *clipboard = QApplication::clipboard();
		QString text = tr("%1 %2").arg(l->scalar_min).arg(l->scalar_max);
		clipboard->setText(text);
		std::cout << "Range: " << l->scalar_min <<","<< l->scalar_max <<" put on clipboard" << std::endl;
	} else if (action == paste_range) {
		QClipboard *clipboard = QApplication::clipboard();
		QStringList sl = clipboard->text().split(" ");
		double min, max; bool ok;
		if (sl.size() == 2) {
			min = sl[0].toDouble(&ok);
			if (ok) max = sl[1].toDouble(&ok);
		} else {
			std::cout << LOG_ERROR << "Clipboard does not contain a valid range, go to a layer with scalar values and copy the range" << std::endl;
			return;
		}
		if (ok) {
			l->scalar_min = min;
			l->scalar_max = max;
			std::cout << "Range: " << l->scalar_min <<","<< l->scalar_max <<" loaded from clipboard" << std::endl;
			l->invalidate_cache();
			l->has_new_range = false;
			emit widget_needs_repaint(); 
		} else {
			std::cout << LOG_ERROR << "Clipboard does not contain a valid range, go to a layer with scalar values and copy the range" << std::endl;
		}

	} else if (action == user_action_1) {
		l->execute_user_property(Managable_layer::USER_PROPERTY_1);
	} else if (action == user_action_2) {
		l->execute_user_property(Managable_layer::USER_PROPERTY_2);
	} else if (action == user_action_3) {
		l->execute_user_property(Managable_layer::USER_PROPERTY_3);
	} else if (action == user_action_4) {
		l->execute_user_property(Managable_layer::USER_PROPERTY_4);
	} else if (action == user_action_5) {
		l->execute_user_property(Managable_layer::USER_PROPERTY_5);
	//} else if (action==emission) {
	//	bool clicked_ok;
	//	QColor new_color;
	//	new_color.setRgba(QColorDialog::getRgba(l->get_emission().rgba(), &clicked_ok, parent));
	//	if (clicked_ok) l->set_emission(new_color);
	} else if (action==image) {
		QString file_name = QFileDialog::getOpenFileName(
			l->widget,
			"Choose the path for the background image",
			l->get_image_file_name(),
			"Image (*.png)");
		if (file_name!="") {
			l->set_image_file_name(file_name);
			l->invalidate_cache();
			l->widget->repaint();
			emit dataChanged(QModelIndex(), QModelIndex());
		} 
	} else {
//	#ifdef Q_WS_WIN
		//point_size_slider->hide();
		//line_width_slider->hide();
		//transparency_slider->hide();
		//shininess_slider->hide();
		//width_slider->hide();
		//scalar_popup->hide();
//		color_map_popup->hide();
/*	#else
		QPoint menu_pos = menu->pos();
		QFontMetrics fm(color->font());
		if (action==point) {
			menu_pos.setY((menu_pos.y()+ point_number * (fm.height()+6)));
			point_size_slider->set_float_value(l->get_point_size());
			point_size_slider->show();
			//menu_pos.setX((menu_pos.x()+menu->width()));
			menu_pos.setX((menu_pos.x()-point_size_slider->width()));
			point_size_slider->move(menu_pos);
			line_width_slider->hide();
			transparency_slider->hide();
		} else if (action==line) {
			menu_pos.setY((menu_pos.y()+ line_number * (fm.height()+6)));
			line_width_slider->set_float_value(l->get_line_width());
			line_width_slider->show();
			//menu_pos.setX((menu_pos.x()+menu->width()));
			menu_pos.setX((menu_pos.x()-line_width_slider->width()));
			line_width_slider->move(menu_pos);
			point_size_slider->hide();
			transparency_slider->hide();
			menu->show();
		} else if (action==transparency) {
			menu_pos.setY((menu_pos.y()+ transparency_number * (fm.height()+6)));
			transparency_slider->set_float_value(l->get_color().alphaF());
			transparency_slider->show();
			//menu_pos.setX((menu_pos.x()+menu->width()));
			menu_pos.setX((menu_pos.x()-transparency_slider->width()));
			transparency_slider->move(menu_pos);
			point_size_slider->hide();
			line_width_slider->hide();
			menu->show();
		} else {
			line_width_slider->hide();
			point_size_slider->hide();
			transparency_slider->hide();
		}

	#endif //Q_WS_WIN
*/

	}
}

void Layer_list_model::clear() {
	layers.clear();
	filtered_layers.clear();
}

void Layer_list_model::filter_apply(const QString& text, bool active) {

	filtered_layers.clear();
	for (unsigned i=0; i < layers.size(); i++) {
		QString name_from_layers = layers[i]->get_name();
		if (name_from_layers.toUpper().indexOf(text.toUpper()) != -1 && (!active || layers[i]->is_active()) )
			filtered_layers.push_back(layers[i]);
	}
	beginInsertRows(QModelIndex(), 0, static_cast<int>(filtered_layers.size()));
	filter_text = text;
	show_only_active = active;
	endInsertRows();

	//emit widget_needs_repaint();

}