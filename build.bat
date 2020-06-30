@echo off
for /f "usebackq tokens=*" %%i in (`call "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
  set InstallDir=%%i
)
if exist "%InstallDir%\MSBuild\Current\Bin\MSBuild.exe" ("%InstallDir%\MSBuild\Current\Bin\MSBuild.exe" "vs\CodingTest.vcxproj" "/p:Configuration=Release" "/p:OutDir=../output/") else (echo Sorry, can not find Viual Studio installation path)
if exist "%InstallDir%\MSBuild\Current\Bin\MSBuild.exe" ("%InstallDir%\MSBuild\Current\Bin\MSBuild.exe" "vs\tests\tests.vcxproj" "/p:OutDir=../../output/") else (echo Sorry, can not find Viual Studio installation path)
