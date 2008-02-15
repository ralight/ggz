#!/bin/sh

# Note: ggzd changes dirs after start so we need 'PWD' instead of '.'

export RUBYLIB=$PWD
ggzd -F

