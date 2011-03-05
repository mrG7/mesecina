/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Settings_table_model.cpp 737 2009-05-16 15:40:46Z miklosb $
 */

#include <gui/app/Settings_table_model.h>
#include <gui/app/static/Application_settings.h>
#include <iostream>
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtGui/QColor>


Settings_table_model::Settings_table_model(QObject *parent) : QAbstractTableModel(parent) {
	filter_text = "";
	filtered_names.clear();
	filtered_types.clear();
}

Settings_table_model::~Settings_table_model() {}

int Settings_table_model::rowCount(const QModelIndex &) const {
//	std::cout << "settings size: " << Application_settings::setting_names.size() << std::endl;
//	return (int)Application_settings::setting_names.size()+1;
	return (int)filtered_names.size()+1;
}

int Settings_table_model::columnCount( const QModelIndex &  ) const {
	return 2;
}

QVariant Settings_table_model::data(const QModelIndex &index, int role) const {
     if (!index.isValid())
         return QVariant();

     if (index.row() < 1 || index.row()-1 >= (int)Application_settings::settings.size())
         return QVariant();

	 QSettings settings; QString settings_name; Application_setting_type settings_type;
	 switch (role) {
		 case Qt::EditRole:
			 if (index.row()==0) return "";
			 if (index.column()==0) {
				 return filtered_names[index.row()-1];
			 }
			 else { 
				 settings_name = filtered_names[index.row()-1];
				 settings_type = filtered_types[index.row()-1];
				 switch (settings_type) {
					case AS_INTEGER:
						return settings.value(settings_name).toInt(); break;
					case AS_DOUBLE:
						return settings.value(settings_name).toString();
					case AS_STRING:
						return settings.value(settings_name).toString();
					//case AS_BOOL:
					//	return Qt::Checked;
					default:
						return QVariant();
				 }
			 }
		 case Qt::DisplayRole:
			 if (index.row()==0) return "";
			 if (index.column()==0) {
				 return filtered_names[index.row()-1];
			 }
			 else { 
				 settings_name = filtered_names[index.row()-1];
				 settings_type = filtered_types[index.row()-1];
				 switch (settings_type) {
					case AS_INTEGER:
						return settings.value(settings_name).toInt(); break;
					case AS_DOUBLE:
						return settings.value(settings_name).toDouble();
					case AS_STRING:
						return settings.value(settings_name).toString();
					//case AS_BOOL:
					//	return Qt::Checked;
					default:
						return QVariant();
				 }
			 }
			 break;
		 case Qt::TextAlignmentRole:
			 if (index.column()==0) return Qt::AlignLeft;
			 else return Qt::AlignRight;
			 break;
		 case Qt::CheckStateRole:
			 if (index.row()==0) return QVariant();
			 if (index.column()==1 && filtered_types[index.row()-1] == AS_BOOL) {
				 settings_name = filtered_names[index.row()-1];
				 if (settings.value(settings_name).toBool())
					 return Qt::Checked;
				 else return Qt::Unchecked;
			 }
			 return QVariant();
			 break;
		 case Qt::ToolTipRole:
			 if (index.row()>0)
				 if (filtered_tooltips[index.row()-1] != "") 
					 return filtered_tooltips[index.row()-1];
			 return QVariant();
			 break;
		 case Qt::UserRole:
			 settings_name = filtered_names[index.row()-1];
			 return Application_settings::get_setting_index(settings_name);
			 break;
		 default:
			 return QVariant();
	 }
}

Qt::ItemFlags Settings_table_model::flags(const QModelIndex &index) const {
	Qt::ItemFlags editableFlags = QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
	Qt::ItemFlags checkableFlags = QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;


	if (index.isValid() && index.row()!=0 && index.column()==1) {
		QSettings settings;
		Application_setting_type settings_type = filtered_types[index.row()-1];
		if (settings_type==AS_BOOL) return checkableFlags;
		return editableFlags;
	}
	else
		return QAbstractItemModel::flags(index);
}

