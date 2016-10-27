@echo off

if not exist w:\build mkdir w:\build
pushd w:\build

set Release=0
set OneFile=0

if %Release%==1 (
set Internal=0
set DebugDisplay=0
set Slow=0
set Rumble=1
set Optimize=/O2
) else (
set Internal=1
set DebugDisplay=1
set Slow=1
set Rumble=0
set Optimize=/Od
)

set DataPath=\"..\\q3\\data\"

set CommonCompilerFlags=%Optimize% -DONE_FILE=%OneFile% -DQ3_RUMBLE=%Rumble% -DQ3_INTERNAL=%Internal% -DQ3_SLOW=%Slow% -DQ3_DEBUG_DISPLAY=%DebugDisplay% -DDATA_PATH=%DataPath% /Zi /fp:fast /W4 /WX /nologo /wd4100 /wd4065 /wd4189 /wd4201 /wd4505
set CommonLinkerFlags=/DEBUG /WX /NOLOGO /OPT:REF

if %OneFile%==0 (
if exist q3.dll move q3.dll temp.dll 1> nul
echo BUILDING > pdb.lock
if %Internal%==1 (
cl %CommonCompilerFlags% /LD w:\q3\code\q3.cpp /link %CommonLinkerFlags% /EXPORT:GameUpdateAndRender /EXPORT:GameGetSoundSamples /EXPORT:DebugCollate /PDB:q3_%RANDOM%.pdb
) else (
cl %CommonCompilerFlags% /LD w:\q3\code\q3.cpp /link %CommonLinkerFlags% /EXPORT:GameUpdateAndRender /EXPORT:GameGetSoundSamples /PDB:q3_%RANDOM%.pdb
)
del pdb.lock
)

cl %CommonCompilerFlags% w:\q3\code\win32_q3.cpp /link %CommonLinkerFlags% user32.lib gdi32.lib winmm.lib opengl32.lib

cl %CommonCompilerFlags% -Od w:\q3\code\asset_packer.cpp /link %CommonLinkerFlags%
REM w:\build\asset_packer.exe

if %OneFile%==1 (
cl %CommonCompilerFlags% -Od w:\q3\code\exe_packer.cpp /link %CommonLinkerFlags%
w:\build\exe_packer.exe
)

popd
