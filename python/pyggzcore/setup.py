#this will compile the ggzd modules. as far as i
#can tell. the cool thing, this will compile it on
#almost any platform, but it does require a few
#libraries to be available, and who knows if
#those will be available to everyone
#
#run with "python setup.py install" to compile
#and install.


from distutils.core import setup, Extension


libs = ['ggzcore', 'ggz']


setup(
        name = "GGZCore",
        version = "0.1",
        author = "Josef Spillner",
        author_email = "josef@ggzgamingzone.org",
        url = "http://www.ggzgamingzone.org",

        ext_modules = [
            Extension('ggzcore', ['src/pyggzcore_init.c'], libraries=libs)
        ],

        include_dirs = "/home/ggz/BUILD/include"
)

