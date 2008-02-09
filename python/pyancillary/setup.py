# Python wrapper module for ancillary messages
# (CMSG over sendmsg()/recvmsg())
#
# Compilation: python setup.py build
# Installation: python setup.py install [--prefix=<prefix>]

from distutils.core import setup, Extension

setup(
    name = "Ancillary",
    version = "0.1",
    author = "Josef Spillner",
    author_email = "josef@ggzgamingzone.org",
    url = "http://www.ggzgamingzone.org",

    ext_modules = [
        Extension('ancillary',
        ['src/pyancillary_init.c'],
        library_dirs = ["/home/josef/sys/lib"],
        libraries = ['ggz'],
        include_dirs = ["/home/josef/sys/include"])
    ]
)

