import os
import os.path

files = os.popen('cat funkplot.qhp | grep "<file>" | cut -d \'>\' -f 2 | cut -d \'<\' -f 1').read().split('\n')

print(files)

html = "<!DOCTYPE html>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" \
        "<head>" \
        "</head>" \
        "<body><p class=\"sdlc-headline\">"

for fn in files:
    if len(fn) and not os.path.exists(fn):
        f = open(fn, "w")
        headline = fn.replace(".htm", "").replace("_", " ").capitalize()
        f.write(html + headline + "</p><p class = \"sdlc-bodytext\">TODO:</p></body></html>")
        f.close()

os.system('qhelpgenerator funkplot.qhcp -o funkplot.qhc')
