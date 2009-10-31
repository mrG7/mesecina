/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Statistics_table_model.cpp 107 2007-05-31 11:04:10Z miklosb $
 */

#include <gui/app/Statistics_table_model.h>
#include <iostream>

Statistics_table_model::Statistics_table_model(QObject *parent) : QAbstractTableModel(parent) {
//	filter_text = "";
	variables.clear();
	filtered_variables.clear();
}

Statistics_table_model::~Statistics_table_model() {}

int Statistics_table_model::rowCount(const QModelIndex &) const {
//	std::cout << "rowCount at statistics " << (int)variables.size()+1 << std::endl;
	return (int)filtered_variables.size()+1;
//	return variables.size()+1;
}

int Statistics_table_model::columnCount( const QModelIndex &  ) const {
	return 2;
}

QVariant Statistics_table_model::data(const QModelIndex &index, int role) const {
     if (!index.isValid())
         return QVariant();

     if (index.row()-1 >= (int)variables.size())
         return QVariant();

	switch (role) {
		 case Qt::DisplayRole:
//			 std::cout << "display role for " << index.row() << " " << index.column() << std::endl;
			 if (index.row()==0) return "";
			 if (index.column()==0) 
				 return filtered_variables[index.row()-1].first;
			 else return filtered_variables[index.row()-1].second;
			 break;
		 case Qt::TextAlignmentRole:
			 if (index.column()==0) return Qt::AlignLeft;
			 else return Qt::AlignRight;
			 break;
		 //case Qt::SizeHintRole:
			// return 1500;
			// break;
		 //case Qt::DecorationRole:
			// if (index.row()==0) return QVariant();
			// if (index.column()==0) return Qt::red;
			// else return QVariant();
			// break;
		 //case Qt::CheckStateRole:
			// return filtered_layers[index.row()]->is_active() ? Qt::Checked : Qt::Unchecked;
			// break;
		 //case Qt::UserRole:
			// return filtered_layers[index.row()]->symbol_brush();
			// break;
		 //case 33:
			// return filtered_layers[index.row()]->has_valid_cache() ? 1 : 0;
		 default:
			 return QVariant();
	 }
}

void Statistics_table_model::set_variable(Application_variable v, bool apply_filter) {

	for (unsigned i=0; i<variables.size(); i++) {
		if (variables[i].first==v.first) {
			variables[i] = v;
			process_filter();
//			emit dataChanged(QModelIndex(), QModelIndex());
			return;
		}
	}

//	std::cout << "setting " << v.first.toStdString() << " pushed to end and ";
	//std::cout << "begin insert row " << std::endl;
//	beginInsertRows(QModelIndex(), 0, 0);
	variables.push_back(v);
	process_filter();
//	endInsertRows();
//	std::cout << "variables.size() " << variables.size() << std::endl;
}

void Statistics_table_model::clear() {
	std::vector<Application_variable> time_variables;
	for (unsigned i=0; i<variables.size(); i++)
		if (variables[i].first.startsWith("T ")) time_variables.push_back(variables[i]);
	size_t removed =variables.size() - time_variables.size();
	if (removed!=0) beginRemoveRows(QModelIndex(), 1, removed);
	variables = time_variables;
	if (removed!=0) endRemoveRows();
//	filter_apply(filter_text);
//	emit dataChanged(QModelIndex(), QModelIndex());
}

void Statistics_table_model::filter_apply(const QString& text) {

	size_t removed =filtered_variables.size();
	if (removed>0) beginRemoveRows(QModelIndex(), 1, removed);
	filtered_variables.clear();
	if (removed>0) endRemoveRows();

	for (unsigned i=0; i < variables.size(); i++) {
		QString name_from_variables = variables[i].first;
		if (name_from_variables.toUpper().indexOf(text.toUpper()) != -1 )
			filtered_variables.push_back(variables[i]);
	}

	if (filtered_variables.size() <= 0) return;
	beginInsertRows(QModelIndex(), 1, static_cast<int>(filtered_variables.size()));
	filter_text = text;
	endInsertRows();
}

void Statistics_table_model::process_filter() {
	filter_apply(filter_text);
}