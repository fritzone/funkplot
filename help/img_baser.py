import os
import os.path

files = os.popen('cat geodraw.qhp | grep "<file>" | cut -d \'>\' -f 2 | cut -d \'<\' -f 1').read().split('\n')

patched_files = {}

for fn in files:
    if len(fn) and os.path.exists("o-" + fn):
        with open("o-" + fn) as file:
            lines = file.readlines()
            lines = [line.rstrip() for line in lines]
            file_patch = ""
            needs_patching = False
            imgsrc_pos = 0
            for line in lines:
                imgsrc_pos = line.find("img src")
                if imgsrc_pos != -1:

                    needs_patching = True

                    before = line[:imgsrc_pos]
                    file_patch += before + "img src=\"data:image/png;base64, "

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
                        command ="base64 -w 0 " + img_name
                        b64d = os.popen(command).read()
                        file_patch += b64d
                        imgsrc_pos = line.find("img src", s_idx)


                        if imgsrc_pos == -1:
                            # ending the line, no more images after this, contains the closing " for the img src 
                            file_patch += line[s_idx:]
                        else:
                            file_patch += '"' # closing quote
                            # pull in everything from s_idx to imgsrc_pos
                            file_patch += line[s_idx : imgsrc_pos] + "img src=\"data:image/png;base64, "

                        
                    
                else:
                    file_patch += line + "\n"

            if needs_patching:
                patched_files[fn] = file_patch

                    
for f, c in patched_files.items():
    print (f)
    with open(f, "w") as myfile:
        myfile.write(c)