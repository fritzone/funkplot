[TOC]

## Introduction

**fũnkplot** is a niche piece of mathematical modeling and plotting software, which is based on an interpreted descriptive language.

Using this programming language you can instruct **fũnkplot** to generate 2-dimensional plots to help you with your mathematic tasks.

After generation these plots and surfaces can be exported into various formats for further operations on them.

**fũnkplot** has python language bindings, it is possible to run python scripts on your generated data to further enlarge the interoperability of data between various programming platforms.

## First steps

After the installation when you have started **fũnkplot** it will be possible to load a few samples (from the samples directory) or just type in and run your program to see the generated plot in the plotting area.

### The **fũnkplot** programming language

**fũnkplot** uses its own programming language influenced by **basic** in its simplicity, with some extensions to allow more complex mathematical notions to be easily handled, and having a decent enough verbosity to feel like a natural language.

The process of creating a plot is very straightforward, just write a small program and run it. One of the most simple ones would take up just the following 2 lines:

```
function f(x) = sin(x)
plot f over (-5, 5)
```

And this will generate the following plot:

![image-20220425144408774](/home/fld/work/p/funkplot/help/mds/imgs/main.png)

**fũnkplot** supports a range of built-in mathematical functions, most of the default operators and has syntax for changing the color, line and rotation of the current plot.

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

## Keywords

The following are the keywords recognized by the **fũnkplot** interpreter.

| Keyword    | Description                                                  |
| :--------- | ------------------------------------------------------------ |
| `if`       | Checks for a condition and executes the given block of code in case of a statement which evaluates to non zero. |
| `for`      | Creates a loop for traversing over a set of data. The loop can be numeric loop with indexes or looping through a list of points. |
| `function` | Defines a function that can be used in plotting or experimenting. |
| `let`      | Defines the value for the given variable.                    |
| `plot`     | Plots the given function or list of points.                  |
| `rotate`   | Rotates the given point(s) around a given point with a specific angle. |
| `script`   | Runs a python script.                                        |
| `set`      | Sets variables altering the behaviour and visualization of the scene. |
| `var`      | Declares a variable.                                         |

### The **if** keyword

The `if` keyword is used to execute a block of code depending on the value of an expression. The generic form of `if` is:

```
var i number
let i = 4
if i == 4 do
  python script begin
    print(i)
  end
done
```

This will declare a variable`i`, and assign the value `4` to it, upon execution it will print the value to the output window:

```
4.0
```

The `if` can have an `else` branch too:

```
set pixel size 4
var ps list of points
var i number
function f(x) = x
let ps = points of f over (0, 1) counts 16
for i = 0 to 15 do

  if i % 2 == 0 do
    set color red
  else
    set color blue
  done

  plot ps[i]
done
```

This will generate the following plot:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/if_1.png" alt="image-20221012104843960" style="zoom: 33%;" />

### The **for** keyword

The `for` keyword is used to declare a loop that will execute a block of code a specific time. There are two forms a `for` loop can take in **fũnkplot**. The first one is a standard numeric loop:

```
var i number
for i = 1 to 5 do
  python script begin
    print(i)
  end
done
```

This will declare a variable`i`, and execute the body of the loop 5 times, which is calling a one line python script printing the value of i. The output should look like:

```
1.0
2.0
3.0
4.0
5.0
```

#### Looping through a list

The other form of the `for` loop is when looping through a list. This looks like:

```
var i number
var l list of numbers
let l = list [1,5,10,15]
for i in l do
  python script begin
    print(i)
  end
done
```

This will generate the following output:

```
1.0
5.0
10.0
15.0
```

#### Looping through the points of a function

The `for` loop can be used to loop through the points of the plot of a function, and it will make possible to perform various operations on individual points, like in the example below:

```
set pixel size 2
var a number
var p point
var ps list of points

function f(x) = x * sin(x)

let ps = points of f over (-2,2) counts 360
let a = 0

set color blueviolet
for p in ps do
  rotate p with a degrees
  let a = a + 1
  plot p
done

set color brown
plot ps
```

This will generate the following image:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/for_1.png" alt="image-20221006115636896" style="zoom:50%;" />

### The **function** keyword

