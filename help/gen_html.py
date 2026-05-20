import os
import subprocess
import sys

def generate_html(md_dir, html_dir, md2html_bin, layout_file):
    if not os.path.exists(html_dir):
        os.makedirs(html_dir)

    with open(layout_file, 'r') as f:
        layout = f.read()

    for filename in os.listdir(md_dir):
        if filename.endswith(".md"):
            name = os.path.splitext(filename)[0]
            md_path = os.path.join(md_dir, filename)
            html_path = os.path.join(html_dir, name + ".html")

            print(f"Converting {filename} to {name}.html")
            
            # Use md2html to get the HTML body
            result = subprocess.run([md2html_bin, md_path], capture_output=True, text=True)
            if result.returncode != 0:
                print(f"Error converting {filename}: {result.stderr}")
                continue

            content = result.stdout
            
            # Wrap in layout
            page = layout.replace("{{title}}", name.replace("_", " ").capitalize())
            page = page.replace("{{content}}", content)

            with open(html_path, 'w') as f:
                f.write(page)

if __name__ == "__main__":
    if len(sys.argv) < 5:
        print("Usage: gen_html.py <md_dir> <html_dir> <md2html_bin> <layout_file>")
        sys.exit(1)
    
    generate_html(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
