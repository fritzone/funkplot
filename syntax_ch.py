#!/usr/bin/env python3
import fileinput
from pathlib import Path
from glob import glob

project_files = glob('./*.cpp') + glob('./*.h')
error_number = 0

for path in project_files:
    filename = str(path)
    print(filename)
    with fileinput.FileInput(filename, inplace=True, backup='.bak') as filen:
        for line in filen:
            print(line.replace("syntax_error_exception(", "syntax_error_exception(" + str(error_number) + ", "), end='')
            if line.find("syntax_error_exception") != -1:
                errofile = open("help/mds/error_" + str(error_number) + ".md", "a")

                errofile.write("## Error: " + str(error_number) + "\n" + line)
                errofile.close()
                error_number += 1
