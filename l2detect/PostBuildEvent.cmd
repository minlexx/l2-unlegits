set out=..\..\out\dist_L2Detect

echo Copy DLL %2\%1.dll - %2\%out%
copy /y %2\%1.dll %2\%out%

echo Copy PDB %2\%1.pdb - %2\%out%
rem copy /y %2\%1.pdb %2\%out%
