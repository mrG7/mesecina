MESECINAVERSION = scale-analysis-2
    
#1. create project once with the GUID line commented, 
#2. copy the GUID from the created project file here and uncomment line
win32 {
	GUID = {EFAF7005-944D-39B4-A3E0-E49E52A2547D}
}	

# own geometry
SOURCES += *.cpp

HEADERS += *.h 
HEADERS += layers/*.h 

include(../geometry_2.pro)



