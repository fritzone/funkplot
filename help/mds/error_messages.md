## Error messages

The following error messages and associated codes are used in **fũnkplot**:

| ERROR  | DESCRIPTION                                                  |
| ------ | ------------------------------------------------------------ |
| **0**  | Erroneous looping conditions detected. A loop with a negative step value, but with the start value being smaller than the end might generate this error. |
| **1**  | Invalid variable declaration. Not specifying the type of a variable might cause this error to pop up. |
| **2**  | Invalid variable type. The list of valid variable types is `number`, `point`, `segment`, `line`, `angle` and `list`. Anything else will lead to this error. |
| **3**  | Missing the keyword <b>of</b> in a statement.                |
| **4**  | Invalid list variable type.                                  |
| **5**  | Untyped lists are not supported.                             |
| **6**  | Untyped lists are not supported                              |
| **7**  | Index error                                                  |
| **8**  | Variable was not declared                                    |
| **9**  | Invalid assignment (missing assignment operator)             |
| **10** | Conflicting type assignment                                  |
| **11** | Invalid point assignment                                     |
| **12** | Conflicting type assignment: array assigned to a non array type |
| **13** | Invalid array assignment: missing <b>[</b> from the expression |
| **14** | Invalid assignment: arithmetic expression assigned to non numeric type |
| **15** | Invalid data to plot (not a point, not an indexed point, not a function) |
| **16** | Invalid function definition. Missing assignment in body      |
| **17** | Improper parametrization of the given function.              |
| **18** | Possible error in a statement. No data before a certain position. |
| **19** | Possible error in a statement. No data after a certain position |
| **20** | Syntax error in a statement. Unmatched empty parenthesis.    |
| **21** | Error in a statement. Not found enclosing parenthesis        |
| **22** | Possible error in a statement. No parameters were found for a function. |
| **23** | Possible error in statement. Meaningless use of a function.  |
| **24** | Possible error in statement. No parameters for a function.   |
| **25** | Invalid plotting interval                                    |
| **26** | Possible error in statement. Improper power expression.      |
| **27** | Possible error in formula. Looks like an invalid statement.  |
| **28** | Possible error in a statement. No data before a certain position. |
| **29** | Possible error in a statement. No data after a certain position. |
| **30** | Possible error in a statement. The statement is not fully understood. |
| **31** | Unknown pixel property, or the expression is nested too deeply (maximum recursion depth exceeded). |
| **32** | Wrong palette indexing                                       |
| **33** | Invalid color to set.                                        |
| **34** | Cannot identify the object assigning from                    |
| **35** | Invalid statement. The line does not start with a known keyword, or the statement is malformed. |
| **36** | Syntax error: unsupported <b>set</b> target.                 |
| **37** | Undeclared variables cannot be used in for loops.            |
| **38** | <b>for</b> statement with direct assignment does not contain the to keyword |
| **39** | Invalid for statement.                                       |
| **40** | <b>for</b> statement does not contain the do keyword.        |
| **41** | Internal error from the underlying libraries                 |
| **42** | Invalid expression                                           |
| **43** | <b>for</b> body does not end with <b>done</b>.               |
| **44** | rotate statement tries to rotate undeclared variable         |
| **45** | rotate statement does not contain <b>with</b> keyword        |
| **46** | Rotation unit must be either degree or radians (default)     |
| **47** | Unknown keyword in rotation.                                 |
| **48** | Invalid reference: (missing **at** keyword)                  |
| **49** | A value was declared and used but not defined to hold a value. Also reported when a segment or a line is used before its defining `let` statement was executed, or when its definition could not be computed. |
| **50** | Cannot plot a number                                         |
| **51** | Invalid assignment missing of keyword                        |
| **52** | No such function                                             |
| **53** | Invalid or missing keyword, such as a missing `and` in a line or intersection definition. |
| **54** | A keyword is missing from a geometric definition, such as `to`, `at`, `length` or `across`. |
| **55** | Index out of bounds                                          |
| **56** | Invalid index (ie: the resulted index is negative).          |
| **57** | Cannot identify the object to assign to.                     |
| **58** | Incompatible assignment types don't match.                   |
| **59** | Append statement does not contain the TO keyword             |
| **60** | No function for **points of** assignment                     |
| **61** | Cannot assign a non numeric value to a numeric variable      |
| **62** | Cannot identify the object assigning from                    |
| **63** | Unsupported list domain in an `append` statement (only `numbers` and `points` are supported) |
| **64** | Cannot identify the assignment source                        |
| **65** | Cannot use a point type variable in that context             |
| **66** | Missing <b>begin</b> keyword from `python` scriptlet         |
| **67** | Missing the <b>script</b> or <b>do</b> keyword from `python` scriptlet |
| **68** | Internal error of the python interpreter                     |
| **69** | Invalid parametric function definition: missing `function` keyword or parenthesis |
| **70** | Invalid parametric function definition: only the coordinates `x` and `y` are supported |
| **71** | Invalid parametric function definition: missing `=` sign     |
| **72** | Missing keyword `to` in a `point on … nearest to` assignment |
| **73** | Missing keyword `from` in a `point on … at distance` or percentage assignment |
| **74** | Expected a percentage (e.g. `25%`) in a point-on-segment assignment |
| **75** | Expected `at` or `nearest` in a point-on-segment assignment  |
| **76** | Missing keyword `of` in a `midpoint` assignment              |
