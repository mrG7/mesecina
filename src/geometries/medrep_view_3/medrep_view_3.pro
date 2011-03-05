MESECINAVERSION = medrep-view-3
    
#1. create project once with the GUID line commented, 
#2. copy the GUID from the created project file here and uncomment line
win32 {
	GUID = {5877F7C4-652F-3CD8-A4EB-E7F4D9C14A3C}
}	

# own geometry
SOURCES += *.cpp

HEADERS += *.h 
HEADERS += layers/*.h 

include(../geometry_3.pro)



