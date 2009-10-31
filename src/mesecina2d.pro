MESECINAVERSION = 2d
include(mesecina_common.pro)

# core source and header files
SOURCES += console/Console_run_2.cpp

HEADERS += console/Console_run_2.h
HEADERS += 2d/*.h

# geometry
# SOURCES += geometries/power_crust_2/*.cpp

# HEADERS += geometries/power_crust_2/*.h \
#		   geometries/power_crust_2/layers/*.h 


# project include directories

INCLUDEPATH += 2d

RESOURCES += mesecina_2.qrc
RC_FILE = mesecina.rc

# general qt setup
CONFIG += qt
QT += opengl
QMAKE_CXXFLAGS_DEBUG += /Fdmesecina2d.pdb

win32 {
	# linker flag
	QMAKE_LFLAGS_WINDOWS += /SUBSYSTEM:WINDOWS
	
	GUID = {BF9010AC-D055-334A-942A-126560118DE3}

	CONFIG(debug, debug|release) {
		LIBS += witness_2d.lib
		LIBS += second_voronoi_2d.lib
		LIBS += power_crust_2d.lib
		LIBS += union_of_balls_2d.lib		
		LIBS += moebius_2d.lib	
		LIBS += gl_widget_2d.lib
	} else {
		LIBS += witness_2.lib
		LIBS += second_voronoi_2.lib
		LIBS += power_crust_2.lib
		LIBS += union_of_balls_2.lib		
		LIBS += moebius_2.lib	
		LIBS += gl_widget_2.lib
	}

}	