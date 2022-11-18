## The **plot** keyword

The `plot` keyword is used to draw the graphical representation of an expression. The colour, and the pixel size for the plot can be specified using the `set` keyword.

### Plotting a point

The following program plots one point on the screen:

```
set color darkgreen
set pixel size 4
var p point
let p = point at (1, 2)
plot p
```

Generating the following plot:

<img src="/home/fld/work/p/geodraw/help/mds/imgs/plot_1.png" alt="image-20221114103033355" style="zoom:50%;" />

### Plotting a function

The following code plots the function $sin(x)$ over the interval $(-π, π)$.

```
function f(x)=sin(x)
plot f over (-3.14, 3.14)
```

For plotting functions It is possible to specify the plot interval and the type of the plot (ie. a continuous plot, the increase step or the number of points used to represent the drawing).

If there is no plot interval, the default interval of $(-1, 1)$ is used. If there is no step or point count defined the default step of $0.01$ is used.

The program above will generate the following image:

<img src="/home/fld/work/p/geodraw/help/mds/imgs/plot_2.png" alt="image-20221114103139259" style="zoom:50%;" />

### Plotting the points of a function

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

<img src="/home/fld/work/p/geodraw/help/mds/plot_3.png" alt="image-20221114103240152" style="zoom:50%;" />
