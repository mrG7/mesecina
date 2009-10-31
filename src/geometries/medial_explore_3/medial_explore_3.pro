MESECINAVERSION = medial-explore-3
    
#1. create project once with the GUID line commented, 
#2. copy the GUID from the created project file here and uncomment line
win32 {
	GUID = {9D4E7BB1-8393-3177-8659-9CAC419F02DB}
}	

# own geometry
SOURCES += *.cpp

HEADERS += *.h 
HEADERS += layers/*.h 

include(../geometry_3.pro)



