require 'mkmf'

ggzbase = `ggz-config -c 2>/dev/null`.chomp
$CFLAGS = "-I #{ggzbase}/../include -Wall -Werror"
$LDFLAGS = "-L #{ggzbase}/../lib"

have_h = have_header('ggzmod.h')
have_l = have_library('ggzmod', 'ggzmod_new')

if have_h and have_l then
	create_makefile('GGZMod')
else
	puts "Please install the ggzmod library."
end

