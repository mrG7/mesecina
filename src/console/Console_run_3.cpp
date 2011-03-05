#include <console/Console_logger.h>
#include <iostream>
#include <fstream>
#include <console/Console_run_3.h>
#include <gui/app/static/Application_settings.h>
#include <gui/app/Application_variable.h>
#include <gui/app/Settings_table_model.h>
#include <windows.h>

#include <QString>

#include <geometries/union_of_balls_3/Union_of_balls_3.h>
#include <geometries/union_of_balls_3/Union_of_balls_3_implement.h>
#include <CGAL/IO/Polyhedron_iostream.h>
extern template class __declspec(dllimport) Union_of_balls_3<Double_kernel>;
typedef Union_of_balls_3<Double_kernel> Union_of_balls_double_3;
typedef Union_of_balls_double_3::Polyhedron Polyhedron;

#include <geometries/mesh_view_3/Mesh_view_3.h>

#include <geometries/medial_explore_3/Medial_explore_3.h>

Console_run_3::Console_run_3(int argc, char* argv[]) : Console_run(argc, argv) {
	Application_settings::add_double_setting("ball-loading-radius-perturb", 1.002);
	Application_settings::add_double_setting("ball-loading-center-perturb", 0.0001);

	Application_settings::add_double_setting("implicit-mesher-bound-angle",30);
	Application_settings::add_double_setting("implicit-mesher-bound-radius",0.1);
	Application_settings::add_double_setting("implicit-mesher-bound-distance",0.1);

	Application_settings::add_int_setting("medial-surface-boundary-smoothing-steps", 0);
	Application_settings::add_int_setting("scale-surface-boundary-smoothing-steps", 0);
	Application_settings::add_double_setting("boundary-smoothing-neighbor-edge-max-squared-ratio",10);
	Application_settings::add_double_setting("boundary-smoothing-max-vertex-movement",0.05);

}

void collect_weighted_points_from_woff(const char* file_name, std::list<Point4D>* points) {
	QFile f(file_name);
	if ( !f.open( QIODevice::ReadOnly ) ) {
		std::cout << LOG_ERROR << "Could not open file for reading" << std::endl;
		//error(tr("File `%1' could not be open for reading!").arg(file_name));
		//statusBar()->showMessage(tr("Error in loading %1").arg(file_name));
		return;
	}

	QTextStream fs( &f );
	QString head; fs >> head;
	double perturb_w = Application_settings::get_double_setting("ball-loading-radius-perturb");
	double perturb_center = Application_settings::get_double_setting("ball-loading-center-perturb");
	if (perturb_w==0) {
		std::cout << LOG_ERROR << "ball-loading-radius-perturb value is 0, we set it to 1" << std::endl;
		Application_settings::set_setting("ball-loading-radius-perturb", 1);
		perturb_w = 1;
	}
#ifdef MESECINA_3D
	QString extension = "WOFF";
#else
	QString extension = "WPOFF";

#endif
	if (QString::compare(head, extension)) {
		std::cout << "wrong header in file" << std::endl;
		//error(tr("File `%1' has no correct header!").arg(current_file_name));
		//statusBar()->showMessage(tr("Error in loading %1").arg(file_name));					
		return;
	}	

	int n, dummy; double x, y, w;
#ifdef MESECINA_3D
	double z;
#endif
	fs >> n; fs >> dummy; fs >> dummy;

#ifdef MESECINA_3D
	points->clear();
	for (int i=0; i<n && !fs.atEnd(); i++) {
		fs >> x;
		fs >> y;
		fs >> z;
		fs >> w;
		double max_movement = perturb_center * w;
		double perturb_x = ((float)rand()/RAND_MAX *2 - 1.0) * max_movement;
		double perturb_y = ((float)rand()/RAND_MAX *2 - 1.0) * max_movement;
		double perturb_z = ((float)rand()/RAND_MAX *2 - 1.0)* max_movement;
		w = w * perturb_w;
		w = w * w;
		points->push_back(Point4D(x+perturb_x,y+perturb_y,z+perturb_z,w));
	}
#else 
	std::list<Point3D> points;
	for (int i=0; i<n && !fs.atEnd(); i++) {
		fs >> x;
		fs >> y;
		fs >> w;
		w = w * perturb_w;
//		w = w * w;
		points.push_back(Point3D(x,y,w));
	}
#endif
	f.close();
}

