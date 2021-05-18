@echo off

clang -std=c18 -Ofast -march=native -fopenmp src/main.c -o main.exe