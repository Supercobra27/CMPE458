# CMPE458 Mini Compiler Project

## Building and running
The project is configured using cmake, see `my-mini-compiler/CMakeLists.txt` for the build configuration. 

To build using CMAKE: build the project using the `CMakeLists.txt` file. From the `my-mini-compiler` directory, run the following commands in the terminal.
```
mkdir build
cd build
cmake ..
cmake --build .
```

Three executables are generated, 
- one for just the lexer from phase 1 `my-mini-compiler1`,
- one for with the combination of the lexer and the parser `my-mini-compiler2`,
- one for the combination of lexer, parser and semantic analyzer `my-mini-compiler3` (work in progress).

Both executables are run the in the terminal in the same way, by running the executable along with your input file of choice. For example:  

```
build/Debug/my-mini-compiler1.exe phase1-w25/test/comments.cisc
```

You can run different tests by changing the input file from `comments.cisc` to perhaps `keywords&identifiers.cisc` or by creating your own file with file extension `.cisc`.

There are multiple sample input files for the parser located in the directory `my-mini-compiler/phase2-w25/test` with file extension `.cisc`.

## Phase 1: Lexer/Scanner

Todo:
- [ ] fix number parsing for ints and floats ("5.5.5" should not be accepted as FLOAT "5.5.5")
- [ ] Clearly list and explain each regular expression for each token (perhaps test and make everything explicit using https://regexer.com) and explain what characters are allowed to follow immediately after a token (address the question of whether "10x" is tokenized into INTEGER "10" and IDENTIFIER "x" or alternatively ERROR "10x", also "2then" is tokenized as INTEGER "2" and KEYWORD "then" which I suspect was not intended). Here's an example of c-compiler error:
    ```
    [build] C:\Users\Hendrix\GitHub\CMPE458\my-mini-compiler\phase3-w25\src\main.c:189:9: error: invalid suffix "dx" on integer constant
    [build]   189 |         1dx;
    [build]       |         ^~~
    ```
- [ ] Clearly distinguish whitespace and comment regular expressions from token lexeme regular expressions

Completed:
- [x] Refactor to make the lexer portable so that it can be used in other programs (done in phase2-w25)
- [x] For each type of tokens and whitespace and comments.
    - [x] regex pattern 
    - [x] parsing in lexer.c
    - [x] error cases
    - [x] test cases
- [x] Phase 1 Documentation
    - [x] Changes overview
    - [x] Change Log
    - [x] Language Design
        - [x] data types
        - [x] control flow patterns (if, else, while, repeat_until) and tentative syntax
        - [x] operators
        - [x] keywords
    - [x] Example Code (correct and incorrect errors)
- [x] work division
    - [x] column tracking for tokens (and global error reporting and recovery) - Hendrix
    - [x] ability to pass input code from a file
    - [x] comment handling - Monica
    - [x] numbers: update to not allow leading zeros, length checking, ... - Ryan
    - [x] keywords and identifier handling - Simon
    - [x] string literal handling - Simon
    - [x] operators - Ryan
    - [x] punctuators - Monica

## Phase 2: Parser

To Do:
- [ ] comprehensive validation of parser could be done as follows:
    - store a list of tokens, for one of each TokenType with sample lexemes.
    - create a valid parse tree using the grammar rules (can iterate over all possible syntatically valid parse trees or something)
    - convert the valid parse tree to a stream of tokens
    - test the parser by parsing the stream of tokens to build a parse tree
    - check that the built parse tree from parsing matches the original parse tree
    - for each valid parse tree, create invalid parse trees. Do this by arbitrarily picking a node of the parse tree to be in error, update the parse tree to reflect this error. Create modified token stream with this error parse tree. parse the token stream and see if you build the same parse tree with the same error.
- [ ] Optional: improve/simplify the parse_cfg_recursive_descent_parse_tree function
- [ ] Optional: implement alternative parsing function such as a bottom-up parser (recursive descent technically performs bottom-up parsing to parse left-recursive grammar rules)


Completed:
- [x] implement ParseTreeNode_print function properly.
- [x] make sure TokenType, ParseToken, and ASTNodeType all make coinside nicely
- [x] write validation logic to ensure grammar is deterministic and that left-recursive rules are in the correct format
- [x] refine the error handling a bit (error messages and types)
- [x] ParseTreeNode_free function (implement in parser.c)
- [x] test and validate the parse_cfg_recursive_descent_parse_tree on our actual grammar (hand written test cases)
    - [x] Scope Statements
    - [x] Declaration Statements
    - [x] Expression Statements ---- lots of stuff to test here
    - [x] ADD READ Statements !!!! NOTICED THIS ON ONQ (we can make this be "read_keyword identifier statement_end" where the identifier must be of type int and this gets one character from the input just as if you did "x = getc(stdin);" in C)
        - [x] add "read" as a keyword to the lexer
        - [x] add TOKEN_READ_KEYWORD to TokenType, PT_READ_KEYWORD to ParseToken, and AST_READ to ASTNodeType.
        - [x] add gramar rule for read keyword.
        - [x] add parsing example test case for reading.
    - [x] Print Statements 
    - [x] Conditional Statements
    - [x] WhileLoop Statements
    - [x] RepeatUntilLoop Statements
- [x] address all C compiler warnings/errors (they are all just -Wunused-function warnings for enum to const char* switch case functions)
- [x] Write up documentation
    - [x] Grammar
    - [x] file structure
    - [x] list of changes


## Phase 3: Semantic Analyzer
### Simple Checklist
1. [x] Convert Parse Tree to AST (Hendrix)
    - [x] Free Dynamic Memory Function
    - [x] Conversion Function
    - Rules are defined in the grammar

2. [x] Define Semantic Rules
    - [x] Declaration/Variables (Simon)
    - [x] Assignment/Expressions (Ryan)
    - [x] Types allowed for Operations (Ryan/Simon)

3. [x] Produce proper compiler error messages
    - [x] get rid of global variables in lexer, use a struct instead.
    - [x] put compiler message printing functions in main.c
    - [x] tokenization error messages
    - [x] revise syntax error messages
    - [x] semantic error messages

4. [x] Check AST based on Rules 
    - [x] For each AST node, check correct # of childrn and types of children
    - [x] Be able to discern the type of Identifiers
    - [x] Be able to keep track of identifier scope

5. [x] Runtime Error Detection (flags)
    - [x] Div/0
    - [x] % (Modulo)



### Detailed Checklist
Todo (in increasing order of dependency):
- [ ] specify semantic/context-sensitive rules for the programming language 
    - [ ] Declaration and use of variables (identifier) in relation to Scope and Expression.
    - [ ] type of arguments allowed in each different Operation.
        - [ ] Assignment (AssignEqual)
        - [ ] Logical (LogicalOr, LogicalAnd, LogicalNot)
        - [ ] Bitwise (BitwiseOr, BitwiseAnd, BitwiseNot)
        - [ ] Bitwise (BitwiseOr, BitwiseAnd, BitwiseNot, ShiftLeft, ShiftRight)
        - [ ] Comparison (CompareEqual, CompareNotEqual, CompareLessEqual, CompareLessThan, CompareGreaterEqual, CompareGreaterThan)
        - [ ] Arithmetic (Add, Subtract, Negate, Multiply, Divide, Modulo, Factorial)
    - [ ] type(s) allowed for the Expression in a Conditional, WhileLoop, and RepeatUntilLoop
    - [ ] type(s) allowed for the Expression in Print.
    - [ ] type(s) allowed for the Expression in Read.
    - [ ] Which operations/constructs can accept different types (are polymorphic) and how do we represent this information to enable code generation?
        - This is mentioned because with these kinds of functions, technically the type is being passed as a parameter (see https://en.wikipedia.org/wiki/Typed_lambda_calculus). So, we either have to be somehow "pass the type as a parameter" to these functions, or recognize the type of the arguments and convert the function to one of a specific type.
        - For example, `Add` is the polymorphic function which can operate on either two arguments of type `int` or two arguments of type `float`. So there could be two different versions of `Add`, one for `ints` such as `AddInt` and one for floats such as `AddFloat` (and this could possibly be even more specific when considering the available processor instruction set). 
        - As far as the semantic validity of the Abstract Syntax Tree is concerned, checking that both arguments to `Add` are the same type is enough.
        - An even more clear difference is between different kinds of `Print` such as between `PrintString` vs. `PrintInt`.
- [ ] What information would be displayed in an error message for each of the semantic rules
- [ ] Specify the type rules for each type of ASTNode somehow in C
    - [ ] perhaps we leave ASTNode as the "untyped"/semantically-unchecked abstract syntax tree and introduce a new data structure which is the "typed"/semantically-valid abstract syntax tree
    - [ ] perhaps we can implement type rules between mutable/immutable at the level of syntax/grammar?
- [ ] implement symbol table data structure 
    - [ ] determine operations needed to be done on the symbol table 
    - [ ] choose underlying data structure to store the entries whether it be contiguous array or hash table or something else
    - [ ] determine what properties need to be kept track of to validate/invalidate semantic rules
    - [ ] determine symbol table entry C struct 
    - [ ] implement the symbol table in C
- [ ] implement ASTNode validation with use of symbol table, perhaps creating "typed"/semantically-valid abstract syntax tree.
- [ ] implement runtime error detection
    - [ ] determine which operations or constructs can incur a runtime error (such as division by zero)
    - [ ] specify this property somehow in the Abstract Syntax Tree data structure
- [ ] Optional: add constant-folding and checking for runtime errors (division by zero) of constant expressions at compile time.

Completed:
- [x] decide whether PT_PROGRAM is present in the AST, or if it is removed and only Scope exists (PT_PROGRAM will exist as the start symbol)
- [x] implement function to convert ParseTreeNode into ASTNode (Abstract Syntax Tree) using grammar rules (see an example here: https://stackoverflow.com/questions/5026517/whats-the-difference-between-parse-trees-and-abstract-syntax-trees-asts)
    - [x] implement function to free dynamic memory of an AST
    - [x] implement conversion function to create ASTNode from ParseTreeNode

## Phase 4: Code Generation

Todo:
- [ ] specify the actual data types fully (how many bits for type `int`, what happens when overflow occurs? what is the underlying binary representation (signed/unsigned)?)

## Grammar
- UpperCase = non-terminals
- lower_case = terminals/tokens/token-classes

Todo:
- [ ] Optional: add `break` and `continue` keyword statements for loops
- [ ] Optional: add character literals that convert to their ascii values (`'\n'` is equal to `10`).
- [ ] Optional: add data types
    - [ ] memory address (pointers and array) and get rid of `string` data type and replace with `char *` or equivalent
    - [ ] different types of `int` and `float` based on number of bits used in representation and storage (8/16/32/64/more?)
    - [ ] unsigned vs. signed `int` types


### Abstract Syntax Tree Grammar
- the ^ symbol following a token indicates that that token is promoted in-place of the left-hand side of the production rule
- the @ symbol following a token indicates that its children are promoted to replace it
```
Program -> Scope
Scope -> StatementList@
StatementList -> Statement StatementList@ | epsilon

Statement -> Scope^
           | Declaration^
           | Expression^
           | Print^
           | Read^
           | Conditional^
           | WhileLoop^
           | RepeatUntilLoop^

Declaration -> Declaration_Type Identifier
Print -> Expression
Read -> Expression
Conditional -> Expression Scope Scope
WhileLoop -> Expression Scope
RepeatUntilLoop -> Scope Expression

Expression -> Operation^ 
            | int_const^ 
            | float_const^ 
            | string_const^ 
            | identifier^

Operation -> AssignEqual^
           | LogicalOr^
           | LogicalAnd^ 
           | BitwiseOr^
           | BitwiseXor^
           | BitwiseAnd^
           | CompareEqual^
           | CompareNotEqual^
           | CompareLessEqual^
           | CompareLessThan^
           | CompareGreaterEqual^
           | CompareGreaterThan^
           | ShiftLeft^
           | ShiftRight^
           | Add^
           | Subtract^
           | Multiply^
           | Divide^
           | Modulo^
           | BitwiseNot^
           | LogicalNot^
           | Negate^
           | Factorial^

AssignEqual         -> Expression Expression
LogicalOr           -> Expression Expression
LogicalAnd          -> Expression Expression
BitwiseOr           -> Expression Expression
BitwiseXor          -> Expression Expression
BitwiseAnd          -> Expression Expression
CompareEqual        -> Expression Expression
CompareNotEqual     -> Expression Expression
CompareLessEqual    -> Expression Expression
CompareLessThan     -> Expression Expression
CompareGreaterEqual -> Expression Expression
CompareGreaterThan  -> Expression Expression
ShiftLeft           -> Expression Expression
ShiftRight          -> Expression Expression
Add                 -> Expression Expression
Subtract            -> Expression Expression
Multiply            -> Expression Expression
Divide              -> Expression Expression
Modulo              -> Expression Expression
BitwiseNot          -> Expression 
LogicalNot          -> Expression 
Negate              -> Expression 
Factorial           -> Expression 
```

### Statement-Level Grammar
```
Program -> Scope TOKEN_EOF

Scope -> StatementList

StatementList -> Statement StatementList | epsilon

Statement -> EmptyStatement
        | Declaration
        | ExpressionStatement
        | PrintStatement
        | ReadStatement
        | Block
        | IfThenElse
        | IfThen
        | WhileLoop
        | RepeatUntilLoop

EmptyStatement -> statement_end
Declaration -> type_keyword identifier statement_end
ExpressionStatement -> Expression statement_end
PrintStatement -> print_keyword Expression statement_end
ReadStatement -> read_keyword Expression statement_end
Block -> block_begin StatementList block_end
Conditional -> if_keyword Expression then_keyword Block OptionalElseBlock 
WhileLoop -> while_keyword Expression Block
RepeatUntilLoop -> repeat_keyword Block until_keyword Expression statement_end

OptionalElseBlock -> else_keyword Block | epsilon
statement_end -> ";"
type_keyword -> "int" | "float" | "string"
identifier -> "[a-zA-Z_][a-zA-Z0-9_]*"
print_keyword -> "print"
read_keyword -> "read"
block_begin -> "{"
block_end -> "}"
if_keyword -> "if"
then_keyword -> "then"
else_keyword -> "else"
while_keyword -> "while"
repeat_keyword -> "repeat"
until_keyword -> "until"

Expression -> AssignmentExpression_r12
```
### Expression-Level Grammar

| precedence | expression/operator                          | Associativity |
| ---------- | -------------------------------------------- | ------------- |
| 12         | assignment                                   | right         |
| 11         | logical or                                   | left          |
| 10         | logical and                                  | left          |
|  9         | bitwise or                                   | left          |
|  8         | bitwise xor                                  | left          |
|  7         | bitwise and                                  | left          |
|  6         | relation (<, >, <=, >=, ==, !=)              | left          |
|  5         | bit shift                                    | left          |
|  4         | add/sub (SumExpression)                      | left          |
|  3         | mul/div/mod (ProductExpression)              | left          |
|  2         | unary prefix (unary negate -, logical/bitwise not !/~, increment/decrement ++/--, ...) | right   |
|  1         | unary suffix (increment/decrement)           | left   |
| high       | factor (mutable or immutable)                | N/A           |

#### Full Version
Full Abstract Expression Grammar (ignoring precedence).
```
Expression -> AssignmentExpression_r12 
            | OrExpression_l11 
            | AndExpression_l10 
            | BitOrExpression_l9 
            | BitXorExpression_l8 
            | BitAndExpression_l7      
            | RelationExpression_l6    
            | ShiftExpression_l5       
            | SumExpression_l4         
            | ProductExpression_l3     
            | UnaryPrefixExpression_r2 
            | UnarySuffixExpression_r2 
            | Factor

AssignmentExpression_r12    -> Expression assignment_operator Expression
OrExpression_l11            -> Expresion or_operator Expresion 
AndExpression_l10           -> Expresion and_operator Expresion 
BitOrExpression_l9          -> Expresion bit_or_operator Expresion 
BitXorExpression_l8         -> Expresion bit_xor_operator Expresion
BitAndExpression_l7         -> Expresion bit_and_operator Expresion
RelationExpression_l6       -> Expresion relation_operator Expresion
ShiftExpression_l5          -> Expresion shift_operator Expresion
SumExpression_l4            -> Expresion sum_operator Expresion
ProductExpression_l3        -> Expresion product_operator Expresion
UnaryPrefixExpression_r2    -> unary_prefix_operator Expression | increment_decrement_operator Expression
UnarySuffixExpression_l1    -> Expression increment_decrement_operator

Factor -> "(" Expression ")" | int_const | float_const | string_const | FactorialCall | identifier
FactorialCall ->  factorial_keyword "(" Expression ")"
```

Full operator grammar including increment_decrement_operator and complex assignment operators.
```
AssignmentExpression_r12 -> OrExpression_l11 AssignmentExpression_rest
AssignmentExpression_rest -> assignment_operator AssignmentExpression_r12 | epsilon
OrExpression_l11 -> OrExpression_l11 or_operator AndExpression_l10 | AndExpression_l10
AndExpression_l10 -> AndExpression_l10 and_operator BitOrExpression_l9 | BitOrExpression_l9
BitOrExpression_l9 -> BitOrExpression_l9 bit_or_operator BitAndExpression_l8 | BitAndExpression_l8
BitXorExpression_l8 -> BitXorExpression_l8 bit_xor_operator BitAndExpression_l7 | BitAndExpression_l7
BitAndExpression_l7 -> BitAndExpression_l7 bit_and_operator RelationExpression_l6 | RelationExpression_l6
RelationExpression_l6 -> RelationExpression_l6 relation_operator ShiftExpression_l5 | ShiftExpression_l5
ShiftExpression_l5 -> AndExpression_l10 shift_operator SumExpression_L4 | SumExpression_L4
SumExpression_l4 -> SumExpression_l4 sum_operator ProductExpression_l3 | ProductExpression_l3
ProductExpression_l3 -> ProductExpression_l3 product_operator UnaryPrefixExpression_r2 | BitOrExpression_l9
UnaryPrefixExpression_r2 -> increment_decrement_operator Mutable | unary_prefix_operator UnarySuffixExpression_l1 | UnarySuffixExpression_l1
UnarySuffix_l1 -> Mutable increment_decrement_operator | Factor

Factor -> "(" Expression ")" | int_const | float_const | string_const | FactorialCall | identifier
FactorialCall -> factorial_keyword "(" Expression ")"

factorial_keyword -> "factorial"
int_literal -> "[0-9]+"
float_literal -> "[0-9]+\.[0-9]+"
string_literal -> "\"[ -~]\""

assignment_operator -> "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "<<=" | ">>=" | "&=" | "^=" | "|="
or_operator -> "||"
and_operator -> "&&"
bit_or_operator -> "|"
bit_and_operator -> "&"
relation_operator -> "<=" | "<" | ">=" | ">" | "==" | "!="
shift_operator -> "<<" | ">>"
sum_operator -> "+" | "-"
product_operator -> "*" | "/" | "%"
increment_decrement_operator -> "++" | "--"
unary_prefix_operator -> "!" | "~" | "-"
```

#### Simplified Version

Simplified Abstract Syntax Grammar (ignoring precedence).
```
Expression -> AssignmentExpression_r12 
            | OrExpression_r11 
            | AndExpression_l10 
            | BitOrExpression_l9 
            | BitXorExpression_l8 
            | BitAndExpression_l7      
            | RelationExpression_l6    
            | ShiftExpression_l5       
            | SumExpression_l4         
            | ProductExpression_l3     
            | UnaryPrefixExpression_r2 
            | Factor

AssignmentExpression_r12    -> Expression assignment_operator Expression
OrExpression_l11            -> Expresion or_operator Expresion 
AndExpression_l10           -> Expresion and_operator Expresion 
BitOrExpression_l9          -> Expresion bit_or_operator Expresion 
BitXorExpression_l8         -> Expresion bit_xor_operator Expresion
BitAndExpression_l7         -> Expresion bit_and_operator Expresion
RelationExpression_l6       -> Expresion relation_operator Expresion
ShiftExpression_l5          -> Expresion shift_operator Expresion
SumExpression_l4            -> Expresion sum_operator Expresion
ProductExpression_l3        -> Expresion product_operator Expresion
UnaryPrefixExpression_r2    -> unary_prefix_operator Expression 

Factor -> "(" Expression ")" | int_const | float_const | string_const | FactorialCall | identifier
FactorialCall -> factorial_keyword "(" Expression ")"
```

Simplified operator grammmar that only includes the simple assignment operator "=" and no increment_decrement_operator.
```
AssignmentExpression_r12 -> OrExpression_l11 AssignmentExpression_rest
AssignmentExpression_rest -> assignment_operator AssignmentExpression_r12 | epsilon
OrExpression_l11 -> OrExpression_l11 or_operator AndExpression_l10 | AndExpression_l10
AndExpression_l10 -> AndExpression_l10 and_operator BitOrExpression_l9 | BitOrExpression_l9
BitOrExpression_l9 -> BitOrExpression_l9 bit_or_operator BitAndExpression_l8 | BitAndExpression_l8
BitXorExpression_l8 -> BitXorExpression_l8 bit_xor_operator BitAndExpression_l7 | BitAndExpression_l7
BitAndExpression_l7 -> BitAndExpression_l7 bit_and_operator RelationExpression_l6 | RelationExpression_l6
RelationExpression_l6 -> RelationExpression_l6 relation_operator ShiftExpression_l5 | ShiftExpression_l5
ShiftExpression_l5 -> AndExpression_l10 shift_operator SumExpression_L4 | SumExpression_L4
SumExpression_l4 -> SumExpression_l4 sum_operator ProductExpression_l3 | ProductExpression_l3
ProductExpression_l3 -> ProductExpression_l3 product_operator UnaryPrefixExpression_r2 | BitOrExpression_l9
UnaryPrefixExpression_r2 -> unary_prefix_operator Factor | Factor

Factor -> "(" Expression ")" | int_const | float_const | string_const | FactorialCall | identifier
FactorialCall -> factorial_keyword "(" Expression ")"

factorial_keyword -> "factorial"
int_literal -> "[0-9]+"
float_literal -> "[0-9]+\.[0-9]+"
string_literal -> "\"[ -~]\""

assignment_operator -> "=" 
or_operator -> "||"
and_operator -> "&&"
bit_or_operator -> "|"
bit_and_operator -> "&"
relation_operator -> "<" | "<=" | ">" | ">=" | "==" | "!="
shift_operator -> "<<" | ">>"
sum_operator -> "+" | "-"
product_operator -> "*" | "/" | "%"
unary_prefix_operator -> "!" | "~" | "-"
```
