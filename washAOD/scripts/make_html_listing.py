#!/bin/env python3
import sys
from pathlib import Path

if __name__ == '__main__':
    if len(sys.argv) != 2 or 'help' in sys.argv[1]:
        print(f'Usage: {sys.argv[0]} path_to_plots')
        sys.exit()
    plot_dir = Path(sys.argv[1])
    print(f'Processing directory {plot_dir}')
    #html_dir = Path(f'/publicweb/a/as2872/iDMPlots/{plot_dir}')
    #if (not html_dir.exists()):
    #    html_dir.mkdir()
    # Check if given dir has subdirs with plots in it or if it is the subdir itself
    subdirs = [x for x in plot_dir.iterdir() if x.is_dir()]
    if len(subdirs) == 0:
        subdirs = [plot_dir]

    for subdir in subdirs:
        #html_file = Path(html_dir/'index.html')
        html_file = Path(subdir/'index.html')
        with html_file.open('wt') as index:
            index.write('<html><head></head><body><pre>')
            index.write('<a href="..">.. (parent directory)</a><br>')
            #for i, f in enumerate(sorted(html_dir.glob('*'))):
            for i, f in enumerate(sorted(subdir.glob('*'))):
                if f.name != 'index.html':
                    print(i, f.name)
                    index.write(f'<a href="#{f.name}">{i}</a>')
                    index.write(f'   ')
                    index.write(f'<a href="{f.name}">{f.name}</a><br>')

            #for i, f in enumerate(sorted(html_dir.glob('*'))):
            for i, f in enumerate(sorted(subdir.glob('*'))):
                if f.name != 'index.html':
                    if f.suffix == '.png':
                        index.write(f'<h4 id="{f.name}"><a href="#{f.name}">{f.name}</a></h4><br>')
                        index.write(f'<a href="{f.name}"><img src="{f.name}" style="max-width: 600px"></a><br><br>\n')
                    #else:
                    #    index.write(f'<a href="{f.name}">{f.name}</a><br><br>\n')
            index.write('</pre></body></html>\n')

