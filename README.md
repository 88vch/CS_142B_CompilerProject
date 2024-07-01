# To Do;
get [Tokenizer] results working 
- i.e. symbol_table, numbers, identifiers, and tokenizer_results => proper & expected output


- changed [SymbolTable::symbol_table from <std::string, int> to <int, std::string>] & all it's respective functions [SymbolTable.hpp] & the calls in [Tokenizer.*]

### Notes
SymbolTable::numbers
- why do we keep track of this again? could we just make it a set if we only care about the numbers we've seen?
- i'm assuming we care about order (and therefore WANT duplicate numbers) because it's a vector.
- BUT i'm not sure! I need to double check when i get there