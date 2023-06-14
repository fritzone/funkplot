import json
import os
import subprocess
from subprocess import Popen
import sys, getopt

tex_parametric_1 = r'''\documentclass{article}
\pagenumbering{gobble}
\usepackage{amsmath}
\begin{document}
    \[f(t)=\begin{cases}
'''
tex_parametric_2 = r'''
    \end{cases}
    t\in ('''

qrcG = '''<RCC>
    <qresource prefix="/builtin">
'''

tex_polar_1 = r'''\documentclass{article}
\pagenumbering{gobble}
\usepackage{amsmath}
\begin{document}
    \[r='''
tex_end = '''\]
\end{document}
'''

tex_cartesian_1 = r'''\documentclass{article}
\pagenumbering{gobble}
\usepackage{amsmath}
\begin{document}
    \[y='''

script = '''
set coordinates start x -6
set coordinates start y -6
set coordinates end x 6
set coordinates end y 6
set coordinates zoom 50
set pixel size 2
'''

# The size of the generates image by default
genwidth = 640
genheight = 300

#
# Latexizes the given string
#
def latexize(form):
    form = form.replace("*", " \\cdot ")
    form = form.replace("3.14", "\\pi")
    return form

#
# Unlatexizes the given text
#
def unlatexize(s):
    s = s.replace("\\pi", "3.14")
    s = s.replace("\\2pi", "6.28")
    s = s.replace("\\cdot", "*")
    s = s.replace("2pi", "6.28")
    s = s.replace("pi", "3.14")

    return s

#
# Renders the function, with the script code and template script
#
def render_function(fun_name, lscript, tscript):

    dir_path = os.getcwd()


    with open(dir_path + "/" + fun_name + ".fnk", "w") as fq:
        fq.write(lscript)
    with open(dir_path + "/" + fun_name + ".t.fnk", "w") as fq:
        fq.write(tscript)


    comp = [dir_path + "/compiler/funkplot-compiler", 
                "--width", str(genwidth), 
                "--height", str(genheight), 
                dir_path + "/" + fun_name + ".fnk", 
                dir_path + "/" + fun_name + ".g.png"]

    print(" ".join(comp))

    subprocess.run(comp)

#
# Create a function with cartesian/polar coordinates
#
def create_coord_script(fun_name, form, vs, st, en, polar):

    vars = []
    lscript = script
    tscript = script

    for v in vs:
        vars.append(str(v["name"]).strip())

    if len(vars) > 0 :
        lscript += "var " + " ".join(vars) + " number\n"
        tscript += "var " + " ".join(vars) + " number\n"

    for v in vs:
        lscript += "let " + v["name"] + " = " + v["value"] + "\n"
        tscript += "let " + v["name"] + " = {{" + v["name"] + "}}\n"

    if polar:
        lscript += "function f(t) = " + form
        tscript += "function f(t) = " + form
    else:
        lscript += "function f(x) = " + form
        tscript += "function f(x) = " + form

    lscript += "\n"
    tscript += "\n"

    if polar:
        lscript += "polar "
        tscript += "polar "

    lscript += "plot f over (" + unlatexize(st) + ", " + unlatexize(en) + ")\n"
    tscript += "plot f over (" + unlatexize(st) + ", " + unlatexize(en) + ")\n"

    render_function(fun_name, lscript, tscript)
    return

#
# Creates a parametric script
#
def create_parametric_script(fun_name, formx, formy, vs, st, en):

    vars = []
    lscript = script
    tscript = script

    for v in vs:
        vars.append(str(v["name"]).strip())

    if len(vars) > 0 :
        lscript += "var " + " ".join(vars) + " number\n"
        tscript += "var " + " ".join(vars) + " number\n"

    for v in vs:
        lscript += "let " + v["name"] + " = " + v["value"] + "\n"
        tscript += "let " + v["name"] + " = {{" + v["name"] + "}}\n"

    lscript += "parametric function f(t)\n"
    lscript += "  x=" + formx + "\n"
    lscript += "  y=" + formy + "\n"
    lscript += "end\n"

    lscript += "plot f over (" + unlatexize(st) + ", " + unlatexize(en) + ")\n"

    # template
    tscript += "parametric function f(t)\n"
    tscript += "  x=" + formx + "\n"
    tscript += "  y=" + formy + "\n"
    tscript += "end\n"
    tscript += "plot f over (" + unlatexize(st) + ", " + unlatexize(en) + ")\n"

    render_function(fun_name, lscript, tscript)
    return

#
# Creates the QRC data, the image files, the script files, for the specified curve, or for all if none
#
def create_data(name, curve):
    qrc = qrcG
    if len(curve) > 0:
        print("Requested:", curve)

    with open(name, 'r') as f:
        v = json.load(f)
        for c in v["curves"]:
            if len(curve) > 0 and c["key"] == curve or len(curve) == 0 and c["key"]:
                print("Generating:", c["key"])

                # parameters
                parameters = []
                if "parameter" in c:
                    for p in c["parameter"]:
                        parameters.append({"name": p["name"], "value": p["default"]})
                qrc += "<file>" + c["key"] + ".png" + "</file>\n"
                qrc += "<file>" + c["key"] + ".t.fnk" + "</file>\n"
                qrc += "<file>" + c["key"] + ".g.png" + "</file>\n"
                finaltex = ''
                start = c["interval"]["start"]
                end = c["interval"]["end"]
                start = start.replace("pi", "\\pi")
                end = end.replace("pi", "\\pi")
                size = "x64"

                if c["equation"]["type"] == "parametric":
                    formx = latexize(c["equation"]["formula"]["x"])
                    formy = latexize(c["equation"]["formula"]["y"])
                    create_parametric_script(c["key"], c["equation"]["formula"]["x"], c["equation"]["formula"]["y"], parameters, start, end)
                    finaltex = tex_parametric_1 + "\n" + formx + "\\\\\n" + formy + tex_parametric_2 + start + ',' + end + ")" + tex_end
                elif c["equation"]["type"] == "polar":
                    form = latexize(c["equation"]["formula"]["r"])
                    size = "x32"
                    finaltex = tex_polar_1 + "\n" + form + tex_end + start + ',' + end + ")" + tex_end
                    create_coord_script(c["key"], c["equation"]["formula"]["r"], parameters, start, end, True)
                elif c["equation"]["type"] == "cartesian":
                    form = latexize(c["equation"]["formula"]["y"])
                    size = "x32"
                    finaltex = tex_cartesian_1 + "\n" + form + tex_end + start + ',' + end + ")" + tex_end
                    create_coord_script(c["key"], c["equation"]["formula"]["y"], parameters, start, end, False)

                with open("" + c["key"]+".tex", "w") as f2:
                    f2.write(finaltex)
                
                proc = Popen(["latex2png", "-c", c["key"]+".tex"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                proc.wait()

                png_file = c["key"]+".png"
                subprocess.run(["convert", png_file, "-transparent", "white", "-geometry", size, png_file] )
                os.remove(c["key"]+".tex")
        qrc += "</qresource>\n</RCC>"
        with open("builtins.qrc", "w") as fq:
            fq.write(qrc)


# Main function
def main(argv):
    opts, args = getopt.getopt(argv,"hn:",["n="])
    cname = ""

    for opt, arg in opts:
        if opt == '-h':
            print ('builtins.py -n <curve>')
            sys.exit()
        elif opt in ("-n", "--n"):
            cname = arg

    create_data('../gui/2dcurves.json', cname)


if __name__ == '__main__':
    main(sys.argv[1:])