#!/bin/sh
echo "Solving:
z = 9x1 + 10x2 + 16x3.
Constraints:
1) 18x1 + 15x2 + 12x3 <= 360,
2) 6x1  + 4x2  + 8x3 <= 192,
3) 5x1  + 3x2  + 3x3 <= 180.
"
make;
cat sample_input.txt | ./simplex.bin;

