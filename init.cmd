echo off

call "c:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"

set dir_bin_debug=bin_debug
set dir_bin_release=bin_release
set dir_bin_debug_libs=bin_debug_libs
set dir_bin_release_libs=bin_release_libs

set dir_extern=extern
set dir_assimp=%dir_extern%\assimp
set dir_assimp_vs_solution=%dir_assimp%\_vs_solution


echo [clear]
echo:
RD /S /Q %dir_bin_debug% %dir_bin_release% %dir_bin_debug_libs% %dir_bin_release_libs% %dir_assimp_vs_solution%

md %dir_bin_debug%
md %dir_bin_release%
md %dir_bin_debug_libs%
md %dir_bin_release_libs%
md %dir_assimp_vs_solution%

echo [fetch submodules]
echo:
git submodule init
git submodule update

echo [extern project: Assimp]
echo:
pushd %dir_assimp_vs_solution%

	echo [Assimp: [cmake vs2015 projects]]
	echo:	
	cmake ..

	echo [Assimp: [build debug]]
	echo:
	devenv Assimp.sln /build Debug

	:: NOTE(ref2401): Assimp does not build under Release mode
	::echo [Assimp: [build release]]
	::echo:
	::devenv Assimp.sln /build Release

popd

echo [Assimp: [copy libraries]]
copy /B /Y %dir_assimp_vs_solution%\code\Debug\assimp-vc140-mt.dll %dir_bin_debug%\assimp-vc140-mt.dll
copy /B /Y %dir_assimp_vs_solution%\code\Debug\assimp-vc140-mt.ilk %dir_bin_debug%\assimp-vc140-mt.ilk
copy /B /Y %dir_assimp_vs_solution%\code\Debug\assimp-vc140-mt.pdb %dir_bin_debug%\assimp-vc140-mt.pdb
copy /B /Y %dir_assimp_vs_solution%\code\Debug\assimp-vc140-mt.exp %dir_bin_debug_libs%\assimp-vc140-mt.exp
copy /B /Y %dir_assimp_vs_solution%\code\Debug\assimp-vc140-mt.lib %dir_bin_debug_libs%\assimp-vc140-mt.lib
copy /B /Y %dir_assimp_vs_solution%\contrib\zlib\Debug\zlibstaticd.lib %dir_bin_debug_libs%\zlibstaticd.lib
:: NOTE(ref2401): Assimp does not build under Release mode
::copy /B /Y %dir_assimp_vs_solution%\code\Release\assimp-vc140-mt.dll %dir_bin_release%\assimp-vc140-mt.dll
::copy /B /Y %dir_assimp_vs_solution%\code\Release\assimp-vc140-mt.ilk %dir_bin_release%\assimp-vc140-mt.ilk
::copy /B /Y %dir_assimp_vs_solution%\code\Release\assimp-vc140-mt.pdb %dir_bin_release%\assimp-vc140-mt.pdb
::copy /B /Y %dir_assimp_vs_solution%\code\Release\assimp-vc140-mt.exp %dir_bin_release_libs%\assimp-vc140-mt.exp
::copy /B /Y %dir_assimp_vs_solution%\code\Release\assimp-vc140-mt.lib %dir_bin_release_libs%\assimp-vc140-mt.lib
::copy /B /Y %dir_assimp_vs_solution%\contrib\zlib\Release\zlibstaticd.lib %dir_bin_release_libs%\zlibstaticd.lib

pause