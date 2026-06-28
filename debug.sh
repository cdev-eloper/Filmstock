cd src

if java -jar ../bin/Filmstock.jar -debug -o 1 -build main.fss ../out/build.filmstock; then
    read -p "Press Enter to continue..."

    cd ../bin
    ./runner ../out/build.filmstock
else
    echo "Error: Java build failed. Exiting script."
    exit 1
fi
