cd src

rm -rf ./out/*

java -jar ../bin/Filmstock.jar -no-info -O 3 -build main.fss ../out/build.filmstock && \
cd ../bin && \
./runner ../out/build.filmstock
