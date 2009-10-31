# other code for linking together GL_widget and Geometry
HEADERS += gui/app/static/*.h

SOURCES += gui/app/static/*.cpp



# project include directories

INCLUDEPATH += ..\include

QMAKE_LIBDIR += ..\lib

# general qt setup
CONFIG += qt
QT += opengl
DEFINES += MESECINA_BRIDGE
QMAKE_CXXFLAGS_DEBUG += /Fdmesecina_static.pdb

MESECINAVERSION = static


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
	
	GUID = {B98DF31E-7EB2-3FFB-8330-47A9DFA85143}
}	