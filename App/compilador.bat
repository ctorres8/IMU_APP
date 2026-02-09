@echo off
title Compilador de IMU Monitor
echo ===========================================
echo   LIMPIANDO VERSIONES ANTERIORES...
echo ===========================================

:: Borrar carpeta 'dist' si existe
if exist dist (
    echo Eliminando carpeta dist...
    rd /s /q dist
)

:: Borrar carpeta 'build' si existe
if exist build (
    echo Eliminando carpeta build...
    rd /s /q build
)

:: Borrar el archivo .spec (opcional, pero recomendado para cambios de config)
if exist "IMU_Monitor.spec" (
    echo Eliminando archivo de configuracion .spec...
    del /q "IMU_Monitor.spec"
)

echo.
echo ===========================================
echo   INICIANDO PYINSTALLER...
echo ===========================================

:: Usamos --collect-all para asegurar que numpy y pyqtgraph lleven todo lo necesario
pyinstaller --noconsole --onefile ^
    --collect-all numpy ^
    --collect-all pyqtgraph ^
    --name "IMU_Monitor" main.py

echo.
echo ===========================================
echo   PROCESO FINALIZADO
echo ===========================================
pause