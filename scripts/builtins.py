import json
import os
import subprocess

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
set coordinates start x -4
set coordinates start y -4
set coordinates end x 4
set coordinates end y 4
set coordinates zoom 110
var '''


def latexize(form):
    form = form.replace("*", " \\cdot ")
    form = form.replace("3.14", "\\pi")
    return form


def unlatexize(s):
    s = s.replace("\\pi", "3.141592")
    return s


def create_coord_script(fun_name, form, vs, st, en, polar):

    lscript = script
    for v in vs:
        lscript += v["name"]

    lscript += " number\n"
    for v in vs:
        lscript += "let " + v["name"] + " = " + v["value"] + "\n"

    if polar:
        lscript += "function f(t) = " + form
    else:
        lscript += "function f(x) = " + form

    if polar:
        lscript += "polar "

    lscript += "plot f over (" + unlatexize(st) + ", " + unlatexize(en) + ")\n"
    dir_path = os.getcwd()

    with open(dir_path + "/" + fun_name + ".fnk", "w") as fq:
        fq.write(lscript)
    subprocess.run([os.getcwd() + "/compiler/funkplot-compiler", "--width", "320", "--height", "200", dir_path + "/" + fun_name + ".fnk", dir_path + "/" + fun_name + ".g.png"])
    return


def create_parametric_script(fun_name, formx, formy, vs, st, en):

    lscript = script
    for v in vs:
        lscript += str(v["name"]).strip()

    lscript += " number\n"
    for v in vs:
        lscript += "let " + v["name"] + " = " + v["value"] + "\n"

    lscript += "parametric function f(t)\n"
    lscript += "  x=" + formx + "\n"
    lscript += "  y=" + formy + "\n"
    lscript += "end\n"

    lscript += "plot f over (" + unlatexize(st) + ", " + unlatexize(en) + ")\n"
    dir_path = os.getcwd()

    with open(dir_path + "/" + fun_name + ".fnk", "w") as fq:
        fq.write(lscript)
    subprocess.run([os.getcwd() + "/compiler/funkplot-compiler", "--width", "320", "--height", "200", dir_path + "/" + fun_name + ".fnk", dir_path + "/" + fun_name + ".g.png"])
    return


def create_data(name):
    qrc = qrcG
    with open(name, 'r') as f:
        v = json.load(f)
        for c in v["curves"]:
            if c["key"]:
                print("Generating:", c["key"])
                # parameters
                parameters = []
                if "parameter" in c:
                    for p in c["parameter"]:
                        parameters.append({"name": p["name"], "value": p["default"]})
                qrc += "<file>" + c["key"] + ".png" + "</file>\n"
                qrc += "<file>" + c["key"] + ".g.png" + "</file>\n"
                finaltex = ''
                start = c["interval"]["start"]
                end = c["interval"]["end"]
                start = start.replace("pi", "\pi")
                end = end.replace("pi", "\pi")
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
                subprocess.run(["latex2png", c["key"]+".tex"])
                png_file = c["key"]+".png"
                subprocess.run(["convert", png_file, "-transparent", "white", "-geometry", size, png_file] )
                os.remove(c["key"]+".tex")
        qrc += "</qresource>\n</RCC>"
        with open("builtins.qrc", "w") as fq:
            fq.write(qrc)


if __name__ == '__main__':
    create_data('../gui/2dcurves.json')
