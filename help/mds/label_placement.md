## Label Placement

By default **fũnkplot** draws every point label at 45° (upper-right), 12 px away from the dot. Four families of `set label` / `set labels` commands let you override every aspect of that placement, either globally or on a per-point basis.

<img src="/home/fld/work/p/funkplot/help/mds/imgs/label_placement.png" alt="Label placement demo" style="zoom:50%;" />

---

### `set label position`

Controls the direction in which the label is offset from the point. The setting applies to all subsequent `plot` commands until it is changed again.

```
set label position before
set label position after
set label position above
set label position below
set label position under
set label position angle <value>
set label position angle <value> degrees
set label position angle <value> radians
```

| Direction | Meaning |
|---|---|
| `before` | Directly to the left (−x direction) |
| `after` | Directly to the right (+x direction) |
| `above` | Directly upward (+y direction) |
| `below` / `under` | Directly downward (−y direction) |
| `angle <value>` | Arbitrary angle; default unit is **degrees** |
| `angle <value> degrees` | Explicit degrees |
| `angle <value> radians` | Explicit radians |

The angle is measured counter-clockwise from the positive x-axis, following the standard mathematical convention. `angle 0` is identical to `after`, `angle 90` is identical to `above`, `angle 180` is identical to `before`, and so on.

```
var pAbove pBelow pBefore pAfter pAngle point

let pAbove  = point at (-4, 3) as "A"
let pBelow  = point at ( 0, 3) as "B"
let pBefore = point at ( 4, 3) as "C"
let pAfter  = point at (-4, 1) as "D"
let pAngle  = point at ( 0, 1) as "E"

set pixel size 2

set label position above
plot pAbove

set label position below
plot pBelow

set label position before
plot pBefore

set label position after
plot pAfter

# upper-left at 135°
set label position angle 135
plot pAngle
```

---

### `set label distance`

Controls how far the label is offset from the dot, measured in screen pixels.

```
set label distance close
set label distance far
```

| Value | Pixel gap |
|---|---|
| `close` | 6 px |
| `far` | 22 px |

The default distance is 12 px, midway between the two named values. Changing the distance does not affect the direction — both settings are independent.

```
var pClose pFar point

let pClose = point at (-3, -2) as "close"
let pFar   = point at ( 3, -2) as "far"

set label position above

set label distance close
plot pClose

set label distance far
plot pFar
```

---

### `set labels on` / `set labels off`

Suppresses or restores all label rendering globally. When labels are off the dot is still drawn; only the text is hidden.

```
set labels off
set labels on
```

The effect applies from the point of the statement onward. Re-enabling with `set labels on` restores all subsequent labels.

```
var pVis pHidden point

let pVis    = point at (2, 0) as "visible"
let pHidden = point at (5, 0) as "hidden"

set label position after
set label distance close

set labels on
plot pVis        # label drawn

set labels off
plot pHidden     # dot only — no label

set labels on    # restore for subsequent statements
```

---

### `set label first … second …`

Overrides the label direction at each endpoint of a segment drawn with `draw segment`. When not set, the engine computes a geometry-driven placement (perpendicular to the segment, away from the other endpoint).

```
set label first <direction> second <direction>
```

Both `first` and `second` accept the same direction tokens as `set label position` (`before`, `after`, `above`, `below`, `under`, `angle <value> [degrees|radians]`). `first` refers to the start endpoint of the segment, `second` to the end endpoint.

Setting `first` and `second` is cumulative with `set label distance` — the pixel gap for endpoint labels is taken from the current label distance.

```
var sAB sCD sEF segment

let sAB = segment from (-7, -4) to (-1, -4) as "AB"
let sCD = segment from ( 1, -4) to ( 7, -5) as "CD"
let sEF = segment from (-5, -7) to ( 5, -7) as "EF"

set label distance close
set pixel size 2

# geometry-driven (perpendicular outward) — no override needed
set color slategray
draw segment sAB

# force A above, B below
set label first above second below
set color seagreen
draw segment sCD

# flank the endpoints horizontally
set label first before second after
set color chocolate
draw segment sEF
```

