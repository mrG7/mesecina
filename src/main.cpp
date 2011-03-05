/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info at http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: main.cpp 737 2009-05-16 15:40:46Z miklosb $
 */

/** \mainpage mesecina
 * 
 * More about mesecina at <a href="http://www.agg.ethz.ch/~miklosb/mesecina">http://www.agg.ethz.ch/~miklosb/mesecina</a>
 *
 */

#include <QtGui/QApplication>
#include <QtCore/QDir>

#include <gui/app/Main_window.h>
#include <constants.h>
#include <QtOpenGL/QGLFormat>
#include <gui/app/static/Application_settings.h>
#include <gui/app/static/Color_map.h>
#ifdef MESECINA_3D
#include <console/Console_run_3.h>
#else
#include <console/Console_run_2.h>
#endif

#define MESECINA_DEVELOPMENT_BUILD

int main(int argc, char* argv[]) {
	QString app_name(APPLICATION_NAME);

	//extern std::vector<QString>  Application_settings::setting_names;
	//extern std::vector<QString> Application_settings::setting_tooltips;
	//extern std::vector<Application_setting_type> Application_settings::setting_types;

	std::cout << "I am doing something" << std::endl;
	if (argc > 1) {
		std::cout << "argc > 1" << std::endl;
		if (QString(argv[1]) == "--nogui") {
			std::cout << "nogui" << std::endl;
			app_name = QString(APPLICATION_NAME) + "-console";
			QCoreApplication::setOrganizationName(COMPANY_NAME);
		    QCoreApplication::setApplicationName(app_name);
#ifdef MESECINA_3D			
			Console_run_3 c(argc, argv);
			c.do_processing();
#else
			Console_run_2 c(argc, argv);
			c.do_processing();
#endif
			return 1;
		}
		else {
			std::cout << "gui" << std::endl;
			app_name =QString(APPLICATION_NAME)+"-"+QString(argv[1]);
		}
	}
	else 
	std::cout << "argc <= 1" << std::endl;

#ifdef MESECINA_PUBLIC_RELEASE
		app_name = QString(APPLICATION_NAME)+"-"+QString("public");
#else
#ifdef MESECINA_DEVELOPMENT_BUILD
		app_name = QString(APPLICATION_NAME)+"-"+QString("dev");;
#else
		app_name = APPLICATION_NAME;
#endif	
#endif


	QApplication app(argc, argv);
//	Q_INIT_RESOURCE(mesecina);
	std::cout << "app constructor" << std::endl;

	QGLFormat fmt;
	fmt.setAlpha(true);
	fmt.setSampleBuffers(true);
	QGLFormat::setDefaultFormat(fmt);
	std::cout << "opengl things" << std::endl;
		
	Main_window w(800,600,app_name);
	w.show();
	return app.exec();
}