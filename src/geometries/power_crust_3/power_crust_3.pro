MESECINAVERSION = power-crust-3
    
#1. create project once with the GUID line commented, 
#2. copy the GUID from the created project file here and uncomment line
win32 {
	GUID ={E3E8D920-B45E-3C09-870C-AC34B435C9D4}
}	

# own geometry
SOURCES += *.cpp

HEADERS += *.h 
HEADERS += layers/*.h 

include(../geometry_3.pro)



