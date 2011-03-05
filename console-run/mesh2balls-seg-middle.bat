for /f %%i In ('dir /b ..\..\models\segbenchmark\*.off') DO call mesh2balls-seg.bat %%i 25-0.015-0.0025-4-1.ini

