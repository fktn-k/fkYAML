@echo off
setlocal

set CALLER_DIR=%cd%

set SCRIPT_DIR=%~dp0
cd $SCRIPT_DIR%\..

set NODE_HPP_PATH=%SCRIPT_DIR%\..\single_include\fkYAML\node.hpp
set FKYAML_FWD_HPP_PATH=%SCRIPT_DIR%\..\single_include\fkYAML\fkyaml_fwd.hpp

move %NODE_HPP_PATH% %NODE_HPP_PATH%~ > nul
move %FKYAML_FWD_HPP_PATH% %FKYAML_FWD_HPP_PATH%~ > nul

call %SCRIPT_DIR%\run_amalgamation.bat > nul

fc %NODE_HPP_PATH% %NODE_HPP_PATH%~ > nul
if %errorlevel% == 0 (
    echo Amalgamation check passed for node.hpp file!
) else (
    echo Amalgamation required for node.hpp file!
    echo Please follow the guideline in the CONTRIBUTING.md file.
)
move %NODE_HPP_PATH%~ %NODE_HPP_PATH% > nul

fc %FKYAML_FWD_HPP_PATH% %FKYAML_FWD_HPP_PATH%~ > nul
if %errorlevel% == 0 (
    echo Amalgamation check passed for fkyaml_fwd.hpp file!
) else (
    echo Amalgamation required for fkyaml_fwd.hpp file!
    echo Please follow the guideline in the CONTRIBUTING.md file.
)
move %FKYAML_FWD_HPP_PATH%~ %FKYAML_FWD_HPP_PATH% > nul

cd %CALLER_DIR%
