@echo off

cd src

java -jar ..\bin\Filmstock.jar -debug -o 1 -build main.fss ..\out\build.filmstock

if %errorlevel% neq 0 (
    echo Error: Java build failed. Exiting script.
    pause
    exit /b 1
)

pause

cd ..\bin
runner.exe ..\out\build.filmstock

cd ..
pause