void Console_run_3::do_processing() {

	// load settings
	char* settingspath = "";
	assign_string_parameter_if_exists(argc, argv, "-settings", settingspath);
	if (QString(settingspath)!="") {
//		std::cout << "Load settings from: " << settingspath << std::endl;
		Settings_table_model::load_current_settings(settingspath);
	}

	char* mode = "";
	assign_string_parameter_if_exists(argc, argv, "-mode", mode);	
	std::string mode_str(mode);

	// meshing the boundary of a union of balls
	if (mode_str=="balls2mesh") {
		Union_of_balls_double_3 ub;

		// open file
		char* filename = "";
		assign_string_parameter_if_exists(argc, argv, "-input", filename);	
		std::fstream infile;
		infile.open (filename);
		if (infile.is_open()) {
			infile.close();
			std::cout << "Input file: " << filename << std::endl;
			std::list<Point4D> weighted_points;
			collect_weighted_points_from_woff(filename, &weighted_points);
			ub.add_weighted_points(&weighted_points);	
		} else {
			std::cout << "Input file could not be opened: " << filename << std::endl;
			return;
		}

		char* outfilename = "out.off";
		assign_string_parameter_if_exists(argc, argv, "-output", outfilename);	
		std::string ofname(outfilename);


		std::cout << "Meshing the boundary of a union of balls" << std::endl;
		print_settings("implicit-");
		std::cout << std::endl;

		std::cout << std::endl << "COMPUTE BEGIN" << std::endl<< std::endl;

		Polyhedron* p = ub.get_boundary_polyhedron();
		std::ofstream outfile;
		outfile.open(outfilename);
		outfile << *p;
		outfile.close();
		std::cout << "File written: " << outfilename << std::endl;

		std::cout << std::endl << "COMPUTE END" << std::endl<< std::endl;
	} else if (mode_str=="mesh2balls") {
		Mesh_view_3 m;
		std::cout << "Converting a shape described by boundary mesh to union of balls" << std::endl;
		
		// open file
		char* filename = "";
		assign_string_parameter_if_exists(argc, argv, "-input", filename);	
		std::fstream infile;
		infile.open (filename);
		if (infile.is_open()) {
			infile.close();
			m.load_generic_file(filename);
			std::cout << "Input file: " << filename << std::endl;
		} else {
			std::cout << "Input file could not be opened: " << filename << std::endl;
			return;
		}

		char* outfilename = "out.woff";
		assign_string_parameter_if_exists(argc, argv, "-output", outfilename);	
		std::string ofname(outfilename);		
		
		print_settings("meshing-");
		std::cout << std::endl;

		std::cout << std::endl << "COMPUTE BEGIN" << std::endl<< std::endl;

		m.write_mesh_balls(ofname,m.get_mesh_polar_balls());

		std::cout << std::endl << "COMPUTE END" << std::endl<< std::endl;
	} else if (mode_str=="balls2moff") {
		std::cout << "Computing the medial axis filtrations and a scale axis of a union of balls" << std::endl;
		//print_settings("implicit-");
		//std::cout << std::endl;

		//std::cout << std::endl << "COMPUTE BEGIN" << std::endl<< std::endl;

		//Polyhedron* p = ub.get_boundary_polyhedron();
		//std::ofstream outfile;
		//outfile.open(outfilename);
		//outfile << *p;
		//outfile.close();
		//std::cout << "File written: " << outfilename << std::endl;

		//std::cout << std::endl << "COMPUTE END" << std::endl<< std::endl;
	} else if (mode_str=="medrep-basic") {
		Mesh_view_3 m;

		// open input file
		char* filename = "";
		assign_string_parameter_if_exists(argc, argv, "-input", filename);	
		std::fstream infile;
		infile.open (filename);
		if (infile.is_open()) {
			infile.close();
			m.load_generic_file(filename);
			std::cout << "Input file opened: " << filename << std::endl;
		} else {
			std::cout << "Input file could not be opened: " << filename << std::endl;
			return;
		}

		double m2b2m = 0.01;
		assign_float_parameter_if_exists(argc, argv, "-m2b2m", m2b2m);	

		// create directory
		QString file_name(QFileInfo(filename).absoluteFilePath());
		short_file_name = file_name.right(file_name.length() - file_name.lastIndexOf("/") - 1);
		QString only_file_name = short_file_name;
		short_file_name.truncate(short_file_name.lastIndexOf('.'));
		short_file_name = QObject::tr("%1-%2").arg(short_file_name).arg(m2b2m);
		out_directory = file_name;
		out_directory.truncate(file_name.lastIndexOf('/'));
		out_directory = QObject::tr("%1/%2").arg(out_directory).arg(short_file_name);
		dir = QDir(out_directory);
		if (!dir.exists()) {
			if (!dir.mkdir(out_directory)) {
				std::cout << "Couldn't create output directory " << out_directory.toStdString() << std::endl;
			} else {
				std::cout << "Created directory " << out_directory.toStdString() << std::endl;
			}
		} else {
			std::cout << "Diretory " << out_directory.toStdString() << " exists" << std::endl;
		}

		//copy input mesh
		QString input_file_copy = QObject::tr("%1/%2").arg(out_directory).arg(only_file_name);
		if (!QFile::exists(input_file_copy)) {
			if (!QFile::copy(file_name, input_file_copy)) {
				std::cout << "Couldn't copy input file to " << input_file_copy.toStdString() << std::endl;
			} else {
				std::cout << "Copied file " << input_file_copy.toStdString() << std::endl;
			}
		} else {
			std::cout << "File " << input_file_copy.toStdString() << " exists" << std::endl;
		}

		// create medrep file
		QString medrep_filename = QObject::tr("%1/%2.medrep").arg(out_directory).arg(short_file_name);
		if (QFile::exists(medrep_filename)) {
			QString balls_filename = QObject::tr("%1/%2.woff").arg(out_directory).arg(short_file_name);
			if (!QFile::exists(balls_filename)) QFile::remove(medrep_filename);	
			else {
				std::cout << "Medrep file and balls file already exist, skipping conversion to balls" << std::endl;
				return;
			}
		}
//		std::cout << "medrep filename: " << medrep_filename << std::endl;
		QSettings medrep(medrep_filename, QSettings::IniFormat);
		medrep.setValue("input", only_file_name);
		medrep.setValue("m2b2m", m2b2m);
		medrep.sync();


		//Application_settings::set_setting("meshing-surface-angle", 25);
		Application_settings::set_setting("meshing-surface-size", m2b2m);
		Application_settings::set_setting("meshing-surface-approximation", m2b2m/5.0);
		//Application_settings::set_setting("meshing-tet-radius-edge-ratio", 3);
		//Application_settings::set_setting("meshing-tet-size", 1);
		print_settings("meshing-");


		std::cout << std::endl << "COMPUTE BEGIN" << std::endl<< std::endl;

		//write mesh file
		QString medit_filename = QObject::tr("%1/%2.mesh").arg(out_directory).arg(short_file_name);
		Mesh_view_3::C3t3* dm = m.get_domain_mesh();
		std::ofstream medit_file(medit_filename.toStdString().c_str());
		dm->output_to_medit(medit_file);
		medit_file.close();
		medrep.setValue("volume-mesh", path_to_filename(medit_filename));
		medrep.sync();
		std::cout << "Volume mesh file written " << medit_filename.toStdString() << std::endl << std::endl;

		//write balls
		QString balls_filename = QObject::tr("%1/%2.woff").arg(out_directory).arg(short_file_name);
//		std::cout << "balls filename " << medit_filename.toStdString() << std::endl;
		m.write_mesh_balls(balls_filename.toStdString(),m.get_mesh_polar_balls());
		medrep.setValue("balls", path_to_filename(balls_filename));
		medrep.sync();

		std::cout << "Balls file written " << balls_filename.toStdString() << std::endl << std::endl;
/*
		// load balls
		Application_settings::set_setting("ball-loading-radius-perturb", 1.002);
		Application_settings::set_setting("ball-loading-center-perturb", 0.0001);
		Application_settings::set_setting("input-balls-scale-factor",1);
		print_settings("ball-loading-");
		Union_of_balls_double_3 ub;
		std::list<Point4D> weighted_points;
		collect_weighted_points_from_woff(balls_filename.toStdString().c_str(), &weighted_points);
		ub.add_weighted_points(&weighted_points);	
		std::cout << "Load balls into union of balls from file " << balls_filename.toStdString() << std::endl << std::endl;

		// compute medial axis

		compute_medial_structures(1.0, true, &ub, medrep);
		compute_medial_structures(1.0, false, &ub, medrep);
		compute_medial_structures(1.1, true, &ub, medrep);
*/
		std::cout << std::endl << "COMPUTE END" << std::endl<< std::endl;

	} else if (mode_str=="medrep-medial"){

		// open input file
		char* filename = "";
		assign_string_parameter_if_exists(argc, argv, "-input", filename);	

		QString medrep_sfn = QObject::tr("%1/%1.medrep").arg(filename);
		QString medrep_filename(QFileInfo(medrep_sfn).absoluteFilePath());
		out_directory = medrep_filename;
		out_directory.truncate(out_directory.lastIndexOf('/'));

		if (!QFile::exists(medrep_filename)) {
			std::cout << LOG_ERROR << "Could not load medrep file from " << filename << " built " << medrep_filename.toStdString() << " absolute filepath which doesn't seem to exist" << std::endl;
		}
		QSettings medrep(medrep_filename, QSettings::IniFormat);
		std::cout << "medrep_filename: " << medrep_filename.toStdString() << std::endl;
		std::cout << "out_directory: " << out_directory.toStdString() << std::endl;

		short_file_name = medrep_filename.right(medrep_filename.length() - medrep_filename.lastIndexOf("/") - 1);
		short_file_name.truncate(short_file_name.lastIndexOf('.'));


		double growth;
		assign_float_parameter_if_exists(argc, argv, "-growth", growth);
		int dummy;
		bool nice = !assign_int_parameter_if_exists(argc, argv, "-artifacts", dummy);
		bool smooth = !assign_int_parameter_if_exists(argc, argv, "-zigzag", dummy);
		std::cout << "smooth value " << smooth << std::endl;


		QString id = get_id_string(growth, nice);

		if (!exists_medrep_value(QObject::tr("%1-mesh").arg(id), medrep) || !exists_medrep_value(QObject::tr("%1-balls").arg(id), medrep)) {

			Union_of_balls_double_3 ub;
			
			if (!exists_medrep_value(QObject::tr("%1-moff").arg(id), medrep)) {
				// load balls
				QString balls_name = medrep.value("balls").toString();
				QString balls_filename = QObject::tr("%1/%2").arg(out_directory).arg(balls_name);
				Application_settings::set_setting("ball-loading-radius-perturb", 1.002);
				Application_settings::set_setting("ball-loading-center-perturb", 0.0001);
				Application_settings::set_setting("input-balls-scale-factor",growth);
				print_settings("ball-loading-");
				std::list<Point4D> weighted_points;
				collect_weighted_points_from_woff(balls_filename.toStdString().c_str(), &weighted_points);
				ub.add_weighted_points(&weighted_points);	
				std::cout << "Load balls into union of balls from file " << balls_filename.toStdString() << std::endl << std::endl;
			} else {
				std::cout << "The computation for the moff file for " << id.toStdString() << " skipped because result file is already available " << std::endl;
			}

			// compute medial axis
			int dummy;
			bool sampleballs = assign_int_parameter_if_exists(argc, argv, "-sampleballs", dummy);
			compute_medial_structures(growth, nice, smooth, &ub, medrep, sampleballs);
		} else {
			std::cout << "The computation for " << id.toStdString() << " skipped because result files are already available " << std::endl;
//			std::cout << "medrep.value " << medrep.value(QObject::tr("%1-mesh").arg(id)).toString().toStdString() << std::endl;
		}

	} else if (mode_str=="medrep-surface"){

		// open input file
		char* filename = "";
		assign_string_parameter_if_exists(argc, argv, "-input", filename);	

		QString medrep_sfn = QObject::tr("%1/%1.medrep").arg(filename);
		QString medrep_filename(QFileInfo(medrep_sfn).absoluteFilePath());
		out_directory = medrep_filename;
		out_directory.truncate(out_directory.lastIndexOf('/'));

		if (!QFile::exists(medrep_filename)) {
			std::cout << LOG_ERROR << "Could not load medrep file from " << filename << " built " << medrep_filename.toStdString() << " absolute filepath which doesn't seem to exist" << std::endl;
		}
		QSettings medrep(medrep_filename, QSettings::IniFormat);
		std::cout << "medrep_filename: " << medrep_filename.toStdString() << std::endl;
		std::cout << "out_directory: " << out_directory.toStdString() << std::endl;

		short_file_name = medrep_filename.right(medrep_filename.length() - medrep_filename.lastIndexOf("/") - 1);
		short_file_name.truncate(short_file_name.lastIndexOf('.'));
		
		QStringList all_keys = medrep.allKeys();
		QStringList::iterator s_it, s_end = all_keys.end();
		for (s_it = all_keys.begin(); s_it!=s_end; ++s_it) {
			QString key = *s_it;
			if (key.endsWith("balls")) {
				mesh_union_of_balls_if_needed(key, medrep);
			}
		}
	} else if (mode_str=="medrep-grown-balls"){

		// open input file
		char* filename = "";
		assign_string_parameter_if_exists(argc, argv, "-input", filename);	

		double growth;
		assign_float_parameter_if_exists(argc, argv, "-growth", growth);

		QString medrep_sfn = QObject::tr("%1/%1.medrep").arg(filename);
		QString medrep_filename(QFileInfo(medrep_sfn).absoluteFilePath());
		out_directory = medrep_filename;
		out_directory.truncate(out_directory.lastIndexOf('/'));

		if (!QFile::exists(medrep_filename)) {
			std::cout << LOG_ERROR << "Could not load medrep file from " << filename << " built " << medrep_filename.toStdString() << " absolute filepath which doesn't seem to exist" << std::endl;
		}
		QSettings medrep(medrep_filename, QSettings::IniFormat);
		std::cout << "medrep_filename: " << medrep_filename.toStdString() << std::endl;
		std::cout << "out_directory: " << out_directory.toStdString() << std::endl;

		short_file_name = medrep_filename.right(medrep_filename.length() - medrep_filename.lastIndexOf("/") - 1);
		short_file_name.truncate(short_file_name.lastIndexOf('.'));


		//Union_of_balls_double_3 ub;

		//if (!exists_medrep_value(QObject::tr("%1-balls").arg(growth), medrep)) {
		//	// load balls
		//	QString balls_name = medrep.value("balls").toString();
		//	QString balls_filename = QObject::tr("%1/%2").arg(out_directory).arg(balls_name);
		//	Application_settings::set_setting("ball-loading-radius-perturb", 1);
		//	Application_settings::set_setting("ball-loading-center-perturb", 0);
		//	Application_settings::set_setting("input-balls-scale-factor",growth);
		//	print_settings("ball-loading-");
		//	std::list<Point4D> weighted_points;
		//	collect_weighted_points_from_woff(balls_filename.toStdString().c_str(), &weighted_points);

		//	QString id = get_id_string(growth, nice);
		//	QString grown_balls_filename = QObject::tr("%1/%2-balls.woff").arg(out_directory).arg(growth);

		//	std::ofstream myfile;
		//	myfile.open (file_name.c_str());
		//	myfile.precision(dbl::digits10);

		//	// write header
		//	myfile << "WOFF " << unique_vertices.size() << " 0 0" << std::endl;

		//	// write vertices
		//	std::list<Point_3>::iterator p_it, p_end = unique_vertices.end();
		//	std::list<double>::iterator r_it, r_end = unique_radii.end();
		//	for (r_it =unique_radii.begin(), p_it=unique_vertices.begin(); p_it!=p_end; ++p_it, ++r_it) {
		//		myfile << std::fixed << p_it->x() << " " 
		//			<< std::fixed << p_it->y() << " " 
		//			<< std::fixed << p_it->z() << " " 
		//			<< std::fixed << *r_it << std::endl;
		//	}
		//	std::cout << "Written " << unique_vertices.size() << " balls." << std::endl;
		//	myfile.close();

		//	std::cout << "Load balls into union of balls from file " << balls_filename.toStdString() << std::endl << std::endl;
		//} else {
		//	std::cout << "The computation for the moff file for " << id.toStdString() << " skipped because result file is already available " << std::endl;
		//}



	} else if (mode_str==""){
		std::cout << "The -mode parameter has value '" << mode_str << "', which is not recognized" << std::endl;
	}
//	std::cout << std::endl << "STATISTICS BEGIN" << std::endl<< std::endl;
////	print_statistics(&s);
//	std::cout << std::endl << "STATISTICS END" << std::endl<< std::endl;

//	s.save_generic_file(ofname);



}

