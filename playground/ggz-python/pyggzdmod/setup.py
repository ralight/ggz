#this will compile the ggzd modules. as far as i
#can tell. the cool thing, this will compile it on
#almost any platform, but it does require a few
#libraries to be available, and who knows if
#those will be available to everyone
#
#run with "python setup.py install" to compile
#and install.


from distutils.core import setup, Extension


libs = ['ggzdmod', 'ggz']


setup(
        name = "GGZDMod",
        version = "0.1",
        author = "Josef",
        author_email = "dr_maux@users.sourceforge.net",
        url = "http://mindx.sourceforge.net",

        ext_modules = [
            Extension('ggzdmod', ['src/pyggzdmod_init.c'], libraries=libs)
        ]
)

