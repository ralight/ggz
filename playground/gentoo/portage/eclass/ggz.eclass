# Copyright 2004 Gentoo Foundation
# Distributed under the terms of the GNU General Public License, v2 or later
# Author Roger Light <oojah@ggzgamingzone.org>
# $Header$

ECLASS=ggz
INHERITED="$INHERITED $ECLASS"

# This eclass contains functions for helping configure GGZ game modules.

fix_ggz_module_makefiles() {
	# This function takes as its arguments a list of the game modules in
	# the current package, so "chess tictactoe dots" for example.
	# It prevents the running of ggz-config from game module
	# Makefiles. Instead, it sets each module.dsc file to be installed to
	# /usr/share/ggz/modules/ The installation/configuration of the module.dsc
	# files must be done with the install_ggz_modules() function.
	# The purpose of the /usr/share/ggz/modules/ directory is to hold the
	# game module information so that in can be removed from 
	# /etc/ggz/ggz.modules when the package is uninstalled.

	local module

	for module in ${*}
	do
		sed -i "s/\$(GGZ_CONFIG).*/\$(INSTALL) -D module.dsc \${DESTDIR}\/usr\/share\/ggz\/modules\/${module}.dsc/" ${WORKDIR}/${P}/${module}/Makefile.in
	done
}

install_ggz_modules() {
	# This function takes as its arguments a list of the game modules in
	# the current package, so "chess tictactoe dots" for example.
	# It will then install each of the module.dsc files into a temporary
	# ggz.modules file and then install the ggz.modules file.

	local module

	# If there are already game modules installed on the system then
	# we need to get the existing definitions into ${T} so that 
	# ggz-config can work on them.
	if [ -f /etc/ggz/ggz.modules ]
	then
		mkdir -p ${T}/etc/ggz
		cp /etc/ggz/ggz.modules ${T}/etc/ggz/
	else
		mkdir -p ${T}/etc/ggz
		touch ${T}/etc/ggz/ggz.modules
	fi

	for module in ${*}
	do
		DESTDIR=${T} /usr/bin/ggz-config -D --install --modfile=${WORKDIR}/${P}/${module}/module.dsc --force
	done

	insinto /etc/ggz
	doins ${T}/etc/ggz/ggz.modules

}

remove_ggz_modules() {
	# This function takes as its arguments a list of the game modules in
	# the current package, so "chess tictactoe dots" for example.
	# It is used when uninstalling a package and removes the appropriate
	# game modules from /etc/ggz/ggz.modules
	# It should be run *before* the package in uninstalled, otherwise the
	# module descriptions in /usr/share/ggz/modules/ will have been removed.
	local module

	if [ -f /etc/ggz/ggz.modules ]
	then
		mkdir -p ${T}/etc/ggz
		cp /etc/ggz/ggz.modules ${T}/etc/ggz/

		for module in ${*}
		do
			DESTDIR=${T} /usr/bin/ggz-config -D --remove --modfile=/usr/share/ggz/modules/${module}.dsc
		done

		insinto /etc/ggz
		# Reinstall the modified configuration file.
		# doins will not work in this case because it installs into
		# ${D} (=/var/...) which is then not merged as we are uninstalling.
		# This code should mimic the behaviour of doins for what we need.
		for cfgnum in $(seq -w 0 9999)
		do
			if [ ! -f /etc/ggz/._cfg${cfgnum}_ggz.modules ]
			then
				install ${T}/etc/ggz/ggz.modules /etc/ggz/._cfg${cfgnum}_ggz.modules
				break
			fi
		done
	fi
}

