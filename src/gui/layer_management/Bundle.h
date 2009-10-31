/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Bundle.h 78 2007-03-25 03:19:20Z miklosb $
 */

#ifndef LAYER_BUNDLE_H
#define LAYER_BUNDLE_H

#include <vector>
#include <QtCore/QString>

class Bundle {
public:
	Bundle(const QString& name);
	~Bundle() {};

	void rename(const QString& new_name);
	QString get_name() const;

	void add_layer(const QString& name, const QByteArray& state);
	int size();
	QString layer_name(int i);
	QByteArray layer_state(int i);

	QByteArray saveState();
	bool restoreState(const QByteArray &state);

private:
	static const qint32 bundle_token = 0xB2;
	static const qint32 version = 0x01;
	QString bundle_name;
	std::vector<QString> layer_names;
	std::vector<QByteArray> layer_states;
};

#endif //LAYER_BUNDLE_H