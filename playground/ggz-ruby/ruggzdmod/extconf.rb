require 'mkmf'

$CFLAGS = '-I /home/ggznew/BUILD/include'
$LDFLAGS = ' -L /home/ggznew/BUILD/lib'

have_h = have_header('ggzdmod.h')
have_l = have_library('ggzdmod', 'ggzdmod_new')

if have_h and have_l then
	create_makefile('GGZDMod')
else
	puts "Please install the ggzdmod library."
end

