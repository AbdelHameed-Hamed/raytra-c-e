# General note keeping file
---
## Timings

This was done on a Windows 10 machine with an i5-3570k and 8GB ram. The target image is 640x360 with 64 samples per pixel.

-O3: ~74.50s
-Ofast: ~78.66s ??????????
-Ofast & -march=native: ~70.20s
-Ofast & -march=native & -fopenmp: ~23.45s