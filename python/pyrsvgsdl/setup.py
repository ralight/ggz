# Python wrapper module for a GGZ library for use with distutils
#
# Compilation: python setup.py build
# Installation: python setup.py install [--prefix=<prefix>]

from distutils.core import setup, Extension

setup(
    name = "RSVGSDL",
    version = "0.1",
    author = "Josef Spillner",
    author_email = "josef@ggzgamingzone.org",
    url = "http://www.ggzgamingzone.org",

    ext_modules = [
        Extension('rsvgsdl',
        ['src/pyrsvgsdl.c'],
		include_dirs = [
			'/usr/include/librsvg-2',
			'/usr/include/gtk-2.0',
			'/usr/include/glib-2.0',
			'/usr/lib/glib-2.0/include'
		],
        libraries = ['rsvg-2'])
    ]
)

