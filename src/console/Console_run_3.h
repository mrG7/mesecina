#ifndef MESECINA_CONSOLE_RUN_3
#define MESECINA_CONSOLE_RUN_3

#include <QtCore/QString>
#include <QSettings>
#include <QDir>
#include "Console_run.h"

class Asia_structure_3;

class Console_run_3 : public Console_run {

public:
	Console_run_3(int argc, char* argv[]);

	virtual void do_processing();
	void print_statistics(Asia_structure_3* s);
private:
	void compute_medial_structures(double growth, bool nice, bool smooth, void* ub, QSettings& medrep, bool sample_medial_balls = true);
	void mesh_union_of_balls_if_needed(const QString& key, QSettings& medrep);
	QString get_id_string(double growth, bool nice);
	bool exists_medrep_value(QString& name, QSettings& medrep);


	QString out_directory;
	QDir dir;
	QString short_file_name;
};

#endif //MESECINA_CONSOLE_RUN_3