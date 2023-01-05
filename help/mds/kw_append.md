## The **append** keyword

You can append values to lists using the `append` keyword. It’s generic format is:

```
append to <list_variable> <numbers|points> <number,number,...|(point1x,point1y),(point2x,point2y),...>
```

And the following are two short examples on how to use it:

```
append to list_variable numbers 1,2,3,4
append to list_variable points (1,2),(3,4)
```

### Example for numbers

The following program

```
var l list of numbers

let l = list[1,2,3,4]
python do print(l)

append to l numbers 5,6,7
python do print(l)
```

will generate the following output

```
[1.0, 2.0, 3.0, 4.0]
[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0]
```

### Example for points

The following program

```
var l list of points
var i number
let l = list[]

for i = 0 to 5 step 0.5 do
  append to l points (i, i)
done

set pixel size 6
plot l
```

Will generate the following plot:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/append_1.png" alt="image-20230105112755505" style="zoom: 33%;" />