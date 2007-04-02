#!/usr/bin/env perl
#
# Test client for the GGZ Community REST API
# Main application

package ggzapi_client;

use strict;
use LWP::UserAgent;
use XML::DOM;
use ggzapi_shell;

my $shell = ggzapi_shell->new;
$shell->cmdloop();

