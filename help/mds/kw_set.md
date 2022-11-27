## The **set** keyword

The `set` keyword is used to set variables altering the behaviour of the rendered scene. It is recommended to place the scene altering code sequences before any of the other code sequences.

The following values can be set:

| Setting        | Description                                                  |
| -------------- | ------------------------------------------------------------ |
| colour / color | Sets the colour of the next pixel to be drawn. Both UK and US spellings are allowed. |
| palette        | Sets the palette to be used for drawing the scene.           |
| coordinates    | Sets the properties of the coordinate system.                |
| pixel          | Sets the properties of the next pixel to be drawn.           |

### Setting the colour

Setting the colour can be done using one of the following available syntaxes:

#### By name

Using the syntax `set colour name` sets the colour based on the name of it, using 255 as alpha value. A list of all colour names is available in the Miscellaneous section of this help system.

#### By name and alpha

Using the syntax `set colour name,alpha` sets the colour based on the name of it, using the specified alpha value.

#### From a palette

Using the syntax `set colour palette[index]` will make possible to set the colour with the given index from the palette that was selected with the `set palette …` statement, and will use the alpha channel set to 255. If there is no such palette found this statement has no effect and the colour black will be used.

#### From a palette with alpha value

Using the syntax `set colour palette[index],alpha` will make possible to set the colour with the given index from the palette that was selected with the `set palette …` statement and will calculate the alpha from the specified value. If there is no such palette this statement has no effect and the colour black will be used.

#### By RGB code

Using the syntax `set colour #<value>` will allow you to directly set the red, green and blue values of the colour using the well know HTML syntax. The following combinations are accepted:

- three digit colour code **RGB** : `set colour #123`.  Will set the specified colour with the alpha channel set to 255.
- six digit colour code **RRGGBB**: `set colour #112233`. Will set the specified colour with the alpha channel set to 255.
- eight digit colour code **RRGGBBAA**: `set colour #11223344`. Will set the specified colour with the specified alpha value.

### Setting the palette

The syntax `set palette name` sets the palette that can be accessed in a `set color palette[index]` statement. A list of available palettes can be found in the Miscellaneous section of this help system.

### Setting the properties of the coordinate system

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

### Setting the pixel properties

For the moment it is possible to set the size of a pixel with the command `set pixel size X` where X denotes an expression that will be evaluated.

### Example

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
