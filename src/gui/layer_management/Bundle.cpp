/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Bundle.cpp 78 2007-03-25 03:19:20Z miklosb $
 */

#include <assert.h>
#include <QtCore/QDataStream>

#include <gui/layer_management/Bundle.h>

Bundle::Bundle(const QString& n) : bundle_name(n) {
	layer_names.clear();
	layer_states.clear();
}

void Bundle::rename(const QString& new_name) {
	bundle_name = new_name;
}

void Bundle::add_layer(const QString& name, const QByteArray& state) {
	layer_names.push_back(name);
	layer_states.push_back(state);
	assert(layer_names.size() == layer_states.size());
}

QByteArray Bundle::saveState() {
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);

	stream << qint32(bundle_token);
	stream << qint32(version);
	
	std::vector<QByteArray>::iterator s_it = layer_states.begin();
	std::vector<QString>::iterator n_it, n_end = layer_names.end();
	for (n_it = layer_names.begin(); n_it != n_end; n_it++, s_it++) {
		stream << *n_it;
		stream << *s_it;
	}

	return data;
}

bool Bundle::restoreState(const QByteArray &state) {
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);

	qint32 token;
    qint32 v;
    stream >> token;
    stream >> v;
    if (token != bundle_token || v != version)
        return false;

	QString l_name;
	QByteArray l_state;
	layer_names.clear();
	layer_states.clear();
	while (! stream.atEnd()) {
		stream >> l_name;
		stream >> l_state;
		add_layer(l_name, l_state);
	}

    return true;
}

QString Bundle::get_name() const { return bundle_name; }

QString Bundle::layer_name(int i) {
	return layer_names[i];
}

QByteArray Bundle::layer_state(int i) {
	return layer_states[i];
}

int Bundle::size() { return (int)layer_names.size(); }