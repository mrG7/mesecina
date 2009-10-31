cd ..\include\QGLViewer
qmake -t vclib -o qglviewer.vcproj
cd ..\..\src
qmake mesecina3d.pro -o mesecina3d.vcproj
qmake mesecina_static.pro -o mesecina_static.vcproj
qmake gl_widget_3.pro -o gl_widget_3.vcproj
cd geometries\mesh_view_3
qmake mesh_view_3.pro -o mesh_view_3.vcproj
cd ..\..\geometries\power_crust_3
qmake power_crust_3.pro -o power_crust_3.vcproj
cd ..\..\geometries\union_of_balls_3
qmake union_of_balls_3.pro -o union_of_balls_3.vcproj
cd ..\..\geometries\skin_surface_3
qmake skin_surface_3.pro -o skin_surface_3.vcproj
cd ..\..\geometries\medial_explore_3
qmake medial_explore_3.pro -o medial_explore_3.vcproj
cd ..\..\geometries\moebius_3
qmake moebius_3.pro -o moebius_3.vcproj
cd ..\..\..\win

@IF NOT errorlevel 1 GOTO end
@PAUSE

:end