MESECINAVERSION = skin-surface-3
    
#1. create project once with the GUID line commented, 
#2. copy the GUID from the created project file here and uncomment line
win32 {
	GUID = {B3B0563E-06E7-3AED-A7E9-2BC4A8EC368D}
}	

# own geometry
SOURCES += *.cpp

HEADERS += *.h 
HEADERS += layers/*.h 

include(../geometry_3.pro)