The `function` keyword is used to declare the mathematical functions you intend to plot. The syntax is:

```
function f(x) = expression
```

Where expression might consist of any of the following terms, even combined with one another:

- mathematical operators:
  - $+$ (addition)
  - $-$ (subtraction)
  - $/$ (division)
  - $*$ (multiplication)
  - `%` (modulo)
  - `^` (power)
- parenthesis $($ and $)$ to denote the precedence of operations
- algebraical and trigonometrical functions:
  - $sin$ (The sine function)
  - $cos$ (The cosine function)
  - $tan$ (The tangent function)
  - $cot$ (The cotangent function)
  - $sec$ (The secant function)
  - $cosec$ (The cosecant function)
  - $asin$ (The arcsine function)
  - $acos$ (The arccosine function)
  - $atan$ (The arctangent function)
  - $actg$ (The arc-cotangent function)
  - $asec$ (The arcsecant function)
  - $acosec$ (The arc-cosecant function)
  - $exp$ (The exponential function)
  - $log$ (The common logarithmic function)
  - $ln$ (The logarithmic function)
  - $sqrt$ (The square root function)

- a variable defined in the `var` section of the application
  - This situation will bind a reference to the variable into the body of the function, so each evaluation of the functions’ values reflects the state of the variable used in it

The order of evaluation of the operators is a mathematical one.

It is possible to declare more than one function in a program.

#### Example

The following code plots a colourful Lissajous curve by defining two functions:

```
set palette firestrm
set pixel size 5

var pssin pscos list of points
var i x y number
var ps pc p point
let i = 0
function ps(x) = 2 * sin(3 * x + 3.14/2)
function pc(x) = 3 * sin(2 * x)

let pssin = points of ps over (-3.14, 3.14) counts 256
let pscos = points of pc over (-3.14, 3.14) counts 256

for i = 0 to 255 do

  let ps = pssin[i]
  let pc = pscos[i]

  let x = ps.y
  let y = pc.y

  let p = point at (x,y)
  set color palette[i]
  plot p

done
```

Resulting in the following plot:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/fun_1.png" alt="image-20221021092112328" style="zoom:33%;" />

### The **let** keyword

The `let` keyword is used to make an assignment to a declared variable. It’s basic syntax is:

```
var a number
let a = 1.2
```

This will assign the value `1.2` to the variable `a`, which was declared to be of type `number`. It will raise a syntax error to assign to a variable which was not declared, or if the type is different.

It is possible to assign arithmetic expressions to numeric type variables, so the following is syntactically correct:

```
var a b c number
let a = 1.2
let b = 2.3
let c = a + b
```

#### Creating a point

The `let` keyword is also used to create a point on the scene.

The following assignment assigns to the `point` type variable called `p` the point at `(1,2)` and plots it, the result is a small dot, indicating the point has been drawn.

```
var p point
let p = point at (1,2)
plot p
```

#### The points of a function

The `let` keyword can be also used to assign the points of a drawable object (such as a function) to a variable which can be manipulated in various ways.

The following piece of code will create such a variable:

```
var ps list of points
function f(x) = sin(x)
let ps = points of f 
plot ps
```

and running it will result in the following drawing:

![image-20220519143757398](/home/fld/work/p/funkplot/help/mds/imgs/let_1.png)

The algorithm operating on the default interval $[-1, 1]$ is a simple iterative one, that starts $x$ from the first point in the default drawing interval (`-1`) till the end of the default drawing interval (`1`) and will use a default step of `0.01` for advancing the function drawing, while calculating the value $f(x)$ as per the mathematical formula of the function.

##### Advanced points assignment

Since there can be situations where we want to plot functions outside of the default $[-1, 1]$ interval, the `let` keyword accepts extra parameters too, specifying the interval, the number of points or segments we want to draw and whether we want to plot using points of function or a continuous drawing.

###### Specifying the interval

The following example specifies the interval which is to be used to draw the function:

```
var ps list of points
function f(x) = sin(x)
let ps = points of f over (-3.14, 3.14)
plot ps
```

and results in the following plot:

![image-20220519144309975](/home/fld/work/p/funkplot/help/mds/imgs/let_2.png)

###### Specifying the number of points in a plot

