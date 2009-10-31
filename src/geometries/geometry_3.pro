INCLUDEPATH += ..\..\3d

QT += opengl xml
DEFINES += MESECINA_3D

win32 {
	CONFIG(debug, debug|release) {
		LIBS += gl_widget_3d.lib
		LIBS += QGLViewer2d.lib
	} else {
		LIBS += gl_widget_3.lib
		LIBS += QGLViewer2.lib
	}
}

include(../geometry_common.pro)
