# **To Do;**
**[07/16/2024]**
[ToDo]: first generate all SSA using Recursive Descent
- [Parser.*] use [OldParser.*] for base code (to revise!)
    - [Parser]: SSA only holds operations (store values yourself)
        - i.e. var/func values/declarations
    - [Parser] uses: {LinkedList.hpp, Result.hpp, SSA.hpp}

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
