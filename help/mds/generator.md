## The surface generator

The surface generator feature of **fṻnkplot** is used to generate 3D surfaces from 2D plots. The surface generators are always created from the counted `points` of a function plot:

```
var p list
function f(x) = sin(x)
let p = points of f over (-3, 3) counts 50
# plot p
```

The reason for this is that `continuous` plots theoretically have an infinite number of points, so we need something that can be counted for the generation of a surface. Please note, in the example the `plot p` is commented out, it is not mandatory to have the plot be drawn at this point.

Once we have the plot we can create a `generator` type variable and assign to it a proper generator object:

```
var g generator
let g = generator for p using 20 iterations
```

The way currently the generators work is that they take the points of a plot, apply some mathematical calculations to them (such as transforming over an axis, rotating, etc …) and then combine the current points in the generator with the previous points, before the transformation, thus creating a band of triangle meshes between the two sets. This requires the generators to have a specific number of iterations, creating the specific number of bands, hence the `iterations` specifier in the creation of the generator.