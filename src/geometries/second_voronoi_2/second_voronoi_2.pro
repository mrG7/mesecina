MESECINAVERSION = second-voronoi-2
    
#1. create project once with the GUID line commented, 
#2. copy the GUID from the created project file here and uncomment line
win32 {
	GUID = {DE054DF1-4295-3AE7-A372-45B98754728F}

}	

# own geometry
SOURCES += *.cpp

HEADERS += *.h \
		   layers/*.h 
		   
win32 {
	# linker flag
	QMAKE_LFLAGS_WINDOWS += /SUBSYSTEM:WINDOWS
	
	GUID = {BF9010AC-D055-334A-942A-126560118DE3}

	CONFIG(debug, debug|release) {
		LIBS += power_crust_2d.lib
		LIBS += witness_2d.lib
	} else {
		LIBS += power_crust_2.lib
		LIBS += witness_2.lib
	}
}

include(../geometry_2.pro)



