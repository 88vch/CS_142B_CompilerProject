# **To Do;**
**get [Tokenizer] results working**
- i.e. symbol_table, numbers, identifiers, and tokenizer_results => proper & expected output

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
[06/30/2024]: changed 
**a.** [SymbolTable::symbol_table from <std::string, int> to <int, std::string>] <br>
**b.** all it's respective functions [SymbolTable.hpp] <br>
**c.** the calls in [Tokenizer.*]
