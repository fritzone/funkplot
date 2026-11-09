## Keywords

The following are the keywords recognized by the **fũnkplot** interpreter.

| Keyword    | Description                                                  |
| :--------- | ------------------------------------------------------------ |
| `append`   | Will append a value to the given list.                       |
| `draw`     | Draws geometric objects: points, segments, lines and angles. |
| `if`       | Checks for a condition and executes the given block of code in case of a statement which evaluates to non zero. It can have an `else` branch, and ends with `done`. |
| `for`      | Creates a loop for traversing over a set of data. The loop can be numeric loop with indexes or looping through a list of points. The body of the loop ends with `done`. |
| `function` | Defines a function that can be used in plotting or experimenting. |
| `let`      | Defines the value for the given variable.                    |
| `parametric` | Defines a parametric function, see the `function` keyword. |
| `plot`     | Plots the given function or list of points.                  |
| `polar`    | Placed before `plot`, interprets the plotted function in polar coordinates. |
| `rotate`   | Rotates the given point(s) around a given point with a specific angle. |
| `python`   | Runs a python script (`python script begin` … `end`), or a single python statement (`python do …`). |
| `set`      | Sets variables altering the behaviour and visualization of the scene. |
| `var`      | Declares a variable.                                         |

Lines starting with `#` are comments. The words `of`, `over`, `counts`, `step`, `continuous`, `to`, `in`, `range`, `do`, `with`, `around`, `at`, `as`, `on`, `from`, `through`, `and` and the other words described on the pages of the individual keywords complete these statements.
