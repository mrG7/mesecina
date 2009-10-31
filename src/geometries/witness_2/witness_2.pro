MESECINAVERSION = witness-2
    
#1. create project once with the GUID line commented, 
#2. copy the GUID from the created project file here and uncomment line
win32 {
	GUID = {5D8E473E-AD0F-3D0A-88CA-489734F004C0}
}	

# own geometry
SOURCES += *.cpp

HEADERS += *.h \
		   layers/*.h 

include(../geometry_2.pro)



