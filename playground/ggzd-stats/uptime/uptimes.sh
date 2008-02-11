#!/bin/bash

grep UPDATE ggzd.log* | awk '{print $7}' | find_uptimes.pl > uptimes.new
mv uptimes.new uptimes
