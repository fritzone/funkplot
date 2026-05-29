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

#### Naming the endpoints of a segment

Any segment form where the endpoints are not already named point variables can have `as "XY"` appended. The string is split into two labels — the first label names the start endpoint, the second names the end endpoint. Each label is one letter optionally followed by digits (`A`, `B`, `P1`, `Q2`, …). When the segment is drawn both endpoints are automatically drawn as dots (using the current pixel size and color) and their labels are rendered in the same italic serif font used for point names, with digit subscripts.

```
# Coordinate-literal form
let s = segment from (0, 0) to (6, 4) as "AB"

# Extended forms
let s2 = segment from p length 5 at angle 30 degrees as "PQ"
let s3 = segment centered at p length 6 at angle alpha as "MN"
let s4 = segment through q parallel to s1 length 4 as "CD"
let s5 = segment through q perpendicular to s1 length 4 as "P1Q1"
```

Draw the segment normally — no extra syntax needed at draw time:

```
draw segment s
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

### Intersection of Two Segments or Lines

The `point at intersection of` syntax creates a point at the crossing of two segments or lines.

```
var s1 s2 segment
var p point

let s1 = segment from (1, 1) to (7, 7)
let s2 = segment from (1, 7) to (7, 1)
let p = point at intersection of s1 and s2
set pixel size 10
plot p
```

---

### Points on a Segment

**fũnkplot** provides a rich set of syntaxes for creating a `point` variable located at a precisely defined position on a segment. All forms follow the general shape:

```
let p = <locator> on <segment> <position-spec>
```

The segment must have been declared and assigned before use.

#### Midpoint

The simplest form — places a point exactly halfway between the two endpoints.

```
var s segment
var p point
let s = segment from (-4, 0) to (4, 0)
let p = midpoint of s
```

The result is always `(0, 0)` for the example above.

---

#### Percentage Along the Segment

Places a point at a given percentage of the total length, measured from either endpoint. The `from` direction is **always required**.

```
let p = point on s at 25% from start
let p = point on s at 75% from end
```

The percentage value may be any arithmetic expression:

```
let p = point on s at (100/3)% from start
```

`from start` counts from the first endpoint toward the second; `from end` counts from the second endpoint toward the first. Both clamp to `[0, 1]` so an out-of-range percentage lands on the nearest endpoint rather than escaping the segment.

---

#### Using Named Endpoints

When a segment is defined using named point variables, those names can be used in place of `start` and `end`.

```
var p1 p2 point
var s segment
let p1 = point at (-4, -2)
let p2 = point at (4, 2)
let s = segment from p1 to p2

var p point
let p = point on s at 25% from p1   # identical to: at 25% from start
let p = point on s at 25% from p2   # identical to: at 25% from end
```

This makes programs that carry geometric meaning self-documenting — referring to `p1` instead of `start` keeps the intention explicit when the endpoints have meaningful names.

---

#### Parametric Parameter t

Places a point at parameter $t \in [0, 1]$ along the segment, where $t = 0$ is the start and $t = 1$ is the end. This is the most direct form — all other positional syntaxes reduce to this internally.

```
let p = point on s at t 0.333
```

Values outside $[0, 1]$ are clamped. The expression after `t` can be any arithmetic expression.

---

#### Distance from an Endpoint

Places a point at a specific Euclidean distance from either endpoint. The `from` direction is required (same as the percentage form).

```
let p = point on s at distance 3.5 from start
let p = point on s at distance 3.5 from end
```

If the distance exceeds the segment length the result is clamped to the far endpoint. Named endpoints work here too:

```
let p = point on s at distance 3.5 from p1
```

---

#### Nearest Point to an External Point

Projects an external point onto the segment, returning the closest point on the segment. The projection is clamped so the result is always on the segment even if the external point is "past" an endpoint.

```
var q point
let q = point at (5, -5)
let p = point on s nearest to q
```

---

#### Point at a Given X Coordinate

Returns the point on the segment whose x-coordinate equals the given value. Returns nothing (the variable is left undefined) if the segment is vertical or if the x value lies outside the segment's horizontal extent.

```
let p = point on s at x 2
```

---

#### Point at a Given Y Coordinate

Returns the point on the segment whose y-coordinate equals the given value. Returns nothing if the segment is horizontal or if the y value lies outside the segment's vertical extent.

```
let p = point on s at y -3
```

---

#### Naming a point on a segment

Any of the above forms can be followed by `as "NAME"` to attach a drawn label to the point. The label is rendered in italic serif, and a trailing digit is drawn as a subscript (so `"A1"` appears as *A₁*):

```
let pa = point on s at 25% from start as "A"
let pb = point on s at t 0.5         as "M"
let pc = point on s at x 2           as "P1"
```

When two points land at the same position their labels are stacked automatically to avoid overlap.

---

### Complete Example

The following program demonstrates all the above syntaxes together. Two segments are used — `s1` for the positional syntaxes and `s2` for the coordinate-based ones.

```
# Demonstration of all point-on-segment syntaxes
#
# s1 : from named points p1(-8,-4) to p2(8,4)  — holds most demos
# s2 : from (-3,-7) to (3,7)                   — used for at-x and at-y
# q  : an external point used for the "nearest to" demo

