## CS 142B: Language Processor (Compiler) Construction

### TODO
##### Note (prio):
1. create a separate project to test the parser
2. copy & paste the parser code
3. verify the [symbolTable] && [ir]
    - create a program that gets the dot visualization of the [ir]
~ (final note): build the functionality/grammar of the parser up incrementally (advice from prof). what I mean by this is to first do EVERYTHING for variables and basic ops[+,-,*,/] only. build up the grammar in a way such that you will be able to add onto it later (because you will be adding quite literally an entire programming language!)

##### Note (Single-Static-Assignment):
- SSA is a property of IR
- SSA: requires every variable to be assigned exactly once and defined before use
- use-definition chains are explicity defined in SSA (meaning each time you define a new variable, even if it's mapped to the same value, it will have it's own separate mapping)
- read more here: https://en.wikipedia.org/wiki/Static_single-assignment_form

##### Note (symbol table):
- ds used to store identifiers (var-names)
    1. define a struct for identifiers (or use the name itself if all you need is the value)
    2. use unordered_map<,> to create the symbol table
    3. each time a new identifier is encountered, add it into the table. Handle existing cases accordingly (if [identifier] already exists in table prior to insert -> update!).
- storing & comparing strings is less efficient compared to integers 
    - turn identifier names into integers with a hash()!

##### Note (registers):
- variables are mapped to registers
- a register contains 2 elements:
    1. kind: the variable stored in Register [var]
    2. var: the register number that stores [kind]
- a variables name should be hashed into a hash table

##### Note (tips for optimization):
- definitely implement:
    - register allocation (welsh-powell graph coloring)
        - https://www.geeksforgeeks.org/welsh-powell-graph-colouring-algorithm/#
- consider implementing:
    - reaching definition analysis (wiki)
        - https://en.wikipedia.org/wiki/Reaching_definition
- etc:
  * Most common way to implement a compiler? 
    - Produce machine code directly, by including back-ends for each instruction set.
        - https://stackoverflow.com/questions/3080213/does-a-compiler-have-an-assembler-too
  * install yasm (assembler)
    - [brew install yasm] https://formulae.brew.sh/formula/yasm
  * compiler walkthrough in cpp:
    - [github] https://github.com/orosmatthew/hydrogen-cpp
    - [youtube] https://www.youtube.com/watch?v=vcSijrRsrY0&t=198s&ab_channel=Pixeled
    



### Questions
"coming up with virtual register numbers is good enough"
- does this mean our "registers" can look as follows?
    Class Register {
        int kind; // const var
        int var; // const reg num
    }



### What is a Compiler?
- a program designed to turn the source code into a lower level representation 
    - Source Code -> Assembly Code -> Binary Code (executable)
        - we do NOT need to handle linking!
    - compiler reads the source code and turns it into a .asm file
        - ex. *.cpp, *.py turns into a .asm file
    - assembler reads the .asm file and turns it into a .o file
        - .asm file turns into a .o file
- Structure
    1. Frontend
        1. Create a Lexical Tokenizer
        2. Create a Parser -> IR
    2. Backend
        1. "Lowering": Optimize IR -> *.o (optimization step)
            - register allocation / optimization (graph coloring)