void Console_run_3::mesh_union_of_balls_if_needed(const QString& key, QSettings& medrep) {
	QString key_value = medrep.value(key).toString();
	QString balls_filename = QObject::tr("%1/%2").arg(out_directory).arg(key_value);
//	std::cout << "Meshing union of balls: " << balls_filename.toStdString() << std::endl;

	QString surface_key = QObject::tr("%1-surface").arg(key);

	if (!exists_medrep_value(surface_key, medrep)) {
		QString surface_key_value = QObject::tr("%1-surf.off").arg(key_value);
		QString surface_filename = QObject::tr("%1/%2").arg(out_directory).arg(surface_key_value);
		//std::cout << "Surface key: " << surface_key.toStdString() << std::endl;
		//std::cout << "Surface key value: " << surface_key_value.toStdString() << std::endl;
		//std::cout << "Surface filepath: " << surface_filename.toStdString() << std::endl;

		std::cout << "Balls input for meshing: " << balls_filename.toStdString() << std::endl;
		std::cout << "Output mesh file: " << surface_filename.toStdString() << std::endl;
		std::cout << "Medrep key: " << surface_key.toStdString() << std::endl;

		double m2b2m = medrep.value("m2b2m").toDouble();
		std::cout << "m2b2m read: " << m2b2m << std::endl;

		double m2b2m_smaller =m2b2m * 0.4;
		Application_settings::set_setting("implicit-mesher-bound-angle",25);
		Application_settings::set_setting("implicit-mesher-bound-radius",m2b2m_smaller);
		Application_settings::set_setting("implicit-mesher-bound-distance",m2b2m_smaller/7.5);
		print_settings("implicit-");

		Application_settings::set_setting("ball-loading-radius-perturb", 1);
		Application_settings::set_setting("ball-loading-center-perturb", 0);
		Application_settings::set_setting("input-balls-scale-factor",1);
		print_settings("ball-loading-");

		Union_of_balls_double_3 ub;

		// open file
		std::fstream infile;
		infile.open (balls_filename.toStdString().c_str());
		if (infile.is_open()) {
			infile.close();
			std::cout << "Input file: " << balls_filename.toStdString() << std::endl;
			std::list<Point4D> weighted_points;
			collect_weighted_points_from_woff(balls_filename.toStdString().c_str(), &weighted_points);
			ub.add_weighted_points(&weighted_points);	
		} else {
			std::cout << "Input file could not be opened: " << balls_filename.toStdString() << std::endl;
			return;
		}

		std::cout << std::endl << "COMPUTE BEGIN" << std::endl<< std::endl;

		Polyhedron* p = ub.get_boundary_polyhedron();
		std::ofstream outfile;
		outfile.open(surface_filename.toStdString().c_str());
		outfile << *p;
		outfile.close();

		medrep.setValue(surface_key, path_to_filename(surface_filename));
		medrep.sync();
		std::cout << "File written: " << surface_filename.toStdString() << std::endl;

		std::cout << std::endl << "COMPUTE END" << std::endl<< std::endl;


	} else {
		std::cout << "Surface " << surface_key.toStdString() << " exists already, skipping computation" << std::endl;
	}
	std::cout << std::endl;

}



