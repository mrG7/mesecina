# other code for linking together GL_widget and Geometry
HEADERS += ../../geometries/*.h 
SOURCES += ../../geometries/*.cpp 


# project include directories
INCLUDEPATH += ..\..\
INCLUDEPATH += ..\..\..\include

QMAKE_LIBDIR += ..\..\..\lib

# general qt setup
CONFIG += qt
QT += opengl
DEFINES += MESECINA_GEOMETRY_LIBRARY




win32 {
	# compile visual studio project file
	TEMPLATE = vclib

	# vcproj file should have filters as directories
	CONFIG -= flat

	# cgal and boost include
	INCLUDEPATH += ..\..\..\include
	INCLUDEPATH += ..\..\..\include\dionysus
	INCLUDEPATH += ..\..\..\include\Moebius_diagram_2\include	
	INCLUDEPATH += $(CGAL_DIR)\include
	INCLUDEPATH += $(CGAL_DIR)\auxiliary\gmp\include
	INCLUDEPATH += $(CGAL_DIR)\auxiliary\taucs\include
	INCLUDEPATH += $(BOOST_ROOT)

	# cgal and boost lib
	QMAKE_LIBDIR += $(CGAL_DIR)\lib
	QMAKE_LIBDIR += $(CGAL_DIR)\auxiliary\gmp\lib
	QMAKE_LIBDIR += $(CGAL_DIR)\auxiliary\taucs\lib
	QMAKE_LIBDIR += $(BOOST_ROOT)\lib
	
	LIBS += glew32.lib
	
	
	MOC_DIR = ..\..\..\win\moc-$$MESECINAVERSION
	RCC_DIR = ..\..\..\win\rcc-$$MESECINAVERSION
	DESTDIR = ..\..\..\lib


	OBJECTS_DIR = ..\..\..\win\release\obj-$$MESECINAVERSION
	DLLDESTDIR = ..\..\..\win\release	
	
	CONFIG(debug, debug|release) {
		OBJECTS_DIR = ..\..\..\win\debug\obj-$$MESECINAVERSION
		DLLDESTDIR = ..\..\..\win\debug	
	}
	
	CONFIG(debug, debug|release) {
		LIBS += mesecina_staticd.lib
	} else {
		LIBS += mesecina_static.lib
	}
	
	QMAKE_CXXFLAGS += /wd4396	
	
	QMAKE_LFLAGS_WINDOWS += /MACHINE:X86
}

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,_debug)
    else: TARGET = $$join(TARGET,,,d)
}

