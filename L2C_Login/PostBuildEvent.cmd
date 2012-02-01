rem %1 = L2Serverd_d
rem %2 = output exe dir: m:\dev\l2\servers\l2c\_build\L2Serverd_Debug\
rem
rem ..\..\dist

rem echo 1 = %1
rem echo 2 = %2

set out=..\..\out\dist_l2c

echo Copy EXE %2\%1.exe - %2\%out%
copy /y %2\%1.exe %2\%out%

echo Copy PDB %2\%1.pdb - %2\%out%
copy /y %2\%1.pdb %2\%out
