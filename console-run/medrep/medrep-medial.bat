@echo off
rem Timestamp Generator
rem Parse the date (e.g., Fri 02/08/2008)
set cur_yyyy=%date:~10,4%
set cur_mm=%date:~4,2%
set cur_dd=%date:~7,2%

rem Parse the time (e.g., 11:17:13.49)
set cur_hh=%time:~0,2%
if %cur_hh% lss 10 (set cur_hh=0%time:~1,1%)
set cur_nn=%time:~3,2%
set cur_ss=%time:~6,2%
set cur_ms=%time:~9,2%
@echo on

rem Set the timestamp format
set timestamp=%cur_yyyy%%cur_mm%%cur_dd%-%cur_hh%%cur_nn%%cur_ss%%cur_ms%

set growth=%2
if "%growth%"=="" set growth=1.1

..\bin\mesecina3d --nogui -mode medrep-medial -input %1 -growth %growth% >log\medrep-medial_%1_%growth%_%timestamp%.txt
mkdir %1\log
move log\medrep-medial_%1_%growth%_%timestamp%.txt %1\log\medrep-medial_%1_%growth%_%timestamp%.txt


@echo off
rem Clear the environment variables
set cur_yyyy=
set cur_mm=
set cur_dd=
set cur_hh=
set cur_nn=
set cur_ss=
set cur_ms=
set timestamp=
@echo on
