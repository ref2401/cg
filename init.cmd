echo off

call "c:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"

set dir_bin_debug=bin_debug
set dir_bin_release=bin_release
set dir_extern=extern
set dir_assimp=%dir_extern%\assimp
set dir_assimp_vs_solution=%dir_assimp%\_vs_solution


echo [1/8 clear]
echo:
RD /S /Q %dir_bin_debug% %dir_bin_release% %dir_assimp_vs_solution%

echo [2/8 fetch submodules]
echo:
git submodule init
git submodule update
sleep 2

echo[3/8 create folders]
echo:
md %dir_bin_debug%
md %dir_bin_release%
md %dir_assimp_vs_solution%

echo [4/8 extern project: Assimp]
echo:
pushd %dir_assimp_vs_solution%

	echo [5/8 Assimp: [cmake vs2015 projects]]
	echo:	
	cmake ..

	echo [6/8 Assimp: [build debug]]
	echo:
	devenv Assimp.sln /build Debug

	echo [7/8 Assimp: [build release]]
	echo:
	devenv Assimp.sln /build Release

popd

echo [8/8 Assimp: [copy libraries]]
copy /B /Y %dir_assimp_vs_solution%\code\Debug\assimp-vc140-mt.dll %dir_bin_debug%\assimp-vc140-mt.dll
copy /B /Y %dir_assimp_vs_solution%\code\Debug\assimp-vc140-mt.ilk %dir_bin_debug%\assimp-vc140-mt.ilk
copy /B /Y %dir_assimp_vs_solution%\code\Debug\assimp-vc140-mt.pdb %dir_bin_debug%\assimp-vc140-mt.pdb
copy /B /Y %dir_assimp_vs_solution%\code\Debug\assimp-vc140-mt.exp %dir_bin_debug%\assimp-vc140-mt.exp
copy /B /Y %dir_assimp_vs_solution%\code\Debug\assimp-vc140-mt.lib %dir_bin_debug%\assimp-vc140-mt.lib
copy /B /Y %dir_assimp_vs_solution%\contrib\zlib\Debug\zlibstaticd.lib %dir_bin_debug%\zlibstaticd.lib
copy /B /Y %dir_assimp_vs_solution%\code\Release\assimp-vc140-mt.dll %dir_bin_release%\assimp-vc140-mt.dll
copy /B /Y %dir_assimp_vs_solution%\code\Release\assimp-vc140-mt.exp %dir_bin_release%\assimp-vc140-mt.exp
copy /B /Y %dir_assimp_vs_solution%\code\Release\assimp-vc140-mt.lib %dir_bin_release%\assimp-vc140-mt.lib
copy /B /Y %dir_assimp_vs_solution%\contrib\zlib\Release\zlibstaticd.lib %dir_bin_release%\zlibstaticd.lib

pause