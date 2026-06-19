set PATH=.;C:\Versa\gnucomp;C:\Versa\gnucomp\bin;%PATH%
if .==%GNUCOMP_PATH%. set GNUCOMP_PATH=C:\Versa\gnucomp
call switch.bat makefile makefile_V4K5K
rem make TARGET=v5400 -w --always-print-command
make TARGET=v5400 -w
pause
copy RC04EX.ppcout ..\obj
copy RC04EX.vec ..\obj

