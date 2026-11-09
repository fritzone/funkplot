#!/usr/bin/python
#
# Regenerates the help pages in html/ from the Markdown sources in mds/.
#
# Usage: gen_html.py <md2html>
#
#   md2html - the md2html binary of md4c (help/md4c), for example built with:
#             cmake -S help/md4c -B /tmp/md4c && cmake --build /tmp/md4c
#
# What it does:
#   1. builds mds/amalgamated.md (the tutorial) from the individual pages
#   2. converts every page listed in funkplot.qhp that has a Markdown source, and the tutorial,
#      wrapping them into layout.html, which carries the style sheet of the Typora exports
#   3. copies the tutorial to the web site (site/amalgamated.html)
#
# The images are embedded into the pages, because the help collection cannot reference files outside of it.

import base64
import html
import os
import re
import shutil
import subprocess
import sys

HELP_DIR = os.path.dirname(os.path.abspath(__file__))
MDS_DIR = os.path.join(HELP_DIR, "mds")
HTML_DIR = os.path.join(HELP_DIR, "html")
SITE_DIR = os.path.join(HELP_DIR, "..", "site")

# The pages of the tutorial, in order. The keyword pages are one heading level deeper.
AMALGAMATED = [
    ("intro", False),
    ("first_steps", False),
    ("program", False),
    ("keywords", False),
    ("kw_append", True),
    ("kw_if", True),
    ("kw_for", True),
    ("kw_function", True),
    ("kw_let", True),
    ("kw_plot", True),
    ("kw_rotate", True),
    ("kw_script", True),
    ("kw_set", True),
    ("kw_var", True),
    ("geometry", False),
    ("label_placement", False),
    ("error_messages", False),
]


def read(path):
    with open(path, encoding="utf-8") as f:
        return f.read()


def write(path, content):
    with open(path, "w", encoding="utf-8") as f:
        f.write(content)


def demote_headings(md):
    # only outside of code blocks, where # starts a comment
    result, in_code = [], False
    for line in md.split("\n"):
        if line.startswith("```"):
            in_code = not in_code
        elif not in_code and line.startswith("#"):
            line = "#" + line
        result.append(line)
    return "\n".join(result)


def build_amalgamated():
    parts = ["[TOC]"]
    for page, demote in AMALGAMATED:
        md = read(os.path.join(MDS_DIR, page + ".md")).strip("\n")
        parts.append(demote_headings(md) if demote else md)
    write(os.path.join(MDS_DIR, "amalgamated.md"), "\n\n".join(parts) + "\n")


def pages_in_help():
    qhp = read(os.path.join(HELP_DIR, "funkplot.qhp"))
    pages = re.findall(r"<file>html/(.+?)\.html</file>", qhp)
    return [p for p in pages if os.path.exists(os.path.join(MDS_DIR, p + ".md"))]


def slug(text, used):
    # the same kind of ids Typora generates, such as "the-let-keyword"
    s = re.sub(r"<[^>]+>", "", html.unescape(text)).strip().lower()
    s = re.sub(r"[^\w]+", "-", s).strip("-") or "section"
    candidate, n = s, 0
    while candidate in used:
        n += 1
        candidate = f"{s}-{n}"
    used.add(candidate)
    return candidate


def embed_image(match):
    tag, quote, src = match.group(0), match.group(1), match.group(2)
    path = os.path.join(MDS_DIR, src)
    if src.startswith("data:") or not os.path.exists(path):
        if not src.startswith("data:"):
            print(f"  warning: image not found: {src}")
        return tag
    with open(path, "rb") as f:
        b64 = base64.b64encode(f.read()).decode("ascii")
    return tag.replace(f"src={quote}{src}{quote}", f'src="data:image/png;base64,{b64}"')


def typorize(body):
    # heading ids, collected for the table of contents
    used, toc = set(), []

    def heading(m):
        level, text = m.group(1), m.group(2)
        hid = slug(text, used)
        toc.append((int(level), hid, re.sub(r"<[^>]+>", "", text)))
        return f"<h{level} id='{hid}'>{text}</h{level}>"

    body = re.sub(r"<h([1-6])>(.*?)</h\1>", heading, body, flags=re.S)

    if "<p>[TOC]</p>" in body:
        items = "".join(
            f'<span role="listitem" class="md-toc-item md-toc-h{level}" data-ref="n{i}">'
            f'<a class="md-toc-inner" href="#{hid}">{text}</a></span>'
            for i, (level, hid, text) in enumerate(toc) if level <= 3)
        body = body.replace("<p>[TOC]</p>",
                            f'<div class=\'md-toc\' mdtype=\'toc\'><p class="md-toc-content" role="list">{items}</p></div>')

    # code blocks look like the fences of Typora
    body = re.sub(r"<pre><code[^>]*>(.*?)</code></pre>",
                  r'<pre class="md-fences md-end-block" spellcheck="false" lang="">\1</pre>', body, flags=re.S)

    body = body.replace("<table>", "<figure class='table-figure'><table>").replace("</table>", "</table></figure>")

    return re.sub(r"<img\b[^>]*\bsrc=([\"'])(.*?)\1[^>]*>", embed_image, body)


def convert(md2html, layout, page):
    md_path = os.path.join(MDS_DIR, page + ".md")
    result = subprocess.run([md2html, "--github", md_path], capture_output=True, text=True, encoding="utf-8")
    if result.returncode != 0:
        sys.exit(f"Error converting {md_path}: {result.stderr}")
    content = typorize(result.stdout)
    write(os.path.join(HTML_DIR, page + ".html"),
          layout.replace("{{title}}", page).replace("{{content}}", content))


def main():
    if len(sys.argv) < 2:
        sys.exit("Usage: gen_html.py <md2html>")
    md2html = sys.argv[1]
    layout = read(os.path.join(HELP_DIR, "layout.html"))

    build_amalgamated()

    for page in pages_in_help() + ["amalgamated"]:
        print(f"Converting {page}.md")
        convert(md2html, layout, page)

    if os.path.isdir(SITE_DIR):
        shutil.copy(os.path.join(HTML_DIR, "amalgamated.html"), os.path.join(SITE_DIR, "amalgamated.html"))
        print("Copied amalgamated.html to the site")


if __name__ == "__main__":
    main()
