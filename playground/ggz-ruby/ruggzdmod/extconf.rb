require 'mkmf'

ggzbase = `ggz-config -c 2>/dev/null`.chomp
$CFLAGS = "-I #{ggzbase}/../include"
$LDFLAGS = "-L #{ggzbase}/../lib"

have_h = have_header('ggzdmod.h')
have_l = have_library('ggzdmod', 'ggzdmod_new')

if have_h and have_l then
	create_makefile('GGZDMod')
else
	puts "Please install the ggzdmod library."
end

