import json
import os
import subprocess

tex1 = r'''\documentclass{article}
\pagenumbering{gobble}
\usepackage{amsmath}
\begin{document}
    \[f(t)=\begin{cases}
'''
tex2 = r'''
    \end{cases}
    t\in ('''
tex3='''\]
\end{document}
'''

qrcG = '''<RCC>
    <qresource prefix="/builtin">
'''

texp1 = r'''\documentclass{article}
\pagenumbering{gobble}
\usepackage{amsmath}
\begin{document}
    \[r='''
texp2 = '''\]
\end{document}
'''


def create_data(name):
    qrc = qrcG
    with open(name, 'r') as f:
        v = json.load(f)
        for c in v["curves"]:
            if c["key"]:
                print("Generating:", c["key"])
                qrc += "<file>builtins/" + c["key"] + ".png" + "</file>"
                finaltex = ''
                start = c["interval"]["start"]
                end = c["interval"]["end"]
                start = start.replace("pi", "\pi")
                end = end.replace("pi", "\pi")
                size = "x64"
                if c["equation"]["type"] == "parametric":
                    formx = c["equation"]["formula"]["x"]
                    formx = formx.replace("*", " \\cdot ")
                    formy = c["equation"]["formula"]["y"]
                    formy = formy.replace("*", " \\cdot ")
                    finaltex = tex1 + "\n" + formx + "\\\\\n" + formy + tex2 + start + ',' + end + ")" + tex3
                else:
                    form = c["equation"]["formula"]["r"]
                    form = form.replace("*", " \\cdot ")
                    size = "x32"
                    finaltex = texp1 + "\n" + form + texp2 + start + ',' + end + ")" + tex3

                with open("" + c["key"]+".tex", "w") as f2:
                    f2.write(finaltex)
                subprocess.run(["latex2png", c["key"]+".tex"])
                png_file = c["key"]+".png"
                subprocess.run( ["convert", png_file, "-transparent", "white", "-geometry", size, png_file] )

                os.rename(png_file, "../gui/builtins/" + png_file)
                os.remove(c["key"]+".tex")
        qrc += "</qresource>\n</RCC>"
        with open("../gui/builtins.qrc", "w") as fq:
            fq.write(qrc)


if __name__ == '__main__':
    create_data('../gui/2dcurves.json')
