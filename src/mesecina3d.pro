MESECINAVERSION = 3d
include(mesecina_common.pro)

# project source and header files

SOURCES += console/Console_run_3.cpp


HEADERS += console/Console_run_3.h
HEADERS += 3d/*.h

DEFINES += MESECINA_3D
# project include directories

INCLUDEPATH += 3d

RESOURCES += mesecina_3.qrc
RC_FILE = mesecina.rc

# general qt setup
CONFIG += qt
QT += opengl xml
QMAKE_CXXFLAGS_DEBUG += /Fdmesecina3d.pdb




win32 {
	# linker flag
	QMAKE_LFLAGS_WINDOWS += /SUBSYSTEM:WINDOWS
	
	CONFIG(debug, debug|release) {
		LIBS += medial_explore_3d.lib
		LIBS += moebius_3d.lib		
		LIBS += skin_surface_3d.lib	
		LIBS += union_of_balls_3d.lib	
		LIBS += power_crust_3d.lib
		LIBS += mesh_view_3d.lib
		LIBS += gl_widget_3d.lib
		LIBS += QGLViewer2d.lib
	} else {
		LIBS += medial_explore_3.lib
		LIBS += moebius_3.lib	
		LIBS += skin_surface_3.lib	
		LIBS += union_of_balls_3.lib	
		LIBS += power_crust_3.lib
		LIBS += mesh_view_3.lib
		LIBS += gl_widget_3.lib
		LIBS += QGLViewer2.lib
	}	
	
#	LIBS += QGLViewer2.lib
	GUID = {A587EAEE-E6C3-37B6-9A7D-77F1CDD643DC}
}	