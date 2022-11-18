## The **rotate** keyword

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

<img src="/home/fld/work/p/geodraw/help/mds/imgs/rotate_1.png" alt="image-20221114103953276" style="zoom:50%;" />
