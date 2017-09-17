@echo off

rem overwrite here if you like
set emsdk=%emsdk%
set bb_playbook_ndk=%bb_playbook_ndk%
set bb_bb10_ndk=%bb_bb10_ndk%
rem -----------------------------------

set scriptdir=%~dp0

if "%emsdk%"=="" (echo no_emscripten)else (
	echo found_emscripten
	cd %emsdk%
	call emsdk_env.bat
	cd %scriptdir%
)

rem call the unix-script that should work on win as well
tools\win32\bash\bash.exe package_all_linux.sh