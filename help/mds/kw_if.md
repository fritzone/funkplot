## The **if** keyword

The `if` keyword is used to execute a block of code depending on the value of an expression. The generic form of `if` is:

```
var i number
let i = 4
if i == 4 do
  python script begin
    print(i)
  end
done
```

This will declare a variable`i`, and assign the value `4` to it, upon execution it will print the value to the output window:

```
4.0
```

The `if` can have an `else` branch too:

```
set pixel size 4
var ps list of points
var i number
function f(x) = x
let ps = points of f over (0, 1) counts 16
for i = 0 to 15 do

  if i % 2 == 0 do
    set color red
  else
    set color blue
  done

  plot ps[i]
done
```

This will generate the following plot:

<img src="/home/fld/work/p/funkplot/help/mds/imgs/if_1.png" alt="image-20221012104843960" style="zoom: 33%;" />
