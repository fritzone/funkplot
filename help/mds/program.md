## A generic **fũnkplot** program

It is recommended, but not mandatory that the **fũnkplot** programs adhere to the following structure:

```python
# generic scene settings
# variable delcarations
# function declarations
# variable initialization
# code
```

This makes possible that the function declaration will use variables without raising an error of undefined variables and that all logical blocks of the program that belong in one place are grouped together.

The generic scene settings deal with parts of the program that for simple scenarios contain settings that affect the whole of the scene, such as the size of the pixels, or whether the coordinate system is visible or not, or the palette used.

The variable declarations declare the variables that will be used in the program, it is preferred to keep one line declarations.

The function declaration contain the functions that will be used in the program, it is possible to have more than one function.

In the variable initialization block you have to initialize the variables. Uninitialized variable don’t play well with the program.

Then finally in the code section you can start playing with the function.

Lines starting with the `#` symbol are considered comments and are ignored by the interpreter.