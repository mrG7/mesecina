# other code for linking together GL_widget and Geometry

HEADERS += gui/gl_widget/*.h \ 
			gui/gl_widget_3/*.h 
			
SOURCES += gui/gl_widget/*.cpp \ 
           gui/gl_widget_3/*.cpp 
			




# project include directories

INCLUDEPATH += ..\include
DEFINES += MESECINA_3D

QMAKE_LIBDIR += ..\lib

# general qt setup
CONFIG += qt
QT += opengl
QT += xml
DEFINES += MESECINA_GL_WIDGET
QMAKE_CXXFLAGS_DEBUG += /Fdgl_widget_3.pdb

INCLUDEPATH += 3d

MESECINAVERSION = gl_widget_3


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
		LIBS += QGLViewer2d.lib
	} else {
		LIBS += mesecina_static.lib
		LIBS += QGLViewer2.lib
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
	
	GUID = {A587EAEE-E6C3-37B6-9A7D-77F1CDD643DC}
}	