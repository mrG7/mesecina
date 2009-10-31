MESECINAVERSION = moebius-3
    
#1. create project once with the GUID line commented, 
#2. copy the GUID from the created project file here and uncomment line
win32 {
	GUID = {B08D669C-CFD7-377B-B7DA-C6FCAAC30AE9}
}	

# own geometry
SOURCES += *.cpp

HEADERS += *.h 
# HEADERS += layers/*.h 

include(../geometry_3.pro)



