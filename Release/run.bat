@echo off
cd /d "%~dp0"

if exist "..\build\Release\HDR_ISP.exe" (
    copy /Y "..\build\Release\HDR_ISP.exe" "HDR_ISP.exe" >nul
)

"%~dp0HDR_ISP.exe" ../cfgs/isp_config_cannon.json
