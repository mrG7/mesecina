INCLUDEPATH += ..\..\2d

win32 {
	CONFIG(debug, debug|release) {
		LIBS += gl_widget_2d.lib
	} else {
		LIBS += gl_widget_2.lib
	}
}

include(../geometry_common.pro)