It is possible to specify how many points should a drawing contain to have a better control over your data using the `counts` keyword. The following example specifies the number of points that will be used to draw the function:

```
var ps list of points
function f(x) = sin(x)
let ps = points of f over (-3.14, 3.14) counts 24
plot ps
```

and results in the following plot:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/let_3.png" alt="image-20221005133909496" style="zoom:50%;" />

This approach will calculate the step value considering the length of the interval to obtain the required number of points.

###### Specifying the step used in a plot

It is possible to specify the step used when drawing the plot of the function using the `step` keyword. The following example specifies the steps that will be used to draw the function:

```
var ps list of points
function f(x) = sin(x)
let ps = points of f over (-3.14, 3.14) step 0.1
plot ps
```

and results in the following plot:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/let_4.png" alt="image-20221005141804680" style="zoom:50%;" />

Since the step is calculated based on the number of points, it is not possible to specify both the steps and the number of points.

#### A list of points

The `let` keyword can be used to make an assignment to a list of points by using predefined values.

```
var ps list of points
let ps = list [(1,2);(3,4)]
```

This will assign the two points at $(1,2)$ and $(3,4)$ to the `ps` variable.

### The **plot** keyword

The `plot` keyword is used to draw the graphical representation of an expression. The colour, and the pixel size for the plot can be specified using the `set` keyword.

#### Plotting a point

The following program plots one point on the screen:

```
set color darkgreen
set pixel size 4
var p point
let p = point at (1, 2)
plot p
```

Generating the following plot:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/plot_1.png" alt="image-20221114103033355" style="zoom:50%;" />

#### Plotting a function

The following code plots the function $sin(x)$ over the interval $(-π, π)$.

```
function f(x)=sin(x)
plot f over (-3.14, 3.14)
```

For plotting functions It is possible to specify the plot interval and the type of the plot (ie. a continuous plot, the increase step or the number of points used to represent the drawing).

If there is no plot interval, the default interval of $(-1, 1)$ is used. If there is no step or point count defined the default step of $0.01$ is used.

The program above will generate the following image:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/plot_2.png" alt="image-20221114103139259" style="zoom:50%;" />

#### Plotting the points of a function

It is possible to plot the points of a function while iterating over them with an index counter, or by point reference.

```
set pixel size 2
var p point
var ps list of points

function f(x) = sin(x)

let ps = points of f over (-3.14, 3.14)

for p in ps do
  plot p
done
```

Generating the following plot:

<img src="/home/fld/work/p/funkplot/help/mds/plot_3.png" alt="image-20221114103240152" style="zoom:50%;" />

### The **rotate** keyword

It is possible to rotate some of the objects found in **fũnkplot** applications. The following syntax is used to achieve this:

```
rotate <point> with <V> [degrees|radians]
```

Where

- `point` is the point that needs to be rotated.
- `V` is the value we are rotating the object with
- It is possible to optionally specify whether the given value is `radians` or `degrees`. If nothing is specified, the default is `degrees`.

The following program:

```
set palette afternoon_heat
set pixel size 2

# variable declarations
var i number
var ps list of points
var p point

# the function to plot   
function f(x) = sin(x)

# the actual code 

let ps = points of f over (-3.14, 3.14) counts 360
for i = 0 to 255 do
  let p = ps[i]
  set color palette[i]
  rotate p with i degrees
  plot p
done

```

Generates the following plot:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/rotate_1.png" alt="image-20221114103953276" style="zoom:50%;" />

### The **script** keyword

It is possible to embed python scripts in **fũnkplot** code. The following syntax is used to achieve this:

```
python script begin
#
# proper python code
#
end
```

The python script will have access to the variables defined in the **fũnkplot** code in the following manner:

- Variables with the type `number` will be treated as python floating point types.
- For variables with type `point` a new type is created, called `Point` and it is introduced in the python propgram, having a simple structure with `x` and `y` members:

```python
class Point(object):
    def __init__(self, x, y):
        self.x = x
        self.y = y
```

- All the variables that are in the **fũnkplot** program are added as an extra codeline before the script is copied over.

#### Example

