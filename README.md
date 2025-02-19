# CMPE458 Mini Compiler Project

## Phase 1: Lexer/Scanner

## Phase 2: Parser

## Phase 3: Semantic Analyzer

## Phase 4: Code Generation

## Grammar
- UpperCase = non-terminals
- lower_case = terminals/tokens/token-classes
- there is some regex stuff mixed in here (* for zero or more of the previous symbol, + for one or more of the previous symbol, [] for character set, . for any character)
- single characters are separated by single quotes (')
- to add `break` and `continue` keywords for loops
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

statement_end -> ';'
VariableDeclaration -> type_keyword identifier statement_end
ExpressionStatement -> Expression statement_end
PrintStatement -> print_keyword Expression statement_end
Block -> '{' Statement* '}'
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
|  2         | unary prefix (unary negate '-', logical not '!', increment/decrement, ...) | right   |
|  1         | unary suffix (increment/decrement)           | left   |
| high       | factor (mutable or immutable)                | N/A           |


```
AssignmentExpression_r12 -> identifier assignment_operator OrExpression_l11 | OrExpression_l11
OrExpression_l11 -> OrExpression_l11 or_operator AndExpression_l10 | AndExpression_l10
AndExpression_l10 -> AndExpression_l10 and_operator BitOrExpression_l9 | BitOrExpression_l9
BitOrExpression_l9 -> BitOrExpression_l9 bit_or_operator BitAndExpression_l8 | BitAndExpression_l8
BitXorExpression_l8 -> BitXorExpression_l8 bit_xor_operator BitAndExpression_l7 | BitAndExpression_l7
BitAndExpression_l7 -> BitAndExpression_l7 bit_and_operator RelationExpression_l6 | RelationExpression_l6
RelationExpression_l6 -> RelationExpression_l6 relation_operator ShiftExpression_l5 | ShiftExpression_l5
ShiftExpression_l5 -> AndExpression_l10 shift_operator SumExpression_L4 | SumExpression_L4
SumExpression_l4 -> SumExpression_l4 sum_operator ProductExpression_l3 | ProductExpression_l3
ProductExpression_l3 -> ProductExpression_l3 product_operator UnaryPrefixExpression_r2 | BitOrExpression_l9
UnaryPrefixExpression_r2 -> increment_decrement_operator identifier | unary_prefix_operator UnarySuffixExpression_l1
UnarySuffix_l1 -> identifier increment_decrement_operator
Factor -> Immutable | identifier
Immutable -> '(' Expression ')' | constant | factorial_keyword '(' Expression ')'
constant -> int_literal | float_literal | string_literal

factorial_keyword -> "factorial"
int_literal -> "[0-9]+"
float_literal -> "[0-9]+\.[0-9]+"
string_literal -> "\"[ -~]\""
```

