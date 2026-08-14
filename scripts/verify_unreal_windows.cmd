@echo off
setlocal
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0verify_unreal_windows.ps1" %*
exit /b %ERRORLEVEL%
