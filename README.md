# **Run**
**[1.18.2025]**
type `make` in the terminal to compile the program
to run the executable type `./bin/tiny`

`make clean` to remove result files

the [main.cpp] file has 2 run methods:
1) runOne=true - run one test file
    - defined in `in_f` [main.cpp::28]
2) runOne=false - run test suite
    - will parse through all files in [tst/] and generate respective DOTs