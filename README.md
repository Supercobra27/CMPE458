# CMPE458 Mini Compiler Project

## Phase 1: Lexer/Scanner

## Phase 2: Parser

To Do:
- [x] implement ParseTreeNode_print function properly.
- [x] make sure TokenType, ParseToken, and ASTNodeType all make coinside nicely
- [x] write validation logic to ensure grammar is deterministic and that left-recursive rules are in the correct format
- [ ] test and validate the parse_cfg_recursive_descent_parse_tree on our actual grammar for 
    - [ ] Scope Statements
    - [ ] Declaration Statements
    - [ ] Expression Statements ---- lots of stuff to test here
    - [ ] ADD READ Statements !!!! NOTICED THIS ON ONQ (we can make this be "read_keyword identifier statement_end" where the identifier must be of type int and this gets one character from the input just as if you did "x = getc(stdin);" in C)
        - [ ] add "read" as a keyword to the lexer
        - [ ] add TOKEN_READ_KEYWORD to TokenType, PT_READ_KEYWORD to ParseToken, and AST_READ to ASTNodeType.
    - [ ] Print Statements 
    - [ ] Conditional Statements
    - [ ] WhileLoop Statements
    - [ ] RepeatUntilLoop Statements
- [x] refine the error handling a bit (error messages and types)
- [x] ParseTreeNode_free function (implement in parser.c)
- [x] address all C compiler warnings/errors (they are all just -Wunused-function warnings for enum to const char* switch case functions)
- [ ] Write up documentation
    - [ ] Grammar
    - [ ] file structure
    - [ ] list of changes



## Phase 3: Semantic Analyzer
- [ ] decide whether Program is present in the AST, or if it is removed and only Scope exists
- [ ] ASTNode_from_ParseTreeNode function using grammar rules to simplify (https://stackoverflow.com/questions/5026517/whats-the-difference-between-parse-trees-and-abstract-syntax-trees-asts)
- [ ] implement runtime error detection (for division by zero)
- [ ] Optional: add `break` and `continue` keyword statements for loops
- [ ] Optional: improve/simplify the parse_cfg_recursive_descent_parse_tree function

## Phase 4: Code Generation

## Grammar
- UpperCase = non-terminals
- lower_case = terminals/tokens/token-classes

### Abstract Syntax Tree Grammar
- the ^ symbol following a token indicates that that token is promoted in-place of the left-hand side of the production rule
- the @ symbol following a token indicates that it's children are promoted to replace it
```
Program -> Scope
Scope -> StatementList@
StatementList -> Statement StatementList@ | epsilon

Statement -> Scope^
           | Declaration^
           | Expression^
           | Print^
           | Conditional^
           | WhileLoop^
           | RepeatUntilLoop^

Declaration -> Declaration_Type Identifier
Print -> Expression
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
Program -> StatementList TOKEN_EOF

StatementList -> Statement StatementList | epsilon

Statement -> EmptyStatement
        | Declaration
        | ExpressionStatement
        | PrintStatement
        | Block
        | IfThenElse
        | IfThen
        | WhileLoop
        | RepeatUntilLoop

EmptyStatement -> statement_end
Declaration -> type_keyword identifier statement_end
ExpressionStatement -> Expression statement_end
PrintStatement -> print_keyword Expression statement_end
Block -> block_begin StatementList block_end
Conditional -> if_keyword Expression then_keyword Block OptionalElseBlock 
WhileLoop -> while_keyword Expression Block
RepeatUntilLoop -> repeat_keyword Block until_keyword Expression statement_end

OptionalElseBlock -> else_keyword Block | epsilon
statement_end -> ";"
type_keyword -> "int" | "float" | "string"
identifier -> "[a-zA-Z_][a-zA-Z0-9_]*"
print_keyword -> "print"
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
AssignmentExpression_r12 -> Mutable assignment_operator AssignmentExpression_r12 | OrExpression_l11
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

AssignmentExpression_r12    -> Mutable assignment_operator Expression
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
AssignmentExpression_r12 -> identifier assignment_operator AssignmentExpression_r12 | OrExpression_l11
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
