# Semantic Rules
Semantic Rules defined for - Assignment/Expressions
Ryan

Rules:
Every operation must have 2 children maximum and must be of the same type (int/float)

## AST Grammar
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