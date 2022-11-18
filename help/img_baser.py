#!/usr/bin/python

# arg[0] - python script name
# arg[1] - md2html location, full path
# arg[2] - where the mds directory is
# arg[2] - where the work happens

import os
import os.path
import sys

to_remove = "<link href='https://fonts.loli.net/css?family=Open+Sans:400italic,700italic,700,400&subset=latin,latin-ext' rel='stylesheet' type='text/css' />"
keywords = ['rotate', 'function', 'plot', 'over', 'continuous', 'let', 'of', 'step', 'counts', 'set', 'in', 'do', 'done', 'range', 'else', 'segments', 'points', 'for', 'if', 'to', 'into', 'around', 'with', 'var', 'begin', 'end', 'run', 'parametric', 'append', 'script']

def colorize(kw):
    if kw in keywords:
        return "<span style='font-weight: bold; color:#021691;'>" + kw + "</span>"
    return kw

files = os.popen('cat funkplot.qhp | grep "<file>" | cut -d \'>\' -f 2 | cut -d \'<\' -f 1').read().split('\n')

patched_files = {}

print(files)

for fn in files:

    print("Checking:", fn)

    if len(fn) > 1 and os.path.exists(fn):
        print("Patching: [", fn, "]")
        with open( fn) as file:
            lines = file.readlines()
            lines = [line.rstrip() for line in lines]
            file_patch = ""
            needs_patching = False
            imgsrc_pos = 0
            for line in lines:

                # remove the offending font loader
                if line.find(to_remove):
                    line.replace(to_remove, "")

                imgsrc_pos = line.find("img src")
                if imgsrc_pos != -1:
                    needs_patching = True
                    before = line[:imgsrc_pos]
                    file_patch += before

                    # get all img srcs from the file
                    while imgsrc_pos != -1:
                        
                        s_idx = imgsrc_pos
                        while s_idx < len(line) and line[s_idx] != '"':
                            s_idx = s_idx + 1
                        s_idx = s_idx + 1
                        img_name = ""
                        while s_idx < len(line) and line[s_idx] != '"':
                            img_name = img_name + line[s_idx]
                            s_idx = s_idx + 1
                        command = "/usr/bin/base64 -w 0 " + img_name

                        if img_name.startswith("/"):
                            print(" .. Trying:", command)
                            b64d = os.popen(command).read()
                            file_patch += "img src=\"data:image/png;base64, " + b64d
                            imgsrc_pos = line.find("img src", s_idx)
                        else:
                            imgsrc_pos = line.find("img src", s_idx)
                            file_patch += "img src=\"" + img_name

                        if imgsrc_pos == -1:
                            # ending the line, no more images after this, contains the closing " for the img src
                            file_patch += line[s_idx:]
                        else:
                            # file_patch += '"'  # closing quote
                            # pull in everything from s_idx to imgsrc_pos
                            file_patch += line[s_idx:imgsrc_pos]

                else:
                    file_patch += line + "\n"

            if needs_patching:
                patched_files[fn] = file_patch
    

                    
for f, c in patched_files.items():
    print ("Writing:", f)
    with open(f, "w") as myfile:
        myfile.write(c)
