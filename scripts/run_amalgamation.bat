@echo off
setlocal

set CALLER_DIR=%cd%

set SCRIPT_PATH=%~dp0
set AMALGAMATION_TOOL_DIR=%SCRIPT_PATH%\..\tool\amalgamation
cd %SCRIPT_PATH%\..

python %AMALGAMATION_TOOL_DIR%\amalgamate.py -c %AMALGAMATION_TOOL_DIR%\fkYAML.json -s . --verbose=yes

if %errorlevel% == 0 (
    echo Amalgamation succeeded!
) else (
    echo Amalgamation failed!
)

cd %CALLER_DIR%
