SOURCES += console/Console_logger.cpp console/Console_logger_buffer.cpp console/Console_run.cpp \
		   geometries/*.cpp \
		   gui/app/*.cpp \
#		   gui/gl_widget/*.cpp \
           gui/layer_management/*.cpp \
           main.cpp

HEADERS += console/Console_logger.h console/Console_logger_buffer.h console/Console_run.h \
		   geometries/*.h \
		   gui/app/*.h \
#		   gui/gl_widget/*.h \
           gui/layer_management/*.h 
		   


win32 {
	# compile visual studio project file
	TEMPLATE = vcapp
	
	DEFINES += BUILDDATE=\\\"$$system('echo %date%')\\\"

	# vcproj file should have filters as directories
	CONFIG -= flat
	
	DEFINES += _CRT_SECURE_NO_WARNINGS	

	# cgal and boost include
	INCLUDEPATH += ..\include
	INCLUDEPATH += ..\include\dionysus
	INCLUDEPATH += ..\include\Moebius_diagram_2\include
	INCLUDEPATH += $(CGAL_DIR)\include
	INCLUDEPATH += $(CGAL_DIR)\auxiliary\gmp\include
	INCLUDEPATH += $(CGAL_DIR)\auxiliary\taucs\include
	INCLUDEPATH += $(BOOST_ROOT)

	# cgal and boost lib
	QMAKE_LIBDIR += ..\lib
	QMAKE_LIBDIR += $(CGAL_DIR)\lib
	QMAKE_LIBDIR += $(CGAL_DIR)\auxiliary\gmp\lib
	QMAKE_LIBDIR += $(CGAL_DIR)\auxiliary\taucs\lib
	QMAKE_LIBDIR += $(BOOST_ROOT)\lib
	
	LIBS += glew32.lib
	
	
	MOC_DIR = ..\win\moc-$$MESECINAVERSION
	RCC_DIR = ..\win\rcc-$$MESECINAVERSION
	
	CONFIG(debug, debug|release) {
		OBJECTS_DIR = ..\win\debug\obj-$$MESECINAVERSION
		DESTDIR = ..\win\debug
		LIBS += mesecina_staticd.lib
	} else {
		OBJECTS_DIR = ..\win\release\obj-$$MESECINAVERSION
		DESTDIR = ..\win\release
		LIBS += mesecina_static.lib
	}	
	
	QMAKE_LFLAGS_WINDOWS += /MACHINE:X86
}