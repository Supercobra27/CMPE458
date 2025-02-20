# CMPE458 Mini Compiler Project

## Phase 1: Lexer/Scanner

## Phase 2: Parser

## Phase 3: Semantic Analyzer

## Phase 4: Code Generation

## Grammar
- UpperCase = non-terminals
- lower_case = terminals/tokens/token-classes
- there is some regex stuff mixed in here, regular expressions are written within double quotes (") (* for zero or more of the previous symbol, + for one or more of the previous symbol, [] for character set, . for any character)
- create a right-recursive only version of this grammar so that we can do recursive descent parsing.
- to add `break` and `continue` keywords for loops

### Statement-Level Grammar
```
Program -> Statement*

Statement -> statement_end
        | VariableDeclaration
        | ExpressionStatement
        | PrintStatement
        | Block
        | ConditionalStatement
        | WhileLoop
        | RepeatUntilLoop

statement_end -> ";"
VariableDeclaration -> type_keyword identifier statement_end
ExpressionStatement -> Expression statement_end
PrintStatement -> print_keyword Expression statement_end
Block -> "{" Statement* "}"
ConditionalStatement -> if_keyword Expression then_keyword Block else_keyword Block
                    | if_keyword Expression then_keyword Block
WhileLoop -> while_keyword Expression Block
RepeatUntilLoop -> repeat_keyword Block until_keyword Expression statement_end

type_keyword -> "int" | "float"
identifier -> "[a-zA-Z_][a-zA-Z0-9_]*"
print_keyword -> "print"
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
            | UnarySuffixExpression_r2 
            | Factor

AssignmentExpression_r12    -> identifier assignment_operator Expression
OrExpression_l11            -> Expresion or_operator Expresion 
AndExpression_l10           -> Expresion and_operator Expresion 
BitOrExpression_l9          -> Expresion bit_or_operator Expresion 
BitXorExpression_l8         -> Expresion bit_xor_operator Expresion
BitAndExpression_l7         -> Expresion bit_and_operator Expresion
RelationExpression_l6       -> Expresion relation_operator Expresion
ShiftExpression_l5          -> Expresion shift_operator Expresion
SumExpression_l4            -> Expresion sum_operator Expresion
ProductExpression_l3        -> Expresion product_operator Expresion
UnaryPrefixExpression_r2    -> unary_prefix_operator Expression | increment_decrement_operator identifier
UnarySuffixExpression_l1    -> identifier increment_decrement_operator

Factor -> Immutable | identifier
Immutable -> ImmutableExpression | constant | FactorialCall
ImmutableExpression -> "(" Expression ")"
FactorialCall ->  factorial_keyword "(" Expression ")"
constant -> int_literal | float_literal | string_literal
```

Full operator grammar including increment_decrement_operator and complex assignment operators.
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
UnaryPrefixExpression_r2 -> increment_decrement_operator identifier | unary_prefix_operator UnarySuffixExpression_l1 | UnarySuffixExpression_l1
UnarySuffix_l1 -> identifier increment_decrement_operator | Factor

Factor -> Immutable | identifier
Immutable -> "(" Expression ")" | constant | factorial_keyword "(" Expression ")"
constant -> int_literal | float_literal | string_literal

factorial_keyword -> "factorial"
int_literal -> "[0-9]+"
float_literal -> "[0-9]+\.[0-9]+"
string_literal -> "\"[ -~]\""

assignment_operator -> "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "<<=" | ">>=" | "&=" | "^=" | "|="
or_operator -> "||"
and_operator -> "&&"
bit_or_operator -> "|"
bit_and_operator -> "&"
relation_operator -> "<" | "<=" | ">" | ">=" | "==" | "!="
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

AssignmentExpression_r12    -> identifier assignment_operator Expression
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

Factor -> Immutable | identifier
Immutable -> "(" Expression ")" | constant | factorial_keyword "(" Expression ")"
constant -> int_literal | float_literal | string_literal
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

Factor -> Immutable | identifier
Immutable -> "(" Expression ")" | constant | factorial_keyword "(" Expression ")"
constant -> int_literal | float_literal | string_literal

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
