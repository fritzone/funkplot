## The **script** keyword

It is possible to embed python scripts in **fũnkplot** code. The following syntax is used to achieve this:

```
python script begin
#
# proper python code
#
end
```

The python script will have access to the variables defined in the **fũnkplot** code in the following manner:

- Variables with the type `number` will be treated as python floating point types.
- For variables with type `point` a new type is created, called `Point` and it is introduced in the python propgram, having a simple structure with `x` and `y` members:

```python
class Point(object):
    def __init__(self, x, y):
        self.x = x
        self.y = y
```

- All the variables that are in the **fũnkplot** program are added as an extra codeline before the script is copied over.

### Example

```
var i number
var p point
var ps list of points
var ns list of numbers

function f(x) = sin(x)

let i = 0
let p = point at (1,2)
let ps = points of f counts 6
let ns = list [1,2,3,5,8,11]

python script begin

print("i=", i)
print("p=(", p.x, ",", p.y, ")")
l = 0
while l < len(ps):
  print("ps[",l,"]=(", ps[l].x, ",", ps[l].y, ")")
  l += 1

for l in range(len(ns)):
  print(ns[l])

end
```

Will generate the following python script:

```python
class Point(object):
    def __init__(self, x, y):
        self.x = x
        self.y = y
ps=[Point(-1.000000, -0.841471),Point(-0.600000, -0.564642),Point(-0.200000, -0.198669),Point(0.200000, 0.198669),Point(0.600000, 0.564642),Point(1.000000, 0.841471)]
p=Point(1.000000, 2.000000)
ns=[1.000000, 2.000000, 3.000000, 5.000000, 8.000000, 11.000000]
i=0.000000

print("i=", i)
print("p=(", p.x, ",", p.y, ")")
l = 0
while l < len(ps):
  print("ps[",l,"]=(", ps[l].x, ",", ps[l].y, ")")
  l += 1

for l in range(len(ns)):
  print(ns[l])
```

And the following output when executed:

```
i= 0.0
p=( 1.0 , 2.0 )
ps[ 0 ]=( -1.0 , -0.841471 )
ps[ 1 ]=( -0.6 , -0.564642 )
ps[ 2 ]=( -0.2 , -0.198669 )
ps[ 3 ]=( 0.2 , 0.198669 )
ps[ 4 ]=( 0.6 , 0.564642 )
ps[ 5 ]=( 1.0 , 0.841471 )
1.0
2.0
3.0
5.0
8.0
11.0

```



