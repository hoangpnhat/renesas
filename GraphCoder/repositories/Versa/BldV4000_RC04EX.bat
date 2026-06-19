set PATH=.;C:\Versa\gnucomp;C:\Versa\gnucomp\bin;%PATH%
if .==%GNUCOMP_PATH%. set GNUCOMP_PATH=C:\Versa\gnucomp
del *.lzh
del *.o
del *.i
del *.err
del *.hpg
del *.cof
del *.vec
del *.ppcdbg
del *.ppcout
del *.pat
del *.bak
del *.dbg
del *.dep
del *.vdb
del *.lst
del *.tmp
del *.pj
del *.dt
del *.tx
del _vk_temp.c
del makefile
del RC04EX.bld
del TEMP.BLD
del /F /Q objs\*.*
call switch.bat makefile makefile_V4K5K
make -w
pause
copy RC04EX.cof ..\obj
copy RC04EX.vec ..\obj