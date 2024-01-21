@echo off

rem clang++ -O3 -march=native -DTRACY_ENABLE -DTRACY_NO_EXIT=1 -c src/tracy-0.7.8/TracyClient.cpp -static

clang -std=c18 -O3 -march=native -fopenmp src/main.c -o main.exe
main.exe
