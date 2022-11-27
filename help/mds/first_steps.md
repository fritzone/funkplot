## First steps

After the installation when you have started **fũnkplot** it will be possible to load a few samples (from the samples directory) or just type in and run your program to see the generated plot in the plotting area.

### The **fũnkplot** programming language

**fũnkplot** uses its own programming language influenced by **basic** in its simplicity, with some extensions to allow more complex mathematical notions to be easily handled, and having a decent enough verbosity to feel like a natural language.

The process of creating a plot is very straightforward, just write a small program and run it. One of the most simple ones would take up just the following 2 lines:

```
function f(x) = sin(x)
plot f over (-5, 5)
```

And this will generate the following plot:

![image-20220425144408774](/home/fld/work/p/funkplot/help/mds/imgs/main.png)

**fũnkplot** supports a range of built-in mathematical functions, most of the default operators and has syntax for changing the color, line and rotation of the current plot.