bool Settings_table_model::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && role == Qt::CheckStateRole) {
		if (index.row()==0) return false;
		if (index.column()==0) return false;
		QSettings settings;
		QString settings_name = filtered_names[index.row()-1];
		Application_setting_type settings_type = filtered_types[index.row()-1];
		if (settings_type==AS_BOOL) {
			bool vi = false;
			if (value == Qt::Checked) vi = true;
			settings.setValue(settings_name,vi);
			emit dataChanged(index, index);
			emit application_settings_changed(settings_name);
			return true;
		}
	}
	if (index.isValid() && role == Qt::EditRole) {
		if (index.row()==0) return false;
		if (index.column()==0) return false;
		QSettings settings;
		QString settings_name = filtered_names[index.row()-1];
		Application_setting_type settings_type = filtered_types[index.row()-1];

		int vi; double vd; bool ok;
		switch (settings_type) {
			case AS_INTEGER:
				vi = value.toInt(&ok);
				if (ok) {
					settings.setValue(settings_name,vi);
//					emit dataChanged(index, index);
					emit dataChanged(QModelIndex(), QModelIndex());
					emit application_settings_changed(settings_name);
					return true;
				} else return false;
				break;
			case AS_DOUBLE:
				vd = value.toDouble(&ok);
				if (ok) {
					settings.setValue(settings_name,vd);
//					emit dataChanged(index, index);
					emit dataChanged(QModelIndex(), QModelIndex());
					emit application_settings_changed(settings_name);
					return true;
				} else return false;
				break;
			case AS_STRING:
				settings.setValue(settings_name,value.toString());
//				emit dataChanged(index, index);
				emit dataChanged(QModelIndex(), QModelIndex());
				emit application_settings_changed(settings_name);
				return true;
			default:
				return false;
		}
	}
	return false;
}


void Settings_table_model::filter_apply(const QString& text) {
	int removed = filtered_names.size();
	if (removed>0) beginRemoveRows(QModelIndex(), 1, removed);
	filtered_names.clear();
	filtered_types.clear();
	filtered_tooltips.clear();
	if (removed>0) endRemoveRows();

	for (unsigned i=0; i < Application_settings::settings.size(); i++) {
		QString name_from_variables = Application_settings::settings[i].name;
		QString ttip_from_variables = Application_settings::settings[i].tooltip;
		if (name_from_variables.toUpper().indexOf(text.toUpper()) != -1 ) {
			filtered_names.push_back(name_from_variables);
			filtered_types.push_back(Application_settings::settings[i].type);
			filtered_tooltips.push_back(ttip_from_variables);

		}
	}
	
	if (filtered_names.size() <= 0) return;
	beginInsertRows(QModelIndex(), 1, static_cast<int>(filtered_names.size()));
	filter_text = text;
	endInsertRows();
//	emit dataChanged(QModelIndex(), QModelIndex());
}

void Settings_table_model::process_filter() {
	filter_apply(filter_text);
}

void Settings_table_model::load_current_settings(const QString& file_name) {
	QSettings ini_settings(file_name, QSettings::IniFormat);
	QStringList keys = ini_settings.allKeys();
	for (int i = 0; i < keys.size(); ++i)
		Application_settings::set_setting(keys[i], ini_settings.value(keys[i]));
	std::cout << "Loaded " << keys.size() << " settings from " << file_name.toStdString() << std::endl;
}

void Settings_table_model::load_settings_and_emit_change(const QString& file_name) {
	QSettings ini_settings(file_name, QSettings::IniFormat);
	QStringList keys = ini_settings.allKeys();
	for (int i = 0; i < keys.size(); ++i) {
		Application_settings::set_setting(keys[i], ini_settings.value(keys[i]));
		emit application_settings_changed(keys[i]);
	}
	std::cout << "Loaded " << keys.size() << " settings from " << file_name.toStdString() << std::endl;
}

void Settings_table_model::save_current_settings(const QString& file_name) {
	if (QFile::exists(file_name)) QFile::remove(file_name);
	QSettings ini_settings(file_name, QSettings::IniFormat);
	for (size_t i=0; i < filtered_names.size(); i++) {
		std::cout << filtered_names[i].toStdString() << std::endl;
		ini_settings.setValue(filtered_names[i], Application_settings::get_setting(filtered_names[i]));
	}
	std::cout << "Saved " << filtered_names.size() << " settings to " << file_name.toStdString() << std::endl;
}