set out=..\..\out\dist_L2OOGHelper

rem Copy EXE
echo Copy %1.exe...
copy /y %2\%1.exe %2\%out% > NUL

rem Copy PDB
echo Copy %1.pdb...
copy /y %2\%1.pdb %2\%out% > NUL

rem Copy scripts...
echo Copy scripts...
copy /y l2ooghelper\scripts\*.lua  %2\%out%\scripts > NUL
copy /y l2ooghelper\scripts\*.luac %2\%out%\scripts > NUL
copy /y l2ooghelper\scripts\*.html %2\%out%\scripts > NUL
copy /y l2ooghelper\sys_scripts\*.lua  %2\%out%\sys_scripts > NUL
copy /y l2ooghelper\sys_scripts\*.luac %2\%out%\sys_scripts > NUL

echo Results copied to dist directory [%out%]