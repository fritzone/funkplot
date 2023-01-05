## The **rotate** keyword

It is possible to rotate some of the objects found in **fũnkplot** applications. The following syntax is used to achieve this:

```
rotate <object> with <V> [degrees|radians]
```

Where

- `object` is the object that needs to be rotated. It can be either a point or a list of points.
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

While the following program:

```
var l list of points
var i number
let l = list[]
for i = 0 to 5 step 0.1 do
  append to l points (i, i)
  set pixel size 12 - 2 * (i + 1)
  rotate l with 10 degrees
  set color i * 25, i * 10, i * 10
  plot l
done
```

Generates the following plot:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/rotate_2.png" alt="image-20230105124209908" style="zoom:33%;" />