To clear the override and revert to geometry-driven placement, set the direction back to `angle 0` or use any value — the geometry mode cannot be explicitly requested; it is active whenever no `set label first/second` has been issued since the last run.

---

### Duplicate label auto-rename

When two or more points share the same label, **fũnkplot** automatically appends a numeric suffix so every label on screen is unique. The suffix starts at `1` and increments for each additional duplicate.

| Draw order | Raw label | Rendered as |
|---|---|---|
| 1st | `"P"` | P |
| 2nd | `"P"` | P1 |
| 3rd | `"P"` | P2 |

This rename happens transparently — the variable names in the script are unchanged, only the on-screen text differs.

```
var pDup1 pDup2 pDup3 point

let pDup1 = point at (7, 5) as "P"
let pDup2 = point at (9, 3) as "P"   # will appear as P1
let pDup3 = point at (7, 1) as "P"   # will appear as P2

set pixel size 10
set label position angle 45
set label distance close
plot pDup1
plot pDup2
plot pDup3
```

The rename counter resets every time the program is re-run, so the first plotted `"P"` always stays as P.

### Complete example

The following program exercises all label-placement features together. Copy it into the editor and run it to verify the rendering.

```
var pA pB pC point
var pAbove pBelow pBefore pAfter point
var pAng135 pAng1r point
var pClose pFar point
var pDup1 pDup2 pDup3 point
var pVis pHidden point
var sGH sIJ sKL segment


# ── Section 1: DEFAULT placement ─────────────────────────────────────────────
# No "set label" needed; defaults are upper-right (45°), distance 12 px.

let pA = point at (-6, 6) as "A"
let pB = point at (-2, 6) as "B"
let pC = point at ( 2, 6) as "C"

set color steelblue
set pixel size 10
plot pA
plot pB
plot pC


# ── Section 2: POSITION DIRECTIONS ───────────────────────────────────────────

let pAbove  = point at (-8,  3) as "above"
let pBelow  = point at (-4,  3) as "below"
let pBefore = point at ( 0,  3) as "before"
let pAfter  = point at ( 4,  3) as "after"
let pAng135 = point at (-6,  1) as "135deg"
let pAng1r  = point at ( 1,  1) as "1.0rad"

set color tomato
set pixel size 10

set label position above
plot pAbove

set label position below
plot pBelow

set label position before
plot pBefore

set label position after
plot pAfter

set label position angle 135
plot pAng135

set label position angle 1.0 radians
plot pAng1r


# ── Section 3: LABEL DISTANCE ─────────────────────────────────────────────────

let pClose = point at (-3, -2) as "close"
let pFar   = point at ( 3, -2) as "far"

set color darkslateblue
set pixel size 10
set label position above

set label distance close
plot pClose

set label distance far
plot pFar


# ── Section 4: SEGMENT ENDPOINT LABELS ───────────────────────────────────────

let sGH = segment from (-7, -4) to (-1, -4) as "GH"
let sIJ = segment from ( 1, -4) to ( 7, -5) as "IJ"
let sKL = segment from (-5, -7) to ( 5, -7) as "KL"

set label distance close

set color slategray
set pixel size 2
draw segment sGH                        # geometry-driven placement

set label first above second below
set color seagreen
draw segment sIJ                        # A above, B below

set label first before second after
set color chocolate
draw segment sKL                        # flanking the endpoints


# ── Section 5: DUPLICATE AUTO-RENAME ─────────────────────────────────────────
# Three points all named "P" → drawn as P, P1, P2

let pDup1 = point at (7,  5) as "P"
let pDup2 = point at (9,  3) as "P"
let pDup3 = point at (7,  1) as "P"

set color mediumvioletred
set pixel size 10
set label position angle 45
set label distance close
plot pDup1
plot pDup2
plot pDup3


# ── Section 6: LABELS ON / OFF ────────────────────────────────────────────────

let pVis    = point at (7, -2) as "visible"
let pHidden = point at (9, -3) as "hidden"

set color teal
set pixel size 10
set label position after
set label distance close

set labels on
plot pVis

set labels off
plot pHidden

set labels on
```
