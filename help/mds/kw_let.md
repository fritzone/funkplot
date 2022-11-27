## The **let** keyword

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

### Creating a point

The `let` keyword is also used to create a point on the scene.

The following assignment assigns to the `point` type variable called `p` the point at `(1,2)` and plots it, the result is a small dot, indicating the point has been drawn.

```
var p point
let p = point at (1,2)
plot p
```

### The points of a function

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

#### Advanced points assignment

Since there can be situations where we want to plot functions outside of the default $[-1, 1]$ interval, the `let` keyword accepts extra parameters too, specifying the interval, the number of points or segments we want to draw and whether we want to plot using points of function or a continuous drawing.

##### Specifying the interval

The following example specifies the interval which is to be used to draw the function:

```
var ps list of points
function f(x) = sin(x)
let ps = points of f over (-3.14, 3.14)
plot ps
```

and results in the following plot:

![image-20220519144309975](/home/fld/work/p/funkplot/help/mds/imgs/let_2.png)

##### Specifying the number of points in a plot

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

##### Specifying the step used in a plot

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

### A list of points

The `let` keyword can be used to make an assignment to a list of points by using predefined values.

```
var ps list of points
let ps = list [(1,2);(3,4)]
```

This will assign the two points at $(1,2)$ and $(3,4)$ to the `ps` variable.