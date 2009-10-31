/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Color_map.cpp 388 2008-12-12 18:17:06Z miklosb $
 */

#include <gui/app/static/Color_map.h>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <iostream>
#include <QtGui/QBrush>

Color_map::Color_map() {
	thumbnail = 0;
	brush = 0;
	name = "";
	min = max = 0;
}
Color_map::Color_map(const QString& map_path, const QString& name) {
	// load the color map
	color_data.clear();
	if (name!="Solid") {
		QFile f(map_path);
		if ( !f.open( QIODevice::ReadOnly ) ) {
			std::cout << "Color map" << map_path.toStdString() << " could not be read!" << std::endl;
		} else { 
			QTextStream fs( &f ); double d;
			while (!fs.atEnd()) {
				fs >> d;
				color_data.push_back(d);
			}
		}

		// load the thumbnail
		QString thumb_path = map_path;
		thumb_path.replace(thumb_path.length()-4, 4, ".png");
		thumbnail = new QPixmap(thumb_path);
		brush = new QBrush(thumbnail->scaledToWidth(16));
	}
	// set other variables;
	this->name = name;
	min = max = 0;
}

QPixmap* Color_map::get_thumbnail() { return thumbnail; }
QBrush* Color_map::get_brush() { return brush; }
QString Color_map::get_name() { return name; }

//void Color_map::set_scale_min(float min) { this->min = min; }
//void Color_map::set_scale_max(float max) { this->max = max; }
void Color_map::set_scale_range(float min, float max) {
	this->min = min;
	this->max = max;
}

QColor Color_map::get_color(float value) {
	unsigned color_numbers = (unsigned)color_data.size()/3;
	if (color_numbers<2) return QColor();

	QColor col;
	if (value >= max) {
		col.setRedF(color_data[(color_numbers-1)*3]);
		col.setGreenF(color_data[(color_numbers-1)*3+1]);
		col.setBlueF(color_data[(color_numbers-1)*3+2]);
	}
	else if (value <= min) {
		col.setRedF(color_data[0]);
		col.setGreenF(color_data[1]);
		col.setBlueF(color_data[2]);
	} else {
		double range = max-min;
		double step = range/(color_numbers-1);
		int interval_number = (int)((value-min)/step);
		double interval_ratio = ((value-min) - interval_number*step)/step;
		//if (interval_ratio==0 && value == max) {
		//	col.setRedF(color_data[(interval_number)*3]);
		//	col.setGreenF(color_data[(interval_number+1)*3+1]);
		//	col.setBlueF(color_data[(interval_number+1)*3+2]);
		//} else {
		if (interval_number==interval_number && interval_ratio == interval_ratio) { // avoid indexing the array if we have infinite or undefined values
			col.setRedF((1-interval_ratio) * color_data[interval_number*3] + interval_ratio * color_data[(interval_number+1)*3]);
			col.setGreenF((1-interval_ratio) * color_data[interval_number*3+1] + interval_ratio * color_data[(interval_number+1)*3+1]);
			col.setBlueF((1-interval_ratio) * color_data[interval_number*3+2] + interval_ratio * color_data[(interval_number+1)*3+2]);
		}
		//}
	}
	return col;
}


Random_color_map::Random_color_map() : Color_map() {
}
Random_color_map::Random_color_map(const int map_resolution, const QString& name) : Color_map() {
	// generate the color map
	color_data.clear();
	for (int i=0; i<map_resolution*3; i++) {
		double random_0_1 = (double)rand()/RAND_MAX;
		color_data.push_back(random_0_1);
	}

		// load the thumbnail
		//QString thumb_path = map_path;
		//thumb_path.replace(thumb_path.length()-4, 4, ".png");
		QString thumb_path = ":/colormap/Jet.png";
		thumbnail = new QPixmap(thumb_path);
		brush = new QBrush(thumbnail->scaledToWidth(16));

	// set other variables;
	this->name = name;
	min = max = 0;
}
