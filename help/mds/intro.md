**geodraw** is a niche piece of mathematical modeling and plotting software, which is based on an interpreted descriptive language.

Using this programming language you can instruct **geodraw** to generate 2-dimensional plots or 3-dimensional surfaces. 

The process of creating plot is very straightforward, the simplest one would take up just the following 2 lines:

```matlab
function f(x) = sin(x)
plot f over (-5, 5)
```

And this will generate the following plot:

![image-20220425144408774](/home/fld/work/p/geodraw/help/mds/imgs/main.png)