cd ..\src
qmake mesecina2d.pro -o mesecina2d.vcproj
qmake mesecina_static.pro -o mesecina_static.vcproj
qmake gl_widget_2.pro -o gl_widget_2.vcproj
cd geometries\power_crust_2
qmake power_crust_2.pro -o power_crust_2.vcproj
cd ..\union_of_balls_2
qmake union_of_balls_2.pro -o union_of_balls_2.vcproj
cd ..\moebius_2
qmake moebius_2.pro -o moebius_2.vcproj
cd ..\second_voronoi_2
qmake second_voronoi_2.pro -o second_voronoi_2.vcproj
cd ..\scale_analysis_2
qmake scale_analysis_2.pro -o scale_analysis_2.vcproj
cd ..\witness_2
qmake witness_2.pro -o witness_2.vcproj
cd ..\..\..\win

@IF NOT errorlevel 1 GOTO end
@PAUSE

:end