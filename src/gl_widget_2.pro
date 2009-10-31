# other code for linking together GL_widget and Geometry

HEADERS += gui/gl_widget/*.h \ 
			gui/gl_widget_2/*.h 
			
SOURCES += gui/gl_widget/*.cpp \ 
           gui/gl_widget_2/*.cpp 
SOURCES += ../res/Qt_widget_xpm_icons.cpp
			




# project include directories

INCLUDEPATH += ..\include

QMAKE_LIBDIR += ..\lib

# general qt setup
CONFIG += qt
QT += opengl
DEFINES += MESECINA_GL_WIDGET
QMAKE_CXXFLAGS_DEBUG += /Fdgl_widget_2.pdb

INCLUDEPATH += 2d

MESECINAVERSION = gl_widget_2


win32 {
	# compile visual studio project file
	TEMPLATE = vclib

	# vcproj file should have filters as directories
	CONFIG -= flat

	# cgal and boost include
	INCLUDEPATH += $(CGAL_DIR)\include
	INCLUDEPATH += $(CGAL_DIR)\auxiliary\gmp\include
	INCLUDEPATH += $(BOOST_ROOT)

	# cgal and boost lib
	QMAKE_LIBDIR += $(CGAL_DIR)\lib
	QMAKE_LIBDIR += $(CGAL_DIR)\auxiliary\gmp\lib
	QMAKE_LIBDIR += $(BOOST_ROOT)\lib
	
	LIBS += glew32.lib
	
	
	MOC_DIR = ..\win\moc-$$MESECINAVERSION
	RCC_DIR = ..\win\rcc-$$MESECINAVERSION
	DESTDIR = ..\lib


	OBJECTS_DIR = ..\win\release\obj-$$MESECINAVERSION
	DLLDESTDIR = ..\win\release	
	
	CONFIG(debug, debug|release) {
		OBJECTS_DIR = ..\win\debug\obj-$$MESECINAVERSION
		DLLDESTDIR = ..\win\debug
		LIBS += mesecina_staticd.lib
	} else {
		LIBS += mesecina_static.lib
	}
	
	QMAKE_LFLAGS_WINDOWS += /MACHINE:X86
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,_debug)
    else: TARGET = $$join(TARGET,,,d)
}



win32 {
	# linker flag
	# QMAKE_LFLAGS_WINDOWS += /SUBSYSTEM:WINDOWS
	
	GUID = {BD4B1EC7-7B79-38E7-B24D-D5744116FBA7}
}	