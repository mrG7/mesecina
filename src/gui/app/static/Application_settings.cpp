/* This source file is part of Mesecina, a software for visualization and studying of
* the medial axis and related computational geometry structures.
* More info: http://www.agg.ethz.ch/~miklosb/mesecina
* Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
*
* $Id: Application_settings.h 560 2009-03-25 13:34:22Z miklosb $
*/

#include <gui/app/static/Application_settings.h>
#include <algorithm>
#include <iostream>

// initializing static members of Application_settings
std::vector<Application_setting> Application_settings::settings;
QString Application_settings::name_ending = "dummy";
//std::vector<QString> /*__declspec( dllexport )*/ Application_settings::setting_names;
//std::vector<QString> /*__declspec( dllexport ) */Application_settings::setting_tooltips;
//std::vector<Application_setting_type> /*__declspec( dllexport )*/ Application_settings::setting_types;

void Application_settings::add_string_setting(const QString& name, const char* defaultvalue, const QString& ttip) {
	QSettings settings;
	if (!settings.contains(name)) 
		settings.setValue(name, defaultvalue);
	Application_settings::add_setting(name, AS_STRING, ttip);
}
std::string Application_settings::get_string_setting(const QString& name) {
	QSettings settings;
	return settings.value(name).toString().toStdString();
}

int Application_settings::get_setting_index(const QString& name) {
	std::vector<Application_setting>::iterator n_it, n_end = settings.end();
	int i=0;
	for (n_it = settings.begin(); n_it!=n_end; ++n_it, ++i) {
		if (n_it->name == name) return i;
	}
	return -1;
}

void Application_settings::add_double_setting(const QString& name, const double defaultvalue, const QString& ttip) {
	QSettings settings;
	if (!settings.contains(name)) 
		settings.setValue(name, defaultvalue);
	Application_settings::add_setting(name, AS_DOUBLE, ttip);
}

double Application_settings::get_double_setting(const QString& name) {
	QSettings settings;
	return settings.value(name).toDouble();
}

void Application_settings::add_int_setting(const QString& name, const int defaultvalue, const QString& ttip) {
	QSettings settings;
	if (!settings.contains(name)) 
		settings.setValue(name, defaultvalue);
	Application_settings::add_setting(name, AS_INTEGER, ttip);
}

int Application_settings::get_int_setting(const QString& name) {
	QSettings settings;
	return settings.value(name).toInt();
}


void Application_settings::add_bool_setting(const QString& name, const bool defaultvalue, const QString& ttip) {
	QSettings settings;
	if (!settings.contains(name)) 
		settings.setValue(name, defaultvalue);
	Application_settings::add_setting(name, AS_BOOL, ttip);
}

int Application_settings::get_bool_setting(const QString& name) {
	QSettings settings;
	return settings.value(name).toBool();
}

void Application_settings::set_setting(const QString& name, const QVariant v) {
	QSettings settings;
	return settings.setValue(name, v);
}

QVariant Application_settings::get_setting(const QString& name) {
	QSettings settings;
	return settings.value(name);
}

void Application_settings::add_setting(const QString& name, const Application_setting_type type, const QString& ttip) {
	std::vector<Application_setting>::iterator n_it, n_end = settings.end();
	for (n_it = settings.begin(); n_it!=n_end; n_it++) 
		if (n_it->name == name) return;
	settings.push_back(Application_setting(name,type,ttip));
	//setting_names.push_back(name);
	//setting_types.push_back(type);
	//setting_tooltips.push_back(ttip);
}

bool Application_settings::to_remove(const Application_setting& s) {
//	std::cout << "checking " << s.name.toStdString() << " for " << name_ending.toStdString() << std::endl;
	if (s.name.endsWith(name_ending)) return true;
	return false;
}

int Application_settings::remove_settings_ending_with(const QString& _name_ending) {
	name_ending = _name_ending;
//	std::cout << "settings size: " << settings.size() << std::endl;
	std::vector<Application_setting>::iterator remove_it = std::remove_if(settings.begin(), settings.end(), &(Application_settings::to_remove));
	settings.erase(remove_it, settings.end());
//	std::cout << "settings size after remove_if: " << settings.size() << std::endl;

	//std::vector<QString>::iterator n_it, n_end = setting_names.end();
	//std::vector<Application_setting_type>::iterator st_it, st_end = setting_types.end();
	//std::vector<QString>::iterator tt_it, tt_end = setting_tooltips.end();
	int removed = 0;
//	for (n_it = setting_names.begin(),
//		 st_it = setting_types.begin(),
//		 tt_it = setting_tooltips.begin(); n_it!=n_end; 
//		 n_it++, st_it++, tt_it++)  {
//		if (n_it->endsWith(name_ending))  {
//			n_it = setting_names.erase(n_it);
//			st_it = setting_types.erase(st_it);
//			tt_it = setting_tooltips.erase(tt_it);
////			n_it--;
//			removed++;
//		}
//	}
	return removed;
}
