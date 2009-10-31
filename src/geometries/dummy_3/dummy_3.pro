MESECINAVERSION = dummy-3
    
#1. create project once with the GUID line commented, 
#2. copy the GUID from the created project file here and uncomment line
win32 {
#	GUID = {B3482D43-E588-37CE-9C28-1C73C785D822}  
}	

# own geometry
SOURCES += *.cpp

HEADERS += *.h 
# HEADERS += layers/*.h 

include(../geometry_3.pro)



