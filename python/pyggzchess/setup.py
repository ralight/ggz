# Python wrapper module for a GGZ library for use with distutils
#
# Compilation: python setup.py build
# Installation: python setup.py install [--prefix=<prefix>]

from distutils.core import setup, Extension

setup(
    name = "GGZChess",
    version = "0.1",
    author = "Josef Spillner",
    author_email = "josef@ggzgamingzone.org",
    url = "http://www.ggzgamingzone.org",

    ext_modules = [
        Extension('ggzchess',
        ['src/pyggzchess.c'],
        libraries = ['ggzchess'])
    ]
)

