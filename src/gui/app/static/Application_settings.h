/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Application_settings.h 586 2009-04-03 20:01:21Z miklosb $
 */

#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H

#include <vector>
#include <QtCore/QString>
#include <QtCore/QSettings>

#ifdef MESECINA_BRIDGE
#define API   __declspec( dllexport )
#else
#define API __declspec( dllimport )
#endif

typedef enum Application_setting_type {AS_INTEGER, AS_DOUBLE, AS_STRING, AS_BOOL };

class API Application_settings {
public:
	static std::vector<QString>  setting_tooltips;
	static std::vector<QString>  setting_names;
	static std::vector<Application_setting_type> setting_types;

	static void add_string_setting(const QString& name, const char* defaultvalue, const QString& ttip = "");
	static std::string get_string_setting(const QString& name);

	static void add_double_setting(const QString& name, const double defaultvalue, const QString& ttip = "");
	static double get_double_setting(const QString& name);

	static void add_int_setting(const QString& name, const int defaultvalue, const QString& ttip = "");
	static int get_int_setting(const QString& name);

	static void add_bool_setting(const QString& name, const bool defaultvalue, const QString& ttip = "");
	static int get_bool_setting(const QString& name);

	static void set_setting(const QString& name, const QVariant v);
	static QVariant get_setting(const QString& name);

	static int get_setting_index(const QString& name);

	static void add_setting(const QString& name, const Application_setting_type type, const QString& ttip = "");
};


#endif //APPLICATION_SETTINGS_H