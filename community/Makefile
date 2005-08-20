version = 0.0.12pre

all:
	echo "Please read setup/setup.txt."

dist:
	rm -rf ../ggz-community-$(version)
	cp -r ../community ../ggz-community-$(version)
	find ../ggz-community-$(version) -name CVS | xargs rm -rf
	cd .. && tar cvzf ggz-community-$(version).tar.gz ggz-community-$(version)
