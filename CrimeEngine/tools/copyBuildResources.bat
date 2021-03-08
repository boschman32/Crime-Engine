@echo off
@echo.%3 | findstr /C:"Jenkins">nul && (
   REM Don't copy
   @echo "Don't copy on Jenkins!"
   goto:eof
) || (
    goto copy_files
)

:copy_files

SET "solutionDir=%1"
SET "fromProject=%2"
SET "outDir=%3"
SET "projectName=%4"
SET "configuration=%5"
echo -- Copy build resources --
echo solution: %solutionDir%
echo outdir: %outDir%
echo projectname: %projectName%
echo "from: %solutionDir%%fromProject%\Libs"
echo "to: %solutionDir%%projectName%\%outDir% on configuration %configuration%"

REM @echo.%configuration% | findstr /C:"Debug">nul && (
    REM xcopy "%solutionDir%Helios\Libs\FMOD\lib\fmodL.dll" %solutionDir%bin\%outDir%\%projectName%\ /y /d /r /I
    REM xcopy "%solutionDir%Helios\Libs\FMOD\lib\fmodstudioL.dll" %solutionDir%bin\%outDir%\%projectName%\ /y /d /r /I
REM ) || (
    REM xcopy "%solutionDir%Helios\Libs\FMOD\lib\fmod.dll" %solutionDir%bin\%outDir%\%projectName%\ /y /d /r /I
    REM xcopy "%solutionDir%Helios\Libs\FMOD\lib\fmodstudio.dll" %solutionDir%bin\%outDir%\%projectName%\ /y /d /r /I
REM )

xcopy "%solutionDir%%fromProject%\Libs\Optick\lib\x64\%configuration%\OptickCore.dll" %solutionDir%%projectName%\%outDir% /y /d /r /I
xcopy "%solutionDir%%fromProject%\Data" %solutionDir%%projectName%\%outDir%\Data\ /y /r /s /i
xcopy "%solutionDir%%projectName%\Data" %solutionDir%%projectName%\%outDir%\Data\ /y /r /s /i
@echo off