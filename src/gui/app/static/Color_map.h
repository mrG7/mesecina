/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Color_map.h 388 2008-12-12 18:17:06Z miklosb $
 */

#ifndef COLOR_MAP_H
#define COLOR_MAP_H

#include <vector>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtGui/QPixmap>
#include <QtGui/QColor>

#ifdef MESECINA_BRIDGE
#define API   __declspec( dllexport )
#else
#define API __declspec( dllimport )
#endif

class API Color_map {
public:

	Color_map();
	Color_map(const QString& map_path, const QString& name);

	QPixmap* get_thumbnail();
	QBrush* get_brush();
	QString get_name();

//	void set_scale_min(float min);
//	void set_scale_max(float max);
	void set_scale_range(float min, float max);
	float get_scale_min() { return min; }
	float get_scale_max() { return max; }


	QColor get_color(float value);

protected:
	float min, max;
	QPixmap* thumbnail;
	QBrush* brush;
	QString name;
	std::vector<double> color_data;
};

class API Random_color_map : public Color_map {

public:
	Random_color_map();
	Random_color_map(const int map_resolution, const QString& name);

};

#endif //COLOR_MAP_H