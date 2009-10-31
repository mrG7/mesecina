MESECINAVERSION = moebius-2
    
#1. create project once with the GUID line commented, 
#2. copy the GUID from the created project file here and uncomment line
win32 {
	GUID ={412A5D6B-E25F-3939-ADF6-1D15AE356D8C}
}	

# own geometry
SOURCES += *.cpp

HEADERS += *.h \
		   layers/*.h 	   

include(../geometry_2.pro)



