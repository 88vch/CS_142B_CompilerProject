## Parser Tests
### Current Status: [REFACTORING]
- [RESUME]: passing in current file tst/temp.ty throws a "LET ERRROR"
- [TODO]; 
    - [tokenize_statement()]: configure a deterministic way to decide when the statement is done (bc it's a non strictly-necessary terminating `;`)
    - get a working warmup P2 vzn
    - create a suite of tests to test P2 vzn
    - generate DOT from P2
##### NOTE: this branch should NEVER be merged with main! This branch was created specifically for testing.


### TODO
##### Assumptions Status=[IP, DONE, REMOVED];
[ASSUMPTION] spaces exist in between all tokens
- in the *.ty files
[ASSUMPTION] all recursive functions in Lexer.hpp
- assume all recursive functions handle whitespace cleanup [skip_whitespace()] before recursively calling another func
[DONE] Lexer.hpp::tokenizer()::determine_type=LET
- [update] REVISED: we actually will grab the entire expression, and pass it into an expr_tokenizer of some sort to handle it
- ASSUMPTION: the file to be passed in is a syntactically valid file
[REMOVED] Lexer.hpp::consume()
- ASSUMPTION: BUFF_SIZE is enough to fit every valid TOKEN
##### Note:
- Lexer.hpp: might have issue's trying to free() a nullptr
- Lexer.hpp: if we're parsing predefined keywords we should be hardcoding the size of the char's that we're looking through 
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
    1. define a struct/class for identifiers (or use the name itself if all you need is the value)
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
    - Lexer.hpp::tokenizer()
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
varDecl = “var” indent { “,” ident } “;”
- is [indent] misspelled to be [ident]???