var p1 p2 q point
var s1 s2 segment
var pa pb pc pd pm pe pf pg ph pi pj point

let p1 = point at (-8, -4)
let p2 = point at (8, 4)
let s1 = segment from p1 to p2
let s2 = segment from (-3, -7) to (3, 7)
let q  = point at (5, -5)

set color slategray
draw segment s1
draw segment s2

set color white
set pixel size 8
plot p1
plot p2

set color orange
set pixel size 8
plot q

# 1. at 25% from start
let pa = point on s1 at 25% from start
set color red
set pixel size 10
plot pa

# 2. at 25% from end
let pb = point on s1 at 25% from end
set color royalblue
set pixel size 10
plot pb

# 3. at 25% from named start-point (same location as pa)
let pc = point on s1 at 25% from p1
set color lime
set pixel size 5
plot pc

# 4. at 25% from named end-point (same location as pb)
let pd = point on s1 at 25% from p2
set color deepskyblue
set pixel size 5
plot pd

# 5. midpoint
let pm = midpoint of s1
set color yellow
set pixel size 14
plot pm

# 6. at distance 7 from start
let pe = point on s1 at distance 7 from start
set color magenta
set pixel size 10
plot pe

# 7. at distance 7 from end
let pf = point on s1 at distance 7 from end
set color deeppink
set pixel size 10
plot pf

# 8. at parametric t
let pg = point on s1 at t 0.15
set color coral
set pixel size 10
plot pg

# 9. nearest to q
let ph = point on s1 nearest to q
set color hotpink
set pixel size 10
plot ph

# 10. at x 2  (on s2)
let pi = point on s2 at x 2
set color gold
set pixel size 10
plot pi

# 11. at y -3  (on s2)
let pj = point on s2 at y -3
set color springgreen
set pixel size 10
plot pj
```

Running this program produces the following output:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/points.png" alt="Point-on-segment demo output" style="zoom:50%;" />

#### What each point represents

The table below lists every plotted variable, the syntax that produced it, and the exact coordinates that result from the geometry.

`s1` runs from `p1 = (-8, -4)` to `p2 = (8, 4)`, giving a total length of $\sqrt{16^2+8^2} \approx 17.89$.
`s2` runs from `(-3, -7)` to `(3, 7)`, giving a total length of $\sqrt{6^2+14^2} \approx 15.23$.

| Variable | Colour | Syntax | Result | Notes |
|---|---|---|---|---|
| `p1` | white | `point at (-8, -4)` | `(-8, -4)` | Start endpoint of s1 |
| `p2` | white | `point at (8, 4)` | `(8, 4)` | End endpoint of s1 |
| `q` | orange | `point at (5, -5)` | `(5, -5)` | External reference for nearest-to |
| `pa` | red | `point on s1 at 25% from start` | `(-4, -2)` | One quarter from p1 |
| `pb` | royalblue | `point on s1 at 25% from end` | `(4, 2)` | One quarter from p2 |
| `pc` | lime | `point on s1 at 25% from p1` | `(-4, -2)` | Same as `pa` — named endpoint form |
| `pd` | deepskyblue | `point on s1 at 25% from p2` | `(4, 2)` | Same as `pb` — named endpoint form |
| `pm` | yellow | `midpoint of s1` | `(0, 0)` | Exact centre of s1 |
| `pe` | magenta | `point on s1 at distance 7 from start` | `≈ (-1.74, -0.87)` | $t \approx 0.391$ |
| `pf` | deeppink | `point on s1 at distance 7 from end` | `≈ (1.74, 0.87)` | $t \approx 0.609$ |
| `pg` | coral | `point on s1 at t 0.15` | `(-5.6, -2.8)` | 15 % along s1 |
| `ph` | hotpink | `point on s1 nearest to q` | `≈ (2, 1)` | Projection of `q=(5,-5)` onto s1 |
| `pi` | gold | `point on s2 at x 2` | `≈ (2, 4.67)` | Where s2 crosses $x = 2$ |
| `pj` | springgreen | `point on s2 at y -3` | `≈ (-1.29, -3)` | Where s2 crosses $y = -3$ |

Points `pc` and `pd` are plotted at size 5 on top of `pa` (size 10) and `pb` (size 10) respectively, to visually confirm that the named-endpoint and positional forms produce the same coordinates.

---

### Angles

The `angle` type stores a rotation value. Internally it is always kept in radians, but you can enter values in degrees (the default) or radians.

#### Declaration

```
var alpha beta angle
```

#### Assignment forms

| Syntax | Meaning |
|---|---|
| `let alpha = 45` | 45 degrees (no unit → degrees by default) |
| `let alpha = 45 degrees` | 45 degrees, explicit |
| `let alpha = pi/4 radians` | π/4 radians |
| `let alpha = angle of s1` | direction of segment `s1` (`atan2(y2−y1, x2−x1)`) |
| `let alpha = angle p1 p2 p3` | interior angle at `p2` formed by the three points |

Any arithmetic expression is valid as the numeric value. The `angle of` form requires a declared `segment` variable. The three-point form returns a value in [0°, 180°].

#### Example

```
var s1 segment
var p1 p2 p3 point
var alpha beta gamma angle

