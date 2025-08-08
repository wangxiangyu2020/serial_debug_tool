@echo off
echo Minimal IKUN deployment...

set QT_DIR=D:\software\Qt\6.5.3\msvc2019_64
set BUILD_DIR=Release
set DEPLOY_DIR=iKUN

if exist %DEPLOY_DIR% rmdir /s /q %DEPLOY_DIR%
mkdir %DEPLOY_DIR%

copy "%BUILD_DIR%\IKUN.exe" "%DEPLOY_DIR%\"

REM Ultra minimal deployment
"%QT_DIR%\bin\windeployqt.exe" ^
    --webenginewidgets ^
    --release ^
    --no-translations ^
    --no-system-d3d-compiler ^
    --no-compiler-runtime ^
    --force ^
    "%DEPLOY_DIR%\IKUN.exe"

echo Minimal deployment complete!
pause