bool  Console_run_3::exists_medrep_value(QString& name, QSettings& medrep) {
	if (medrep.contains(name)) {
		QString filename = QObject::tr("%1/%2").arg(out_directory).arg(medrep.value(name).toString());
		if (QFile::exists(filename)) return true;
	}
	return false;
}


QString Console_run_3::get_id_string(double growth, bool nice) {
	QString id;
	if (growth==1) id = "medial";
	else id = QObject::tr("scale-%1").arg(growth);
	if (!nice) id = QObject::tr("%1-ugly").arg(id);

	return id;
}


void Console_run_3::compute_medial_structures(double growth, bool nice, bool smooth, void *p, QSettings& medrep, bool sample_medial_balls) {
	std::cout << "smooth value in compute_medial_structures: " << smooth << std::endl;
	int smoothing_steps = 3;
	if (!smooth) smoothing_steps = 0;
	if (growth==1) {
		Application_settings::set_setting("medial-surface-boundary-smoothing-steps", smoothing_steps);
		std::cout << "Set smoothing steps for medial axis: " << smoothing_steps << std::endl;
	}
	if (growth > 1 && nice) {
		Application_settings::set_setting("scale-surface-boundary-smoothing-steps", smoothing_steps);
		std::cout << "Set smoothing steps for scale axis: "  << smoothing_steps << std::endl;
	}
	Application_settings::set_setting("boundary-smoothing-neighbor-edge-max-squared-ratio",10);
	Application_settings::set_setting("boundary-smoothing-max-vertex-movement",0.05);
	print_settings("boundary-smoothing-");

	QString id = get_id_string(growth, nice);
	QString moff_filename = QObject::tr("%1/%2-%3.moff").arg(out_directory).arg(id).arg(short_file_name);

	if (!exists_medrep_value(QObject::tr("%1-moff").arg(id), medrep)) {
		Application_settings::set_setting("medial-axis-export-vertex-squared-distance-collapse", 0);

		if (nice) Application_settings::set_setting("ignore-incomplete-v-voronoi-faces-for-medial-axis", true);
		else Application_settings::set_setting("ignore-incomplete-v-voronoi-faces-for-medial-axis", false);
		Union_of_balls_double_3* ub = static_cast<Union_of_balls_double_3*>(p);
		ub->has_moff_string = ub->has_medial_edges = false;

		if (ub->growth_ratio != growth) {
			Application_settings::set_setting("input-balls-scale-factor",growth);
			ub->invalidate_cache();
			ub->update_ball_scaling();
		}

		ub->write_medial_axis_to_moff(moff_filename.toStdString());
		medrep.setValue(QObject::tr("%1-moff").arg(id), path_to_filename(moff_filename));
		medrep.sync();
		std::cout << "Moff written " << moff_filename.toStdString() << std::endl << std::endl;
	} 

	// move it to medial explore
	Application_settings::set_setting("scale-axis-balls-min-intersection-depth", 140);
	Medial_explore_3 me;
	//		std::stringstream ss(*moff_string);
	me.mat.read_from_moff(moff_filename.toStdString().c_str());
	me.has_mat = true;
	me.mat.compute_face_normals();
	me.mat.identify_sheets();
	if (growth>1 && nice) {
		// find sheets and compute filtration
		me.mat.compute_topology_angle_sheet_filtration();
	}

	// write mesh files
	Application_settings::set_setting("topology-filtration-angle-lower-limit", 30.0);
	Application_settings::set_setting("scale-axis-balls-center-max-distance",0.000001);
	QString medial_axis_mesh_filename = QObject::tr("%1/%2-%3.off").arg(out_directory).arg(id).arg(short_file_name);
	if (growth>1 && nice) {
		me.mat.write_to_off(medial_axis_mesh_filename.toStdString().c_str(),
			&(me.mat.sheet_top_angle),
			Application_settings::get_double_setting("topology-filtration-angle-lower-limit"));
	} else {
		me.mat.write_to_off(medial_axis_mesh_filename.toStdString().c_str());
	}

	medrep.setValue(QObject::tr("%1-mesh").arg(id), path_to_filename(medial_axis_mesh_filename));
	medrep.sync();
	std::cout << "Medial mesh written " << medial_axis_mesh_filename.toStdString() << std::endl << std::endl;


	if (sample_medial_balls) {
		// write mesh balls
		Application_settings::set_setting("scale-axis-balls-min-intersection-depth", 140.0);
		QString medial_balls_filename = QObject::tr("%1/%2-%3.woff").arg(out_directory).arg(id).arg(short_file_name);
		std::vector<Ball> balls;
		me.get_sampled_balls();
		if (growth>1 && nice) {
			me.mat.fill_balls_with_criteria(CRIT_SHEET_TOPANGLE, Application_settings::get_double_setting("topology-filtration-angle-lower-limit"), balls);
			me.write_mesh_balls(medial_balls_filename.toStdString(), &balls);
		} else {
			me.mat.fill_balls_with_criteria(CRIT_ANGLE, -1, balls);
			me.write_mesh_balls(medial_balls_filename.toStdString(), &balls);
		}
	
		medrep.setValue(QObject::tr("%1-balls").arg(id), path_to_filename(medial_balls_filename));
		medrep.sync();
		std::cout << "Medial balls written " << medial_balls_filename.toStdString() << std::endl << std::endl;
	}



}

void Console_run_3::print_statistics(Asia_structure_3* /*s*/) {

	//int n = s->get_variable_count();
	//for (int i=0; i<n; i++) {
	//	Application_variable av = s->get_variable(i);
	//	std::cout << av.first.toStdString() << ": " << av.second.toStdString() << std::endl;

	//}
}
