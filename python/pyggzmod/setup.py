#this will compile the ggzd modules. as far as i
#can tell. the cool thing, this will compile it on
#almost any platform, but it does require a few
#libraries to be available, and who knows if
#those will be available to everyone
#
#run with "python setup.py install" to compile
#and install.


from distutils.core import setup, Extension


libs = ['ggzmod', 'ggz']


setup(
        name = "GGZMod",
        version = "0.1",
        author = "Josef Spillner",
        author_email = "dr_maux@users.sourceforge.net",
        url = "http://ggz.sourceforge.net",

        ext_modules = [
            Extension('ggzmod', ['src/pyggzmod_init.c'], libraries=libs)
        ]
)

