# Common debian rules
# imports: RULES_CONFIGURE (and optionally RULES_MAKE)
# sets for shell: CFLAGS, CXXFLAGS

prefix	:= $(PWD)/debian/tmp
share	:= /usr/share

### enable cross-compilation

DEB_HOST_GNU_TYPE  ?= $(shell dpkg-architecture -qDEB_HOST_GNU_TYPE)
DEB_BUILD_GNU_TYPE ?= $(shell dpkg-architecture -qDEB_BUILD_GNU_TYPE)

config_flags	:= --prefix=/usr \
	--sysconfdir=/etc \
	--mandir=$(share)/man \
	--build $(DEB_HOST_GNU_TYPE)
ifneq ($(DEB_BUILD_GNU_TYPE), $(DEB_HOST_GNU_TYPE))
config_flags	+= --host $(DEB_HOST_GNU_TYPE)
endif

### setting options for C/C++ compilers

cflags	:= -g -Wall
ifeq (,$(findstring noopt,$(DEB_BUILD_OPTIONS)))
cflags	+= -O2
else
cflags	+= -O0
endif

export CFLAGS=$(cflags)
export CXXFLAGS=$(cflags)

### configure the package
### the RULES_CONFIGURE come from debian/rules

config:	config-stamp
config-stamp:
	dh_testdir
	$(SHELL) ./configure $(config_flags) \
		$(RULES_CONFIGURE)
	touch $@

### build the package and clean the build

build:	config build-stamp
build-stamp:
	$(MAKE)
	touch $@

clean:	checkroot
	dh_clean
	rm -f *-stamp
	rm -f config.log
	rm -f debian/files
	-$(MAKE) distclean

### misc rules

RULES_MAKE::

binary-indep:	checkroot build

binary-arch::	checkroot build
	dh_clean
	dh_installdirs

	$(MAKE) install DESTDIR=$(prefix)
	$(MAKE) -f debian/rules RULES_MAKE

	dh_install

	dh_installchangelogs ChangeLog
	dh_installdocs

	dh_strip
	dh_compress
	dh_link
	dh_fixperms
	dh_makeshlibs -V
	dh_installdeb
	dh_shlibdeps -l$(prefix)/usr/lib
	dh_gencontrol
	dh_md5sums
	dh_builddeb

binary:	binary-indep binary-arch

checkroot:
	dh_testdir
	dh_testroot

.PHONY: binary binary-indep binary-arch clean checkroot build config
