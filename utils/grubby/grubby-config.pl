#!/usr/bin/perl


## Get keyboard input from user
sub GetInput()
{
  local($key, $word);

  while($key ne "\n")
  {
    $key = getc();
    if ($key ne "\n") {$word = join("",$word,$key);}
  }

  return $word
}



print "\f";
print "Welcome, this script will help you configure you new GRUB.\n";
print "\n\n";

print "What would you like your GRUB's name to be? [Grubby] ";
$grubby_name = GetInput();
if($grubby_name eq "") {$grubby_name = "Grubby";}

print "\n";

print "Who should be $grubby_name owner be? [Grubby] ";
$grubby_owner = GetInput();
if($grubby_owner eq "") {$grubby_owner = "Grubby";}

print "\n";

print "What server should $grubby_name connect to? [localhost] ";
$grubby_server = GetInput();
if($grubby_server eq "") {$grubby_server = "localhost";}

print "\n";

print "What port on $grubby_server should $grubby_name connect to? [5688] ";
$grubby_port = GetInput();
if($grubby_port eq "") {$grubby_port = "5688";}

room:

print "\n";

print "Should $grubby_name autojoin a room? [y] ";
$grubby_autojoin = GetInput();
if($grubby_autojoin eq "") {$grubby_autojoin = "y";}
if ($grubby_autojoin ne "y" && $grubby_autojoin ne "n" &&
    $grubby_autojoin ne "yea" && $grubby_autojoin ne "no") {goto room;}

print "\n";

if($grubby_autojoin eq "y" || $grubby_autojoin eq "yes")
{
  print "What room should $grubby_name autojoin? [0] ";
  $grubby_autoroom = GetInput();
  if($grubby_autoroom eq "") {$grubby_autoroom = "0";}
}

lang:

print "\n";

print "Should $grubby_name check for bad words? [y] ";
$grubby_lang = GetInput();
if($grubby_lang eq "") {$grubby_lang = "y";}
if ($grubby_lang ne "y" && $grubby_lang ne "n" &&
    $grubby_lang ne "yea" && $grubby_lang ne "no") {goto lang;}

print "\n";

print "Name:       $grubby_name\n";
print "Owner:      $grubby_owner\n";
print "Server:     $grubby_server\n";
print "Port:       $grubby_port\n";
print "Autojoin:   $grubby_autojoin\n";
if($grubby_autojoin eq "y" || $grubby_autojoin eq "yes") {print "Room:       $grubby_autoroom\n";}
print "Lang check: $grubby_lang\n";

print "\n\nYou should *not* continue if you arelogged in as root!!\n";
print "If you do you will shutdown *all* grubbies running on this computer,\n";
print "and will end up running grubby as root!\n";
print "\nPress ENTER to continue";

GetInput();

print "\n\nShutting down all grubbies...\n";
@args = ("killall", "grubby");
system(@args);

$pid = getppid();
print "Backing-up current memmory file...\n";
@args = ("mv", "$ENV{HOME}/.ggz/grubby.rc", "$ENV{HOME}/.ggz/grubby.rc.backup.$pid");
system(@args);

print "Writing new configuration...\n";
open (F, ">$ENV{HOME}/.ggz/grubby.rc");
print F "[GRUBBYSETTINGS]\n";
print F "NAME = $grubby_name\n";
print F "OWNER = $grubby_owner\n";
print F "HOST = $grubby_server\n";
print F "PORT = $grubby_port\n";
if($grubby_autojoin eq "y" || $grubby_autojoin eq "yes") {print F "AUTOJOIN = $grubby_autoroom\n";}
if($grubby_lang eq "y" || $grubby_lang eq "yes") {print F "LANGCHECK = 0\n";}
close (F);

print "Starting $grubby_name ...\n";
@args = ("grubby&");
system(@args);

print "\nGrubby should be running :-) Have a nice day\n\n";
