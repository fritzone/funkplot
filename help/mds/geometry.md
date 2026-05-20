## Geometric Objects

**fũnkplot** supports various geometric primitives that can be drawn directly or stored in variables for later manipulation.

### Line and Segment Variables

You can declare and assign variables of type `line` and `segment`.

*   **Segment**: Represents a finite line segment between two points.
*   **Line**: Represents an infinite line passing through two points.

#### Declaration
```
var s segment
var l line
```

#### Assignment
Assignments can use point variables or literal coordinate pairs.

```
var p1 p2 point
let p1 = point at (0, 0)
let p2 = point at (10, 10)

# Assign a segment between p1 and p2
let s = segment from p1 to p2

# Assign an infinite line through p1 and (5, 5)
let l = line through p1 and (5, 5)
```

### Direct Drawing

Use the `draw` command to render geometric primitives on the scene.

#### Drawing Segments and Lines
You can draw objects stored in variables or define them on the fly.

```
# Draw variables
draw segment s
draw line l

# Draw directly
draw segment from (1, 1) to (2, 2)
draw line through p1 and p2
```

#### Drawing Points
```
draw point at (3, 4)
```

### Discrete Point Generation

Both `line` and `segment` can be used to generate a discrete list of points.

#### Using the `counts` specifier
When drawing a segment or line, you can specify how many points to render instead of a continuous line.

```
# Draw 5 discrete points along a segment
draw segment from (0, 0) to (4, 0) counts 5
```

#### Assigning to Point Lists
Lines and segments can be treated like functions to populate a `list of points`.

```
var ps list of points
let ps = points of s counts 100
plot ps

# For an infinite line, specify a range
let ps = points of l over (-10, 10) counts 256
```
