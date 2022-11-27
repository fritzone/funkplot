## The **function** keyword

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

### Example

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
