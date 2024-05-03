#include "Parser.hpp"


// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}
// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}


// Function to perform common subexpression elimination (CSE)
void performCSE(std::vector<IRNode>& ir) {
    std::unordered_map<std::string, std::string> expressions;
    for (auto it = ir.begin(); it != ir.end(); ++it) {
        if (it->operation != "print") {
            // Check if expression already exists
            std::string expression = it->operation + " " + it->operand1 + " " + it->operand2;
            if (expressions.find(expression) != expressions.end()) {
                // Replace current result with existing result
                it->result = expressions[expression];
            } else {
                // Store new expression
                expressions[expression] = it->result;
            }
        }
    }
}

// Function to print IR
void printIR(const std::vector<IRNode>& ir) {
    for (const auto& node : ir) {
        std::cout << node.result << " = " << node.operation << " " << node.operand1 << " " << node.operand2 << std::endl;
    }
}



// Parser class
class Parser {
public:
    Parser(const std::string& source) : source(source), currentPos(0) {}

    void parse_FIRSTPASS() {
        currentToken = getNextToken();
        while (currentToken.type != END_OF_FILE) {
            parseStatement();
        }
    }

    // Get intermediate representation (IR)
    std::vector<IRNode> getIR() {
        return ir;
    }

private:
    std::string source;
    size_t currentPos;
    TOKEN currentToken;
    std::unordered_map<std::string, std::string> symbolTable; // For copy propagation
    std::vector<IRNode> ir; // Intermediate representation (IR)

    TOKEN tokenize() {
        
    }

    // Helper function to get next token
    TOKEN getNextToken() {
        // called: should run right after we consume current token
        // ltrim() the string
        // determine next TOKEN_TYPE
        // create & return TOKEN
    }

    // Helper function to consume current token and advance to next token
    void consume(TOKEN_TYPE expectedType) {
        // Implementation omitted for brevity
        int cutIdx = -1, i =0;
        auto iit = this->source.begin();
        while ((iit + i) != this->source.end() && cutIdx == -1) {
            if (*iit != ' ') {
                cutIdx = i;
            }
            i++;
        }
    }

    // Statement parsing functions
    void parseStatement() {
        if (currentToken.type == LET) {
            parseDeclaration();
        } else if (currentToken.type == IDENTIFIER) {
            parseAssignment();
        } else if (currentToken.type == PRINT) {
            parsePrintStatement();
        } else {
            std::cerr << "Syntax error: Unexpected token " << currentToken.lexeme << std::endl;
            currentToken = getNextToken(); // Skip token
        }
    }

    // Declaration parsing function
    void parseDeclaration() {
        consume(LET); // Consume 'let' keyword
        std::string identifier = currentToken.lexeme;
        consume(IDENTIFIER); // Consume identifier
        consume('='); // Consume '='
        std::string expressionResult = parseExpression();
        symbolTable[identifier] = expressionResult; // Store result in symbol table for copy propagation
        consume(SEMICOLON); // Consume ';'
    }

    // Assignment parsing function
    void parseAssignment() {
        std::string identifier = currentToken.lexeme;
        consume(IDENTIFIER); // Consume identifier
        consume('='); // Consume '='
        std::string expressionResult = parseExpression();
        symbolTable[identifier] = expressionResult; // Store result in symbol table for copy propagation
        consume(SEMICOLON); // Consume ';'
    }

    // Print statement parsing function
    void parsePrintStatement() {
        consume(PRINT); // Consume 'print' keyword
        std::string expressionResult = parseExpression();
        ir.push_back({"", "print", expressionResult, ""}); // Generate IR for print statement
        consume(SEMICOLON); // Consume ';'
    }

    // Expression parsing function
    std::string parseExpression() {
        std::string left = parseTerm();
        while (currentToken.type == PLUS || currentToken.type == MINUS) {
            TOKEN_TYPE op = currentToken.type;
            consume(op); // Consume operator
            std::string right = parseTerm();
            // Generate IR for addition or subtraction
            std::string result = generateIRBinaryOperation(left, right, op);
            // Perform copy propagation
            left = propagateCopy(result);
        }
        return left;
    }

    // Term parsing function
    std::string parseTerm() {
        std::string left = parseFactor();
        while (currentToken.type == MULTIPLY || currentToken.type == DIVIDE) {
            TOKEN_TYPE op = currentToken.type;
            consume(op); // Consume operator
            std::string right = parseFactor();
            // Generate IR for multiplication or division
            std::string result = generateIRBinaryOperation(left, right, op);
            // Perform copy propagation
            left = propagateCopy(result);
        }
        return left;
    }

    // Factor parsing function
    std::string parseFactor() {
        if (currentToken.type == IDENTIFIER || currentToken.type == NUMBER) {
            std::string factorResult = currentToken.lexeme;
            consume(currentToken.type); // Consume identifier or number
            return factorResult;
        } else if (currentToken.type == '(') {
            consume('('); // Consume '('
            std::string expressionResult = parseExpression();
            consume(')'); // Consume ')'
            return expressionResult;
        } else {
            std::cerr << "Syntax error: Unexpected token " << currentToken.lexeme << std::endl;
            currentToken = getNextToken(); // Skip token
            return ""; // Return empty string for error recovery
        }
    }

    // Helper function to generate IR for binary operations
    std::string generateIRBinaryOperation(const std::string& left, const std::string& right, TOKEN_TYPE op) {
        std::string result = "t" + std::to_string(ir.size()); // Temporary variable name
        ir.push_back({result, getTokenString(op), left, right}); // Generate IR for binary operation
        return result;
    }

    // Helper function to perform copy propagation
    std::string propagateCopy(const std::string& result) {
        for (auto& entry : symbolTable) {
            std::string& value = entry.second;
            if (value == result) {
                return entry.first; // Replace use with value
            }
        }
        return result;
    }

    // Helper function to get string representation of token
    std::string getTokenString(TOKEN_TYPE type) {
        switch (type) {
            case PLUS: return "+";
            case MINUS: return "-";
            case MULTIPLY: return "*";
            case DIVIDE: return "/";
            case SEMICOLON: return ";";
            case PRINT: return "PRINT"; // TODO: fix this
            default: return "";
        }
    }
};

int main() {
    std::string source = "let a = 3 + 5 * (4 - 2); print a;";
    Parser parser(source);
    parser.parse_FIRSTPASS();
    std::vector<IRNode> ir = parser.getIR();

    // Perform common subexpression elimination (CSE)
    performCSE(ir);

    // Print IR
    printIR(ir);

    return 0;
}