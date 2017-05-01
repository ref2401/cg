echo off

call "c:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"

set dir_bin_debug=bin\\debug
set dir_bin_release=bin\\release
set dir_extern_dll=extern\\_dll
set dir_assimp=%dir_extern%\assimp
set dir_assimp_vs_solution=%dir_assimp%\_vs_solution


echo [copy dependencies]
echo:
RD /S /Q %dir_bin_debug% %dir_bin_release% %dir_assimp_vs_solution% %dir_assimp_vs_solution%
md %dir_bin_debug%
md %dir_bin_release%
md %dir_assimp_vs_solution%

for %%i in (.\\%dir_extern_dll%\\debug\\*) do (
	copy /Y /b %%i %dir_bin_debug%\\ >nul
)

for %%i in (.\\%dir_extern_dll%\\release\\*) do (
	copy /Y /b %%i %dir_bin_release%\\ >nul
)

echo [fetch submodules]
echo:
git submodule init
git submodule update
sleep 2

echo [init assimp: library]
echo:
pushd %dir_assimp_vs_solution%
	cmake ..
popd

pause