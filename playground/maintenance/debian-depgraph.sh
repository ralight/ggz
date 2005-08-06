#!/bin/sh

apt-cache dotty \
	`apt-cache search ggz | cut -d " " -f 1 | grep ggz | grep -v oggz` | \
	egrep '(ggz|^[^"]|})' > ggz-dependencies.deps

dot -Tps -oggz-debian-depgraph.ps ggz-dependencies.deps
dot -Tpng -oggz-debian-depgraph.png ggz-dependencies.deps

rm ggz-dependencies.deps

#dot ggz-dependencies.deps > ggz-dependencies.dots
#neato -Tps < ggz-dependencies.dots > ggz-dependencies.ps

