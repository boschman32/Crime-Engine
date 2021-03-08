@echo off
echo "Start build..."
SET MSBuild= "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\amd64\msbuild.exe"

%MSBuild% "%WORKSPACE%\CrimeEngine.sln" /p:Configuration=%1 /p:Platform=%2
@echo %off