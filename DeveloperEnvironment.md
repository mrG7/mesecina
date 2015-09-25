# How to get Mesecina to Compile #

_**Disclaimer**. This is just a snapshot of a source code base from the daily development of Mesecina for internal use. Use at your own responsibility, it is possible that you discover or encounter strange behavior/bugs. I am happy if you report them back, send me comments or even code to improve Mesecina._


### Requirements ###

  1. OpenGL
  1. Boost 1.38.0 or newer
  1. gmp, mpfr, taucs (I use the ones from cgal windows installer)
  1. cgal 3.5 or newer
  1. qt 4.5.0 or newer

**Currently the Mesecina development is set up for Windows / Visual Studio 2008 only.** Since all project files are generated using qmake and all libraries are supported on linux and mac, as well, one should be able to compile Mesecina on other platforms, as well. I would be interested in other platform, especially mac, so I would be delighted if someone could help me set up the project files for unix systems.

### The structure of Mesecina ###

Mesecina is has a source code base which is shared by the 2d and 3d version of Mesecina. These are two different executables which have the same user interface except the viewing widget which is specialize for the 3d and 2d versions. The project files are in the qt's "pro" format but currently they were used to generate only the windows version, they might need customization for mac/linux platforms.

So there are two windows solution files. Both solutions are broken down to several projects/libraries and a main program. The main program is mesecina2d/mesecina3d, there is mesecina\_static shared by both versions and gl\_widget\_2/gl\_widget\_3 are the visualization widgets. The rest of the libraries are called _geometries_, you can think of those as libraries which contain geometric algorithms and provide visualization structures as layers. The main program will use the classes in the _geometry_ libraries to fill up the user interface.

### Windows step-by-step ###

  1. Install openGL, boost, gmp, mpfr, cgal, qt
  1. Set the following environment variables
    * BOOST\_ROOT = the directory where boost is (for example C:\boost\boost\_1\_38)
    * CGAL\_DIR = the directory where cgal is (for example C:\CGAL-3.4-win32)
    * add to PATH the C:\Qt\4.5.1-vc2008\qt\bin\ directory for qt
    * add to PATH the C:\CGAL-3.4-win32\auxiliary\gmp\lib directory for cgal gmp
    * make sure you have the visual studio 2008 bin directory in path in case you have more than one visual studio installation. For me it is C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin
  1. Run the win\update\_project\_file\_2d.bat to generate the 2d project files (similarly for 3d)
  1. Use the win\Mesecina2d.sln to open the Visual Studio 2008 solution. You might need to set the startup project to mesecina2d, but otherwise it should compile and run.