```
var i number
var p point
var ps list of points
var ns list of numbers

function f(x) = sin(x)

let i = 0
let p = point at (1,2)
let ps = points of f counts 6
let ns = list [1,2,3,5,8,11]

python script begin

print("i=", i)
print("p=(", p.x, ",", p.y, ")")
l = 0
while l < len(ps):
  print("ps[",l,"]=(", ps[l].x, ",", ps[l].y, ")")
  l += 1

for l in range(len(ns)):
  print(ns[l])

end
```

Will generate the following python script:

```python
class Point(object):
    def __init__(self, x, y):
        self.x = x
        self.y = y
ps=[Point(-1.000000, -0.841471),Point(-0.600000, -0.564642),Point(-0.200000, -0.198669),Point(0.200000, 0.198669),Point(0.600000, 0.564642),Point(1.000000, 0.841471)]
p=Point(1.000000, 2.000000)
ns=[1.000000, 2.000000, 3.000000, 5.000000, 8.000000, 11.000000]
i=0.000000

print("i=", i)
print("p=(", p.x, ",", p.y, ")")
l = 0
while l < len(ps):
  print("ps[",l,"]=(", ps[l].x, ",", ps[l].y, ")")
  l += 1

for l in range(len(ns)):
  print(ns[l])
```

And the following output when executed:

```
i= 0.0
p=( 1.0 , 2.0 )
ps[ 0 ]=( -1.0 , -0.841471 )
ps[ 1 ]=( -0.6 , -0.564642 )
ps[ 2 ]=( -0.2 , -0.198669 )
ps[ 3 ]=( 0.2 , 0.198669 )
ps[ 4 ]=( 0.6 , 0.564642 )
ps[ 5 ]=( 1.0 , 0.841471 )
1.0
2.0
3.0
5.0
8.0
11.0

```



### The **set** keyword

The `set` keyword is used to set variables altering the behaviour of the rendered scene. It is recommended to place the scene altering code sequences before any of the other code sequences.

The following values can be set:

| Setting        | Description                                                  |
| -------------- | ------------------------------------------------------------ |
| colour / color | Sets the colour of the next pixel to be drawn. Both UK and US spellings are allowed. |
| palette        | Sets the palette to be used for drawing the scene.           |
| coordinates    | Sets the properties of the coordinate system.                |
| pixel          | Sets the properties of the next pixel to be drawn.           |

#### Setting the colour

Setting the colour can be done using one of the following available syntaxes:

##### By name

Using the syntax `set colour name` sets the colour based on the name of it, using 255 as alpha value. A list of all colour names is available in the Miscellaneous section of this help system.

##### By name and alpha

Using the syntax `set colour name,alpha` sets the colour based on the name of it, using the specified alpha value.

##### From a palette

Using the syntax `set colour palette[index]` will make possible to set the colour with the given index from the palette that was selected with the `set palette …` statement, and will use the alpha channel set to 255. If there is no such palette found this statement has no effect and the colour black will be used.

##### From a palette with alpha value

Using the syntax `set colour palette[index],alpha` will make possible to set the colour with the given index from the palette that was selected with the `set palette …` statement and will calculate the alpha from the specified value. If there is no such palette this statement has no effect and the colour black will be used.

##### By RGB code

Using the syntax `set colour #<value>` will allow you to directly set the red, green and blue values of the colour using the well know HTML syntax. The following combinations are accepted:

- three digit colour code **RGB** : `set colour #123`.  Will set the specified colour with the alpha channel set to 255.
- six digit colour code **RRGGBB**: `set colour #112233`. Will set the specified colour with the alpha channel set to 255.
- eight digit colour code **RRGGBBAA**: `set colour #11223344`. Will set the specified colour with the specified alpha value.

#### Setting the palette

The syntax `set palette name` sets the palette that can be accessed in a `set color palette[index]` statement. A list of available palettes can be found in the Miscellaneous section of this help system.

#### Setting the properties of the coordinate system

It is possible to modify the properties of the coordinate system, such as the bounding box of it, the rotation and the zoom, or to hide the coordinate system if desired. The following table lists the values that can be changed.

