@echo off
setlocal

set CALLER_DIR=%cd%

set SCRIPT_DIR=%~dp0
cd $SCRIPT_DIR%\..

set SINGLE_HEADER_PATH=%SCRIPT_DIR%\..\single_include\fkYAML\node.hpp

move %SINGLE_HEADER_PATH% %SINGLE_HEADER_PATH%~ > nul
call %SCRIPT_DIR%\run_amalgamation.bat > nul

fc %SINGLE_HEADER_PATH% %SINGLE_HEADER_PATH%~ > nul
if %errorlevel% == 0 (
    echo Amalgamation check passed!
) else (
    echo Amalgamation required!
    echo Please follow the guideline in the CONTRIBUTING.md file.
)

move %SINGLE_HEADER_PATH%~ %SINGLE_HEADER_PATH% > nul

cd %CALLER_DIR%
