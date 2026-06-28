@echo off

cd src

if exist ..\out\* del /q ..\out\*

java -jar ..\bin\Filmstock.jar -no-info -O 3 -build main.fss ..\out\build.filmstock && (
    cd ..\bin
    runner.exe ..\out\build.filmstock
)

cd ..
pause
