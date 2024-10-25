# **To Do;**
**[10.25.2024]**
generate dot(png): dot -Tpng graph.dot -o graph.png

**[10/03/2024]**
linked-list
- most recenly added SSA-stmt should be searched first (i.e. placed at head), OR maintain a ptr at end of each LL (turn it into a LL which iterates from tail to head) and search from tail

if-statement
- create empty [phi():SSA] at the start
- each time we have a new SSA, update the phi based on which path (if/else) SSA belongs to
- last SSA for [if-path] should have a branch (to phi if else exists)

**[07/28/2024]**
Think we should figure out a way to ensure that (the previous [SSA] we're using) definitely DOM's the [current/new SSA] being generated
**[07/25/2024]**
true, we should return [SSA] instead of [Res::Result]
**[07/23/2024]**
we shouldn't be returning [Res::Result] rather [SSA]?
**[07/22/2024]**
[Remark]: optimizing compiler will require us to remove SSA instrs if possible, won't this cause a change in the SSA-instr-num (we're currently js using size of arr rn)
[Question]: do we return from [p_expr(), p_term(), etc...] as SSA?
    Or do we actually compute the value and return the newly computed value (i.e. 5-4 return's 1 or the SSA instr with the `sub` operation?)
    - I'm leaning towards the later: returning the SSA instr with sub op 

**[07/16/2024]**
[ToDo]: first generate all SSA using Recursive Descent
- [Parser.*] use [OldParser.*] for base code (to revise!)
    - [Parser]: SSA only holds operations (store values yourself)
        - i.e. var/func values/declarations
    - [Parser] uses: {LinkedList.hpp, Result.hpp, SSA.hpp}

[07/17/2024]: assumtion that we can skip the AST generation step and go straight into Recursive-Descent to parse the tokens into SSA whilst doing error handling
**generate [AST]**
[Parser.*]: [parse_first()] generate's AST
- Recursive Descent LL(1) Parsing -> AST
    - handle's computation's during parsing (whatever this means)

- following Upenn CIS 341 Lectures
Recursive Descent LL(1) Parsing to Generate AST
    - https://www.youtube.com/watch?v=SToUyjAsaFk&ab_channel=hhp3
    - https://www.youtube.com/watch?v=bxpc9Pp5pZM&ab_channel=Computerphile
        (haven't watched this one yet)

<br>

## **Notes**
### SymbolTable::numbers

**[06/30/2024; 23:30]**
<br>

- the point was assuming we cared about order, 
    but we don't because we're only using [this], [identifiers] and [keywords] as lookup's for 
        the [key of SymbolTable::symbol_table]
- QUESTION: are we only using [SymbolTable::numbers], [SymbolTable::identifiers], & [SymbolTable::keywords]
    during [Tokenizer] tokenization process? 
    - (I ask bc I assume after tokenizer we only use lookup on [SymbolTable::symbol_table]) <br><br>


[06/30/2024; 23:00]
<br>

- why do we keep track of this again? could we just make it a set if we only care about the numbers we've seen?
- i'm assuming we care about order (and therefore WANT duplicate numbers) because it's a vector.
- BUT i'm not sure! I need to double check when i get there

<br>

## **Done** 
[07/11/2024]: changed
**a.** [Results] now generated

[07/04/2024]: changed 
**a.** [Tokenizer] results work

[06/30/2024]: changed 
**a.** [SymbolTable::symbol_table from <std::string, int> to <int, std::string>] <br>
**b.** all it's respective functions [SymbolTable.hpp] <br>
**c.** the calls in [Tokenizer.*]
