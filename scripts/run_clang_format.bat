@echo off
setlocal

set CALLER_DIR=%cd%

set SCRIPT_PATH=%~dp0
cd %SCRIPT_PATH%

set ROOT_PATH=%SCRIPT_PATH%\..

if not exist .\venv_clang_format\Scripts\clang-format.exe (
    python -m venv .\venv_clang_format
    .\venv_clang_format\Scripts\pip.exe install clang-format==14.0.0
)

for /r %ROOT_PATH%\include %%f in (*.hpp) do (
    .\venv_clang_format\Scripts\clang-format.exe %%f -i
)

for /r %ROOT_PATH%\test %%f in (*.cpp) do (
    .\venv_clang_format\Scripts\clang-format.exe %%f -i
)

for /r %ROOT_PATH%\docs\examples %%f in (*.cpp) do (
    .\venv_clang_format\Scripts\clang-format.exe %%f -i
)

cd %CALLER_DIR%
