@echo off

rem clang -std=c18 -Og -march=native -fopenmp src/main.c -o main.exe

clang++ -O3 -march=native -DTRACY_ENABLE -DTRACY_NO_EXIT=1 -c src/tracy-0.7.8/TracyClient.cpp -static
clang -std=c18 -O3 -march=native -DTRACY_ENABLE -DTRACY_NO_EXIT=1 TracyClient.o -fblocks -fopenmp src/main.c -o main.exe