| What         | Description                                                  | Example                               |
| ------------ | ------------------------------------------------------------ | ------------------------------------- |
| `on` / `off` | Shows or hides the coordinate system. The coordinate system by default is shown so you might consider hiding it if you wish. | `set coordinates off`                 |
| `start x`    | Sets the start value of the coordinate systems’ X axis.      | `set coordinates start x -4`          |
| `start y`    | Sets the start value of the coordinate systems’ Y axis.      | `set coordinates start y -4`          |
| `end x`      | Sets the start end of the coordinate systems’ X axis.        | `set coordinates end x 4`             |
| `end y`      | Sets the end value of the coordinate systems’ Y axis.        | `set coordinates end y 4`             |
| `zoom`       | Sets the zoom used on the coordinate system. The optimal value is depending on your screen resolution. | `set coordinates zoom 110`            |
| `rotation`   | Sets the rotation of the coordinate system. The value can be either degrees or radians (default). | `set coordinates rotation 45 degrees` |
| `grid`       | Shows or hides the grid of the coordinate system. By default the grid is visible, so you might consider hiding it. Accepts `on` or `off` as parameter. | `set coordinates grid off`            |

Due to the way the presentation of data is handled in the application, the statements affecting the behaviour of the coordinate system are executed before any other code is executed, so it is recommended to group the before any other statements

#### Setting the pixel properties

For the moment it is possible to set the size of a pixel with the command `set pixel size X` where X denotes an expression that will be evaluated.

#### Example

The following program sets the basic properties of the coordinate system and then draws a colourful line with increasing pixel sizes.

```
set coordinates start x -4
set coordinates start y -4
set coordinates end x 4
set coordinates end y 4
set coordinates zoom 110
set coordinates rotation 45 degrees

set palette precolombinas

var ps list of points
var i number

function f(x) = x
let ps = points of f over (-2, 2) counts 256

for i = 0 to 255 do
  set pixel size i/3+4
  set colour palette[255-i]
  plot ps[i]
done
```

Resulting in the following plot:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/set_1.png" alt="image-20221019145527702" style="zoom:33%;" />

### The **var** keyword

The `var` keyword is used to declare variables that later can be used to perform various operations with. The syntax is

```
var a b c number
```

This will declare`a`, `b` and `c` to be numeric variables. In order for proper behaviour you have to initializer the variables and then you can use them in operations later on.

The following variable types are recognized in **fũnkplot**.

- `number`
- `point`
- `list of numbers`. Please note the plural form of `number`.
- `list of points`. Please note the plural form of `point`.

For easier readability it is recommended that you organize your variables by type like in the example below:

```
var a b c d i number
var p1 p2 p3 point
var l1 l2 list of numbers
var ps1 ps2 list of points
```

## Error messages

The following error messages and associated codes are used in **fũnkplot**:

| ERROR  | DESCRIPTION                                                  |
| ------ | ------------------------------------------------------------ |
| **0**  | Erroneous looping conditions detected. A loop with a negative step value, but with the start value being smaller than the end might generate this error. |
| **1**  | Invalid variable declaration. Not specifying the type of a variable might cause this error to pop up. |
| **2**  | Invalid variable type. The list of valid variable types is `number`, `point` and `list`. Anything else will lead to this error. |
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
| **31** | Unknown pixel property                                       |
| **32** | Wrong palette indexing                                       |
| **33** | Invalid color to set.                                        |
| **34** | Cannot identify the object assigning from                    |
| **35** | Invalid statement                                            |
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
| **49** | A value was declared and used but not defined to hold a value |
| **50** | Cannot plot a number                                         |
| **51** | Invalid assignment missing of keyword                        |
| **52** | No such function                                             |
| **53** | Invalid keyword                                              |
| **54** | Something messy with an index                                |
| **55** | Index out of bounds                                          |
| **56** | Invalid index (ie: the resulted index is $< 0$).             |
| **57** | Cannot identify the object to assign to.                     |
| **58** | Incompatible assignment types don't match.                   |
| **59** | Append statement does not contain the TO keyword             |
| **60** | No function for **points of** assignment                     |
| **61** | Cannot assign a non numeric value to a numeric variable      |
| **62** | Cannot identify the object assigning from                    |
| **64** | Cannot identify the assignment source                        |
| **65** | Cannot use a point type variable in that context             |

