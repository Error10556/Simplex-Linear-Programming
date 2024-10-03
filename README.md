### Simplex solver

#### Running the program in interactive mode

Use `make run` or `./run.sh`

#### Running a test

Use `./run_test.sh 2` to run the second test. Substiture the `2` with a number
from 1 to 5 to run that test.

#### Solving the linear programming problem

Use `./run_problem.sh` to see the solution to the selected real-world problem.

You may find the input data in the file `problem_input.txt`.

#### Makefile

The `make` utility supports the following commands:

1. `make compile` (default): build the executable `simplex.bin`.
2. `make run`: build and run the program `simplex.bin`.
3. `make clean`: remove the executable `simplex.bin`.