let s1 = segment from (0, 0) to (3, 4)
let p1 = point at (0, 0)
let p2 = point at (4, 0)
let p3 = point at (4, 3)

let alpha = 30 degrees
let beta  = angle of s1
let gamma = angle p1 p2 p3
```

---

### Extended Segment Definitions

In addition to `segment from P to Q`, **fũnkplot** provides four extended forms that let you define a segment by its position, length, and orientation without having to compute the second endpoint by hand.

All four forms accept an angle variable or a literal angle value (with an optional `degrees`/`radians` unit; default is degrees). They also accept a length expression.

#### 1. From a point, given length and angle

Places the segment starting at `P`, extending along direction `alpha` for `length` units:

```
let s = segment from P length 5 at angle alpha
let s = segment from P length 5 at angle 45 degrees
let s = segment from P length 5 at angle pi/4 radians
```

The start endpoint is `P`; the end endpoint is `P + length × (cos α, sin α)`.

#### 2. Centered at a point, given length and angle

Places the segment so that its midpoint is exactly at `P`, with the given length and direction:

```
let s = segment centered at P length 5 at angle alpha
let s = segment centered at P length 6 at angle 30 degrees
```

The start is `P − (length/2) × (cos α, sin α)` and the end is `P + (length/2) × (cos α, sin α)`.

#### 3. Through a point, parallel to a reference segment

Creates a segment centered on `P` with the same direction as `refSeg` and the given length:

```
let s = segment through P parallel to refSeg length 5
```

The angle is taken from `refSeg` using `atan2`. The resulting segment is centered on `P`.

#### 4. Through a point, perpendicular to a reference segment

Creates a segment centered on `P` perpendicular to `refSeg` with the given length:

```
let s = segment through P perpendicular to refSeg length 5
```

The angle is `atan2(refSeg) + π/2`. The resulting segment is centered on `P`.

#### Extended segment example

```
var p q point
var s1 s2 s3 s4 s5 segment
var alpha angle

let p = point at (0, 0)
let q = point at (2, 1)
let s1 = segment from (-4, -1) to (4, 1) as "AB"

let alpha = 30 degrees

# from p at 30°, length 6 — label endpoints C and D
let s2 = segment from p length 6 at angle alpha as "CD"

# centered at q at 30°, length 4 — label endpoints M and N
let s3 = segment centered at q length 4 at angle alpha as "MN"

# through p, parallel to s1, length 6
let s4 = segment through p parallel to s1 length 6

# through q, perpendicular to s1, length 4
let s5 = segment through q perpendicular to s1 length 4

draw segment s1
draw segment s2
draw segment s3
draw segment s4
draw segment s5
```
