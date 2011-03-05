for /f %%i In ('dir /b ..\..\models\segbenchmark\*.off') DO call mesh2balls-seg.bat %%i 25-0.006-0.0012-4-1.ini

