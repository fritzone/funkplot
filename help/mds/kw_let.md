## First steps

After you have started **geodraw** it will greet you with the following screen

![image-20220502131328491](/home/fld/work/p/geodraw/help/mds/imgs/first_gui.png)

On the left of the GUI of the application you can find a text editing area where you can type in your program, and on the right you see the plotting area, which is a dockable window, ie. you can detach it from the main screen of the application.

You can zoom in the plotting area, and using your mouse you can change the position of the coordinate system.

On the main ribbon of the application there are buttons for basic operations, such as creating a new file, opening an existing one, saving the current one, running the current program and exporting the result of the plot under various formats.

### The **geodraw** programming language

**geodraw** uses its own programming language influenced by basic in its simplicity, with some extensions to allow more complex mathematical notions to be easily handled.

The process of creating plot is very straightforward, write a small program and run it. The most simplest one would take up just the following 2 lines:

```matlab
function f(x) = sin(x)
plot f over (-5, 5)
```

And this will generate the following plot:

![image-20220425144408774](/home/fld/work/p/geodraw/help/mds/imgs/main.png)

**geodraw** supports a range of builtin mathematical functions, most of the default operators and has syntax for changing the color, line and rotation of the current plot.